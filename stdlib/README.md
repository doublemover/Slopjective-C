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
- `stdlib/compatibility_gates.json`
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

Support truth:

- `docs/support/capability_matrix.md`
- `docs/support/evidence_map.md`
- `docs/support/hard_cutover_capability_truth.md`

Stdlib docs describe checked-in modules, package surfaces, and examples. They
do not claim runtime behavior, alias adapters, alternate imports, or
stdlib-wide completeness beyond implemented capability rows.

## Non-goals

This root is not a place for:

- milestone-only notes
- ad hoc package layouts outside `tmp/pkg/`
- duplicate tutorial text
- duplicate showcase sources
- a second stdlib onboarding tree outside `docs/tutorials/` and `showcase/`

## Working model

- canonical module names come from `spec/STANDARD_LIBRARY_CONTRACT.md`
- stability and allowed cross-module dependencies come from `stdlib/stability_policy.json`
- compiler-visible import mapping comes from `stdlib/package_surface.json`
- `stdlib/core_architecture.json` defines the core-stdlib ownership split for
  foundational utility, text/data, collection, option, and result families
- `stdlib/advanced_architecture.json` defines the advanced-stdlib ownership split for
  concurrency, reflection, interop, and runtime-composition helper families
- `stdlib/semantic_policy.json` defines the observable stability and helper
  semantics for the current core and advanced stdlib surfaces
- `stdlib/compatibility_gates.json` binds stdlib major-version `1` ABI,
  semantic, package, and conformance gates to checked-in runtime-backed
  evidence; it is the source for fail-closed compatibility decisions
- `stdlib/lowering_import_surface.json` defines the real smoke-compile artifact
  names, import identity fields, and machine-owned lowering roots
- `stdlib/advanced_helper_package_surface.json` defines how the advanced helper
  subset is expected to appear in the shared stdlib package and runnable bundle
- `stdlib/program_surface.json` defines the live docs, example, site-routing,
  and capability-demo boundary that stdlib-program work must stay inside
- the checked-in module roots will live under `stdlib/modules/`
- canonical module names map onto compiler-visible module identifiers because
  the current frontend module declaration syntax is identifier-based rather
  than dotted
- machine-owned materializations belong under `tmp/artifacts/stdlib/`
- validation reports belong under `tmp/reports/stdlib/`
- runnable package staging stays on the existing
  `npm run objc3c -- package-runnable-toolchain` flow
- validation and packaging commands for this root use the public
  `npm run objc3c -- <action>` surface
- retired command surfaces, direct helper commands, stdlib-local wrappers,
  alternate import/package support lanes, and retired-source support claims are not
  public stdlib entrypoints
- reader-facing onboarding, comparison, and capability-demo work stays on the
  live `docs/tutorials/`, `showcase/`, and `site/src/index.body.md` surfaces
