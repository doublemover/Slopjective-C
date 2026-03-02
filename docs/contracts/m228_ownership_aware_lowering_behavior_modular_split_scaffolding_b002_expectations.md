# M228 Ownership-Aware Lowering Behavior Modular Split Scaffolding Expectations (B002)

Contract ID: `objc3c-ownership-aware-lowering-behavior-modular-split-scaffolding/m228-b002-v1`
Status: Accepted
Scope: ownership-aware lowering modular split scaffold continuity across pipeline fail-closed emission routing.

## Objective

Freeze the B002 modular split/scaffolding boundary so ownership-aware lowering
contracts remain deterministic and fail-closed before direct LLVM IR emission.

## Scope Anchors

- `native/objc3c/src/pipeline/objc3_ownership_aware_lowering_behavior_scaffold.h`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Deterministic Invariants

1. `Objc3OwnershipAwareLoweringBehaviorScaffold` remains the canonical B002
   modular split scaffold surface for ownership-lowering readiness.
2. `BuildObjc3OwnershipAwareLoweringBehaviorScaffold(...)` remains the only
   canonical scaffold builder for ownership qualifier, retain/release,
   autoreleasepool scope, and ARC diagnostics/fix-it lowering contracts.
3. `IsObjc3OwnershipAwareLoweringBehaviorScaffoldReady(...)` remains fail-closed
   and is enforced from `BuildObjc3FrontendArtifacts(...)` before IR emission.
4. Ownership replay keys continue to include lane-contract suffixes:
   - `kObjc3OwnershipQualifierLoweringLaneContract`
   - `kObjc3RetainReleaseOperationLoweringLaneContract`
   - `kObjc3AutoreleasePoolScopeLoweringLaneContract`
   - `kObjc3ArcDiagnosticsFixitLoweringLaneContract`
5. Architecture/spec anchors explicitly mention the M228 lane-B B002 modular
   split scaffold.

## Validation

- `python scripts/check_m228_b002_ownership_aware_lowering_behavior_modular_split_scaffolding_contract.py`
- `python -m pytest tests/tooling/test_check_m228_b002_ownership_aware_lowering_behavior_modular_split_scaffolding_contract.py -q`
- `npm run check:objc3c:m228-b002-lane-b-readiness`

## Evidence Path

- `tmp/reports/m228/M228-B002/ownership_aware_lowering_behavior_modular_split_scaffolding_contract_summary.json`
