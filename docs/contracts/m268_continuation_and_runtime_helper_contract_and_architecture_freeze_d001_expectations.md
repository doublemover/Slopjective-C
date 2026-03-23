# M268 Continuation And Runtime-Helper Contract And Architecture Freeze Expectations (D001)

Contract ID: `objc3c-part7-continuation-runtime-helper-api/m268-d001-v1`

Goal:
Freeze the first truthful private Part 7 runtime helper ABI for logical continuation allocation, executor handoff, and resume traffic.

Required behavior:

1. The helper ABI remains private to `objc3_runtime_bootstrap_internal.h` and does not widen `objc3_runtime.h`.
2. The helper cluster must include:
   - `objc3_runtime_allocate_async_continuation_i32`
   - `objc3_runtime_handoff_async_continuation_to_executor_i32`
   - `objc3_runtime_resume_async_continuation_i32`
3. The runtime must publish one private testing snapshot:
   - `objc3_runtime_copy_async_continuation_state_for_testing`
4. Emitted IR must carry:
   - `; part7_continuation_runtime_helper = ...`
   - `!objc3.objc_part7_continuation_runtime_helper = !{!91}`
5. The positive fixture must compile and show the helper declarations plus the dedicated Part 7 runtime-helper boundary in emitted IR.
6. The runtime probe `tests/tooling/runtime/m268_d001_continuation_runtime_helper_probe.cpp` must prove the helper entrypoints execute and the testing snapshot records deterministic allocation, handoff, resume, and live-handle retirement facts.
7. This issue must remain truthful: the current direct-call async lowering slice still does not consume the helper cluster for live suspension or executor scheduling.

Evidence path:

- `tmp/reports/m268/M268-D001/continuation_runtime_helper_contract_summary.json`
