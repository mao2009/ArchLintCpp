# Releasing ArchLintCpp

ArchLintCpp follows Semantic Versioning (`MAJOR.MINOR.PATCH`). While the project is pre-1.0, minor versions may include breaking configuration or CLI changes when clearly documented.

## Supported release artifacts

The release workflow currently publishes:

- Linux x86_64
- macOS arm64

Each archive is accompanied by a SHA-256 checksum file.

Windows binaries are intentionally not published yet. LLVM/Clang packaging and runtime layout on Windows differ substantially from the Linux/macOS paths used by the project today. Windows support should be added only after CI can build and test against a deterministic LLVM 18 toolchain without repository-specific setup.

## Versioning

- Patch: bug fixes and diagnostics corrections that do not intentionally change the public configuration contract.
- Minor: new rules, CLI features, output modes, supported platforms, or intentional pre-1.0 breaking changes.
- Major: reserved for post-1.0 incompatible changes.

Before tagging a release:

1. Update `CHANGELOG.md` and replace the target version's `TBD` date.
2. Ensure the project version in `CMakeLists.txt` matches the intended tag.
3. Ensure CI on `main` is green.
4. Create an annotated or lightweight tag named `vMAJOR.MINOR.PATCH` and push it.

The `Release` GitHub Actions workflow will build, test, package, checksum, and publish the release artifacts. The release itself uses generated GitHub release notes in addition to the curated changelog.

## Reproducibility expectations

Release archives contain only the installed ArchLintCpp payload produced by CMake. Build directories, source trees, and transient dependency caches are excluded. Artifact names include the semantic version tag and target platform.

A release should not be published manually from a developer workstation when the GitHub Actions workflow can produce it.
