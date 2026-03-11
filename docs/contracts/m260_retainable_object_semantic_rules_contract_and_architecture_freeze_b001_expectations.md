# M260 Retainable Object Semantic Rules Contract And Architecture Freeze Expectations (B001)

Contract ID: `objc3c-retainable-object-semantic-rules-freeze/m260-b001-v1`

Issue: `#7170`

## Objective

Freeze the truthful semantic boundary for retain, release, weak, unowned,
autoreleasepool, and destruction ordering as they currently apply to
runtime-backed objects.

## Required implementation

1. Add the issue-local assets:
   - `docs/contracts/m260_retainable_object_semantic_rules_contract_and_architecture_freeze_b001_expectations.md`
   - `spec/planning/compiler/m260/m260_b001_retainable_object_semantic_rules_contract_and_architecture_freeze_packet.md`
   - `scripts/check_m260_b001_retainable_object_semantic_rules_contract_and_architecture_freeze.py`
   - `tests/tooling/test_check_m260_b001_retainable_object_semantic_rules_contract_and_architecture_freeze.py`
   - `scripts/run_m260_b001_lane_b_readiness.py`
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
3. Freeze the truthful semantic rules around:
   - runtime-backed property/member ownership metadata being live
   - retain/release legality remaining summary-driven
   - weak/unowned legality remaining summary-driven
   - `@autoreleasepool` remaining non-runnable
   - destruction ordering remaining deferred
4. The checker must prove the frozen boundary with one live compile of:
   - `tests/tooling/fixtures/native/m260_runtime_backed_object_ownership_attribute_surface_positive.objc3`
   It must fail closed unless the manifest/IR still show live runtime-backed
   ownership metadata while retain/release and autoreleasepool execution remain
   zero-site legacy lowering truths.
5. `package.json` must wire:
   - `check:objc3c:m260-b001-retainable-object-semantic-rules`
   - `test:tooling:m260-b001-retainable-object-semantic-rules`
   - `check:objc3c:m260-b001-lane-b-readiness`
6. The contract must explicitly hand off to `M260-B002`.

## Canonical models

- Semantic model:
  `runtime-backed-object-semantic-rules-freeze-property-member-ownership-metadata-while-retain-release-and-storage-legality-remain-summary-driven`
- Destruction model:
  `destruction-order-autoreleasepool-and-live-arc-execution-stay-fail-closed-outside-runtime-backed-storage-legality`
- Failure model:
  `fail-closed-on-retainable-object-semantic-drift-or-premature-live-storage-legality-claim`

## Non-goals

- No live ARC retain/release/autorelease execution semantics yet.
- No executable function/method ownership qualifier support yet.
- No runnable destruction-order semantics yet.

## Evidence

- `tmp/reports/m260/M260-B001/retainable_object_semantic_rules_contract_summary.json`
