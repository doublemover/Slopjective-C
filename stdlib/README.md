# objc3c Standard Library

This directory is the checked-in root for the Objective-C 3 standard-library
foundation surface.

## Boundary

Live stdlib work must stay on these paths:

- `stdlib/README.md`
- `stdlib/workspace.json`
- `stdlib/module_inventory.json`
- `stdlib/stability_policy.json`
- `stdlib/package_surface.json`
- `stdlib/core_architecture.json`
- `stdlib/modules/`
- `docs/runbooks/objc3c_stdlib_foundation.md`
- `docs/runbooks/objc3c_stdlib_core.md`
- `tmp/artifacts/stdlib/`
- `tmp/reports/stdlib/`

Authoritative semantic contract:

- `spec/STANDARD_LIBRARY_CONTRACT.md`

## Non-goals

This root is not a place for:

- milestone-only notes
- temporary package layouts outside `tmp/pkg/`
- duplicate tutorial text
- duplicate showcase sources

## Working model

- canonical module names come from `spec/STANDARD_LIBRARY_CONTRACT.md`
- stability and allowed cross-module dependencies come from `stdlib/stability_policy.json`
- import/package alias mapping comes from `stdlib/package_surface.json`
- `stdlib/core_architecture.json` defines the `M306` ownership split for
  foundational utility, text/data, collection, option, and result families
- the checked-in module roots will live under `stdlib/modules/`
- canonical module names map onto implementation aliases because the current
  frontend module declaration syntax is identifier-based rather than dotted
- machine-owned materializations belong under `tmp/artifacts/stdlib/`
- validation reports belong under `tmp/reports/stdlib/`
- runnable package staging stays on the existing runnable toolchain bundle flow
