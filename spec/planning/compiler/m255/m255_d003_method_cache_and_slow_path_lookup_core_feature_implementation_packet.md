# M255-D003 Method Cache and Slow-Path Lookup Core Feature Implementation Packet

Packet: `M255-D003`
Milestone: `M255`
Lane: `D`
Dependencies: `M255-D002`, `M255-C004`

## Purpose

Implement live method-cache and slow-path lookup behavior in the runtime so
normalized instance/class dispatch can resolve callable emitted method bodies
from registered class/metaclass metadata.

## Scope Anchors

- Contract:
  `docs/contracts/m255_method_cache_and_slow_path_lookup_core_feature_implementation_d003_expectations.md`
- Checker:
  `scripts/check_m255_d003_method_cache_and_slow_path_lookup_core_feature_implementation.py`
- Tooling tests:
  `tests/tooling/test_check_m255_d003_method_cache_and_slow_path_lookup_core_feature_implementation.py`
- Runtime probe:
  `tests/tooling/runtime/m255_d003_method_cache_slow_path_probe.cpp`
- Fixture:
  `tests/tooling/fixtures/native/m255_d003_live_method_dispatch.objc3`
- Lane readiness:
  `scripts/run_m255_d003_lane_d_readiness.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m255-d003-method-cache-and-slow-path-lookup-core-feature-implementation`
  - `test:tooling:m255-d003-method-cache-and-slow-path-lookup-core-feature-implementation`
  - `check:objc3c:m255-d003-lane-d-readiness`

## Runtime Boundary

- Contract id `objc3c-runtime-method-cache-slow-path-lookup/m255-d003-v1`
- Preserved public runtime contract
  `objc3c-runtime-lookup-dispatch-freeze/m255-d001-v1`
- Preserved selector-table dependency
  `objc3c-runtime-selector-lookup-tables/m255-d002-v1`
- Required private snapshot symbols:
  - `objc3_runtime_copy_method_cache_state_for_testing`
  - `objc3_runtime_copy_method_cache_entry_for_testing`
- Canonical models:
  - receiver normalization model
    `known-class-and-class-self-receivers-normalize-to-one-metaclass-cache-key`
  - slow-path resolution model
    `registered-class-and-metaclass-records-drive-deterministic-slow-path-method-resolution`
  - cache model
    `normalized-receiver-plus-selector-stable-id-positive-and-negative-cache`
  - fallback model
    `unsupported-or-ambiguous-runtime-resolution-falls-back-to-compatibility-dispatch-formula`

## Acceptance Checklist

- emitted class/metaclass method tables carry callable implementation pointers
- the runtime resolves a live instance method body on first dispatch
- the runtime resolves a live class method body on first dispatch
- class-self and known-class receivers normalize onto one metaclass cache key
- positive cache entries are reused for repeat live dispatch
- unresolved selectors populate negative cache entries
- unresolved selectors reuse negative cache entries while preserving fallback
  results
- emitted IR/object artifacts expose the live D003 slow-path metadata anchors
