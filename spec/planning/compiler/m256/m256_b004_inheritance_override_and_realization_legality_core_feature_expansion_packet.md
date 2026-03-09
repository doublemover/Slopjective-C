# M256-B004 Inheritance, Override, and Realization Legality Core Feature Expansion Packet

- Packet: `M256-B004`
- Issue: `#7135`
- Milestone: `M256`
- Lane: `B`
- Contract ID: `objc3c-inheritance-override-realization-legality/m256-b004-v1`
- Dependencies: `M256-B003`, `M256-A002`
- Next issue: `M256-B005`

## Scope

`M256-B004` converts the frozen `M256-B001` inheritance and override rules into
live legality for realized classes. This issue is about executable semantic
rejection, not new metadata inventory.

## Required implementation surface

- validate realized-class superclass closure before runtime realization
- reject missing superclass interfaces for realized classes
- reject superclass cycles for realized classes
- reject realized subclasses whose superclass implementation is absent
- reject inherited method overrides with incompatible signatures
- reject selector-kind drift across superclass chains
- reject inherited property redeclarations with incompatible signatures
- preserve deterministic diagnostics and source ownership

## Non-goals

- new runtime metadata sections
- new lowering schemas
- category merge policy changes beyond the already-landed `M256-B003` surface
- protocol-conformance policy changes beyond the already-landed `M256-B002`
  surface

## Validation

- static anchors across parser, sema, IR, docs, spec, and package scripts
- native positive compile proving the legality pass is non-blocking on the
  supported inherited override path
- native negative compile probes proving each fail-closed edge emits `O3S220`
- lane-B readiness chain including `M256-A003`, `M256-B001`, `M256-B002`,
  `M256-B003`, and `M256-B004`
