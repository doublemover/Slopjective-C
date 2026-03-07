# M242 Interop Behavior for Qualified Generic APIs Edge-case Expansion and Robustness Expectations (D006)

Contract ID: `objc3c-build-runtime-integration-for-preprocessing/m242-d006-v1`
Status: Accepted
Dependencies: `M242-C001`
Scope: M242 lane-D interop behavior for qualified generic APIs edge-case expansion and robustness for nullability, generics, and qualifier completeness interop continuity.

## Objective

Fail closed unless lane-D interop behavior for qualified generic APIs anchors
remain explicit, deterministic, and traceable across code/spec anchors and
milestone optimization improvements as mandatory scope inputs.

## Dependency Scope

- Issue `#6377` defines canonical lane-D edge-case expansion and robustness scope.
- Dependencies: `M242-C001`
- Packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m242/m242_d006_build_runtime_integration_for_preprocessing_edge_case_expansion_and_robustness_packet.md`
  - `scripts/check_m242_d006_build_runtime_integration_for_preprocessing_contract.py`
  - `tests/tooling/test_check_m242_d006_build_runtime_integration_for_preprocessing_contract.py`
- Lane-D D006 freeze remains issue-local and fail-closed against `M242-C001` drift.
- Dependency anchor assets from `M242-C001` remain mandatory:
  - `docs/contracts/m242_expanded_source_lowering_traceability_edge_case_expansion_and_robustness_c001_expectations.md`
  - `scripts/check_m242_c001_expanded_source_lowering_traceability_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit lane-C `M242-C001`
  qualified type lowering and ABI representation anchors that lane-D interop
  freeze checks consume as fail-closed dependency continuity.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-C qualified type
  lowering and ABI representation fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-C
  qualified type lowering and ABI representation metadata anchor wording.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m242-c001-expanded-source-lowering-traceability-contract`.
- `package.json` includes
  `test:tooling:m242-c001-expanded-source-lowering-traceability-contract`.
- `package.json` includes `check:objc3c:m242-c001-lane-c-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `compile:objc3c`.
- `package.json` includes `proof:objc3c`.
- `package.json` includes `test:objc3c:execution-replay-proof`.
- `package.json` includes `test:objc3c:perf-budget`.

## Validation

- `python scripts/check_m242_d006_build_runtime_integration_for_preprocessing_contract.py`
- `python -m pytest tests/tooling/test_check_m242_d006_build_runtime_integration_for_preprocessing_contract.py -q`

## Evidence Path

- `tmp/reports/m242/M242-D006/build_runtime_integration_for_preprocessing_contract_summary.json`






