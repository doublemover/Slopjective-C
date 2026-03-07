# M237 Interop Behavior for Qualified Generic APIs Edge-case Expansion and Robustness Expectations (D006)

Contract ID: `objc3c-block-runtime-abi-wiring-and-linkage/m237-d006-v1`
Status: Accepted
Dependencies: `M237-C001`
Scope: M237 lane-D interop behavior for qualified generic APIs edge-case expansion and robustness for nullability, generics, and qualifier completeness interop continuity.

## Objective

Fail closed unless lane-D interop behavior for qualified generic APIs anchors
remain explicit, deterministic, and traceable across code/spec anchors and
milestone optimization improvements as mandatory scope inputs.

## Dependency Scope

- Issue `#5927` defines canonical lane-D edge-case expansion and robustness scope.
- Dependencies: `M237-C001`
- Packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m237/m237_d006_block_runtime_abi_wiring_and_linkage_edge_case_expansion_and_robustness_packet.md`
  - `scripts/check_m237_d006_block_runtime_abi_wiring_and_linkage_contract.py`
  - `tests/tooling/test_check_m237_d006_block_runtime_abi_wiring_and_linkage_contract.py`
- Lane-D D006 freeze remains issue-local and fail-closed against `M237-C001` drift.
- Dependency anchor assets from `M237-C001` remain mandatory:
  - `docs/contracts/m237_block_lowering_and_invoke_emission_edge_case_expansion_and_robustness_c001_expectations.md`
  - `scripts/check_m237_c001_block_lowering_and_invoke_emission_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit lane-C `M237-C001`
  qualified type lowering and ABI representation anchors that lane-D interop
  freeze checks consume as fail-closed dependency continuity.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-C qualified type
  lowering and ABI representation fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-C
  qualified type lowering and ABI representation metadata anchor wording.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m237-c001-block-lowering-and-invoke-emission-contract`.
- `package.json` includes
  `test:tooling:m237-c001-block-lowering-and-invoke-emission-contract`.
- `package.json` includes `check:objc3c:m237-c001-lane-c-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `compile:objc3c`.
- `package.json` includes `proof:objc3c`.
- `package.json` includes `test:objc3c:execution-replay-proof`.
- `package.json` includes `test:objc3c:perf-budget`.

## Validation

- `python scripts/check_m237_d006_block_runtime_abi_wiring_and_linkage_contract.py`
- `python -m pytest tests/tooling/test_check_m237_d006_block_runtime_abi_wiring_and_linkage_contract.py -q`

## Evidence Path

- `tmp/reports/m237/M237-D006/block_runtime_abi_wiring_and_linkage_contract_summary.json`






