# objc3c Advanced Stdlib Helper Surface

This runbook defines the live `M307` boundary for concurrency, reflection,
interop, and runtime-composition helper work in the checked-in standard
library.

It extends `docs/runbooks/objc3c_stdlib_foundation.md` and
`docs/runbooks/objc3c_stdlib_core.md` with the exact ownership split for the
advanced helper modules that sit above the `M306` core surface.

## Working boundary

Live `M307` work must stay on these paths:

- `stdlib/advanced_architecture.json`
- `stdlib/modules/objc3.concurrency/`
- `stdlib/modules/objc3.keypath/`
- `stdlib/modules/objc3.system/`
- `docs/runbooks/objc3c_stdlib_advanced.md`
- `tmp/artifacts/stdlib/`
- `tmp/reports/stdlib/`

Exact live implementation paths for downstream work:

- `stdlib/modules/objc3.concurrency/module.objc3`
- `stdlib/modules/objc3.keypath/module.objc3`
- `stdlib/modules/objc3.system/module.objc3`
- `stdlib/modules/objc3.concurrency/module.json`
- `stdlib/modules/objc3.keypath/module.json`
- `stdlib/modules/objc3.system/module.json`
- `stdlib/advanced_architecture.json`
- `scripts/check_stdlib_surface.py`
- `scripts/materialize_objc3c_stdlib_workspace.py`
- `scripts/run_objc3c_stdlib_workspace_smoke.py`
- `scripts/package_objc3c_runnable_toolchain.ps1`
- `scripts/objc3c_public_workflow_runner.py`

## Advanced family split

`objc3.concurrency` owns:

- structured child-task spawn helpers
- detached-task helper entrypoints
- join, wait, and task-group helper shapes
- cancellation query and checkpoint helpers
- executor hop and actor-adjacent helper hooks

`objc3.keypath` owns:

- typed key-path lookup and application helpers
- key-path metadata shape helpers
- reflection-facing adapters for key-path metadata
- interop/runtime-composition adapters that preserve typed key-path identity

`objc3.system` owns:

- resource-cleanup helpers that are profile-gated to Strict System
- borrowed-lifetime and runtime-cooperation hooks
- strict-system diagnostics helper entrypoints
- runtime-composition hooks that should not be widened into core imports

## Expected shipped API families

The checked-in architecture contract requires these families to stay visible:

- `objc3.concurrency`
  - `structured-child-spawn`
  - `detached-spawn`
  - `join-and-wait`
  - `task-group-scope`
  - `cancellation-observation`
  - `executor-hop`
- `objc3.keypath`
  - `typed-keypath-application`
  - `typed-keypath-text-compatibility`
  - `typed-keypath-metadata`
  - `reflection-interop`
  - `runtime-composition-adapter`
- `objc3.system`
  - `resource-cleanup`
  - `borrowed-lifetime-hook`
  - `strict-system-diagnostics`
  - `runtime-composition-hook`

Downstream implementation issues may add concrete helpers inside these
families, but they should not move ownership between modules without updating
the checked-in architecture contract.

## Exact checked-in source surface

`objc3.concurrency` currently exports:

- `objc3_concurrency_spawn_token`
- `objc3_concurrency_cancellation_checkpoint`

`objc3.keypath` currently exports:

- `objc3_keypath_apply_index`
- `objc3_keypath_component_count`
- `objc3_keypath_text_compatibility_score`
- `objc3_keypath_text_compatibility_diagnostic`

`objc3.system` currently exports:

- `objc3_system_resource_token`

## Layering rules

- `objc3.concurrency` may depend on `objc3.core` and `objc3.errors`
- `objc3.keypath` may depend on `objc3.core` and `objc3.errors`
- `objc3.system` may depend on `objc3.core`, `objc3.errors`,
  `objc3.concurrency`, and `objc3.keypath`
- `M307` does not move `M306` core helpers out of their existing modules

## Explicit non-goals

This milestone does not justify:

- a duplicate advanced-helper tree outside `stdlib/`
- public reflection or executor ABI claims without checked-in helper exports
- moving strict-system hooks into always-on core imports
- adding a second stdlib packaging flow or milestone-local wrapper scripts
