# M274 Foreign Call And Lifetime Lowering Core Feature Implementation Expectations (C002)

Issue: `#7368`
Contract ID: `objc3c-part11-foreign-call-and-lifetime-lowering/m274-c002-v1`
Canonical manifest surface: `frontend.pipeline.semantic_surface.objc_part11_foreign_call_and_lifetime_lowering`

## Required outcomes

- foreign calls lower into deterministic IR and object-artifact facts rather than remaining freeze-only metadata
- lifetime and ownership bridges are preserved as explicit manifest facts alongside foreign-call lowering
- metadata preservation stays deterministic for imported C, Objective-C, C++, and Swift-facing surfaces
- the Part 11 interop lowering freeze from `M274-C001` remains an explicit dependency
- the C++ ownership/throws/async interaction bundle from `M274-B003` remains an explicit dependency
- the emitted IR carries a dedicated Part 11 foreign-call and lifetime-lowering proof comment and metadata anchor

## Canonical packet expectations

The emitted manifest packet is expected to expose these stable facts:

- `objc_part11_foreign_call_and_lifetime_lowering`
- `objc_part11_interop_lowering_and_abi_contract`
- `objc_part11_foreign_surface_interface_and_module_preservation`

The foreign-call/lifetime packet is expected to preserve:

- foreign-call counts for imported C and Objective-C call surfaces
- lifetime bridge counts for strong and weak ownership surfaces
- metadata preservation counts for import-module, C++-name, header-name, and Swift-name annotations
- a deterministic replay key and ready-for-IR bit

The bundle is also expected to preserve the current manifest contract IDs:

- `objc3c-part11-interop-lowering-and-abi-contract/m274-c001-v1`
- `objc3c-part11-cpp-ownership-throws-and-async-interaction-completion/m274-b003-v1`

## IR proof expectations

- the positive compile emits `module.ll`
- the IR carries a dedicated foreign-call and lifetime-lowering comment emitted by the frontend
- the IR carries a dedicated Part 11 metadata anchor for the foreign-call and lifetime-lowering packet

## Fixture policy

- one positive fixture exercises the happy path for imported foreign call lowering, lifetime/ownership bridges, and metadata preservation
- the positive corpus should include:
  - foreign C callables
  - foreign Objective-C method declarations
  - C++-named callables
  - Swift-named and Swift-private callables
  - strong and weak property ownership bridges
  - foreign-surface metadata-preservation annotations
- no negative corpus is required for this issue-local bundle
