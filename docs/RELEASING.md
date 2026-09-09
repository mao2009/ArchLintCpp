# Releasing ArchLintCpp

ArchLintCpp follows Semantic Versioning (`MAJOR.MINOR.PATCH`). While the project is pre-1.0, minor versions may include breaking configuration or CLI changes when clearly documented.

## Supported release artifacts

The release workflow publishes:

| Platform | Architecture | Runner | Archive |
| --- | --- | --- | --- |
| Linux | x86_64 | `ubuntu-24.04` | `ArchLintCpp-vX.Y.Z-linux-x86_64.tar.gz` |
| Linux | arm64 | `ubuntu-24.04-arm` | `ArchLintCpp-vX.Y.Z-linux-arm64.tar.gz` |
| macOS | arm64 | `macos-15` | `ArchLintCpp-vX.Y.Z-macos-arm64.tar.gz` |
| macOS | x86_64 | `macos-15-intel` | `ArchLintCpp-vX.Y.Z-macos-x86_64.tar.gz` |
| Windows | x86_64 | `windows-2022` | `ArchLintCpp-vX.Y.Z-windows-x86_64.zip` |

Linux and macOS archives are `.tar.gz`; the Windows archive is `.zip` (matching platform convention). Every archive is accompanied by a SHA-256 checksum file (`<archive>.sha256`).

A target is only added to this table once its configure/build/CTest/install/package steps run unattended and reproducibly in GitHub Actions — see "Reproducibility expectations" below. A target that fails any of those steps is not attached to the GitHub Release, even if other targets in the same run succeed.

### Windows LLVM/Clang 18 toolchain

The official LLVM Windows installers (and the Chocolatey/winget packages built from them) ship the `clang`/`clang-cl` binaries but do not include `LLVMConfig.cmake`, `ClangConfig.cmake`, or the static libraries needed to build against LLVM/Clang as a library — this is a known upstream packaging gap, not specific to this project. To get a reproducible, version-pinned dev environment instead, the Windows release/CI jobs install `llvmdev=18.1.8` and `clangdev=18.1.8` from the `conda-forge` channel via [`mamba-org/setup-micromamba`](https://github.com/mamba-org/setup-micromamba). This provides `LLVMConfig.cmake` / `ClangConfig.cmake` under the environment's `Library\lib\cmake\{llvm,clang}` directories, so `LLVM_DIR` and `Clang_DIR` can be set explicitly:

```bash
cmake -S . -B build -G Ninja \
  -DLLVM_DIR="$CONDA_PREFIX/Library/lib/cmake/llvm" \
  -DClang_DIR="$CONDA_PREFIX/Library/lib/cmake/clang" \
  -DCMAKE_C_COMPILER=cl \
  -DCMAKE_CXX_COMPILER=cl \
  -DCMAKE_BUILD_TYPE=Release
```

The project itself is built with MSVC (`cl.exe`, set up via [`ilammy/msvc-dev-cmd`](https://github.com/ilammy/msvc-dev-cmd)), matching the MSVC ABI that conda-forge's Windows LLVM/Clang packages are built with — this avoids mixing incompatible C++ ABIs the way AppleClang/Homebrew-Clang would on macOS.

`archlint-cpp.exe` dynamically links `zlib.dll` and `zstd.dll` from the conda-forge toolchain (verified with `objdump -p`), so the release workflow copies both into the archive next to the executable and then extracts the packaged archive into a clean directory and runs it with a minimal `PATH` (no conda/MSVC environment) before uploading, to catch a missing-DLL regression before it reaches users. The Windows binary otherwise depends only on the standard Windows system DLLs and the Microsoft Visual C++ Redistributable (`MSVCP140.dll`/`VCRUNTIME140*.dll`) — already present on virtually all Windows systems, and preinstalled on the GitHub-hosted runner used to build it.

### Deferred targets

**Windows arm64** is intentionally deferred and not part of the release matrix.

- **Reason**: no reproducible source of LLVM/Clang 18 *development* packages (headers, static libs, CMake config files) for `win-arm64` was found. `conda-forge`'s `llvmdev`/`clangdev` packages are not published for `win-arm64` (only `win-64`/`win-32`), and the official LLVM Windows-on-Arm installer has the same missing-CMake-config gap as the x86_64 installer, with no dev-package alternative.
- **What's missing to resume**: a `win-arm64` build of `llvmdev`/`clangdev` on conda-forge, an official LLVM release that includes CMake config files for `win-arm64`, or an equivalent maintained, version-pinned source of LLVM/Clang 18 dev libraries for that target.
- **Resume condition**: once such a source exists, add a `windows-11-arm` matrix entry (GitHub-hosted Windows arm64 runners are generally available for public repositories) using the same pattern as Windows x86_64, verify build/CTest/install/package succeed in GitHub Actions, then add it to this table.

### Differences from a local build

Local builds (see the root `README.md`) point `LLVM_DIR`/`Clang_DIR` at whatever LLVM 18 installation is available on the developer's machine (e.g. a Homebrew, apt, or self-built LLVM). Release/CI builds instead pin an exact LLVM/Clang 18 patch version per platform (`llvm-18`/`clang-18` from Ubuntu's archive, `llvm@18` from Homebrew, `llvmdev=18.1.8`/`clangdev=18.1.8` from conda-forge on Windows) so that release artifacts are reproducible across runs. A local build is not guaranteed to use the exact same LLVM patch version as the release artifacts.

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

The `Release` workflow also accepts manual `workflow_dispatch` runs (e.g. from a feature branch). A manual run builds, tests, packages, checksums, and uploads every matrix target as workflow artifacts, but skips the `publish` job — no GitHub Release is created. Use this to verify the full release matrix (including Windows) before cutting a tag.

## Reproducibility expectations

Release archives contain only the installed ArchLintCpp payload produced by CMake. Build directories, source trees, and transient dependency caches are excluded. Artifact names include the semantic version tag and target platform.

A release should not be published manually from a developer workstation when the GitHub Actions workflow can produce it.
