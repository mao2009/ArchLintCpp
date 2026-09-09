#include "archlint/RuleEngine.hpp"

#include <algorithm>
#include <sstream>

namespace archlint {
namespace {

bool namespaceMatches(const std::string& candidate, const std::string& prefix) {
    return candidate == prefix ||
           (candidate.size() > prefix.size() && candidate.starts_with(prefix) &&
            candidate.compare(prefix.size(), 2, "::") == 0);
}

} // namespace

RuleEngine::RuleEngine(Config config) : config_(std::move(config)) {}

std::optional<std::string> RuleEngine::componentFor(const std::string& nameSpace,
                                                    const std::string& file) const {
    for (const auto& layer : config_.layers) {
        for (const auto& prefix : layer.namespaces) {
            if (namespaceMatches(nameSpace, prefix)) {
                return layer.name;
            }
        }
        for (const auto& path : layer.paths) {
            if (!path.empty() && file.find(path) != std::string::npos) {
                return layer.name;
            }
        }
    }
    return std::nullopt;
}

std::vector<Diagnostic> RuleEngine::evaluate(const DependencyGraph& graph) const {
    std::vector<Diagnostic> diagnostics;

    for (const auto& dependency : graph) {
        const auto source = componentFor(dependency.source_namespace, dependency.location.file);
        const auto target = componentFor(dependency.target_namespace, {});
        if (!source || !target || source == target) {
            continue;
        }

        for (const auto& rule : config_.rules) {
            if (rule.from != *source ||
                std::find(rule.must_not_depend_on.begin(), rule.must_not_depend_on.end(), *target) ==
                    rule.must_not_depend_on.end()) {
                continue;
            }

            std::ostringstream message;
            message << *source << " must not depend on " << *target;
            if (!dependency.target_symbol.empty()) {
                message << " (reference to " << dependency.target_symbol << ')';
            }

            diagnostics.push_back(Diagnostic{
                .id = "ARCH001",
                .message = message.str(),
                .location = dependency.location,
                .source_component = *source,
                .target_component = *target,
            });
        }
    }

    return diagnostics;
}

} // namespace archlint
