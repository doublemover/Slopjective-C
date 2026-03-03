# M228 Object Emission and Link Path Reliability Conformance Matrix Implementation Expectations (D009)

Contract ID: `objc3c-object-emission-link-path-reliability-conformance-matrix-implementation/m228-d009-v1`
Status: Accepted
Scope: lane-D object emission/link-path conformance matrix implementation guardrails on top of D008 recovery and determinism hardening closure.

## Objective

Expand lane-D object emission/link-path reliability closure by hardening
conformance matrix consistency/readiness and deterministic conformance-matrix
key-readiness validation so backend route/output conformance drift remains
fail-closed.

## Dependency Scope

- Dependencies: `M228-D008`
- M228-D008 recovery and determinism hardening anchors remain mandatory prerequisites:
  - `docs/contracts/m228_object_emission_link_path_reliability_recovery_determinism_hardening_d008_expectations.md`
  - `spec/planning/compiler/m228/m228_d008_object_emission_link_path_reliability_recovery_determinism_hardening_packet.md`
  - `scripts/check_m228_d008_object_emission_link_path_reliability_recovery_determinism_hardening_contract.py`
  - `tests/tooling/test_check_m228_d008_object_emission_link_path_reliability_recovery_determinism_hardening_contract.py`
- Packet/checker/test assets for D009 remain mandatory:
  - `spec/planning/compiler/m228/m228_d009_object_emission_link_path_reliability_conformance_matrix_implementation_packet.md`
  - `scripts/check_m228_d009_object_emission_link_path_reliability_conformance_matrix_implementation_contract.py`
  - `tests/tooling/test_check_m228_d009_object_emission_link_path_reliability_conformance_matrix_implementation_contract.py`

## Deterministic Invariants

1. `Objc3ToolchainRuntimeGaOperationsCoreFeatureSurface` carries explicit D009
   conformance matrix closure guardrails:
   - `conformance_matrix_consistent`
   - `conformance_matrix_ready`
   - `conformance_matrix_key_ready`
   - `conformance_matrix_key`
2. `BuildObjc3ToolchainRuntimeGaOperationsConformanceMatrixKey(...)` remains
   deterministic and includes backend route/output identity plus D008
   recovery-determinism key-readiness continuity.
3. `BuildObjc3ToolchainRuntimeGaOperationsCoreFeatureSurface(...)` computes
   conformance-matrix consistency/readiness deterministically from:
   - D008 recovery/determinism consistency/readiness/key-readiness closure
   - deterministic backend route/output dispatch and path determinism signals
   - deterministic conformance-matrix key synthesis
4. `core_feature_impl_ready` remains fail-closed and now requires:
   - `conformance_matrix_ready`
   - `conformance_matrix_key_ready`
5. `IsObjc3ToolchainRuntimeGaOperationsConformanceMatrixReady(...)` provides
   explicit fail-closed conformance-matrix readiness reasoning.
6. Failure reasons remain explicit for conformance-matrix inconsistency,
   readiness drift, and key-readiness drift.

## Build and Readiness Integration

- Shared-file deltas required for full lane-D readiness (not lane-owned scope in
  this packet):
  - `package.json`
    - add `check:objc3c:m228-d009-object-emission-link-path-reliability-conformance-matrix-implementation-contract`
    - add `test:tooling:m228-d009-object-emission-link-path-reliability-conformance-matrix-implementation-contract`
    - add `check:objc3c:m228-d009-lane-d-readiness` chained from D008 -> D009
  - `native/objc3c/src/ARCHITECTURE.md`
    - add M228 lane-D D009 conformance matrix anchor text
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
    - add M228 lane-D D009 fail-closed conformance-matrix wiring text
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
    - add deterministic lane-D D009 conformance-matrix metadata anchors

## Validation

- `python scripts/check_m228_d008_object_emission_link_path_reliability_recovery_determinism_hardening_contract.py`
- `python scripts/check_m228_d009_object_emission_link_path_reliability_conformance_matrix_implementation_contract.py --summary-out tmp/reports/m228/M228-D009/object_emission_link_path_reliability_conformance_matrix_implementation_contract_summary.json`
- `python -m pytest tests/tooling/test_check_m228_d009_object_emission_link_path_reliability_conformance_matrix_implementation_contract.py -q`

## Evidence Path

- `tmp/reports/m228/M228-D009/object_emission_link_path_reliability_conformance_matrix_implementation_contract_summary.json`
- `tmp/reports/m228/M228-D009/closeout_validation_report.md`
