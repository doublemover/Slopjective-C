# M271-B001 Packet: System-Extension Semantic Model - Contract And Architecture Freeze

Issue: `#7323`

## Intent

Freeze one truthful lane-B sema packet for the current Part 8 system-extension
surface before later `M271` issues widen legality, lowering, and runtime work.

## Scope

- publish one dedicated Part 8 sema packet
- consume the existing `M271-A001`, `M271-A002`, and `M271-A003` source packets
- preserve deterministic counts for:
  - cleanup hooks
  - resource locals
  - borrowed pointer spellings
  - borrowed-return relations
  - explicit block capture lists
  - retainable-family declarations and compatibility aliases
- keep resource move legality, borrowed escape legality, retainable-family
  legality, lowering, and runtime behavior explicitly deferred

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

- the packet is sema/accounting only
- the lane-B proof must stay frontend-runner based
- resource move legality, borrowed escape legality, retainable-family legality,
  lowering, and runtime behavior remain later `M271` work

## Validation

- static checker verifies docs/spec/code/package anchors
- dynamic checker ensures the frontend runner accepts the positive fixture with
  `--no-emit-ir --no-emit-object`
- dynamic checker verifies the emitted manifest publishes deterministic Part 8
  sema counts under
  `frontend.pipeline.semantic_surface.objc_part8_system_extension_semantic_model`

## Next issue

- `M271-B002`
