# M268-C002 Packet: Async Function Await And Continuation Lowering - Core Feature Implementation

Issue: `#7288`

## Intent

Publish the truthful runnable Part 7 lowering slice that already emits real IR/object artifacts for supported async functions, async Objective-C methods, and `await` on the non-suspending happy path.

## Scope

- publish a dedicated manifest packet for the current runnable async lowering slice
- make the emitted packet state explicitly that the supported path is direct-call-based and non-suspending
- prove the native compiler emits real `.ll` and `.obj` artifacts for the positive fixture
- prove the emitted IR lowers the async function and async method bodies through direct calls rather than placeholder stubs

## Required code anchors

- `native/objc3c/src/lower/objc3_lowering_contract.h`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`

## Required docs/spec anchors

- `docs/objc3c-native.md`
- `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md`
- `spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md`

## Truth boundary

- this issue does not implement a full continuation runtime
- the current supported Part 7 lowering slice is truthful but narrow:
  - async functions and async Objective-C methods emit runnable IR/object code
  - `await` lowers through the operand direct-call path
  - no continuation allocation, suspend/resume lowering, or async state machine is emitted for the supported happy path
- continuation allocation, suspension cleanup, resume cleanup, and executor scheduling remain later `M268` work

## Validation

- static checker verifies docs/spec/code/package anchors
- dynamic checker proves the positive fixture writes `frontend.pipeline.semantic_surface.objc_part7_async_function_await_and_continuation_lowering` into the manifest with exact deterministic facts for the direct-call lowering slice
- dynamic checker proves the emitted `.ll` and `.obj` exist
- dynamic checker proves the emitted `.ll` contains direct calls to `@fetchValue()` from both the async function and async method bodies, plus a call to `@runTask()` from `main`
