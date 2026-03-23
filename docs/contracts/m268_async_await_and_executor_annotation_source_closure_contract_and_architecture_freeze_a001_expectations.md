# M268 Async, Await, And Executor-Annotation Source Closure Contract And Architecture Freeze Expectations (A001)

Contract ID: `objc3c-part7-async-source-closure/m268-a001-v1`

Frontend source boundary:

- parser-owned async and executor surface on functions and Objective-C methods

Required truths:

- `async fn` is admitted as a source-owned frontend declaration form
- Objective-C methods admit source-owned `async` declaration modifiers
- `await <expr>` is admitted as a source-owned frontend expression marker
- `__attribute__((objc_executor(main)))`, `__attribute__((objc_executor(global)))`, and `__attribute__((objc_executor(named("..."))))` are admitted on functions and Objective-C methods
- this issue does not claim runnable continuation lowering, suspension, or executor runtime behavior yet

Required probes:

- `python scripts/ensure_objc3c_native_build.py --mode fast --reason m268-a001-readiness`
- `artifacts/bin/objc3c-frontend-c-api-runner.exe tests/tooling/fixtures/native/m268_async_await_executor_source_closure_positive.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m268/a001/positive --emit-prefix module --no-emit-ir --no-emit-object`

Positive probe interpretation:

- the positive source-closure probe is satisfied by a successful frontend-only compile with a written `module.manifest.json`
- the proof is parser/frontend ownership of the syntax, not runnable async execution

Expected positive summary facts:

- `async_fn_sites = 1`
- `async_method_sites = 2`
- `await_expression_sites >= 2`
- `executor_attribute_sites = 3`
- `executor_named_sites = 1`
- `executor_main_sites = 2`

