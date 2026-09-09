#include "archlint/ClangDependencyExtractor.hpp"

#include <clang/AST/ASTContext.h>
#include <clang/AST/Decl.h>
#include <clang/AST/Expr.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Tooling/CompilationDatabase.h>
#include <clang/Tooling/Tooling.h>

#include <set>
#include <sstream>
#include <tuple>

namespace archlint {
namespace {

std::string namespaceOf(const clang::Decl* decl) {
    if (decl == nullptr) {
        return {};
    }

    std::vector<std::string> parts;
    const clang::DeclContext* context = decl->getDeclContext();
    while (context != nullptr) {
        if (const auto* ns = llvm::dyn_cast<clang::NamespaceDecl>(context); ns != nullptr && !ns->isAnonymousNamespace()) {
            parts.push_back(ns->getNameAsString());
        }
        context = context->getParent();
    }

    std::string result;
    for (auto it = parts.rbegin(); it != parts.rend(); ++it) {
        if (!result.empty()) {
            result += "::";
        }
        result += *it;
    }
    return result;
}

class DependencyCallback final : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
    explicit DependencyCallback(DependencyGraph& graph) : graph_(graph) {}

    void run(const clang::ast_matchers::MatchFinder::MatchResult& result) override {
        const auto* source = result.Nodes.getNodeAs<clang::FunctionDecl>("source");
        const auto* target = result.Nodes.getNodeAs<clang::NamedDecl>("target");
        const auto* reference = result.Nodes.getNodeAs<clang::Expr>("reference");
        if (source == nullptr || target == nullptr || reference == nullptr || result.SourceManager == nullptr) {
            return;
        }

        const auto location = result.SourceManager->getExpansionLoc(reference->getExprLoc());
        if (location.isInvalid() || result.SourceManager->isInSystemHeader(location)) {
            return;
        }

        const auto presumed = result.SourceManager->getPresumedLoc(location);
        if (presumed.isInvalid()) {
            return;
        }

        const std::string sourceNamespace = namespaceOf(source);
        const std::string targetNamespace = namespaceOf(target);
        if (sourceNamespace.empty() || targetNamespace.empty()) {
            return;
        }

        const auto key = std::make_tuple(
            std::string(presumed.getFilename()),
            presumed.getLine(),
            presumed.getColumn(),
            source->getQualifiedNameAsString(),
            target->getQualifiedNameAsString());
        if (!seen_.insert(key).second) {
            return;
        }

        graph_.push_back(Dependency{
            .source_namespace = sourceNamespace,
            .target_namespace = targetNamespace,
            .source_symbol = source->getQualifiedNameAsString(),
            .target_symbol = target->getQualifiedNameAsString(),
            .location = SourceLocation{
                .file = presumed.getFilename(),
                .line = presumed.getLine(),
                .column = presumed.getColumn(),
            },
        });
    }

private:
    DependencyGraph& graph_;
    std::set<std::tuple<std::string, unsigned, unsigned, std::string, std::string>> seen_;
};

} // namespace

AnalysisResult extractDependencies(const AnalysisOptions& options) {
    std::string databaseError;
    auto database = clang::tooling::CompilationDatabase::loadFromDirectory(
        options.compilation_database_path, databaseError);
    if (!database) {
        return AnalysisResult{.error = "unable to load compilation database: " + databaseError, .tool_result = 2};
    }

    std::vector<std::string> sources = options.source_paths;
    if (sources.empty()) {
        sources = database->getAllFiles();
    }
    if (sources.empty()) {
        return AnalysisResult{.error = "compilation database contains no translation units", .tool_result = 2};
    }

    DependencyGraph graph;
    DependencyCallback callback(graph);
    clang::ast_matchers::MatchFinder finder;

    using namespace clang::ast_matchers;
    finder.addMatcher(
        declRefExpr(
            to(namedDecl().bind("target")),
            hasAncestor(functionDecl().bind("source")))
            .bind("reference"),
        &callback);
    finder.addMatcher(
        memberExpr(
            member(namedDecl().bind("target")),
            hasAncestor(functionDecl().bind("source")))
            .bind("reference"),
        &callback);

    clang::tooling::ClangTool tool(*database, sources);
    const int result = tool.run(clang::tooling::newFrontendActionFactory(&finder).get());
    if (result != 0) {
        return AnalysisResult{.graph = std::move(graph), .error = "Clang tooling failed", .tool_result = result};
    }

    return AnalysisResult{.graph = std::move(graph), .tool_result = 0};
}

} // namespace archlint
