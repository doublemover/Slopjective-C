# M268 Await Suspension And Resume Semantics Core Feature Implementation Expectations (B002)

Contract ID: `objc3c-part7-await-suspension-resume-semantics/m268-b002-v1`

Frontend semantic-surface path:

- `frontend.pipeline.semantic_surface.objc_part7_await_suspension_and_resume_semantics`

Required truths:

- sema now enforces that `await` is only valid inside an async function or Objective-C method
- the positive async fixture proves the dedicated Part 7 await packet is emitted deterministically
- the negative fixture proves non-async `await` fails closed with `O3S223`
- this issue still does not claim runnable async frame layout, suspension cleanup, or executor runtime execution

Required probes:

- `python scripts/ensure_objc3c_native_build.py --mode fast --reason m268-b002-readiness`
- `artifacts/bin/objc3c-frontend-c-api-runner.exe tests/tooling/fixtures/native/m268_async_await_executor_source_closure_positive.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m268/b002/positive --emit-prefix module --no-emit-ir --no-emit-object`
- `artifacts/bin/objc3c-frontend-c-api-runner.exe tests/tooling/fixtures/native/m268_b002_non_async_await_rejected.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m268/b002/negative --emit-prefix module --no-emit-ir --no-emit-object`

Expected positive summary facts:

- `async_callable_sites = 3`
- `await_expression_sites = 2`
- `await_in_async_callable_sites = 2`
- `illegal_await_sites = 0`
- `await_suspension_point_sites = 1`
- `await_placement_enforced = true`
- `non_async_await_fail_closed = true`
- `deterministic = true`
- `ready_for_lowering_and_runtime = true`

Expected negative summary facts:

- frontend runner exits nonzero
- the emitted C API summary contains diagnostic `O3S223`
- the diagnostic text contains `await` and `async function or method`
