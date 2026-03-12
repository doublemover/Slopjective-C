# M266 Defer And Guard Lowering With Cleanup Insertion Core Feature Implementation Expectations (C002)

Contract ID: `objc3c-part5-defer-guard-lowering-implementation/m266-c002-v1`
Status: Draft
Issue: `#7263`
Scope: M266 lane-C implementation that upgrades the frozen `M266-C001` lowering boundary into truthful native lowering for pure `guard` and runnable `defer` cleanup insertion without claiming runnable statement-form `match`.

## Objective

Implement the first runnable native lowering slice for the admitted Part 5 control-flow surface.

`M266-C002` is specifically about two behaviors:

- pure `guard` lowering in the native LLVM/object path
- runnable `defer` lowering with lexical-scope cleanup insertion on ordinary and non-local exits

This issue does not need to make statement-form `match` runnable. If statement-form `match` remains fail-closed after `M266-C002`, that is still truthful.

## Required Invariants

1. `lower/objc3_lowering_contract.h` becomes the canonical declaration point for:
   - `objc3c-part5-defer-guard-lowering-implementation/m266-c002-v1`
   - guard lowering model
     `pure-guard-condition-lists-lower-into-short-circuit-branching-with-else-edge-cleanup-before-exit`
   - defer lowering model
     `defer-bodies-lower-into-lifo-lexical-cleanups-that-run-on-ordinary-and-nonlocal-exit`
   - scope model
     `cleanup-insertion-reuses-lexical-scope-unwind-authority-without-reordering-existing-owned-cleanups`
   - match model
     `statement-match-remains-out-of-scope-for-m266-c002-and-may-remain-fail-closed`
   - fail-closed model
     `unsupported-part5-surfaces-still-fail-closed-rather-than-silently-lowering-incompletely`.
2. `ir/objc3_ir_emitter.cpp` publishes the live lane-C boundary directly in emitted IR through
   `; part5_defer_guard_lowering_implementation = ...` and
   `!objc3.objc_part5_defer_guard_lowering_implementation`.
3. `pipeline/objc3_frontend_artifacts.cpp` becomes the single manifest handoff path for the live C002 lowering packet at
   `frontend.pipeline.semantic_surface.objc_part5_defer_guard_lowering_implementation`.
4. The emitted lowering packet must point back to both:
   - `objc3c-part5-control-flow-safety-lowering/m266-c001-v1`
   - `objc3c-part5-control-flow-semantic-model/m266-b001-v1`
5. `guard` lowering proof must be limited to pure-guard cases. `M266-C002` does not claim runnable mixed `guard + match` lowering.
6. `defer` lowering proof must show cleanup insertion on:
   - ordinary lexical scope exit
   - early `return` from the guarded scope
7. Existing cleanup ownership must remain truthful. `M266-C002` must not claim ARC, autoreleasepool, or block-dispose ordering beyond the cleanup stack semantics the native lowering path actually reuses.

## Dynamic Coverage

The issue-local checker must prove all of the following for the native path:

1. A generated pure-`guard` probe compiles with `artifacts/bin/objc3c-native.exe`, emits `module.ll`, `module.obj`, and `module.manifest.json`, and publishes the C002 lowering packet with:
   - `guard_statement_sites = 1`
   - `live_guard_lowering_sites = 1`
   - `defer_statement_sites = 0`
   - `match_lowering_sites = 0`
   - `ready_for_native_match_lowering = false`
2. A generated ordinary-exit `defer` probe compiles with `artifacts/bin/objc3c-native.exe`, emits `module.ll`, `module.obj`, and `module.manifest.json`, and publishes the C002 lowering packet with:
   - `defer_statement_sites = 1`
   - `live_defer_cleanup_insertion_sites = 1`
   - `live_defer_nonlocal_exit_cleanup_sites = 0`
3. A generated early-return `defer` probe compiles with `artifacts/bin/objc3c-native.exe`, emits `module.ll`, `module.obj`, and `module.manifest.json`, and publishes the C002 lowering packet with:
   - `defer_statement_sites = 1`
   - `live_defer_cleanup_insertion_sites = 1`
   - `live_defer_nonlocal_exit_cleanup_sites = 1`
4. The same generated probes prove the emitted `module.ll` contains the C002 boundary comment and named metadata.
5. A mixed `guard + match` probe may remain fail-closed. The checker may record that result in evidence, but it must not require runnable `match` support for C002 acceptance.

## Non-Goals And Fail-Closed Rules

- `M266-C002` does not make statement-form `match` runnable.
- `M266-C002` does not make expression-form `match` runnable.
- `M266-C002` does not claim guarded-pattern or type-test-pattern lowering.
- `M266-C002` does not claim cross-function cleanup scheduling.
- `M266-C002` does not claim ownership-model-specific destruction semantics beyond reused scope-unwind cleanup insertion.
- Any unsupported Part 5 control-flow surface must still fail closed with deterministic diagnostics.

## Architecture And Spec Anchors

- `native/objc3c/src/lower/objc3_lowering_contract.h`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `docs/objc3c-native.md`
- `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md`
- `spec/CROSS_CUTTING_RULE_INDEX.md`
- `native/objc3c/src/ARCHITECTURE.md`

## Validation

- `python scripts/check_m266_c002_defer_and_guard_lowering_with_cleanup_insertion_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m266_c002_defer_and_guard_lowering_with_cleanup_insertion_core_feature_implementation.py -q`
- `python scripts/run_m266_c002_lane_c_readiness.py`

## Evidence Path

- `tmp/reports/m266/M266-C002/defer_and_guard_lowering_with_cleanup_insertion_summary.json`
