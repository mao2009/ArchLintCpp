# Configuration

ArchLintCpp reads a YAML file that defines named layers/components and forbidden dependencies.

## Example

```yaml
layers:
  - name: Domain
    namespaces:
      - example::domain
  - name: Infrastructure
    namespaces:
      - example::infra
  - name: Cli
    paths:
      - /src/cli/

rules:
  - from: Domain
    must_not_depend_on:
      - Infrastructure
      - Cli
```

## Layers

Each layer requires a unique `name` and may declare namespace prefixes, source-path fragments, or both.

Namespace matching respects C++ namespace boundaries. For example, `example::domain` matches `example::domain::model` but does not match `example::domain2`.

Path matching is currently substring-based and is intended primarily for components that are easier to identify by source layout than namespace.

When both namespace and path rules can match, the first layer in the configuration wins. Keep mappings non-overlapping where possible.

## Rules

The initial rule format is a directed deny-list:

```yaml
rules:
  - from: Domain
    must_not_depend_on:
      - Infrastructure
```

This emits `ARCH001` whenever a dependency from the `Domain` component targets `Infrastructure`.

## CLI

```bash
archlint-cpp \
  --config archlint.yml \
  --compile-db build \
  --format text
```

If no source files are supplied, all translation units from the compilation database are analyzed. Passing source files positionally limits analysis to those files.

For machine-readable output, use `--format json`.
