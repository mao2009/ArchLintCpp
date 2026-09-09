#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace archlint {

struct SourceLocation {
    std::string file;
    std::uint32_t line{};
    std::uint32_t column{};
};

struct Dependency {
    std::string source_namespace;
    std::string target_namespace;
    std::string source_symbol;
    std::string target_symbol;
    SourceLocation location;
};

using DependencyGraph = std::vector<Dependency>;

struct Diagnostic {
    std::string id;
    std::string message;
    SourceLocation location;
    std::string source_component;
    std::string target_component;
};

} // namespace archlint
