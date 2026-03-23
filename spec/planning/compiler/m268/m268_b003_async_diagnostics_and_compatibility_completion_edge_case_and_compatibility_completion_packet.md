# M268-B003 Packet: Async Diagnostics And Compatibility Completion - Edge-Case And Compatibility Completion

Issue: `#7286`

## Intent

Close the remaining Part 7 semantic topology gaps by turning currently-admitted
unsupported async combinations into deterministic fail-closed diagnostics and a
dedicated compatibility packet.

## Scope

- reject `objc_executor(...)` on non-async functions and Objective-C methods
- reject async function prototypes until continuation lowering lands
- reject async throws functions until async error propagation lands
- publish a dedicated compatibility packet for the resulting live sema boundary

## Required code anchors

- `native/objc3c/src/sema/objc3_sema_contract.h`
- `native/objc3c/src/sema/objc3_semantic_passes.h`
- `native/objc3c/src/sema/objc3_semantic_passes.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_types.h`
- `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`

## Required docs/spec anchors

- `docs/objc3c-native.md`
- `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md`
- `spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md`

## Truth boundary

- these async topology diagnostics are now live in sema rather than being left
  to later lowering failures or silent acceptance
- the semantic packet is deterministic and records the exact fail-closed
  topology counts
- runnable async frame layout, async error propagation, and executor runtime
  execution remain later `M268` work

## Validation

- static checker verifies docs/spec/code/package anchors
- dynamic checker proves the positive fixture writes
  `frontend.pipeline.semantic_surface.objc_part7_async_diagnostics_and_compatibility_completion`
  into the manifest with exact replay-stable counts
- dynamic checker proves negative fixtures fail closed with `O3S224`,
  `O3S225`, and `O3S226`
