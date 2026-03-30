# objc3c Runtime Performance Boundary

## Working Boundary

This runbook defines the live runtime-performance boundary for `M294`.

Use it when changing:

- startup registration and replay costs
- selector lookup and dispatch cache behavior
- property/reflection query hot paths
- ARC, weak, autoreleasepool, and ownership-helper hot paths
- runtime performance summaries, regression artifacts, and packaged validation

Downstream `M294` work must stay on the existing runtime library, runtime
acceptance helpers, public workflow runner, and runnable toolchain package
surfaces listed here. Do not add a milestone-local runtime benchmark harness,
standalone spreadsheet flow, or synthetic performance proof path.

## Runtime Hot-Path Taxonomy

The current truthful runtime-performance workload families are:

- `startup-installation`
  - objective: measure image registration, staged-table walking, reset, and
    replay behavior through the live runtime bootstrap path
- `dispatch-cache`
  - objective: measure selector lookup, method-cache seeding, cache-hit
    dispatch, and deterministic fallback dispatch through
    `objc3_runtime_dispatch_i32`
- `reflection-query`
  - objective: measure realized class/property/protocol reflection queries
    through the live object-model and property-registry snapshot helpers
- `ownership-helpers`
  - objective: measure ARC/current-property/weak/autoreleasepool helper traffic
    through the live bootstrap-internal runtime helper ABI
- `runtime-counter-snapshot`
  - objective: preserve the counter and snapshot fields that explain a runtime
    timing result instead of publishing wall-clock values alone

## Bottleneck Map

The current bottleneck map is:

- startup registration:
  - `objc3_runtime_register_image`
  - `TryWalkRegistrationTableUnlocked`
  - `RebuildRealizedClassGraphUnlocked`
  - `SeedDispatchIntentFastPathCacheUnlocked`
- selector lookup and dispatch:
  - `LookupSelectorUnlocked`
  - `MaterializeSelectorLookupEntryUnlocked`
  - `ResolveMethodSlowPathUnlocked`
  - `objc3_runtime_dispatch_i32`
- reflection and ownership:
  - `FindRuntimePropertyAccessorByNameUnlocked`
  - `objc3_runtime_copy_property_entry_for_testing`
  - `objc3_runtime_copy_object_model_query_state_for_testing`
  - `objc3_runtime_bind_current_property_context_for_testing`
  - `objc3_runtime_copy_memory_management_state_for_testing`
  - `objc3_runtime_copy_arc_debug_state_for_testing`

## Optimization Correctness Policy

Runtime-performance work is only valid when it preserves these invariants:

- the canonical runtime dispatch entrypoint remains `objc3_runtime_dispatch_i32`
- selector lookup remains rooted in `objc3_runtime_lookup_selector`
- startup publication remains rooted in `objc3_runtime_register_image`,
  staged-table walking, reset, and replay
- reflection and ownership proofs remain rooted in the existing private testing
  snapshot helpers, not a second reporting ABI
- benchmark summaries must include both timing results and the coupled runtime
  counter/snapshot fields that explain those timings

Allowed optimization moves:

- reduce repeated work inside selector materialization, registration-time cache
  seeding, and realized-property lookup
- reserve or reuse runtime-owned container capacity where the live registration
  and class-graph model makes the size knowable
- publish new private testing counters or summaries through the existing
  bootstrap-internal helper boundary
- add public workflow actions only when they execute the live runtime code and
  authoritative runtime probes

Disallowed optimization moves:

- no alternate dispatch entrypoint, benchmark-only runtime shim, or synthetic
  property/reflection path
- no widening of `native/objc3c/src/runtime/objc3_runtime.h` just to expose
  performance counters
- no sidecar-only timing report with no coupled runtime probe result
- no claim that a cache fast path is active unless the runtime snapshot state
  reports it directly

## Exact Live Implementation Paths

- runtime library:
  - `native/objc3c/src/runtime/objc3_runtime.h`
  - `native/objc3c/src/runtime/objc3_runtime.cpp`
  - `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`
  - `native/objc3c/src/runtime/ARCHITECTURE.md`
- compile/build/runtime harness:
  - `scripts/build_objc3c_native.ps1`
  - `scripts/check_objc3c_runtime_acceptance.py`
  - `scripts/objc3c_public_workflow_runner.py`
  - `scripts/package_objc3c_runnable_toolchain.ps1`
- authoritative live runtime probes:
  - `tests/tooling/runtime/runtime_installation_loader_lifecycle_probe.cpp`
  - `tests/tooling/runtime/live_dispatch_fast_path_probe.cpp`
  - `tests/tooling/runtime/object_model_lookup_reflection_runtime_probe.cpp`
  - `tests/tooling/runtime/arc_debug_instrumentation_probe.cpp`
  - `tests/tooling/runtime/runtime_backed_storage_ownership_reflection_probe.cpp`
- workload manifest and checked-in boundary metadata:
  - `tests/tooling/fixtures/runtime_performance/source_surface.json`
  - `tests/tooling/fixtures/runtime_performance/workload_manifest.json`
  - `tests/tooling/fixtures/runtime_performance/README.md`

## Exact Live Artifact And Output Paths

- machine-owned runtime benchmark roots:
  - `tmp/artifacts/runtime-performance/`
  - `tmp/reports/runtime-performance/`
  - `tmp/pkg/objc3c-native-runnable-toolchain/`
- existing runtime acceptance/report roots consumed by this milestone:
  - `tmp/reports/runtime/acceptance/summary.json`
  - `tmp/reports/objc3c-public-workflow/`
- packaged runnable manifest root:
  - `tmp/pkg/objc3c-native-runnable-toolchain/*/artifacts/package/objc3c-runnable-toolchain-package.json`

## Exact Live Commands

- build the native runtime surface before measuring:
  - `python scripts/objc3c_public_workflow_runner.py build-native-binaries`
  - `npm run build:objc3c-native`
- inspect the live runtime boundary already used by developer tooling:
  - `python scripts/objc3c_public_workflow_runner.py inspect-runtime-inspector`
  - `npm run inspect:objc3c:runtime`
- benchmark the runtime hot-path surface:
  - `python scripts/objc3c_public_workflow_runner.py benchmark-runtime-performance`
  - `npm run inspect:objc3c:runtime-performance`
- validate the integrated runtime-performance surface:
  - `python scripts/objc3c_public_workflow_runner.py validate-runtime-performance`
  - `npm run test:objc3c:runtime-performance`
- validate the staged runnable runtime-performance surface:
  - `python scripts/objc3c_public_workflow_runner.py validate-runnable-runtime-performance`
  - `npm run test:objc3c:runnable-runtime-performance`

## Explicit Non-Goals

- no benchmark-only runtime shim or alternate dispatch entrypoint
- no sidecar-only performance claim without a coupled runtime probe or snapshot
- no hidden optimization toggle that bypasses the checked-in runtime path
- no second public runtime ABI just for performance measurement
- no milestone-local probe copies when an existing runtime probe already covers
  the workload
