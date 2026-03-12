# M265-C001 - Optional and key-path lowering contract

Issue: `#7249`

## Objective

Freeze the first lowering-owned Part 3 packet so the live compiler publishes one
deterministic native-lowering contract for optional bindings, optional sends,
and nil-coalescing while validated typed key-path literals now lower into
retained descriptor handles and broader key-path runtime behavior remains
deferred.

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
- lower validated typed key-path literals into retained native descriptor
  artifacts and stable nonzero handles

## Dynamic proof shape

- native positive probe for optional-flow lowering:
  - optional bindings
  - ordinary optional-send legality
  - nil-coalescing
- native executable proof for optional-send argument short-circuit on a nil
  receiver
- source-only positive probe for a typed key-path literal that proves the live
  artifact packet is published truthfully
- native positive probe for the same typed key-path literal proving native IR
  emits retained key-path descriptors and preserves generic-metadata evidence

## Notes

- This issue is the lowering freeze, not full executable typed key-path
  application/runtime support.
- `?.` optional-member access is now inside the admitted lowering surface.
- Later issues in `M265` are expected to convert the deferred typed key-path
  surface into full runtime behavior.
