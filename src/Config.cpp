#include "archlint/Config.hpp"

#include <llvm/Support/YAMLTraits.h>

#include <fstream>
#include <sstream>
#include <system_error>

LLVM_YAML_IS_SEQUENCE_VECTOR(archlint::LayerConfig)
LLVM_YAML_IS_SEQUENCE_VECTOR(archlint::DependencyRuleConfig)

namespace llvm::yaml {

template <>
struct MappingTraits<archlint::LayerConfig> {
    static void mapping(IO& io, archlint::LayerConfig& value) {
        io.mapRequired("name", value.name);
        io.mapOptional("namespaces", value.namespaces);
        io.mapOptional("paths", value.paths);
    }
};

template <>
struct MappingTraits<archlint::DependencyRuleConfig> {
    static void mapping(IO& io, archlint::DependencyRuleConfig& value) {
        io.mapRequired("from", value.from);
        io.mapRequired("must_not_depend_on", value.must_not_depend_on);
    }
};

template <>
struct MappingTraits<archlint::Config> {
    static void mapping(IO& io, archlint::Config& value) {
        io.mapRequired("layers", value.layers);
        io.mapRequired("rules", value.rules);
    }
};

} // namespace llvm::yaml

namespace archlint {

std::optional<Config> loadConfig(const std::string& path, std::string& error) {
    std::ifstream input(path);
    if (!input) {
        error = "unable to open configuration: " + path;
        return std::nullopt;
    }

    std::ostringstream buffer;
    buffer << input.rdbuf();

    Config config;
    llvm::yaml::Input yaml(buffer.str());
    yaml >> config;
    if (std::error_code ec = yaml.error()) {
        error = "invalid configuration: " + ec.message();
        return std::nullopt;
    }

    if (config.layers.empty()) {
        error = "configuration must define at least one layer";
        return std::nullopt;
    }

    return config;
}

} // namespace archlint
