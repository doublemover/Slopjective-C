# M262-C002 Automatic Retain Release Autorelease Insertion Core Feature Implementation Packet

Packet: `M262-C002`
Milestone: `M262`
Wave: `W54`
Lane: `C`
Issue: `#7200`
Contract ID: `objc3c-arc-automatic-insertion/m262-c002-v1`
Dependencies: `M262-C001`, `M262-B002`

## Objective

Implement automatic retain, release, and autorelease insertion as a real lowering capability for the supported runnable ARC slice.

## Canonical ARC Automatic-Insertion Boundary

- contract id `objc3c-arc-automatic-insertion/m262-c002-v1`
- source model `lane-c-consumes-arc-semantic-insertion-flags-for-supported-function-and-method-param-return-lowering`
- lowering model `owned-params-retain-on-entry-release-on-exit-and-autoreleasing-returns-lower-through-private-runtime-helpers`
- fail-closed model `only-supported-runnable-arc-param-return-insertion-paths-materialize-automatic-helper-calls`
- non-goal model `no-general-local-lifetime-inference-no-full-cleanup-stack-no-cross-module-arc-optimization`
- emitted IR comment `; arc_automatic_insertions = ...`
- emitted IR named metadata `!objc3.objc_arc_automatic_insertions = !{...}`

## Acceptance Criteria

- Add explicit ARC automatic-insertion constants and summary declaration in `native/objc3c/src/lower/objc3_lowering_contract.h`.
- Add a deterministic summary helper in `native/objc3c/src/lower/objc3_lowering_contract.cpp`.
- Keep `native/objc3c/src/sema/objc3_semantic_passes.cpp` explicit that sema owns legality, insertion packets, and normalized property ownership metadata while lowering consumes those packets for supported helper placement.
- Keep `native/objc3c/src/sema/objc3_sema_pass_manager.cpp` explicit that the ARC handoff now preserves insertion flags for lowering consumption without moving helper emission into sema.
- Keep `native/objc3c/src/pipeline/objc3_ownership_aware_lowering_behavior_scaffold.h` explicit that the scaffold remains the source-side replay packet for ARC helper placement.
- Have `native/objc3c/src/ir/objc3_ir_emitter.cpp` publish the automatic-insertion boundary directly and lower real retain/release/autorelease helper calls for the supported runnable slice.
- Add deterministic docs/spec/package/checker/test evidence.
- Happy-path native ARC emission over the supported fixtures must emit non-empty `module.obj` artifacts with the expected automatic helper calls.
- Non-ARC emission over the same supported inference fixture must not silently emit the ARC automatic helper-call sites.

## Dynamic Probes

1. Native ARC compile probe over `tests/tooling/fixtures/native/m262_arc_inference_lifetime_positive.objc3` proving emitted IR/object output carries:
   - `; arc_automatic_insertions = ...`
   - `!objc3.objc_arc_automatic_insertions = !{...}`
   - `@project(i32 %arg0)` retains `%arg0` before storing into `%value.addr.0`
   - `@project(i32 %arg0)` retains the return value before the cleanup release and releases `%value.addr.0` on exit
   - `@objc3_method_Box_instance_currentValueToken_(i32 %arg0)` carries the same supported insertion pattern
   - successful non-empty `module.obj` emission.
2. Native non-ARC compile probe over `tests/tooling/fixtures/native/m262_arc_inference_lifetime_positive.objc3` proving the automatic retain/release insertion sites above are absent.
3. Native ARC compile probe over `tests/tooling/fixtures/native/m262_arc_autorelease_return_positive.objc3` proving emitted IR/object output carries:
   - `; arc_automatic_insertions = ...`
   - `!objc3.objc_arc_automatic_insertions = !{...}`
   - `@bounce(i32 %arg0)` lowered through `call i32 @objc3_runtime_autorelease_i32(...)`
   - successful non-empty `module.obj` emission.

## Non-Goals

- `M262-C002` does not add generalized local ARC lifetime inference.
- `M262-C002` does not add a full cleanup stack or exception cleanup widening.
- `M262-C002` does not add return-slot optimization.
- `M262-C002` does not add cross-module ARC optimization.

## Validation Commands

- `python scripts/check_m262_c002_automatic_retain_release_autorelease_insertion_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m262_c002_automatic_retain_release_autorelease_insertion_core_feature_implementation.py -q`
- `npm run check:objc3c:m262-c002-lane-c-readiness`

## Evidence Path

- `tmp/reports/m262/M262-C002/arc_automatic_insertion_summary.json`
