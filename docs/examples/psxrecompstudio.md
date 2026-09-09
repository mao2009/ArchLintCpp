# PSXRecompStudio integration

[PSXRecompStudio](https://github.com/mao2009/PSXRecompStudio) is the first real-world project using ArchLintCpp as a CI architecture gate.

## Integration model

The project keeps ArchLintCpp out of the normal local build. Instead, a dedicated GitHub Actions job:

1. Checks out PSXRecompStudio.
2. Checks out a pinned ArchLintCpp revision.
3. Builds ArchLintCpp with LLVM/Clang 18.
4. Generates `compile_commands.json` for `src/PSXRecomp.Native`.
5. Runs ArchLintCpp with the project's architecture configuration.

This means ordinary development builds are not forced to install ArchLintCpp or LLVM development packages, while CI still enforces the selected architecture rules.

## Initial rules

The first rule set is intentionally conservative. It focuses on native component boundaries that are already meaningful to the project, including preventing the public C API layer from depending on native implementation components and preventing implementation components from depending back on the API bridge.

The purpose of the first integration is not to model every dependency in the native runtime. It is to establish a reliable real-world feedback loop.

## What dogfooding found

The integration exposed a generic configuration-loading bug in ArchLintCpp: the YAML parser was given a reference to a temporary string whose lifetime had already ended by the time parsing occurred.

The fix was made in ArchLintCpp itself and accompanied by regression coverage. No PSXRecompStudio-specific workaround was added to the analyzer.

This is an important design constraint for the project:

> Generic analyzer limitations discovered by consumers should be fixed generically, not encoded as special cases for one repository.

## Why this matters

Synthetic fixtures are useful for rule-engine correctness, but a real C++ project exercises compilation databases, include paths, namespaces, toolchain behavior, and CI integration in combinations that unit tests do not fully reproduce.

PSXRecompStudio therefore serves as both a consumer and a practical compatibility test for ArchLintCpp.
