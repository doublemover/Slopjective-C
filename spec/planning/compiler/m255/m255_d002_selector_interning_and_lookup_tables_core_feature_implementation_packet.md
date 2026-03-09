# M255-D002 Selector Interning and Lookup Tables Core Feature Implementation Packet

Packet: `M255-D002`
Milestone: `M255`
Lane: `D`
Dependencies: `M255-D001`, `M254-D003`

## Purpose

Implement selector interning and lookup tables as a real runtime capability backed by emitted startup selector pools, retained registration tables, and reset/replay.

## Scope Anchors

- Contract:
  `docs/contracts/m255_selector_interning_and_lookup_tables_core_feature_implementation_d002_expectations.md`
- Checker:
  `scripts/check_m255_d002_selector_interning_and_lookup_tables_core_feature_implementation.py`
- Tooling tests:
  `tests/tooling/test_check_m255_d002_selector_interning_and_lookup_tables_core_feature_implementation.py`
- Runtime probe:
  `tests/tooling/runtime/m255_d002_selector_lookup_tables_probe.cpp`
- Lane readiness:
  `scripts/run_m255_d002_lane_d_readiness.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m255-d002-selector-interning-and-lookup-tables-core-feature-implementation`
  - `test:tooling:m255-d002-selector-interning-and-lookup-tables-core-feature-implementation`
  - `check:objc3c:m255-d002-lane-d-readiness`

## Runtime Boundary

- Contract id `objc3c-runtime-selector-lookup-tables/m255-d002-v1`
- Preserved public runtime contract
  `objc3c-runtime-lookup-dispatch-freeze/m255-d001-v1`
- Required bootstrap dependencies:
  - `objc3c-runtime-bootstrap-registrar-image-walk/m254-d002-v1`
  - `objc3c-runtime-bootstrap-reset-replay/m254-d003-v1`
- Required private snapshot symbols:
  - `objc3_runtime_copy_selector_lookup_table_state_for_testing`
  - `objc3_runtime_copy_selector_lookup_entry_for_testing`
- Canonical models:
  - selector interning model
    `registered-selector-pools-materialize-process-global-stable-id-table`
  - merge model
    `per-image-selector-pools-deduplicated-and-merged-across-registration-order`
  - dynamic fallback model
    `unknown-selector-lookups-remain-dynamic-until-m255-d003`
  - replay model
    `reset-replay-rebuilds-metadata-backed-selector-table-in-registration-order`

## Acceptance Checklist

- emitted startup selector pools populate the runtime-owned selector table
- duplicate selector spellings inside one selector pool fail closed
- duplicate selector spellings across registered images merge onto one stable ID
- provider-count and selector-pool provenance remain queryable through the
  private snapshot surface
- reset clears the live selector table
- replay rebuilds the same metadata-backed selector table deterministically
- unknown selector lookup remains available as explicit dynamic fallback until
  `M255-D003`
