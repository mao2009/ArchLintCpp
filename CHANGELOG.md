# Changelog

All notable changes to ArchLintCpp will be documented in this file.

The format is based on Keep a Changelog, and this project follows Semantic Versioning.

## [Unreleased]

### Added
- Linux arm64, macOS x86_64, and Windows x86_64 CI and release coverage.
- Windows LLVM/Clang 18 toolchain via conda-forge (`llvmdev`/`clangdev` 18.1.8) and MSVC.
- `workflow_dispatch` trigger on the `Release` workflow for verifying the full artifact matrix without publishing.

### Notes
- Windows arm64 remains intentionally deferred; see `docs/RELEASING.md`.

## [0.1.0] - 2026-09-09

### Added
- Initial Clang-based dependency extraction pipeline.
- YAML-configured architecture rules.
- Text and JSON diagnostics.
- CMake/CTest build and test infrastructure.
- Linux and macOS CI coverage.
- Linux x86_64 and macOS arm64 release artifacts.
- Reproducible release packaging with SHA-256 checksum files.
