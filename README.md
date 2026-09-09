# ArchLintCpp

ArchLintCpp is an architecture and dependency linter for modern C++ projects powered by Clang.

The project aims to provide architecture-level checks similar in spirit to analyzers used in managed-language ecosystems, while fitting naturally into CMake/Clang-based C++ workflows.

## Goals

- Analyze C++ projects through Clang tooling and `compile_commands.json`
- Build a reusable dependency model from translation units
- Enforce configurable layer, component, namespace, and dependency boundaries
- Emit actionable diagnostics for local development and CI
- Remain independent from any single consumer project

## Planned workflow

```text
CMake / compile_commands.json
            |
            v
      Clang frontend
            |
            v
     Dependency graph
            |
            v
        Rule engine
            |
            v
       Diagnostics
```

## Status

ArchLintCpp is in the initial design and bootstrap phase. The implementation roadmap is tracked in GitHub Issues.

The first real-world dogfooding target is the native C++ portion of PSXRecompStudio, while ArchLintCpp itself will remain a general-purpose tool.

## License

MIT License. See [LICENSE](LICENSE).
