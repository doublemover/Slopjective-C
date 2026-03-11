# M260 Retain, Release, Weak, And Unowned Legality For Runtime-Backed Storage Core Feature Implementation Expectations (B002)

Contract ID: `objc3c-runtime-backed-storage-ownership-legality/m260-b002-v1`

Issue: `#7171`

## Objective

Turn the frozen `M260-B001` retainable-object boundary into live semantic
legality for runtime-backed Objective-C object property storage.

## Required implementation

1. Add the issue-local assets:
   - `docs/contracts/m260_retain_release_weak_and_unowned_legality_for_runtime_backed_storage_core_feature_implementation_b002_expectations.md`
   - `spec/planning/compiler/m260/m260_b002_retain_release_weak_and_unowned_legality_for_runtime_backed_storage_core_feature_implementation_packet.md`
   - `scripts/check_m260_b002_retain_release_weak_and_unowned_legality_for_runtime_backed_storage_core_feature_implementation.py`
   - `tests/tooling/test_check_m260_b002_retain_release_weak_and_unowned_legality_for_runtime_backed_storage_core_feature_implementation.py`
   - `scripts/run_m260_b002_lane_b_readiness.py`
2. Add explicit anchors to:
   - `docs/objc3c-native.md`
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
   - `native/objc3c/src/sema/objc3_semantic_passes.cpp`
   - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
   - `native/objc3c/src/lower/objc3_lowering_contract.h`
   - `native/objc3c/src/lower/objc3_lowering_contract.cpp`
   - `package.json`
3. Implement real semantic legality for runtime-backed object properties:
   - explicit `__weak` qualifiers must agree with `weak` storage or no explicit ownership modifier
   - explicit `__unsafe_unretained` qualifiers must agree with `assign` storage or no explicit ownership modifier
   - explicit `__strong` qualifiers must agree with `strong`, `copy`, or no explicit ownership modifier
   - conflicting qualifier/modifier pairs must fail before metadata emission
4. The checker must prove the capability with:
   - one positive compile of `tests/tooling/fixtures/native/m260_runtime_backed_storage_ownership_legality_positive.objc3`
   - one negative compile of `tests/tooling/fixtures/native/m260_runtime_backed_storage_ownership_weak_mismatch_negative.objc3`
   - one negative compile of `tests/tooling/fixtures/native/m260_runtime_backed_storage_ownership_unowned_mismatch_negative.objc3`
5. `package.json` must wire:
   - `check:objc3c:m260-b002-runtime-backed-storage-ownership-legality`
   - `test:tooling:m260-b002-runtime-backed-storage-ownership-legality`
   - `check:objc3c:m260-b002-lane-b-readiness`
6. The contract must explicitly hand off to `M260-B003`.

## Canonical models

- Owned storage model:
  `explicit-strong-object-property-qualifiers-remain-legal-for-owned-runtime-backed-storage-while-conflicting-weak-or-unowned-modifiers-fail-closed`
- Weak/unowned model:
  `explicit-weak-and-unsafe-unretained-object-property-qualifiers-bind-runtime-backed-storage-legality-and-reject-conflicting-property-modifiers`
- Failure model:
  `fail-closed-on-runtime-backed-object-property-ownership-qualifier-modifier-drift`

## Non-goals

- No live ARC retain/release/autorelease execution hooks yet.
- No executable function/method ownership qualifier support yet.
- No runnable `@autoreleasepool` or destruction-order semantics yet.

## Evidence

- `tmp/reports/m260/M260-B002/runtime_backed_storage_ownership_legality_summary.json`
