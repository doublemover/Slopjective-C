# M268 Frontend Async Entry And Await Surface Completion Core Feature Implementation Expectations (A002)

Contract ID: `objc3c-part7-async-source-closure/m268-a002-v1`

Frontend semantic-surface path:

- `frontend.pipeline.semantic_surface.objc_part7_async_source_closure`

Required truths:

- the frontend publishes a dedicated Part 7 async source packet in the manifest
- `async fn` and Objective-C method `async` modifiers contribute deterministic source counts
- `await` expressions contribute deterministic source counts
- canonical `objc_executor(main)`, `objc_executor(global)`, and `objc_executor(named("..."))` contribute deterministic source counts
- this issue still does not claim runnable continuation ABI, suspension cleanup, or executor runtime behavior

Required probes:

- `python scripts/ensure_objc3c_native_build.py --mode fast --reason m268-a002-readiness`
- `artifacts/bin/objc3c-frontend-c-api-runner.exe tests/tooling/fixtures/native/m268_async_await_executor_source_closure_positive.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m268/a002/positive --emit-prefix module --no-emit-ir --no-emit-object`

Expected positive summary facts:

- `async_keyword_sites = 3`
- `async_function_sites = 1`
- `async_method_sites = 2`
- `await_keyword_sites = 2`
- `await_expression_sites = 2`
- `executor_attribute_sites = 3`
- `executor_main_sites = 2`
- `executor_global_sites = 0`
- `executor_named_sites = 1`
