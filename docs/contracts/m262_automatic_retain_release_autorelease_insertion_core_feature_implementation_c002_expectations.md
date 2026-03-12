# M262 Automatic Retain Release Autorelease Insertion Core Feature Implementation Expectations (C002)

Contract ID: `objc3c-arc-automatic-insertion/m262-c002-v1`
Status: Accepted
Issue: `#7200`
Scope: M262 lane-C implementation of automatic retain, release, and autorelease insertion for the supported runnable ARC slice.

## Objective

Promote the frozen ARC lowering boundary from `M262-C001` into a real lowering capability for the supported runnable subset. Lane C must now consume the existing ARC semantic insertion packets and emit concrete retain, release, and autorelease helper calls for supported function and method parameters, returns, and tracked ARC-owned storage instead of publishing summary-only intent.

## Required Invariants

1. `native/objc3c/src/lower/objc3_lowering_contract.h` remains the canonical declaration point for:
   - `objc3c-arc-automatic-insertion/m262-c002-v1`
   - source model `lane-c-consumes-arc-semantic-insertion-flags-for-supported-function-and-method-param-return-lowering`
   - lowering model `owned-params-retain-on-entry-release-on-exit-and-autoreleasing-returns-lower-through-private-runtime-helpers`
   - fail-closed model `only-supported-runnable-arc-param-return-insertion-paths-materialize-automatic-helper-calls`
   - non-goal model `no-general-local-lifetime-inference-no-full-cleanup-stack-no-cross-module-arc-optimization`.
2. `native/objc3c/src/lower/objc3_lowering_contract.cpp` publishes one deterministic summary for ARC automatic insertion and explicitly hands off to `M262-C003`.
3. `native/objc3c/src/sema/objc3_semantic_passes.cpp` remains explicit that sema owns ARC legality, insertion flags, and normalized property ownership metadata, while lane C consumes those packets to place helper calls.
4. `native/objc3c/src/sema/objc3_sema_pass_manager.cpp` remains explicit that the lowering handoff now carries semantic insertion flags for supported helper placement without claiming that sema itself emits ARC helper calls.
5. `native/objc3c/src/pipeline/objc3_ownership_aware_lowering_behavior_scaffold.h` remains explicit that the scaffold carries the source-side replay packets that lane C consumes for helper placement.
6. `native/objc3c/src/ir/objc3_ir_emitter.cpp` publishes the live ARC automatic-insertion boundary directly into emitted IR through:
   - `; arc_automatic_insertions = ...`
   - `!objc3.objc_arc_automatic_insertions = !{...}`
7. Supported ARC lowering must now emit private helper calls for the runnable slice through:
   - `objc3_runtime_retain_i32`
   - `objc3_runtime_release_i32`
   - `objc3_runtime_autorelease_i32`.

## Dynamic Coverage

1. Native ARC compile probe over `tests/tooling/fixtures/native/m262_arc_inference_lifetime_positive.objc3` proves emitted IR/object output carries:
   - `; arc_automatic_insertions = ...`
   - `!objc3.objc_arc_automatic_insertions = !{...}`
   - retain-on-entry for `@project(i32 %arg0)`
   - retain-before-return plus release-on-exit for `@project(i32 %arg0)`
   - the same supported automatic insertion path for `@objc3_method_Box_instance_currentValueToken_`
   - successful non-empty `module.obj` emission.
2. Non-ARC compile probe over `tests/tooling/fixtures/native/m262_arc_inference_lifetime_positive.objc3` proves the automatic insertion sites above do not appear when `-fobjc-arc` is absent.
3. Native ARC compile probe over `tests/tooling/fixtures/native/m262_arc_autorelease_return_positive.objc3` proves emitted IR/object output carries:
   - `; arc_automatic_insertions = ...`
   - `!objc3.objc_arc_automatic_insertions = !{...}`
   - `@bounce(i32 %arg0)` lowered through `call i32 @objc3_runtime_autorelease_i32(...)`
   - successful non-empty `module.obj` emission.

## Non-Goals And Fail-Closed Rules

- `M262-C002` does not add generalized local ARC lifetime inference beyond the supported semantic packets already exported by sema.
- `M262-C002` does not add a full cleanup stack, exception cleanup widening, or return-slot optimization pipeline.
- `M262-C002` does not add cross-module ARC optimization.
- `M262-C002` must fail closed for unsupported ownership-qualified signatures rather than silently widening helper placement.

## Architecture And Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md`
- `docs/objc3c-native.md`

## Build And Readiness Integration

- `package.json` includes `check:objc3c:m262-c002-automatic-retain-release-autorelease-insertion-contract`.
- `package.json` includes `test:tooling:m262-c002-automatic-retain-release-autorelease-insertion-contract`.
- `package.json` includes `check:objc3c:m262-c002-lane-c-readiness`.

## Validation

- `python scripts/check_m262_c002_automatic_retain_release_autorelease_insertion_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m262_c002_automatic_retain_release_autorelease_insertion_core_feature_implementation.py -q`
- `npm run check:objc3c:m262-c002-lane-c-readiness`

## Evidence Path

- `tmp/reports/m262/M262-C002/arc_automatic_insertion_summary.json`
