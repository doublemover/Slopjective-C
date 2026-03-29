# objc3c Standard Library

This directory is the checked-in root for the Objective-C 3 standard-library
foundation surface.

## Boundary

Live stdlib work must stay on these paths:

- `stdlib/README.md`
- `stdlib/workspace.json`
- `stdlib/modules/`
- `docs/runbooks/objc3c_stdlib_foundation.md`
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
- the checked-in module roots will live under `stdlib/modules/`
- machine-owned materializations belong under `tmp/artifacts/stdlib/`
- validation reports belong under `tmp/reports/stdlib/`
- runnable package staging stays on the existing runnable toolchain bundle flow
