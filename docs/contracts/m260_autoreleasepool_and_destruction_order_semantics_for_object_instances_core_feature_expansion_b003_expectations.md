# M260 Autoreleasepool And Destruction-Order Semantics For Object Instances Core Feature Expansion Expectations (B003)

Contract ID: `objc3c-runtime-backed-autoreleasepool-destruction-order-semantics/m260-b003-v1`

Issue: `#7172`

## Objective

Expand the fail-closed semantic model for runtime-backed object ownership so
`@autoreleasepool` rejection distinguishes plain pool usage from the
ownership-sensitive case where owned runtime-backed object or synthesized
property storage would require destruction-order runtime support.

## Required implementation

1. Add the issue-local assets:
   - `docs/contracts/m260_autoreleasepool_and_destruction_order_semantics_for_object_instances_core_feature_expansion_b003_expectations.md`
   - `spec/planning/compiler/m260/m260_b003_autoreleasepool_and_destruction_order_semantics_for_object_instances_core_feature_expansion_packet.md`
   - `scripts/check_m260_b003_autoreleasepool_and_destruction_order_semantics_for_object_instances_core_feature_expansion.py`
   - `tests/tooling/test_check_m260_b003_autoreleasepool_and_destruction_order_semantics_for_object_instances_core_feature_expansion.py`
   - `scripts/run_m260_b003_lane_b_readiness.py`
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
3. Keep the frozen boundary truthful:
   - plain `@autoreleasepool` still fails closed
   - `@autoreleasepool` plus owned runtime-backed object or synthesized property storage emits an additional destruction-order diagnostic
   - no live autoreleasepool lowering or destruction-order runtime support lands in this issue
4. The checker must prove the capability with:
   - one positive compile of `tests/tooling/fixtures/native/m260_runtime_backed_storage_ownership_legality_positive.objc3`
   - one negative compile of `tests/tooling/fixtures/native/m260_autoreleasepool_owned_storage_destruction_order_negative.objc3`
5. `package.json` must wire:
   - `check:objc3c:m260-b003-autoreleasepool-destruction-order-semantics`
   - `test:tooling:m260-b003-autoreleasepool-destruction-order-semantics`
   - `check:objc3c:m260-b003-lane-b-readiness`
6. The contract must explicitly hand off to `M260-C001`.

## Canonical models

- Autoreleasepool model:
  `autoreleasepool-scopes-remain-fail-closed-while-owned-runtime-backed-object-storage-publishes-destruction-order-edge-diagnostics`
- Destruction model:
  `owned-runtime-backed-object-or-synthesized-property-storage-inside-autoreleasepool-requires-deferred-destruction-order-runtime-support`
- Failure model:
  `fail-closed-on-autoreleasepool-destruction-order-semantic-drift-for-owned-runtime-backed-storage`

## Non-goals

- No live autoreleasepool lowering or runtime support yet.
- No destruction-order runtime remains yet.
- No ARC runtime hook emission yet.

## Evidence

- `tmp/reports/m260/M260-B003/autoreleasepool_destruction_order_semantics_summary.json`
