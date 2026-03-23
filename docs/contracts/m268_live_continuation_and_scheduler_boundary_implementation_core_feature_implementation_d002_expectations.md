# M268 Live Continuation And Scheduler-Boundary Implementation Expectations (D002)

Contract ID: `objc3c-part7-live-continuation-runtime-integration/m268-d002-v1`

Goal:
Turn the frozen Part 7 continuation helper ABI into a live executable boundary for the supported non-suspending async slice.

Required behavior:

1. Emitted IR must carry:
   - `; part7_live_continuation_runtime_integration = ...`
   - `!objc3.objc_part7_live_continuation_runtime_integration = !{!92}`
2. Supported `await` sites in async functions and async Objective-C methods must emit helper call sites for:
   - `objc3_runtime_allocate_async_continuation_i32`
   - `objc3_runtime_handoff_async_continuation_to_executor_i32`
   - `objc3_runtime_resume_async_continuation_i32`
3. The linked runtime probe `tests/tooling/runtime/m268_d002_live_continuation_runtime_integration_probe.cpp` must prove compiled async entrypoints execute through that helper cluster and publish deterministic snapshot counts.
4. This issue must remain truthful: the supported path is still direct-call-only and non-suspending. No suspension-frame materialization or general scheduler runtime claim is allowed.

Evidence path:

- `tmp/reports/m268/M268-D002/live_continuation_runtime_integration_summary.json`
