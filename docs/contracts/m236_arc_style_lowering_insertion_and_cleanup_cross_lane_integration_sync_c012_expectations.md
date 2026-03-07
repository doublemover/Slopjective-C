# M236 Qualified Type Lowering and ABI Representation Cross-lane Integration Sync Expectations (C012)

Contract ID: `objc3c-arc-style-lowering-insertion-and-cleanup-contract/m236-c012-v1`
Status: Accepted
Dependencies: none
Scope: M236 lane-C qualified type lowering and ABI representation cross-lane integration sync for nullability, generics, and qualifier completeness.

## Objective

Fail closed unless lane-C qualified type lowering and ABI representation
anchors remain explicit, deterministic, and traceable across code/spec
anchors and milestone optimization improvements as mandatory scope inputs.

## Dependency Scope

- Issue `#5903` defines canonical lane-C cross-lane integration sync scope.
- Dependencies: none
- Packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m236/m236_c012_arc_style_lowering_insertion_and_cleanup_cross_lane_integration_sync_packet.md`
  - `scripts/check_m236_c012_arc_style_lowering_insertion_and_cleanup_contract.py`
  - `tests/tooling/test_check_m236_c012_arc_style_lowering_insertion_and_cleanup_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M236 lane-C C012
  qualified type lowering and ABI representation fail-closed anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-C qualified type
  lowering and ABI representation fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-C
  qualified type lowering and ABI representation metadata anchor wording.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m236-c012-arc-style-lowering-insertion-and-cleanup-contract`.
- `package.json` includes
  `test:tooling:m236-c012-arc-style-lowering-insertion-and-cleanup-contract`.
- `package.json` includes `check:objc3c:m236-c012-lane-c-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `compile:objc3c`.
- `package.json` includes `proof:objc3c`.
- `package.json` includes `test:objc3c:execution-replay-proof`.
- `package.json` includes `test:objc3c:perf-budget`.

## Validation

- `python scripts/check_m236_c012_arc_style_lowering_insertion_and_cleanup_contract.py`
- `python -m pytest tests/tooling/test_check_m236_c012_arc_style_lowering_insertion_and_cleanup_contract.py -q`
- `npm run check:objc3c:m236-c012-lane-c-readiness`

## Evidence Path

- `tmp/reports/m236/M236-C012/arc_style_lowering_insertion_and_cleanup_contract_summary.json`













