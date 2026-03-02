# M228 Object Emission and Link Path Reliability Core Feature Expansion Expectations (D004)

Contract ID: `objc3c-object-emission-link-path-reliability-core-feature-expansion/m228-d004-v1`
Status: Accepted
Scope: lane-D object emission/link-path core-feature expansion guardrails on top of D003.

## Objective

Expand lane-D object emission/link-path reliability closure with explicit backend
marker path/payload determinism and expansion readiness/key synthesis so backend
object dispatch remains deterministic and fail-closed when marker artifacts drift.

## Dependency Scope

- Dependencies: `M228-D003`
- M228-D003 core-feature implementation anchors remain mandatory prerequisites:
  - `docs/contracts/m228_object_emission_link_path_reliability_core_feature_implementation_d003_expectations.md`
  - `spec/planning/compiler/m228/m228_d003_object_emission_link_path_reliability_core_feature_implementation_packet.md`
  - `scripts/check_m228_d003_object_emission_link_path_reliability_core_feature_implementation_contract.py`
  - `tests/tooling/test_check_m228_d003_object_emission_link_path_reliability_core_feature_implementation_contract.py`
- Packet/checker/test assets for D004 remain mandatory:
  - `spec/planning/compiler/m228/m228_d004_object_emission_link_path_reliability_core_feature_expansion_packet.md`
  - `scripts/check_m228_d004_object_emission_link_path_reliability_core_feature_expansion_contract.py`
  - `tests/tooling/test_check_m228_d004_object_emission_link_path_reliability_core_feature_expansion_contract.py`

## Deterministic Invariants

1. `Objc3ToolchainRuntimeGaOperationsCoreFeatureSurface` carries explicit D004
   expansion guardrails:
   - `backend_output_path_deterministic`
   - `backend_output_payload_consistent`
   - `core_feature_expansion_ready`
   - `core_feature_expansion_key`
2. `BuildObjc3ToolchainRuntimeGaOperationsCoreFeatureSurface(...)` remains the
   canonical lane-D builder and computes expansion readiness from:
   - `BuildObjc3ToolchainRuntimeGaOperationsCoreFeatureExpansionKey(...)`
   - backend marker path suffix determinism (`.object-backend.txt`)
   - backend marker payload consistency with backend route (`clang` / `llvm-direct`)
3. `frontend_anchor.cpp` wires deterministic backend marker path/payload inputs
   into expansion checks before object emission success is accepted:
   - `backend_out`
   - `backend_output_payload`
   - `BuildObjc3ToolchainRuntimeGaOperationsCoreFeatureSurface(...)`
4. D004 remains fail-closed through
   `IsObjc3ToolchainRuntimeGaOperationsCoreFeatureSurfaceReady(...)` with
   deterministic diagnostic code `O3E002`.
5. Shared architecture/spec anchors explicitly mention M228 lane-D D004 closure:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Validation

- `python scripts/check_m228_d004_object_emission_link_path_reliability_core_feature_expansion_contract.py`
- `python -m pytest tests/tooling/test_check_m228_d004_object_emission_link_path_reliability_core_feature_expansion_contract.py -q`
- `npm run check:objc3c:m228-d004-lane-d-readiness`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m228/M228-D004/object_emission_link_path_reliability_core_feature_expansion_contract_summary.json`
- `tmp/reports/m228/M228-D004/closeout_validation_report.md`
