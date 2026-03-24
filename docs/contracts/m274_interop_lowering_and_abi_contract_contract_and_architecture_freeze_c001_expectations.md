# M274 Interop Lowering And ABI Contract And Architecture Freeze Expectations (C001)

Issue: `#7367`
Contract ID: `objc3c-part11-interop-lowering-and-abi-contract/m274-c001-v1`
Canonical manifest surface: `frontend.pipeline.semantic_surface.objc_part11_interop_lowering_and_abi_contract`

## Required outcomes

- the Part 11 semantic summaries remain deterministic and replay-stable
- foreign declaration and import surfaces remain explicit manifest facts in the emitted manifest
- C++-facing and Swift-facing interop annotation surfaces remain explicit manifest facts in the emitted manifest
- the dedicated Part 11 interop lowering packet remains visible in the emitted manifest
- the emitted IR still carries the frozen lowering boundary comments that prove the front end lowered the module
- ABI lowering and runtime bridge generation remain deferred to later milestone work

## Canonical packet expectations

The emitted manifest packet is expected to expose these stable facts:

- `objc_part11_foreign_declaration_and_import_source_closure`
- `objc_part11_cpp_and_swift_interop_annotation_source_completion`
- `objc_part11_interop_semantic_model`
- `objc_part11_c_and_objc_runtime_parity_semantics`
- `objc_part11_cpp_ownership_throws_and_async_interactions`
- `objc_part11_swift_metadata_and_isolation_mapping`
- `objc_part11_interop_lowering_and_abi_contract`
- `objc_part11_foreign_surface_interface_and_module_preservation`

The Part 11 lowering packet is expected to preserve:

- semantic/runtime-parity/C++-interaction/Swift-isolation dependency contract IDs
- foreign callable and Objective-C runtime parity totals
- ownership-bridge and error-surface totals
- async-boundary and Swift concurrency-metadata totals
- interface/module-preserved foreign-callable and annotation totals
- a deterministic replay key and ready-for-IR bit

The freeze bundle is also expected to preserve the current manifest contract IDs:

- `objc3c-part11-foreign-surface-interface-preservation/m274-a003-v1`
- `objc3c-part11-foreign-declaration-import-source-closure/m274-a001-v1`
- `objc3c-part11-cpp-swift-interop-annotation-source-completion/m274-a002-v1`
- `objc3c-part11-interop-semantic-model/m274-b001-v1`
- `objc3c-part11-c-and-objc-runtime-parity-semantics/m274-b002-v1`
- `objc3c-part11-cpp-ownership-throws-and-async-interaction-completion/m274-b003-v1`
- `objc3c-part11-swift-metadata-and-isolation-mapping-completion/m274-b004-v1`

## IR proof expectations

- the positive compile emits `module.ll`
- the IR still carries the frozen lowering boundary comment emitted by the frontend
- the IR still carries the Part 11 lowering comment and metadata anchor alongside the preservation comments

## Fixture policy

- one positive fixture exercises the current Part 11 interop surface without claiming runtime ABI completion
- the positive corpus should include:
  - foreign C callables
  - foreign Objective-C method declarations
  - C++-named callables
  - Swift-named and Swift-private callables
  - interface/protocol preservation coverage
- no negative corpus is required for this freeze bundle