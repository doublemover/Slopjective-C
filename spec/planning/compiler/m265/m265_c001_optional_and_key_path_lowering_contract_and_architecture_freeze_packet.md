# M265-C001 - Optional and key-path lowering contract

Issue: `#7249`

## Objective

Freeze the first lowering-owned Part 3 packet so the live compiler publishes one
deterministic native-lowering contract for optional bindings, optional sends,
and nil-coalescing while typed key-path literals remain truthful deferred
surfaces.

## Dependencies

- `M265-B002`
- `M265-B003`
- `M259-E002`

## Required implementation

- publish `frontend.pipeline.semantic_surface.objc_part3_optional_keypath_lowering_contract`
- add lowering contract constants and deterministic replay key generation for
  the C001 packet
- publish the C001 packet from frontend artifacts without widening the central
  semantic summary structs
- emit an explicit IR boundary comment for the Part 3 lowering contract
- lower optional sends without evaluating selector arguments on the nil-receiver
  arm
- keep typed key-path literals deferred from native IR/object lowering and fail
  closed with deterministic diagnostics on the native path

## Dynamic proof shape

- native positive probe for optional-flow lowering:
  - optional bindings
  - ordinary optional-send legality
  - nil-coalescing
- native executable proof for optional-send argument short-circuit on a nil
  receiver
- source-only positive probe for a typed key-path literal that proves the
  deferred packet is published truthfully
- native fail-closed probe for the same typed key-path literal proving native IR
  lowering still rejects unresolved `__objc3_keypath_literal`

## Notes

- This issue is the lowering freeze, not executable typed key-path support.
- `?.` optional-member access remains outside the admitted lowering surface.
- Later issues in `M265` are expected to convert the deferred typed key-path
  surface into live lowering/runtime behavior.
