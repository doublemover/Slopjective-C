# M268 Async Function Await And Continuation Lowering Core Feature Implementation Expectations (C002)

Contract ID: `objc3c-part7-async-direct-call-lowering/m268-c002-v1`

Frontend lowering-surface path:

- `frontend.pipeline.semantic_surface.objc_part7_async_function_await_and_continuation_lowering`

Required truths:

- the native compiler currently supports a truthful runnable Part 7 lowering slice for async functions, async Objective-C methods, and `await` on the non-suspending happy path
- the lowering path emits real `.ll` and `.obj` artifacts rather than a contract-only placeholder
- `await` currently lowers by reusing the operand direct-call path rather than materializing continuation allocation, suspend/resume helpers, or a state machine
- the emitted manifest packet must publish that narrow truth explicitly and fail closed against claiming a fuller continuation runtime than actually exists
- this issue still does not claim continuation allocation, suspend/resume lowering, suspension cleanup, or executor runtime scheduling

Required probes:

- `python scripts/ensure_objc3c_native_build.py --mode fast --reason m268-c002-readiness`
- `artifacts/bin/objc3c-native.exe tests/tooling/fixtures/native/m268_c002_async_lowering_positive.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m268/c002/positive --emit-prefix module`

Expected positive summary facts:

- `async_function_sites = 1`
- `async_method_sites = 2`
- `await_expression_sites = 2`
- `continuation_allocation_sites = 0`
- `continuation_resume_sites = 0`
- `continuation_suspend_sites = 0`
- `async_state_machine_sites = 0`
- `await_resume_sites = 0`
- `await_state_machine_sites = 0`
- `await_continuation_sites = 0`
- `direct_call_lowering_supported = true`
- `non_suspending_happy_path_only = true`
- `object_emission_supported = true`
- `runtime_scheduler_required = false`
- `deterministic = true`
- `ready_for_ir_object_emission = true`

Expected IR proof:

- IR contains `define i32 @fetchValue() {`
- IR contains `define i32 @runTask() {`
- IR contains `define i32 @objc3_method_Loader_instance_loadValue() {`
- IR contains `define i32 @main() {`
- IR contains `call i32 @fetchValue()` in both the async function and async method bodies
- IR contains `call i32 @runTask()` in `main`
- IR still carries the Part 7 lowering replay comments and named metadata anchors from `M268-C001`
