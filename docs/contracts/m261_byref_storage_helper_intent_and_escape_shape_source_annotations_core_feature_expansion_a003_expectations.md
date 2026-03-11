# M261 Byref Storage Helper Intent And Escape Shape Source Annotations Core Feature Expansion Expectations (A003)

Contract ID: `objc3c-executable-block-source-storage-annotation/m261-a003-v1`

## Objective

Expand the truthful lane-A block source model so later runnable block work can
consume explicit byref-candidate, helper-intent, and escape-shape annotations
without pretending that runnable block lowering or helper emission already
exists.

## Required implementation

1. Add a canonical expectations document for the block source storage
   annotation boundary.
2. Add this packet, a deterministic checker, tooling tests, and a direct lane-A
   readiness runner:
   - `scripts/check_m261_a003_byref_storage_helper_intent_and_escape_shape_source_annotations_core_feature_expansion.py`
   - `tests/tooling/test_check_m261_a003_byref_storage_helper_intent_and_escape_shape_source_annotations_core_feature_expansion.py`
   - `scripts/run_m261_a003_lane_a_readiness.py`
3. Add `M261-A003` anchor text to:
   - `docs/objc3c-native/src/20-grammar.md`
   - `docs/objc3c-native.md`
   - `spec/PART_0_BASELINE_AND_NORMATIVE_REFERENCES.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `native/objc3c/src/ARCHITECTURE.md`
   - `native/objc3c/src/parse/objc3_parser.cpp`
   - `native/objc3c/src/ast/objc3_ast.h`
   - `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`
   - `native/objc3c/src/lower/objc3_lowering_contract.h`
   - `native/objc3c/src/lower/objc3_lowering_contract.cpp`
   - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
   - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
4. Freeze the current source-annotation contract around:
   - `BlockLiteralSourceUseKind`
   - `Expr::block_mutated_capture_names_lexicographic`
   - `Expr::block_byref_capture_names_lexicographic`
   - `Expr::block_copy_helper_intent_required`
   - `Expr::block_dispose_helper_intent_required`
   - `Expr::block_escape_shape_symbol`
   - `Expr::block_escape_shape_profile`
   - `Objc3ExecutableBlockSourceStorageAnnotationSummary()`
   - `Objc3BlockSourceStorageAnnotationReplayKey(...)`
5. The checker must prove the boundary with three live probes:
   - `tests/tooling/fixtures/native/hello.objc3` on the native emit path to
     confirm the emitted IR carries the canonical block source storage
     annotation summary line.
   - `tests/tooling/fixtures/native/m261_block_source_storage_annotations_positive.objc3`
     through `artifacts/bin/objc3c-frontend-c-api-runner.exe --no-emit-ir --no-emit-object`
     to prove the source-only positive path and emitted manifest surface.
   - the same block fixture through `artifacts/bin/objc3c-native.exe` to prove
     runnable block lowering still fails closed with `O3S221`.
6. `package.json` must wire:
   - `check:objc3c:m261-a003-block-source-storage-annotations`
   - `test:tooling:m261-a003-block-source-storage-annotations`
   - `check:objc3c:m261-a003-lane-a-readiness`
7. The contract must explicitly hand off to `M261-B001`.

## Canonical models

- Byref storage model:
  `block-source-model-publishes-deterministic-byref-capture-candidates-before-runnable-block-byref-lowering`
- Helper intent model:
  `block-source-model-publishes-copy-dispose-helper-intent-before-runnable-block-helper-lowering`
- Escape shape model:
  `block-source-model-publishes-heap-promotion-relevant-escape-shape-categories-before-runnable-block-escape-analysis`
- Evidence model:
  `hello-ir-boundary-plus-source-only-positive-probe-plus-native-fail-closed-negative-probe`
- Failure model:
  `fail-closed-on-block-byref-helper-or-escape-shape-source-annotation-drift-before-runnable-block-lowering`

## Non-goals

- No explicit `__block` byref storage spelling yet.
- No runnable byref lowering yet.
- No copy/dispose helper lowering yet.
- No runnable block object lowering, helper emission, or heap promotion yet.

## Evidence

- `tmp/reports/m261/M261-A003/block_source_storage_annotations_summary.json`
