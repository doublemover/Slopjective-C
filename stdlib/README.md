# objc3c Standard Library

This directory is the checked-in root for the Objective-C 3 standard-library
foundation and adoption surface.

## Boundary

Live stdlib work must stay on these paths:

- `stdlib/README.md`
- `stdlib/workspace.json`
- `stdlib/module_inventory.json`
- `stdlib/stability_policy.json`
- `stdlib/package_surface.json`
- `stdlib/core_architecture.json`
- `stdlib/advanced_architecture.json`
- `stdlib/semantic_policy.json`
- `stdlib/lowering_import_surface.json`
- `stdlib/advanced_helper_package_surface.json`
- `stdlib/program_surface.json`
- `stdlib/modules/`
- `docs/runbooks/objc3c_stdlib_foundation.md`
- `docs/runbooks/objc3c_stdlib_core.md`
- `docs/runbooks/objc3c_stdlib_advanced.md`
- `docs/runbooks/objc3c_stdlib_program.md`
- `docs/tutorials/`
- `showcase/`
- `site/src/index.body.md`
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
- a second stdlib onboarding tree outside `docs/tutorials/` and `showcase/`

## Working model

- canonical module names come from `spec/STANDARD_LIBRARY_CONTRACT.md`
- stability and allowed cross-module dependencies come from `stdlib/stability_policy.json`
- import/package alias mapping comes from `stdlib/package_surface.json`
- `stdlib/core_architecture.json` defines the `M306` ownership split for
  foundational utility, text/data, collection, option, and result families
- `stdlib/advanced_architecture.json` defines the `M307` ownership split for
  concurrency, reflection, interop, and runtime-composition helper families
- `stdlib/semantic_policy.json` defines the observable compatibility and helper
  semantics for the current core and advanced stdlib surfaces
- `stdlib/lowering_import_surface.json` defines the real smoke-compile artifact
  names, import identity fields, and machine-owned lowering roots
- `stdlib/advanced_helper_package_surface.json` defines how the advanced helper
  subset is expected to appear in the shared stdlib package and runnable bundle
- `stdlib/program_surface.json` defines the live docs, example, site-routing,
  and capability-demo boundary that `M308` work must stay inside
- the checked-in module roots will live under `stdlib/modules/`
- canonical module names map onto implementation aliases because the current
  frontend module declaration syntax is identifier-based rather than dotted
- machine-owned materializations belong under `tmp/artifacts/stdlib/`
- validation reports belong under `tmp/reports/stdlib/`
- runnable package staging stays on the existing runnable toolchain bundle flow
- reader-facing onboarding, comparison, and capability-demo work stays on the
  live `docs/tutorials/`, `showcase/`, and `site/src/index.body.md` surfaces
