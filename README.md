# ArchLintCpp

ArchLintCpp is an architecture and dependency linter for modern C++ projects powered by Clang.

It analyzes a project's Clang compilation database, extracts dependencies from the AST, maps code to configured architectural components, and reports forbidden dependencies locally or in CI.

## Features

- Clang Tooling integration via `compile_commands.json`
- Namespace- and source-path-based component mapping
- Configurable forbidden dependency rules
- Stable `ARCH001` diagnostics with source locations
- Human-readable and JSON output
- CMake/CTest build and test workflow
- GitHub Actions CI and release artifacts for Linux (x86_64/arm64), macOS (arm64/x86_64), and Windows (x86_64) — see [docs/RELEASING.md](docs/RELEASING.md)

## Architecture

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

The Clang frontend is intentionally separated from the rule engine so architecture policy can be tested without constructing AST nodes. See [docs/architecture.md](docs/architecture.md).

## Requirements

- CMake 3.20+
- C++20 compiler
- LLVM 18
- Clang 18 development libraries

## Build

```bash
cmake -S . -B build \
  -DLLVM_DIR=/path/to/llvm/lib/cmake/llvm \
  -DClang_DIR=/path/to/llvm/lib/cmake/clang
cmake --build build
ctest --test-dir build --output-on-failure
```

CMake enables `CMAKE_EXPORT_COMPILE_COMMANDS`, so CMake projects can generate the compilation database ArchLintCpp consumes.

## Configuration

Start from [`archlint.example.yml`](archlint.example.yml):

```yaml
layers:
  - name: Domain
    namespaces:
      - example::domain
  - name: Infrastructure
    namespaces:
      - example::infra

rules:
  - from: Domain
    must_not_depend_on:
      - Infrastructure
```

See [docs/configuration.md](docs/configuration.md) for the complete initial configuration reference.

## Usage

```bash
archlint-cpp \
  --config archlint.yml \
  --compile-db build
```

To produce machine-readable diagnostics:

```bash
archlint-cpp --config archlint.yml --compile-db build --format json
```

Exit codes are deterministic: `0` for success, `1` for architecture violations, `2` for configuration/CLI errors, and `3` for Clang analysis failures.

## Project status

ArchLintCpp is pre-1.0 and the dependency extractor is intentionally conservative. The first implementation records declaration and member references inside functions; additional dependency kinds will be added as real-world usage identifies them.

The first dogfooding target is the native C++ portion of [PSXRecompStudio](https://github.com/mao2009/PSXRecompStudio), while ArchLintCpp itself remains a general-purpose tool.

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md).

## License

MIT License. See [LICENSE](LICENSE).
