# M262-B003 Weak, Autorelease-Return, Property-Synthesis, And Block-Interaction ARC Semantics Core Feature Expansion Packet

Issue: `#7198`
Packet: `M262-B003`
Milestone: `M262`
Lane: `B`

## Goal

Close the semantic ARC interaction inventory that sits above the `B002`
inference baseline without widening into deferred cleanup automation.

## Required Deliverables

- Explicit source/spec/architecture anchors for the ARC interaction boundary.
- Live semantic propagation for attribute-only strong and weak properties.
- Dynamic evidence for autorelease-return profiling.
- Dynamic evidence for owned vs non-owning block capture ARC behavior.
- Stable evidence output under `tmp/reports/m262/M262-B003/`.

## Acceptance Summary

- Attribute-only strong properties publish `strong-owned` synthesized accessor
  ownership packets under `-fobjc-arc`.
- Attribute-only weak properties publish `weak` plus
  `objc-weak-side-table` synthesized accessor ownership packets under
  `-fobjc-arc`.
- Explicit `__autoreleasing` returns remain visible through autorelease
  insertion accounting.
- Owned block captures preserve nonzero retain/release accounting.
- Weak and unowned block captures remain non-owning.
- Emitted IR carries `; arc_interaction_semantics = ...` and
  `!objc3.objc_arc_interaction_semantics`.

`M262-C001` is the explicit next handoff after this implementation lands.
