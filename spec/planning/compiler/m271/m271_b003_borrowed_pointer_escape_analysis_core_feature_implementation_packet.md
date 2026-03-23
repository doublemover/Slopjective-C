# M271-B003 Packet: Borrowed-Pointer Escape Analysis - Core Feature Implementation

Issue: `#7325`

## Intent

Land the first truthful live borrowed-pointer legality slice by enforcing fail-closed
borrowed call boundaries, escaping-block capture rejection, and borrowed-return
owner-contract validation before later retainable-family, lowering, and runtime work.

## Scope

- publish one dedicated Part 8 sema packet
- consume the existing `M271-B002` sema packet
- allow borrowed values across call boundaries only when the callee parameter is
  itself marked `borrowed`
- reject borrowed capture by escaping blocks
- reject borrowed returns without a valid
  `objc_returns_borrowed(owner_index=...)` contract
- keep retainable-family legality, lowering, and runtime behavior explicitly deferred

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
- retainable-family legality, lowering, and runtime behavior remain later `M271` work

## Validation

- static checker verifies docs/spec/code/package anchors
- dynamic checker ensures the frontend runner accepts the positive fixture with
  `--no-emit-ir --no-emit-object`
- dynamic checker verifies the emitted manifest publishes deterministic Part 8
  sema counts under `frontend.pipeline.semantic_surface.objc_part8_borrowed_pointer_escape_analysis`
- dynamic checker verifies negative fixtures fail closed with `O3S298` and `O3S300`

## Next issue

- `M271-B004`
