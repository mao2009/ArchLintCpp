# Contributing

Thanks for contributing to ArchLintCpp.

## Development setup

ArchLintCpp currently targets LLVM/Clang 18 and requires CMake 3.20+ and a C++20 compiler.

```bash
cmake -S . -B build \
  -DLLVM_DIR=/path/to/llvm/lib/cmake/llvm \
  -DClang_DIR=/path/to/llvm/lib/cmake/clang
cmake --build build
ctest --test-dir build --output-on-failure
```

## Design guidelines

- Keep Clang AST handling inside the frontend/extractor layer where possible.
- Keep architecture policy inside the rule engine.
- Do not add consumer-project-specific paths, namespaces, or exceptions to the core.
- Add small synthetic regression tests for generic analyzer bugs.
- Preserve stable diagnostic IDs once released.

## Pull requests

Keep changes focused and include tests for rule behavior or dependency extraction changes. Document user-visible configuration changes in `docs/configuration.md`.

## Compatibility

The initial compatibility target is LLVM/Clang 18. Supporting additional major versions should be explicit and covered by CI before being documented as supported.
