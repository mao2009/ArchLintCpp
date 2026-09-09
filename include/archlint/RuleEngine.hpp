#pragma once

#include "archlint/Config.hpp"
#include "archlint/Model.hpp"

#include <optional>
#include <string>
#include <vector>

namespace archlint {

class RuleEngine {
public:
    explicit RuleEngine(Config config);

    [[nodiscard]] std::vector<Diagnostic> evaluate(const DependencyGraph& graph) const;
    [[nodiscard]] std::optional<std::string> componentFor(const std::string& nameSpace,
                                                          const std::string& file) const;

private:
    Config config_;
};

} // namespace archlint
