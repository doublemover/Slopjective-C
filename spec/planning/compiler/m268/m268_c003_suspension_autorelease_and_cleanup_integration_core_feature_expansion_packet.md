# M268-C003 Packet: Suspension Autorelease And Cleanup Integration - Core Feature Expansion

Issue: `#7289`

## Intent

Publish the truthful integration boundary showing that the current supported non-suspending async slice composes with existing autoreleasepool scope lowering and defer-cleanup lowering through real IR/object emission.

## Scope

- publish a dedicated manifest packet for async/autoreleasepool/defer integration
- tie the current async direct-call slice to the live Part 5 defer-cleanup and autoreleasepool scope replay surfaces
- prove the native compiler emits real `.ll` and `.obj` artifacts for an async fixture that uses `await`, `@autoreleasepool`, and `defer`
- prove the emitted IR preserves deterministic ordering for the supported happy path without claiming a separate suspension runtime

## Required code anchors

- `native/objc3c/src/lower/objc3_lowering_contract.h`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`

## Required docs/spec anchors

- `docs/objc3c-native.md`
- `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md`
- `spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md`

## Truth boundary

- this issue does not implement continuation-frame cleanup or suspension resume cleanup
- the current supported Part 7 integration slice is truthful but narrow:
  - `await` still lowers through the direct-call happy path
  - autoreleasepool scopes lower through the existing push/pop hooks
  - defer cleanup remains on the existing Part 5 scope-exit path
- executor resume scheduling and a real suspension cleanup runtime remain later `M268` work

## Validation

- static checker verifies docs/spec/code/package anchors
- dynamic checker proves the positive fixture writes `frontend.pipeline.semantic_surface.objc_part7_suspension_autorelease_and_cleanup_integration` into the manifest with deterministic integration facts
- dynamic checker proves the emitted `.ll` and `.obj` exist
- dynamic checker proves `runTask` and `@objc3_method_Loader_instance_loadValue()` contain the supported integration ordering in emitted IR
