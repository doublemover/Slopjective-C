# M236 Interop Behavior for Qualified Generic APIs Edge-case Expansion and Robustness Expectations (D006)

Contract ID: `objc3c-runtime-ownership-abi-and-interoperability/m236-d006-v1`
Status: Accepted
Dependencies: `M236-C001`
Scope: M236 lane-D interop behavior for qualified generic APIs edge-case expansion and robustness for nullability, generics, and qualifier completeness interop continuity.

## Objective

Fail closed unless lane-D interop behavior for qualified generic APIs anchors
remain explicit, deterministic, and traceable across code/spec anchors and
milestone optimization improvements as mandatory scope inputs.

## Dependency Scope

- Issue `#5932` defines canonical lane-D edge-case expansion and robustness scope.
- Dependencies: `M236-C001`
- Packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m236/m236_d006_runtime_ownership_abi_and_interoperability_edge_case_expansion_and_robustness_packet.md`
  - `scripts/check_m236_d006_runtime_ownership_abi_and_interoperability_contract.py`
  - `tests/tooling/test_check_m236_d006_runtime_ownership_abi_and_interoperability_contract.py`
- Lane-D D006 freeze remains issue-local and fail-closed against `M236-C001` drift.
- Dependency anchor assets from `M236-C001` remain mandatory:
  - `docs/contracts/m236_arc_style_lowering_insertion_and_cleanup_edge_case_expansion_and_robustness_c001_expectations.md`
  - `scripts/check_m236_c001_arc_style_lowering_insertion_and_cleanup_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit lane-C `M236-C001`
  qualified type lowering and ABI representation anchors that lane-D interop
  freeze checks consume as fail-closed dependency continuity.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-C qualified type
  lowering and ABI representation fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-C
  qualified type lowering and ABI representation metadata anchor wording.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m236-c001-arc-style-lowering-insertion-and-cleanup-contract`.
- `package.json` includes
  `test:tooling:m236-c001-arc-style-lowering-insertion-and-cleanup-contract`.
- `package.json` includes `check:objc3c:m236-c001-lane-c-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `compile:objc3c`.
- `package.json` includes `proof:objc3c`.
- `package.json` includes `test:objc3c:execution-replay-proof`.
- `package.json` includes `test:objc3c:perf-budget`.

## Validation

- `python scripts/check_m236_d006_runtime_ownership_abi_and_interoperability_contract.py`
- `python -m pytest tests/tooling/test_check_m236_d006_runtime_ownership_abi_and_interoperability_contract.py -q`

## Evidence Path

- `tmp/reports/m236/M236-D006/runtime_ownership_abi_and_interoperability_contract_summary.json`






