# M228 Ownership-Aware Lowering Behavior Contract and Architecture Freeze (B001)

Contract ID: `objc3c-ownership-aware-lowering-behavior-freeze/m228-b001-v1`
Status: Accepted
Scope: ownership-aware lowering lane-B freeze anchors for qualifier contracts, retain/release behavior, autoreleasepool scope, ARC diagnostics/fixit replay keys, and architecture/spec references.

## Objective

Freeze ownership-aware lowering behavior before lane-B modular split work so
ownership semantics feeding direct LLVM IR emission remain deterministic,
replay-key stable, and fail-closed.

## Scope Anchors

- `native/objc3c/src/lower/objc3_lowering_contract.h`
- `native/objc3c/src/lower/objc3_lowering_contract.cpp`
- `native/objc3c/src/sema/objc3_sema_contract.h`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `native/objc3c/src/ARCHITECTURE.md`

## Deterministic Invariants

1. Ownership lane contract identifiers remain explicit:
   - `kObjc3OwnershipQualifierLoweringLaneContract`
   - `kObjc3RetainReleaseOperationLoweringLaneContract`
   - `kObjc3AutoreleasePoolScopeLoweringLaneContract`
   - `kObjc3ArcDiagnosticsFixitLoweringLaneContract`
2. Ownership-aware lowering contracts remain structured and validated:
   - `Objc3OwnershipQualifierLoweringContract`
   - `Objc3RetainReleaseOperationLoweringContract`
   - `Objc3AutoreleasePoolScopeLoweringContract`
   - `Objc3ArcDiagnosticsFixitLoweringContract`
   - `IsValidObjc3*LoweringContract(...)` + `Objc3*ReplayKey(...)`
3. Frontend artifacts continue fail-closed contract build/validation wiring:
   - `BuildOwnershipQualifierLoweringContract(...)`
   - `BuildRetainReleaseOperationLoweringContract(...)`
   - `BuildAutoreleasePoolScopeLoweringContract(...)`
   - `BuildArcDiagnosticsFixitLoweringContract(...)`
4. Ownership lane contracts are surfaced into manifest metadata with replay keys.
5. Architecture/spec anchors explicitly mention the M228 lane-B B001 freeze.

## Validation

- `python scripts/check_m228_b001_ownership_aware_lowering_behavior_contract.py`
- `python -m pytest tests/tooling/test_check_m228_b001_ownership_aware_lowering_behavior_contract.py -q`
- `npm run check:objc3c:m228-b001-lane-b-readiness`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m228/M228-B001/ownership_aware_lowering_behavior_contract_summary.json`
