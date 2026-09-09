#pragma once

#include "archlint/Model.hpp"

#include <string>
#include <vector>

namespace archlint {

struct AnalysisOptions {
    std::string compilation_database_path;
    std::vector<std::string> source_paths;
};

struct AnalysisResult {
    DependencyGraph graph;
    std::string error;
    int tool_result{};
};

AnalysisResult extractDependencies(const AnalysisOptions& options);

} // namespace archlint
