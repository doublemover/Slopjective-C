# M228 IR Emission Completeness Recovery and Determinism Hardening Expectations (C008)

Contract ID: `objc3c-ir-emission-completeness-recovery-determinism-hardening/m228-c008-v1`
Status: Accepted
Scope: lane-C recovery/determinism hardening closure for IR emission completeness on top of C007 diagnostics hardening.

## Objective

Extend lane-C IR-emission closure with deterministic recovery/determinism
consistency/readiness and recovery-key transport so direct LLVM IR emission
fails closed when replay continuity drifts.

## Dependency Scope

- Dependencies: `M228-C007`
- M228-C007 diagnostics hardening anchors remain mandatory prerequisites:
  - `docs/contracts/m228_ir_emission_completeness_diagnostics_hardening_c007_expectations.md`
  - `scripts/check_m228_c007_ir_emission_completeness_diagnostics_hardening_contract.py`
  - `tests/tooling/test_check_m228_c007_ir_emission_completeness_diagnostics_hardening_contract.py`
  - `spec/planning/compiler/m228/m228_c007_ir_emission_completeness_diagnostics_hardening_packet.md`
- Packet/checker/test assets for C008 remain mandatory:
  - `spec/planning/compiler/m228/m228_c008_ir_emission_completeness_recovery_determinism_hardening_packet.md`
  - `scripts/check_m228_c008_ir_emission_completeness_recovery_determinism_hardening_contract.py`
  - `tests/tooling/test_check_m228_c008_ir_emission_completeness_recovery_determinism_hardening_contract.py`

## Deterministic Invariants

1. `Objc3IREmissionCoreFeatureImplementationSurface` carries explicit C008
   recovery/determinism guardrails:
   - `pass_graph_recovery_determinism_ready`
   - `parse_artifact_recovery_determinism_hardening_consistent`
   - `recovery_determinism_consistent`
   - `recovery_determinism_key_transport_ready`
   - `core_feature_recovery_determinism_ready`
   - `pass_graph_recovery_determinism_key`
   - `parse_artifact_recovery_determinism_hardening_key`
   - `recovery_determinism_key`
2. `BuildObjc3IREmissionCoreFeatureRecoveryDeterminismHardeningKey(...)`
   remains deterministic and keyed by C007 diagnostics hardening closure plus
   pass-graph/parse recovery continuity.
3. `BuildObjc3FrontendArtifacts(...)` remains fail-closed for lane-C C008
   through `IsObjc3IREmissionCoreFeatureRecoveryDeterminismHardeningReady(...)`
   with deterministic diagnostic code `O3L319`.
4. IR metadata transport includes C008 readiness/key anchors:
   - `Objc3IRFrontendMetadata::ir_emission_core_feature_recovery_determinism_ready`
   - `Objc3IRFrontendMetadata::ir_emission_core_feature_recovery_determinism_key`
   - IR text lines:
     - `; ir_emission_core_feature_recovery_determinism = ...`
     - `; ir_emission_core_feature_recovery_determinism_ready = ...`
5. Failure reasons remain explicit for pass-graph recovery drift,
   parse-artifact recovery inconsistency, and recovery-key transport drift.

## Build and Readiness Integration

Shared-file deltas required for full lane-C readiness (not lane-owned scope in
this packet):

- `package.json`
  - add/check `check:objc3c:m228-c008-ir-emission-completeness-recovery-determinism-hardening-contract`
  - add/check `test:tooling:m228-c008-ir-emission-completeness-recovery-determinism-hardening-contract`
  - add/check `check:objc3c:m228-c008-lane-c-readiness` chained from C007 -> C008
- `native/objc3c/src/ARCHITECTURE.md`
  - add M228 lane-C C008 recovery/determinism anchor text
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - add M228 lane-C C008 fail-closed recovery/determinism wiring text
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
  - add deterministic lane-C C008 recovery/determinism metadata anchors

## Validation

- `python scripts/check_m228_c007_ir_emission_completeness_diagnostics_hardening_contract.py`
- `python scripts/check_m228_c008_ir_emission_completeness_recovery_determinism_hardening_contract.py --summary-out tmp/reports/m228/M228-C008/ir_emission_completeness_recovery_determinism_hardening_contract_summary.json`
- `python -m pytest tests/tooling/test_check_m228_c008_ir_emission_completeness_recovery_determinism_hardening_contract.py -q`

## Evidence Path

- `tmp/reports/m228/M228-C008/ir_emission_completeness_recovery_determinism_hardening_contract_summary.json`
- `tmp/reports/m228/M228-C008/closeout_validation_report.md`
