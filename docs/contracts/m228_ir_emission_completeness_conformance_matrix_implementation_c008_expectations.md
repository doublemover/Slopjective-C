# M228 IR Emission Completeness Conformance Matrix Implementation Expectations (C009)

Contract ID: `objc3c-ir-emission-completeness-conformance-matrix-implementation/m228-c009-v1`
Status: Accepted
Scope: lane-C IR-emission conformance-matrix implementation closure on top of C008 recovery/determinism hardening.

## Objective

Extend lane-C IR-emission closure with explicit conformance-matrix
consistency/readiness and conformance-key continuity so direct LLVM IR
emission fails closed when conformance evidence drifts.

## Dependency Scope

- Dependencies: `M228-C008`
- M228-C008 recovery and determinism hardening anchors remain mandatory
  prerequisites:
  - `docs/contracts/m228_ir_emission_completeness_recovery_determinism_hardening_c008_expectations.md`
  - `scripts/check_m228_c008_ir_emission_completeness_recovery_determinism_hardening_contract.py`
  - `tests/tooling/test_check_m228_c008_ir_emission_completeness_recovery_determinism_hardening_contract.py`
  - `spec/planning/compiler/m228/m228_c008_ir_emission_completeness_recovery_determinism_hardening_packet.md`
- Packet/checker/test assets for C009 remain mandatory:
  - `spec/planning/compiler/m228/m228_c009_ir_emission_completeness_conformance_matrix_implementation_packet.md`
  - `scripts/check_m228_c009_ir_emission_completeness_conformance_matrix_implementation_contract.py`
  - `tests/tooling/test_check_m228_c009_ir_emission_completeness_conformance_matrix_implementation_contract.py`

## Deterministic Invariants

1. `Objc3IREmissionCoreFeatureImplementationSurface` carries explicit C009
   conformance-matrix guardrails:
   - `pass_graph_conformance_matrix_ready`
   - `parse_artifact_conformance_matrix_consistent`
   - `conformance_matrix_consistent`
   - `conformance_matrix_key_transport_ready`
   - `core_feature_conformance_matrix_ready`
   - `pass_graph_conformance_matrix_key`
   - `parse_artifact_conformance_matrix_key`
   - `conformance_matrix_key`
2. `BuildObjc3IREmissionCoreFeatureConformanceMatrixKey(...)` remains
   deterministic and keyed by C008 recovery/determinism closure plus pass-graph
   and parse conformance matrix evidence continuity.
3. `BuildObjc3IREmissionCoreFeatureImplementationSurface(...)` computes
   conformance-matrix fail-closed from recovery/determinism readiness and
   pass-graph/parse conformance consistency plus key transport continuity.
4. `IsObjc3IREmissionCoreFeatureConformanceMatrixReady(...)` fails closed when
   conformance consistency/readiness or conformance-key transport drifts.
5. `BuildObjc3FrontendArtifacts(...)` remains fail-closed for lane-C C009
   through `IsObjc3IREmissionCoreFeatureConformanceMatrixReady(...)` with
   deterministic diagnostic code `O3L320`.
6. IR metadata transport includes C009 readiness/key anchors:
   - `Objc3IRFrontendMetadata::ir_emission_core_feature_conformance_matrix_ready`
   - `Objc3IRFrontendMetadata::ir_emission_core_feature_conformance_matrix_key`
   - IR text lines:
     - `; ir_emission_core_feature_conformance_matrix = ...`
     - `; ir_emission_core_feature_conformance_matrix_ready = ...`

## Build and Readiness Integration

Shared-file deltas required for full lane-C readiness (not lane-owned scope in
this packet):

- `package.json`
  - add/check `check:objc3c:m228-c009-ir-emission-completeness-conformance-matrix-implementation-contract`
  - add/check `test:tooling:m228-c009-ir-emission-completeness-conformance-matrix-implementation-contract`
  - add/check `check:objc3c:m228-c009-lane-c-readiness` chained from C008 -> C009
- `native/objc3c/src/ARCHITECTURE.md`
  - add M228 lane-C C009 conformance matrix implementation anchor text
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - add M228 lane-C C009 fail-closed conformance-matrix wiring text
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
  - add deterministic lane-C C009 conformance-matrix metadata anchors

## Validation

- `python scripts/check_m228_c008_ir_emission_completeness_recovery_determinism_hardening_contract.py`
- `python scripts/check_m228_c009_ir_emission_completeness_conformance_matrix_implementation_contract.py --summary-out tmp/reports/m228/M228-C009/ir_emission_completeness_conformance_matrix_implementation_contract_summary.json`
- `python -m pytest tests/tooling/test_check_m228_c009_ir_emission_completeness_conformance_matrix_implementation_contract.py -q`
- `npm run check:objc3c:m228-c009-lane-c-readiness`

## Evidence Path

- `tmp/reports/m228/M228-C009/ir_emission_completeness_conformance_matrix_implementation_contract_summary.json`
- `tmp/reports/m228/M228-C009/closeout_validation_report.md`
