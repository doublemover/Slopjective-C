# M271-B004 Packet: Capture-List And Retainable-Family Legality Completion - Edge-Case And Compatibility Completion

Issue: `#7326`

## Intent

Land the next truthful Part 8 sema packet after `M271-B003` by completing the remaining
capture-list edge cases and retainable-family compatibility legality before later lowering
and runtime work.

## Scope

- publish one dedicated Part 8 sema packet
- consume the existing `M271-B003` sema packet
- reject duplicate explicit capture entries
- reject `weak` and `unowned` explicit capture entries on non-object bindings
- reject conflicting retainable-family annotations on the same callable
- reject compatibility aliases without a supporting object-return retainable-family surface
- keep lowering and runtime behavior explicitly deferred

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
- lowering and runtime behavior remain later `M271` work

## Validation

- static checker verifies docs/spec/code/package anchors
- dynamic checker ensures the frontend runner accepts the positive fixture with
  `--no-emit-ir --no-emit-object`
- dynamic checker verifies the emitted manifest publishes deterministic Part 8
  sema counts under `frontend.pipeline.semantic_surface.objc_part8_capture_list_and_retainable_family_legality_completion`
- dynamic checker verifies negative fixtures fail closed for the four edge-case
  buckets listed above

## Next issue

- `M271-B005`

