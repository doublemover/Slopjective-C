# M261-A003 Byref Storage Helper Intent And Escape Shape Source Annotations Core Feature Expansion Packet

Issue: `#7181`  
Packet: `M261-A003`  
Milestone: `M261`  
Lane: `A`

## Scope

Expand the truthful `M261-A002` block source model with source-owned byref,
helper-intent, and escape-shape annotations that later runnable block sema,
lowering, and runtime issues consume directly.

Runnable block lowering remains out of scope here.

## Required code anchors

- `native/objc3c/src/parse/objc3_parser.cpp`
- `native/objc3c/src/ast/objc3_ast.h`
- `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`
- `native/objc3c/src/lower/objc3_lowering_contract.cpp`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`

## Required docs/checker assets

- `docs/contracts/m261_byref_storage_helper_intent_and_escape_shape_source_annotations_core_feature_expansion_a003_expectations.md`
- `scripts/check_m261_a003_byref_storage_helper_intent_and_escape_shape_source_annotations_core_feature_expansion.py`
- `tests/tooling/test_check_m261_a003_byref_storage_helper_intent_and_escape_shape_source_annotations_core_feature_expansion.py`
- `tests/tooling/fixtures/native/m261_block_source_storage_annotations_positive.objc3`

## Acceptance

- byref-candidate, helper-intent, and escape-shape source annotations land as a
  real compiler capability.
- source-only frontend runs admit the positive fixture and emit the completed
  source-annotation manifest surface.
- the native IR boundary carries the canonical source-storage annotation summary.
- native emit paths still fail closed with `O3S221`.
- evidence lands under `tmp/reports/m261/M261-A003/`.

## Next handoff

`M261-B001` is the explicit next issue after this implementation lands.
