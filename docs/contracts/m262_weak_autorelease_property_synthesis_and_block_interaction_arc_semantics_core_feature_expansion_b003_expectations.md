# M262 Weak, Autorelease-Return, Property-Synthesis, And Block-Interaction ARC Semantics Core Feature Expansion Expectations (B003)

Contract ID: `objc3c-arc-interaction-semantics/m262-b003-v1`

## Required Behavior

`M262-B003` closes the next ARC semantic interaction layer above the `M262-B002`
inference baseline for the supported runnable slice.

- Under `-fobjc-arc`, attribute-only strong object properties must publish a
  `strong-owned` ownership lifetime profile and a synthesized accessor
  ownership packet.
- Under `-fobjc-arc`, attribute-only weak object properties must publish a
  `weak` ownership lifetime profile, the `objc-weak-side-table` runtime hook
  profile, and a synthesized accessor ownership packet that carries the same
  weak/runtime-hook state.
- Explicit `__autoreleasing` returns must remain visible through autorelease
  insertion accounting.
- ARC-owned block captures must remain distinguishable from weak and unowned
  block captures through retain/release accounting.
- The emitted IR must publish the `arc_interaction_semantics` boundary and
  `!objc3.objc_arc_interaction_semantics`.

## Deferred Surface

- No generalized ARC cleanup insertion.
- No full method-family ARC automation.
- No broader cross-module ARC interop.

## Evidence Requirement

The checker must prove the behavior above with:

- `tests/tooling/fixtures/native/m262_arc_property_interaction_positive.objc3`
- `tests/tooling/fixtures/native/m262_arc_autorelease_return_positive.objc3`
- `tests/tooling/fixtures/native/m261_owned_object_capture_runtime_positive.objc3`
- `tests/tooling/fixtures/native/m261_nonowning_object_capture_runtime_positive.objc3`

The contract must explicitly hand off to `M262-C001`.
