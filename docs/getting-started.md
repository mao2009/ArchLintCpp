# Getting Started

ArchLintCpp consumes the same compilation information that Clang-based tooling normally uses: a `compile_commands.json` database.

## Requirements

- CMake 3.20 or newer
- A C++20 compiler
- LLVM/Clang 18 development packages
- Ninja or another supported CMake generator

## Build ArchLintCpp

On Ubuntu 24.04, install the required LLVM/Clang packages:

```bash
sudo apt-get update
sudo apt-get install -y llvm-18-dev libclang-18-dev clang-18 ninja-build
```

Configure and build:

```bash
cmake -S . -B build -G Ninja \
  -DLLVM_DIR=/usr/lib/llvm-18/lib/cmake/llvm \
  -DClang_DIR=/usr/lib/llvm-18/lib/cmake/clang \
  -DCMAKE_BUILD_TYPE=Release

cmake --build build --parallel
```

Run the tests:

```bash
ctest --test-dir build --output-on-failure
```

## Prepare a target project

For a CMake project, enable compilation database generation:

```bash
cmake -S . -B build -G Ninja \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
```

This creates:

```text
build/compile_commands.json
```

ArchLintCpp uses that database to analyze translation units with their actual compiler flags and include paths.

## Create a configuration

Start with a small set of components and only encode rules that are architecturally meaningful.

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

See [Configuration](configuration.md) for mapping semantics.

## Run

```bash
./build/archlint-cpp \
  --config archlint.yml \
  --compile-db /path/to/project/build \
  --format text
```

Exit codes are intended for automation:

| Code | Meaning |
| --- | --- |
| `0` | No architecture violations |
| `1` | Architecture violations found |
| `2` | CLI or configuration error |
| `3` | Clang tooling or analysis failure |

## CI integration

A recommended pattern is to keep ArchLintCpp as a dedicated architecture job rather than making every normal local build depend on LLVM development packages.

That gives projects an explicit architecture gate while leaving ordinary developer builds unchanged.
