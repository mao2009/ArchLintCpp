#pragma once

#include <optional>
#include <string>
#include <vector>

namespace archlint {

struct LayerConfig {
    std::string name;
    std::vector<std::string> namespaces;
    std::vector<std::string> paths;
};

struct DependencyRuleConfig {
    std::string from;
    std::vector<std::string> must_not_depend_on;
};

struct Config {
    std::vector<LayerConfig> layers;
    std::vector<DependencyRuleConfig> rules;
};

std::optional<Config> loadConfig(const std::string& path, std::string& error);

} // namespace archlint
