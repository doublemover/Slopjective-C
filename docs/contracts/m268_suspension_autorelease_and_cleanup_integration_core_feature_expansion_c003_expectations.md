# M268 Suspension Autorelease And Cleanup Integration Core Feature Expansion Expectations (C003)

Contract ID: `objc3c-part7-suspension-autorelease-cleanup-integration/m268-c003-v1`

Frontend lowering-surface path:

- `frontend.pipeline.semantic_surface.objc_part7_suspension_autorelease_and_cleanup_integration`

Required truths:

- the current supported non-suspending async slice must publish explicit integration evidence for autoreleasepool scope lowering and defer-cleanup lowering
- the native compiler must emit real `.ll` and `.obj` artifacts for an async fixture that composes `await`, `@autoreleasepool`, and `defer`
- the emitted manifest packet must state the current truth narrowly:
  - direct-call lowering is live
  - autoreleasepool scope hooks are live
  - defer cleanup is live
  - no separate suspension runtime or state-machine cleanup is claimed
- this issue still does not claim continuation-frame cleanup, suspension resume cleanup, or executor runtime scheduling

Required probes:

- `python scripts/ensure_objc3c_native_build.py --mode fast --reason m268-c003-readiness`
- `artifacts/bin/objc3c-native.exe tests/tooling/fixtures/native/m268_c003_async_cleanup_integration_positive.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m268/c003/positive --emit-prefix module`

Expected positive summary facts:

- `defer_statement_sites = 2`
- `live_defer_cleanup_sites = 2`
- `continuation_allocation_sites = 0`
- `continuation_resume_sites = 0`
- `continuation_suspend_sites = 0`
- `await_resume_sites = 0`
- `await_state_machine_sites = 0`
- `await_continuation_sites = 0`
- `autoreleasepool_scope_supported = true`
- `defer_cleanup_supported = true`
- `direct_call_lowering_supported = true`
- `suspension_runtime_required = false`
- `state_machine_emission_present = false`
- `deterministic = true`
- `ready_for_ir_object_emission = true`

Expected IR proof:

- IR contains `call void @objc3_runtime_push_autoreleasepool_scope()`
- IR contains `call void @objc3_runtime_pop_autoreleasepool_scope()`
- IR contains the `runTask` body sequence:
  - push autoreleasepool
  - direct call to `@fetchValue()` for the awaited operand
  - pop autoreleasepool
  - direct call to `@fetchValue()` for the deferred cleanup block
- IR contains the same ordering pattern in `@objc3_method_Loader_instance_loadValue()`
