# M262 ARC Lowering ABI And Cleanup Model Contract And Architecture Freeze Expectations (C001)

Contract ID: `objc3c-arc-lowering-abi-cleanup-model/m262-c001-v1`
Status: Accepted
Issue: `#7199`
Scope: M262 lane-C freeze of the current ARC lowering ABI and cleanup boundary that already exists in the native pipeline.

## Objective

Freeze the current ARC lowering ABI/cleanup boundary so later retain/release insertion, cleanup emission, weak lowering, and autorelease-return work extend one explicit contract instead of inferring it from older semantic packets.

## Required Invariants

1. `lower/objc3_lowering_contract.h` remains the canonical declaration point for:
   - `objc3c-arc-lowering-abi-cleanup-model/m262-c001-v1`
   - ABI model `owned-value-lowering-targets-private-runtime-retain-release-autorelease-and-weak-helper-entrypoints-without-public-runtime-abi-expansion`
   - cleanup model `cleanup-scheduling-remains-explicit-summary-and-helper-boundary-only-until-m262-c002`
   - fail-closed model `no-implicit-cleanup-scope-insertion-no-helper-rebinding-no-public-runtime-abi-widening-before-later-lane-c-runtime-work`
2. `lower/objc3_lowering_contract.cpp` publishes one deterministic summary for the ARC lowering ABI/cleanup boundary.
3. `sema/objc3_semantic_passes.cpp`, `sema/objc3_sema_pass_manager.cpp`, and `pipeline/objc3_ownership_aware_lowering_behavior_scaffold.h` remain explicit that sema/scaffold own semantic ARC packets only and do not claim cleanup scheduling or helper-call placement.
4. `ir/objc3_ir_emitter.cpp` publishes the boundary directly into emitted IR through `; arc_lowering_abi_cleanup_model = ...`.
5. The frozen helper boundary remains explicit and private/runtime-internal:
   - `objc3_runtime_retain_i32`
   - `objc3_runtime_release_i32`
   - `objc3_runtime_autorelease_i32`
   - `objc3_runtime_load_weak_current_property_i32`
   - `objc3_runtime_store_weak_current_property_i32`
   - `objc3_runtime_push_autoreleasepool_scope`
   - `objc3_runtime_pop_autoreleasepool_scope`

## Dynamic Coverage

1. Native compile probe over `tests/tooling/fixtures/native/m262_arc_property_interaction_positive.objc3` proves emitted IR carries the new lowering-boundary line and still references the current retain/release/weak helper surface.
2. Native compile probe over `tests/tooling/fixtures/native/m262_arc_autorelease_return_positive.objc3` proves emitted IR carries the same boundary and the current autorelease helper boundary.

## Non-Goals And Fail-Closed Rules

- `M262-C001` does not implement automatic retain/release/autorelease insertion.
- `M262-C001` does not implement general cleanup-scope emission.
- `M262-C001` does not implement generalized weak load/store lowering.
- `M262-C001` does not widen any runtime helper to a new public ABI surface.
- If this boundary drifts, later lane-C and lane-D work must fail closed instead of silently widening ARC lowering behavior.
- The contract must explicitly hand off to `M262-C002`.

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

- `tmp/reports/m262/M262-C001/arc_lowering_abi_cleanup_model_summary.json`
