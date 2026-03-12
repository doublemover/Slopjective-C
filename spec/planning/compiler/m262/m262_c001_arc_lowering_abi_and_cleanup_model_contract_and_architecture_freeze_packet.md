# M262-C001 ARC Lowering ABI And Cleanup Model Contract And Architecture Freeze Packet

Packet: `M262-C001`
Milestone: `M262`
Wave: `W54`
Lane: `C`
Issue: `#7199`
Contract ID: `objc3c-arc-lowering-abi-cleanup-model/m262-c001-v1`
Dependencies: None

## Objective

Freeze the current lane-C ARC lowering ABI and cleanup boundary that already exists in the native IR/object path.

## Canonical ARC Lowering Boundary

- contract id `objc3c-arc-lowering-abi-cleanup-model/m262-c001-v1`
- source model `arc-inference-and-interaction-semantic-packets-feed-one-lane-c-helper-call-and-cleanup-boundary`
- abi model `private-runtime-helper-call-boundary-over-retain-release-autorelease-weak-property-and-block-helpers`
- cleanup model `helper-call-plus-autoreleasepool-scope-lowering-without-general-cleanup-stack-or-return-slot-optimization`
- fail-closed model `unsupported-ownership-qualified-signatures-and-generalized-arc-cleanups-remain-fail-closed`
- non-goal model `no-full-arc-automation-no-exception-cleanup-widening-no-objc-runtime-abi-parity-claim`
- emitted IR comment `; arc_lowering_abi_cleanup_model = ...`

## Acceptance Criteria

- Add explicit ARC lowering ABI/cleanup constants in `native/objc3c/src/lower/objc3_lowering_contract.h`.
- Add a deterministic summary helper in `native/objc3c/src/lower/objc3_lowering_contract.cpp`.
- Keep `native/objc3c/src/sema/objc3_semantic_passes.cpp` explicit that sema owns ARC legality and ownership packets, not cleanup scheduling.
- Keep `native/objc3c/src/sema/objc3_sema_pass_manager.cpp` explicit that the ARC handoff preserves semantic packets plus unwind-cleanup accounting only.
- Keep `native/objc3c/src/pipeline/objc3_ownership_aware_lowering_behavior_scaffold.h` explicit that the scaffold is the source-side replay boundary, not the cleanup implementation.
- Have `native/objc3c/src/ir/objc3_ir_emitter.cpp` publish the new boundary comment directly.
- Add deterministic docs/spec/package/checker/test evidence.
- Happy-path native emission over the supported ARC fixtures must still emit non-empty `module.obj` artifacts while retaining the current private helper ABI surface.

## Dynamic Probes

1. Native compile probe over `tests/tooling/fixtures/native/m262_arc_inference_lifetime_positive.objc3` proving emitted IR/object output carries:
   - `; arc_lowering_abi_cleanup_model = ...`
   - `@objc3_runtime_retain_i32`
   - `@objc3_runtime_release_i32`
   - `@objc3_runtime_autorelease_i32`
   - `@__objc3_block_copy_helper_*`
   - `@__objc3_block_dispose_helper_*`
   - successful `module.obj` emission.
2. Native compile probe over `tests/tooling/fixtures/native/m262_arc_property_interaction_positive.objc3` proving emitted IR/object output carries:
   - `; arc_lowering_abi_cleanup_model = ...`
   - `@objc3_runtime_load_weak_current_property_i32`
   - `@objc3_runtime_store_weak_current_property_i32`
   - `@objc3_runtime_autorelease_i32`
   - successful `module.obj` emission.

## Non-Goals

- `M262-C001` does not add generalized ARC cleanup insertion.
- `M262-C001` does not add automatic autorelease-return rewrite automation.
- `M262-C001` does not add public ARC runtime ABI exposure.
- `M262-C001` does not widen unsupported ownership-qualified signatures into the runnable slice.

## Validation Commands

- `python scripts/check_m262_c001_arc_lowering_abi_and_cleanup_model_contract_and_architecture_freeze.py`
- `python -m pytest tests/tooling/test_check_m262_c001_arc_lowering_abi_and_cleanup_model_contract_and_architecture_freeze.py -q`
- `npm run check:objc3c:m262-c001-lane-c-readiness`

## Evidence Path

- `tmp/reports/m262/M262-C001/arc_lowering_abi_cleanup_model_contract_summary.json`
