#include "archlint/ClangDependencyExtractor.hpp"
#include "archlint/Config.hpp"
#include "archlint/RuleEngine.hpp"

#include <llvm/Support/CommandLine.h>
#include <llvm/Support/raw_ostream.h>

#include <string>
#include <vector>

namespace {

std::string jsonEscape(const std::string& input) {
    std::string output;
    for (const char ch : input) {
        switch (ch) {
        case '\\': output += "\\\\"; break;
        case '"': output += "\\\""; break;
        case '\n': output += "\\n"; break;
        case '\r': output += "\\r"; break;
        case '\t': output += "\\t"; break;
        default: output += ch; break;
        }
    }
    return output;
}

} // namespace

int main(int argc, char** argv) {
    llvm::cl::OptionCategory category("ArchLintCpp options");
    llvm::cl::opt<std::string> configPath(
        "config", llvm::cl::desc("Path to ArchLintCpp YAML configuration"),
        llvm::cl::value_desc("file"), llvm::cl::Required, llvm::cl::cat(category));
    llvm::cl::opt<std::string> compilationDatabase(
        "compile-db", llvm::cl::desc("Directory containing compile_commands.json"),
        llvm::cl::value_desc("directory"), llvm::cl::init("."), llvm::cl::cat(category));
    llvm::cl::opt<std::string> format(
        "format", llvm::cl::desc("Diagnostic output format: text or json"),
        llvm::cl::value_desc("format"), llvm::cl::init("text"), llvm::cl::cat(category));
    llvm::cl::list<std::string> sources(
        llvm::cl::Positional, llvm::cl::desc("[source files...]"), llvm::cl::ZeroOrMore,
        llvm::cl::cat(category));

    llvm::cl::HideUnrelatedOptions(category);
    llvm::cl::SetVersionPrinter([](llvm::raw_ostream& os) {
        os << "ArchLintCpp " ARCHLINT_VERSION "\n";
    });
    llvm::cl::ParseCommandLineOptions(argc, argv, "C++ architecture dependency linter\n");

    if (format != "text" && format != "json") {
        llvm::errs() << "error: --format must be 'text' or 'json'\n";
        return 2;
    }

    std::string configError;
    const auto config = archlint::loadConfig(configPath, configError);
    if (!config) {
        llvm::errs() << "configuration error: " << configError << '\n';
        return 2;
    }

    const auto analysis = archlint::extractDependencies(archlint::AnalysisOptions{
        .compilation_database_path = compilationDatabase,
        .source_paths = std::vector<std::string>(sources.begin(), sources.end()),
    });
    if (analysis.tool_result != 0) {
        llvm::errs() << "analysis error: " << analysis.error << '\n';
        return 3;
    }

    const archlint::RuleEngine engine(*config);
    const auto diagnostics = engine.evaluate(analysis.graph);

    if (format == "json") {
        llvm::outs() << "{\"diagnostics\":[";
        for (std::size_t i = 0; i < diagnostics.size(); ++i) {
            const auto& diagnostic = diagnostics[i];
            if (i != 0) llvm::outs() << ',';
            llvm::outs() << "{\"id\":\"" << jsonEscape(diagnostic.id)
                         << "\",\"message\":\"" << jsonEscape(diagnostic.message)
                         << "\",\"file\":\"" << jsonEscape(diagnostic.location.file)
                         << "\",\"line\":" << diagnostic.location.line
                         << ",\"column\":" << diagnostic.location.column
                         << ",\"sourceComponent\":\"" << jsonEscape(diagnostic.source_component)
                         << "\",\"targetComponent\":\"" << jsonEscape(diagnostic.target_component)
                         << "\"}";
        }
        llvm::outs() << "]}\n";
    } else {
        for (const auto& diagnostic : diagnostics) {
            llvm::outs() << diagnostic.location.file << ':' << diagnostic.location.line << ':'
                         << diagnostic.location.column << ": error " << diagnostic.id << ": "
                         << diagnostic.message << '\n';
        }
    }

    return diagnostics.empty() ? 0 : 1;
}
