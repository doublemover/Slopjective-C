# M262 ARC Lowering ABI And Cleanup Model Contract And Architecture Freeze Expectations (C001)

Contract ID: `objc3c-arc-lowering-abi-cleanup-model/m262-c001-v1`
Status: Accepted
Issue: `#7199`
Scope: M262 lane-C freeze of the current ARC lowering ABI and cleanup boundary for the supported runnable slice.

## Objective

Freeze the current ARC lowering ABI and cleanup boundary that lane C already depends on. The compiler currently publishes ARC semantic packets, unwind-cleanup accounting, and private runtime helper entrypoints into emitted IR, but it does not yet perform generalized ARC cleanup insertion or broader ARC automation. This issue makes that boundary explicit so later implementation can widen it deliberately.

## Required Invariants

1. `native/objc3c/src/lower/objc3_lowering_contract.h` remains the canonical declaration point for:
   - `objc3c-arc-lowering-abi-cleanup-model/m262-c001-v1`
   - abi model `private-runtime-helper-call-boundary-over-retain-release-autorelease-weak-property-and-block-helpers`
   - cleanup model `helper-call-plus-autoreleasepool-scope-lowering-without-general-cleanup-stack-or-return-slot-optimization`
   - fail-closed model `unsupported-ownership-qualified-signatures-and-generalized-arc-cleanups-remain-fail-closed`
   - non-goal model `no-full-arc-automation-no-exception-cleanup-widening-no-objc-runtime-abi-parity-claim`.
2. `native/objc3c/src/lower/objc3_lowering_contract.cpp` publishes one deterministic summary for the current ARC lowering ABI and cleanup boundary.
3. `native/objc3c/src/sema/objc3_semantic_passes.cpp` remains explicit that sema owns ARC legality, inferred lifetime packets, synthesized property ownership packets, and interaction semantics, but not lowering helper placement or cleanup scheduling.
4. `native/objc3c/src/sema/objc3_sema_pass_manager.cpp` remains explicit that the ARC handoff preserves semantic packets and unwind-cleanup accounting without claiming a completed cleanup-insertion pipeline.
5. `native/objc3c/src/pipeline/objc3_ownership_aware_lowering_behavior_scaffold.h` remains explicit that the scaffold is the source-side ownership/ARC replay surface and not the cleanup implementation itself.
6. `native/objc3c/src/ir/objc3_ir_emitter.cpp` publishes the boundary directly into emitted IR through `; arc_lowering_abi_cleanup_model = ...`.
7. The current helper boundary remains private/runtime-internal and explicitly names:
   - `objc3_runtime_retain_i32`
   - `objc3_runtime_release_i32`
   - `objc3_runtime_autorelease_i32`
   - `objc3_runtime_load_weak_current_property_i32`
   - `objc3_runtime_store_weak_current_property_i32`
   - `objc3_runtime_push_autoreleasepool_scope`
   - `objc3_runtime_pop_autoreleasepool_scope`.

## Dynamic Coverage

1. Native compile probe over `tests/tooling/fixtures/native/m262_arc_inference_lifetime_positive.objc3` proves emitted IR carries the new boundary line, retains the private retain/release/autorelease helper ABI, and keeps block copy/dispose helper ownership calls inside a non-empty `module.obj`.
2. Native compile probe over `tests/tooling/fixtures/native/m262_arc_property_interaction_positive.objc3` proves emitted IR carries the same boundary line and retains the weak property plus autorelease helper ABI inside a non-empty `module.obj`.

## Non-Goals And Fail-Closed Rules

- `M262-C001` does not introduce generalized ARC cleanup insertion.
- `M262-C001` does not introduce automatic autorelease-return rewrite automation.
- `M262-C001` does not widen unsupported ownership-qualified signature spellings into the runnable native slice.
- `M262-C001` does not claim public ARC runtime ABI stability.
- If the ARC lowering boundary drifts, later lane-C implementation must fail closed rather than silently widening helper placement or cleanup scope behavior.

## Architecture And Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md`
- `docs/objc3c-native.md`

## Build And Readiness Integration

- `package.json` includes `check:objc3c:m262-c001-arc-lowering-abi-and-cleanup-model-contract`.
- `package.json` includes `test:tooling:m262-c001-arc-lowering-abi-and-cleanup-model-contract`.
- `package.json` includes `check:objc3c:m262-c001-lane-c-readiness`.

## Validation

- `python scripts/check_m262_c001_arc_lowering_abi_and_cleanup_model_contract_and_architecture_freeze.py`
- `python -m pytest tests/tooling/test_check_m262_c001_arc_lowering_abi_and_cleanup_model_contract_and_architecture_freeze.py -q`
- `npm run check:objc3c:m262-c001-lane-c-readiness`

## Evidence Path

- `tmp/reports/m262/M262-C001/arc_lowering_abi_cleanup_model_contract_summary.json`
