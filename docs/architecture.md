# Architecture

ArchLintCpp separates compiler integration from architecture policy.

```text
compile_commands.json
        |
        v
ClangDependencyExtractor
        |
        v
  DependencyGraph
        |
        v
     RuleEngine
        |
        v
    Diagnostic[]
```

## Clang frontend

`ClangDependencyExtractor` loads the compilation database through Clang Tooling and analyzes the selected translation units. The first implementation records declaration references and member references that occur inside functions, preserving the source location and source/target namespaces.

The extractor produces only the language-facing dependency model. It does not know which dependencies are allowed.

## Dependency model

The core model is intentionally small: a dependency has a source namespace/symbol, a target namespace/symbol, and a source location. This keeps rule evaluation independent from Clang AST node lifetimes and allows future frontends or fixture-based tests to construct dependencies directly.

## Rule engine

The rule engine maps namespaces or source paths to named components/layers and evaluates configured forbidden dependencies. The initial rule is `ARCH001`, emitted when a source component depends on a configured forbidden target component.

## Diagnostics

Diagnostics have stable IDs, source locations, messages, and source/target component names. The CLI can emit either compiler-style text or JSON.

## Exit codes

- `0`: analysis succeeded with no violations
- `1`: architecture violations were found
- `2`: command-line or configuration error
- `3`: Clang analysis/tooling failure

## Design constraints

- No PSXRecompStudio-specific knowledge in the analyzer core.
- Configuration and rule evaluation remain independent from Clang callbacks.
- The dependency model should stay stable even if the Clang frontend evolves.
- New rules should consume the dependency model instead of directly traversing AST nodes unless the rule fundamentally requires syntax-specific information.
