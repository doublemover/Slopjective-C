# M228 IR Emission Completeness Core Feature Expansion Expectations (C004)

Contract ID: `objc3c-ir-emission-completeness-core-feature-expansion/m228-c004-v1`
Status: Accepted
Scope: lane-C core feature expansion for IR emission completeness over direct LLVM IR emission hardening, with explicit `M228-C003` dependency governance.

## Objective

Extend C003 core-feature implementation closure with deterministic expansion
readiness/key transport so direct LLVM IR emission fails closed when pass-graph
expansion continuity, metadata transport, boundary handoff, or entrypoint
readiness drifts.
Code/spec anchors and milestone optimization improvements are mandatory scope
inputs for this closure.

## Required Invariants

1. Dedicated core-feature expansion surface exists:
   - `Objc3IREmissionCoreFeatureImplementationSurface`
   - `BuildObjc3IREmissionCoreFeatureExpansionKey(...)`
   - `IsObjc3IREmissionCoreFeatureExpansionReady(...)`
2. Core-feature expansion surface is synthesized deterministically from
   `Objc3FrontendPipelineResult`:
   - `BuildObjc3IREmissionCoreFeatureImplementationSurface(...)` computes
     expansion readiness from pass-graph expansion readiness,
     expansion metadata transport, boundary handoff, and IR entrypoint state.
   - `BuildObjc3FrontendArtifacts(...)` evaluates expansion readiness before emit.
3. Frontend artifacts enforce fail-closed C004 gate:
   - `IsObjc3IREmissionCoreFeatureExpansionReady(...)`
   - deterministic diagnostic code `O3L314`.
4. IR metadata includes core-feature expansion readiness/key transport:
   - `Objc3IRFrontendMetadata::ir_emission_core_feature_expansion_ready`
   - `Objc3IRFrontendMetadata::ir_emission_core_feature_expansion_key`
   - IR text lines:
     - `; ir_emission_core_feature_expansion = ...`
     - `; ir_emission_core_feature_expansion_ready = ...`
5. C003 remains a mandatory prerequisite:
   - `docs/contracts/m228_ir_emission_completeness_core_feature_implementation_c003_expectations.md`
   - `scripts/check_m228_c003_ir_emission_completeness_core_feature_implementation_contract.py`
   - `tests/tooling/test_check_m228_c003_ir_emission_completeness_core_feature_implementation_contract.py`
6. C004 planning/checker/test anchors remain mandatory:
   - `spec/planning/compiler/m228/m228_c004_ir_emission_completeness_core_feature_expansion_packet.md`
   - `scripts/check_m228_c004_ir_emission_completeness_core_feature_expansion_contract.py`
   - `tests/tooling/test_check_m228_c004_ir_emission_completeness_core_feature_expansion_contract.py`
7. Build/readiness wiring remains deterministic and fail-closed:
   - `check:objc3c:m228-c004-ir-emission-completeness-core-feature-expansion-contract`
   - `test:tooling:m228-c004-ir-emission-completeness-core-feature-expansion-contract`
   - `check:objc3c:m228-c004-lane-c-readiness`
8. Architecture/spec anchors include C004 core-feature expansion scope.

## Validation

- `python scripts/check_m228_c004_ir_emission_completeness_core_feature_expansion_contract.py`
- `python -m pytest tests/tooling/test_check_m228_c004_ir_emission_completeness_core_feature_expansion_contract.py -q`
- `npm run check:objc3c:m228-c004-lane-c-readiness`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m228/M228-C004/ir_emission_completeness_core_feature_expansion_contract_summary.json`
- `tmp/reports/m228/M228-C004/closeout_validation_report.md`
