# M228 Object Emission and Link Path Reliability Edge-Case and Compatibility Completion Expectations (D005)

Contract ID: `objc3c-object-emission-link-path-reliability-edge-case-and-compatibility-completion/m228-d005-v1`
Status: Accepted
Scope: lane-D object emission/link-path edge-case and compatibility completion guardrails on top of D004 core-feature expansion.

## Objective

Complete lane-D object emission/link-path reliability closure by hardening
edge-case compatibility consistency/readiness and deterministic compatibility
key synthesis so backend route/output drift remains fail-closed.

## Dependency Scope

- Dependencies: `M228-D004`
- M228-D004 core-feature expansion anchors remain mandatory prerequisites:
  - `docs/contracts/m228_object_emission_link_path_reliability_core_feature_expansion_d004_expectations.md`
  - `spec/planning/compiler/m228/m228_d004_object_emission_link_path_reliability_core_feature_expansion_packet.md`
  - `scripts/check_m228_d004_object_emission_link_path_reliability_core_feature_expansion_contract.py`
  - `tests/tooling/test_check_m228_d004_object_emission_link_path_reliability_core_feature_expansion_contract.py`
- Packet/checker/test assets for D005 remain mandatory:
  - `spec/planning/compiler/m228/m228_d005_object_emission_link_path_reliability_edge_case_and_compatibility_completion_packet.md`
  - `scripts/check_m228_d005_object_emission_link_path_reliability_edge_case_and_compatibility_completion_contract.py`
  - `tests/tooling/test_check_m228_d005_object_emission_link_path_reliability_edge_case_and_compatibility_completion_contract.py`

## Deterministic Invariants

1. `Objc3ToolchainRuntimeGaOperationsCoreFeatureSurface` carries explicit D005
   edge-case compatibility closure guardrails:
   - `edge_case_compatibility_consistent`
   - `edge_case_compatibility_ready`
   - `edge_case_compatibility_key`
2. `BuildObjc3ToolchainRuntimeGaOperationsCoreFeatureSurface(...)` computes
   compatibility consistency/readiness deterministically from:
   - backend route compatibility (`clang` / `llvm-direct`) gates
   - backend output marker compatibility gates
   - deterministic compatibility key synthesis via
     `BuildObjc3ToolchainRuntimeGaOperationsEdgeCaseCompatibilityKey(...)`
3. `core_feature_impl_ready` remains fail-closed and now requires:
   - `edge_case_compatibility_ready`
   - non-empty `edge_case_compatibility_key`
4. `frontend_anchor.cpp` keeps lane-D object emission fail-closed through
   `IsObjc3ToolchainRuntimeGaOperationsCoreFeatureSurfaceReady(...)` with
   deterministic diagnostic code `O3E002`.
5. Failure reasons remain explicit for edge-case compatibility inconsistency,
   readiness drift, and compatibility key readiness drift.

## Build and Readiness Integration

- `package.json` includes:
  - `check:objc3c:m228-d005-object-emission-link-path-reliability-edge-case-and-compatibility-completion-contract`
  - `test:tooling:m228-d005-object-emission-link-path-reliability-edge-case-and-compatibility-completion-contract`
  - `check:objc3c:m228-d005-lane-d-readiness`
- lane-D readiness chaining remains deterministic and fail-closed:
  - `check:objc3c:m228-d004-lane-d-readiness`
  - `check:objc3c:m228-d005-lane-d-readiness`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes M228 lane-D D005 compatibility
  completion anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes M228 lane-D D005
  fail-closed compatibility completion wiring text.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-D D005
  compatibility completion metadata anchors.

## Validation

- `python scripts/check_m228_d005_object_emission_link_path_reliability_edge_case_and_compatibility_completion_contract.py`
- `python -m pytest tests/tooling/test_check_m228_d005_object_emission_link_path_reliability_edge_case_and_compatibility_completion_contract.py -q`
- `npm run check:objc3c:m228-d005-lane-d-readiness`
- `npm run check:objc3c:m228-d004-lane-d-readiness && python scripts/check_m228_d005_object_emission_link_path_reliability_edge_case_and_compatibility_completion_contract.py && python -m pytest tests/tooling/test_check_m228_d005_object_emission_link_path_reliability_edge_case_and_compatibility_completion_contract.py -q`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m228/M228-D005/object_emission_link_path_reliability_edge_case_and_compatibility_completion_contract_summary.json`
- `tmp/reports/m228/M228-D005/closeout_validation_report.md`
