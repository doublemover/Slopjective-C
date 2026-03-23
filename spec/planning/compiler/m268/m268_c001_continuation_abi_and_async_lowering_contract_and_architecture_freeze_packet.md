# M268-C001 Packet: Continuation ABI And Async Lowering Contract - Contract And Architecture Freeze

Issue: `#7287`

## Intent

Freeze the Part 7 continuation ABI and await suspension lowering boundary as a real emitted compiler contract.

## Scope

- derive the async continuation lowering contract from the live Part 7 semantic summaries
- derive the await suspension lowering contract from the live Part 7 await summary
- publish a dedicated manifest packet for the combined lowering boundary
- thread the replay-stable continuation and await lowering metadata into emitted LLVM IR frontend metadata

## Required code anchors

- `native/objc3c/src/lower/objc3_lowering_contract.h`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`

## Required docs/spec anchors

- `docs/objc3c-native.md`
- `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md`
- `spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md`

## Truth boundary

- this issue does not invent runnable async execution; it freezes the lowering handoff that existing IR/frontend surfaces were already shaped to carry
- the continuation lowering and await suspension lowering replay keys now become truthful emitted artifacts rather than zero-value placeholders
- runnable async frame layout, resume cleanup, suspension cleanup, and executor runtime execution remain later `M268` work

## Validation

- static checker verifies docs/spec/code/package anchors
- dynamic checker proves the positive async fixture writes `frontend.pipeline.semantic_surface.objc_part7_continuation_abi_and_async_lowering_contract` into the manifest with exact deterministic counts
- dynamic checker proves the emitted `.ll` carries the replay keys, frontend lowering profile comments, and named metadata anchors for both continuation and await lowering
