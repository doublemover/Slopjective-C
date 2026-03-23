# M268 Continuation ABI And Async Lowering Contract And Architecture Freeze Expectations (C001)

Contract ID: `objc3c-part7-continuation-abi-async-lowering-contract/m268-c001-v1`

Frontend lowering-surface path:

- `frontend.pipeline.semantic_surface.objc_part7_continuation_abi_and_async_lowering_contract`

Required truths:

- the already-landed Part 7 semantic async surface now lowers into explicit continuation and await suspension contracts rather than leaving emitted IR frontend metadata at zero-value placeholders
- the positive async fixture proves the continuation replay key and await suspension replay key are emitted deterministically into both the manifest packet and the LLVM IR frontend metadata comments
- emitted IR continues to carry the dedicated named metadata anchors for async continuation lowering and await suspension lowering
- this issue still does not claim runnable async frame layout, suspension cleanup, resume cleanup, or executor runtime execution

Required probes:

- `python scripts/ensure_objc3c_native_build.py --mode fast --reason m268-c001-readiness`
- `artifacts/bin/objc3c-frontend-c-api-runner.exe tests/tooling/fixtures/native/m268_async_await_executor_source_closure_positive.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m268/c001/positive --emit-prefix module`

Expected positive summary facts:

- `async_continuation_sites = 5`
- `async_keyword_sites = 2`
- `async_function_sites = 1`
- `continuation_allocation_sites = 0`
- `continuation_resume_sites = 0`
- `continuation_suspend_sites = 0`
- `async_state_machine_sites = 0`
- `async_normalized_sites = 5`
- `async_gate_blocked_sites = 0`
- `await_suspension_sites = 3`
- `await_keyword_sites = 1`
- `await_suspension_point_sites = 1`
- `await_resume_sites = 0`
- `await_state_machine_sites = 0`
- `await_continuation_sites = 0`
- `await_normalized_sites = 3`
- `await_gate_blocked_sites = 0`
- `deterministic_handoff = true`
- `ready_for_ir_emission = true`

Expected IR proof:

- IR contains `; async_continuation_lowering = `
- IR contains `; await_lowering_suspension_state_lowering = `
- IR contains `; frontend_objc_async_continuation_lowering_profile = async_continuation_sites=5`
- IR contains `; frontend_objc_await_lowering_suspension_state_lowering_profile = await_suspension_sites=3`
- IR contains `!objc3.objc_async_continuation_lowering = !{!43}`
- IR contains `!objc3.objc_await_lowering_suspension_state_lowering = !{!42}`
