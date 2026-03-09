# M255-D004 Protocol and Category-Aware Method Resolution Core Feature Expansion Packet

Packet: `M255-D004`
Milestone: `M255`
Lane: `D`
Dependencies: `M255-D001`, `M255-D002`, `M255-D003`, `M253-C003`

## Purpose

Implement the next live runtime lookup tier after class/metaclass bodies so
category implementation records provide the next live dispatch tier, and
preserve fail-closed negative lookup evidence by consuming adopted and
inherited protocol method graphs.

## Scope Anchors

- Contract:
  `docs/contracts/m255_protocol_and_category_aware_method_resolution_core_feature_expansion_d004_expectations.md`
- Checker:
  `scripts/check_m255_d004_protocol_and_category_aware_method_resolution_core_feature_expansion.py`
- Tooling tests:
  `tests/tooling/test_check_m255_d004_protocol_and_category_aware_method_resolution_core_feature_expansion.py`
- Runtime probe:
  `tests/tooling/runtime/m255_d004_protocol_category_probe.cpp`
- Fixture:
  `tests/tooling/fixtures/native/m255_d004_protocol_category_dispatch.objc3`
- Lane readiness:
  `scripts/run_m255_d004_lane_d_readiness.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m255-d004-protocol-and-category-aware-method-resolution-core-feature-expansion`
  - `test:tooling:m255-d004-protocol-and-category-aware-method-resolution-core-feature-expansion`
  - `check:objc3c:m255-d004-lane-d-readiness`

## Runtime Boundary

- Contract id `objc3c-runtime-protocol-category-method-resolution/m255-d004-v1`
- Preserved public/runtime contracts:
  - `objc3c-runtime-lookup-dispatch-freeze/m255-d001-v1`
  - `objc3c-runtime-selector-lookup-tables/m255-d002-v1`
  - `objc3c-runtime-method-cache-slow-path-lookup/m255-d003-v1`
- Canonical models:
  - category resolution model
    `class-bodies-win-first-category-implementation-records-supply-next-live-method-tier`
  - protocol declaration model
    `adopted-and-inherited-protocol-method-lists-provide-declaration-aware-negative-resolution`
  - fallback model
    `conflicting-category-or-protocol-resolution-fails-closed-to-compatibility-dispatch`

## Acceptance Checklist

- preferred category implementation records resolve live instance methods after
  class-body lookup misses
- preferred category implementation records resolve live class methods after
  metaclass-body lookup misses
- protocol declaration graphs contribute explicit negative lookup evidence
  without being treated as callable bodies
- method-cache snapshots expose category/protocol probe counts for both live and
  negative cache entries
- repeat positive and negative dispatches reuse cached category/protocol probe
  evidence
- emitted protocol/category method-list ref counts match the actual emitted
  method-table header counts
- IR publishes the D004 contract comment and object output preserves populated
  protocol/category descriptor sections
