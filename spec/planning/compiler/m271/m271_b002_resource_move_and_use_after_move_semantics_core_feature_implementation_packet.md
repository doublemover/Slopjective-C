# M271-B002 Packet: Resource Move And Use-After-Move Semantics - Core Feature Implementation

Issue: `#7324`

## Intent

Land the first truthful live legality slice for Part 8 cleanup/resource locals by
tracking ownership transfer through explicit `move` block captures and rejecting
later use or duplicate transfer of the same cleanup-owned local.

## Scope

- publish one dedicated Part 8 sema packet
- consume the existing `M271-B001` sema packet
- track cleanup/resource-backed locals through statement bodies
- reject `move` capture of non-resource locals
- reject use-after-move of cleanup/resource-backed locals
- reject duplicate move transfer of cleanup/resource-backed locals
- keep borrowed escape legality, retainable-family legality, lowering, and
  runtime behavior explicitly deferred

## Required code anchors

- `native/objc3c/src/sema/objc3_sema_contract.h`
- `native/objc3c/src/sema/objc3_semantic_passes.h`
- `native/objc3c/src/sema/objc3_semantic_passes.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_types.h`
- `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`

## Required docs/spec anchors

- `docs/objc3c-native.md`
- `docs/objc3c-native/src/20-grammar.md`
- `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md`
- `spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md`
- `spec/CONFORMANCE_PROFILE_CHECKLIST.md`

## Truth boundary

- this issue is live sema/accounting only
- the lane-B proof must stay frontend-runner based
- borrowed escape legality, retainable-family legality, lowering, and runtime
  behavior remain later `M271` work

## Validation

- static checker verifies docs/spec/code/package anchors
- dynamic checker ensures the frontend runner accepts the positive fixture with
  `--no-emit-ir --no-emit-object`
- dynamic checker verifies the emitted manifest publishes deterministic Part 8
  sema counts under
  `frontend.pipeline.semantic_surface.objc_part8_resource_move_and_use_after_move_semantics`
- dynamic checker verifies negative fixtures fail closed with `O3S295`, `O3S296`,
  and `O3S297`

## Next issue

- `M271-B003`
