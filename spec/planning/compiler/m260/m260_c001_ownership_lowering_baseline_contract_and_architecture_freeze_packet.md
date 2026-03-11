# M260-C001 Ownership Lowering Baseline Contract And Architecture Freeze Packet

Packet: `M260-C001`
Issue: `#7173`
Milestone: `M260`
Lane: `C`
Wave: `W52`
Dependencies: none
Next issue: `M260-C002`

## Scope

Freeze the truthful lane-C lowering boundary for runtime-backed object ownership.
The native compiler already emits live runtime-backed ownership metadata and
live storage-legality results, but it still represents executable ownership
behavior through the preserved `m161`/`m162`/`m163`/`m164` lowering summaries
rather than live runtime hook calls.

## Acceptance criteria

1. `lower/objc3_lowering_contract.h` declares the canonical contract id, models,
   and summary entrypoint for the ownership lowering baseline.
2. `lower/objc3_lowering_contract.cpp` publishes one deterministic summary that
   preserves the canonical `m161`/`m162`/`m163`/`m164` lowering lanes.
3. `sema/objc3_semantic_passes.cpp` remains explicit that executable ownership
   qualifiers still fail closed semantically while lane-C preserves only the
   legacy lowering summaries.
4. `ir/objc3_ir_emitter.cpp` publishes `; ownership_lowering_baseline = ...`
   directly in emitted IR and keeps the existing ownership lowering profile
   comments/named metadata intact.
5. A live native compile of
   `tests/tooling/fixtures/native/m260_runtime_backed_storage_ownership_legality_positive.objc3`
   proves the baseline summary, canonical lowering replay keys, and absence of
   emitted live ownership runtime hook calls.
6. Validation evidence lands at
   `tmp/reports/m260/M260-C001/ownership_lowering_baseline_contract_summary.json`.

## Canonical models

- Contract id:
  `objc3c-ownership-lowering-baseline-freeze/m260-c001-v1`
- Ownership qualifier model:
  `ownership-qualifier-lowering-remains-legacy-summary-driven-for-runtime-backed-object-metadata`
- Runtime hook model:
  `retain-release-autorelease-and-weak-lowering-stays-summary-only-without-live-runtime-hook-emission`
- Autoreleasepool model:
  `autoreleasepool-lowering-remains-summary-only-without-emitted-push-pop-hooks`
- Failure model:
  `no-live-ownership-runtime-hooks-no-arc-weak-side-table-entrypoints-no-destruction-lowering-yet`

## Canonical proof fixture

- `tests/tooling/fixtures/native/m260_runtime_backed_storage_ownership_legality_positive.objc3`

## Non-goals

- No live retain/release/autorelease runtime hook emission.
- No live autoreleasepool push/pop runtime hook emission.
- No live weak side-table runtime hook emission.
- No destruction-order lowering.
