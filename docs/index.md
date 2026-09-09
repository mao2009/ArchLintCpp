# ArchLintCpp

**Architecture and dependency linter for modern C++ projects powered by Clang.**

ArchLintCpp analyzes C++ source using Clang tooling and a `compile_commands.json` compilation database, then checks architectural dependency rules defined in YAML.

It is intended for projects that want architecture checks to run alongside normal CI without coupling those checks to a specific application framework or build system.

## Why ArchLintCpp?

C++ projects can enforce formatting, warnings, tests, and static-analysis rules, but architectural boundaries are often left to documentation and code review. ArchLintCpp makes selected dependency rules executable.

Current capabilities include:

- Clang AST-based dependency extraction rather than text or regex parsing.
- `compile_commands.json` support for real project compilation settings.
- Configurable layers/components based on namespaces and paths.
- Forbidden dependency rules between components.
- Stable diagnostics suitable for CI.
- Text and JSON output.
- Linux CI support, with cross-platform validation being expanded.

## Example

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

Run the analyzer against an existing compilation database:

```bash
archlint-cpp \
  --config archlint.yml \
  --compile-db build \
  --format text
```

An architecture violation causes a non-zero exit code, which makes the tool suitable for a dedicated CI architecture gate.

## Real-world dogfooding

ArchLintCpp's first real-world consumer is [PSXRecompStudio](https://github.com/mao2009/PSXRecompStudio), where it checks native C++ component boundaries in a dedicated GitHub Actions job.

That integration has already been useful beyond validation: it exposed a generic configuration-loading lifetime bug in ArchLintCpp, which was fixed in the analyzer rather than worked around in the consumer project.

## Project status

ArchLintCpp is an early-stage, pre-1.0 project. Dependency extraction is intentionally conservative and the supported rule set will grow based on real-world usage.

The goal is not to claim complete C++ architectural analysis today. The goal is to provide a small, testable core that can evolve without embedding project-specific knowledge.

[Get started](getting-started.md){ .md-button .md-button--primary }
[View on GitHub](https://github.com/mao2009/ArchLintCpp){ .md-button }
