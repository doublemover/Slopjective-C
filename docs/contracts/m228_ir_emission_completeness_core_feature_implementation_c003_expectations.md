# M228 IR Emission Completeness Core Feature Implementation Expectations (C003)

Contract ID: `objc3c-ir-emission-completeness-core-feature-implementation/m228-c003-v1`
Status: Accepted
Scope: lane-C core feature implementation for IR emission completeness over direct LLVM IR emission hardening.

## Objective

Establish an explicit IR-emission core-feature implementation surface that fails
closed when modular split readiness, metadata transport, boundary handoff, or
direct IR entrypoint readiness drifts.

## Required Invariants

1. Dedicated core-feature implementation surface exists:
   - `Objc3IREmissionCoreFeatureImplementationSurface`
   - `BuildObjc3IREmissionCoreFeatureImplementationSurface(...)`
   - `IsObjc3IREmissionCoreFeatureImplementationReady(...)`
2. Core-feature implementation surface is synthesized deterministically from
   `Objc3FrontendPipelineResult`:
   - `BuildObjc3IREmissionCoreFeatureImplementationSurface(...)` consumes
     modular-split scaffold, boundary replay handoff, and IR entrypoint state.
   - `BuildObjc3FrontendArtifacts(...)` evaluates this surface before emit.
3. Frontend artifacts enforce fail-closed C003 gate:
   - `IsObjc3IREmissionCoreFeatureImplementationReady(...)`
   - deterministic diagnostic code `O3L306`.
4. IR metadata includes core-feature implementation readiness/key transport:
   - `Objc3IRFrontendMetadata::ir_emission_core_feature_impl_ready`
   - `Objc3IRFrontendMetadata::ir_emission_core_feature_impl_key`
   - IR text lines:
     - `; ir_emission_core_feature_impl = ...`
     - `; ir_emission_core_feature_impl_ready = ...`
5. Architecture/spec anchors include C003 core-feature implementation scope.

## Validation

- `python scripts/check_m228_c003_ir_emission_completeness_core_feature_implementation_contract.py`
- `python -m pytest tests/tooling/test_check_m228_c003_ir_emission_completeness_core_feature_implementation_contract.py -q`
- `npm run check:objc3c:m228-c003-lane-c-readiness`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m228/M228-C003/ir_emission_completeness_core_feature_implementation_contract_summary.json`
