#include "archlint/Config.hpp"
#include "archlint/RuleEngine.hpp"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>

namespace {

void require(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "FAILED: " << message << '\n';
        std::exit(1);
    }
}

archlint::Config makeConfig() {
    return archlint::Config{
        .layers = {
            archlint::LayerConfig{.name = "Domain", .namespaces = {"example::domain"}},
            archlint::LayerConfig{.name = "Infrastructure", .namespaces = {"example::infra"}},
            archlint::LayerConfig{.name = "Cli", .paths = {"/src/cli/"}},
        },
        .rules = {
            archlint::DependencyRuleConfig{.from = "Domain", .must_not_depend_on = {"Infrastructure"}},
        },
    };
}

void testConfigLoading() {
    const char* path = "archlint-config-test.yml";
    {
        std::ofstream output(path);
        output << R"(layers:
  - name: Domain
    namespaces:
      - example::domain
  - name: Infrastructure
    paths:
      - /src/infra/
rules:
  - from: Domain
    must_not_depend_on:
      - Infrastructure
)";
    }

    std::string error;
    const auto config = archlint::loadConfig(path, error);
    std::remove(path);

    require(config.has_value(), "valid YAML configuration should load");
    require(error.empty(), "valid YAML configuration should not return an error");
    require(config->layers.size() == 2, "configuration should preserve layers");
    require(config->layers[0].name == "Domain", "configuration should preserve layer names");
    require(config->layers[0].namespaces.size() == 1,
            "configuration should preserve namespace mappings");
    require(config->rules.size() == 1, "configuration should preserve rules");
    require(config->rules[0].must_not_depend_on.size() == 1,
            "configuration should preserve forbidden dependencies");
}

} // namespace

int main() {
    testConfigLoading();

    const archlint::RuleEngine engine(makeConfig());

    require(engine.componentFor("example::domain::model", "") == "Domain",
            "nested namespace should map to Domain");
    require(engine.componentFor("example::infrastructure", "") == std::nullopt,
            "namespace prefix must respect :: boundary");
    require(engine.componentFor("", "/project/src/cli/main.cpp") == "Cli",
            "path mapping should map CLI source");

    const archlint::Dependency allowed{
        .source_namespace = "example::infra",
        .target_namespace = "example::domain",
        .source_symbol = "example::infra::Repository::load",
        .target_symbol = "example::domain::Entity",
        .location = {.file = "/project/src/infra/repository.cpp", .line = 10, .column = 5},
    };
    require(engine.evaluate({allowed}).empty(), "allowed dependency should not produce diagnostics");

    const archlint::Dependency forbidden{
        .source_namespace = "example::domain::service",
        .target_namespace = "example::infra",
        .source_symbol = "example::domain::service::run",
        .target_symbol = "example::infra::Database::save",
        .location = {.file = "/project/src/domain/service.cpp", .line = 42, .column = 9},
    };

    const auto diagnostics = engine.evaluate({forbidden});
    require(diagnostics.size() == 1, "forbidden dependency should produce one diagnostic");
    require(diagnostics[0].id == "ARCH001", "diagnostic ID should be stable");
    require(diagnostics[0].source_component == "Domain", "source component should be Domain");
    require(diagnostics[0].target_component == "Infrastructure", "target component should be Infrastructure");
    require(diagnostics[0].location.line == 42, "diagnostic should preserve source location");

    std::cout << "All ArchLintCpp tests passed\n";
    return 0;
}
