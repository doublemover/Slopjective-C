# M268 Async Effect And Suspension Semantic Model Contract And Architecture Freeze Expectations (B001)

Contract ID: `objc3c-part7-async-effect-suspension-semantic-model/m268-b001-v1`

Frontend semantic-surface path:

- `frontend.pipeline.semantic_surface.objc_part7_async_effect_and_suspension_semantic_model`

Required truths:

- the frontend publishes a dedicated Part 7 semantic packet for async-effect and suspension legality
- the packet consumes the existing live sema summaries for async continuations, await suspension, actor isolation, task cancellation, and concurrency replay guards
- the positive async fixture proves a deterministic happy path with exact replay-stable counts
- this issue still does not claim runnable async frame lowering, suspension cleanup, or executor runtime execution

Required probes:

- `python scripts/ensure_objc3c_native_build.py --mode fast --reason m268-b001-readiness`
- `artifacts/bin/objc3c-frontend-c-api-runner.exe tests/tooling/fixtures/native/m268_async_await_executor_source_closure_positive.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m268/b001/positive --emit-prefix module --no-emit-ir --no-emit-object`

Expected positive summary facts:

- `async_continuation_sites = 5`
- `async_keyword_sites = 2`
- `async_function_sites = 1`
- `async_method_sites = 2`
- `executor_attribute_sites = 3`
- `await_suspension_sites = 3`
- `await_keyword_sites = 1`
- `await_expression_sites = 2`
- `concurrency_replay_race_guard_sites = 1`
- `deterministic = true`
- `ready_for_lowering_and_runtime = true`
