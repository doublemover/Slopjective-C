# M274-C001 Packet: Interop Lowering And ABI Contract - Contract And Architecture Freeze

- Issue: `M274-C001`
- Milestone: `M274`
- Lane: `C`
- Contract ID: `objc3c-part11-interop-lowering-and-abi-contract/m274-c001-v1`
- Surface path: `frontend.pipeline.semantic_surface.objc_part11_interop_lowering_and_abi_contract`

## Summary

This packet freezes the Part 11 lowering boundary that carries foreign-callable surfaces, ownership/error bridge interaction facts, Swift-facing concurrency metadata, and the already-emitted module/interface preservation data into one deterministic lowering contract.

## Lowering surface

- foreign declaration and import source closure remains deterministic
- C++/Swift interop annotations remain manifest-visible
- the semantic and legality Part 11 packets remain explicit dependencies in the emitted manifest
- one dedicated `objc_part11_interop_lowering_and_abi_contract` packet becomes the canonical lowering artifact
- the lowering handoff preserves foreign-callable surfaces and module/interface preservation facts
- the IR proof includes the existing native lowering boundary plus a dedicated Part 11 lowering comment and metadata anchor

## Contract continuity

- the Part 11 lowering contract keeps the current manifest dependency contract IDs intact:
  - `objc3c-part11-foreign-surface-interface-preservation/m274-a003-v1`
  - `objc3c-part11-foreign-declaration-import-source-closure/m274-a001-v1`
  - `objc3c-part11-cpp-swift-interop-annotation-source-completion/m274-a002-v1`
  - `objc3c-part11-interop-semantic-model/m274-b001-v1`
  - `objc3c-part11-c-and-objc-runtime-parity-semantics/m274-b002-v1`
  - `objc3c-part11-cpp-ownership-throws-and-async-interaction-completion/m274-b003-v1`
  - `objc3c-part11-swift-metadata-and-isolation-mapping-completion/m274-b004-v1`

## Deferred boundary

- no live foreign-call lowering is claimed here
- no new bridge-generation runtime is claimed here
- the lowering contract remains a truthful handoff into later runnable interop work
