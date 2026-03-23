# M268 Async Diagnostics And Compatibility Completion Edge-Case And Compatibility Completion Expectations (B003)

Contract ID: `objc3c-part7-async-diagnostics-compatibility-completion/m268-b003-v1`

Frontend semantic-surface path:

- `frontend.pipeline.semantic_surface.objc_part7_async_diagnostics_and_compatibility_completion`

Required truths:

- sema now fail-closes unsupported async topology interactions instead of silently admitting them
- the positive async fixture proves the compatibility packet is emitted deterministically
- negative fixtures prove:
  - non-async `objc_executor(...)` fails closed with `O3S224`
  - async function prototypes fail closed with `O3S225`
  - async throws functions fail closed with `O3S226`
- this issue still does not claim runnable async frame layout, async error propagation, or executor runtime execution

Required probes:

- `python scripts/ensure_objc3c_native_build.py --mode fast --reason m268-b003-readiness`
- `artifacts/bin/objc3c-frontend-c-api-runner.exe tests/tooling/fixtures/native/m268_async_await_executor_source_closure_positive.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m268/b003/positive --emit-prefix module --no-emit-ir --no-emit-object`
- `artifacts/bin/objc3c-frontend-c-api-runner.exe tests/tooling/fixtures/native/m268_b003_executor_on_sync_rejected.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m268/b003/executor-on-sync --emit-prefix module --no-emit-ir --no-emit-object`
- `artifacts/bin/objc3c-frontend-c-api-runner.exe tests/tooling/fixtures/native/m268_b003_async_prototype_rejected.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m268/b003/async-prototype --emit-prefix module --no-emit-ir --no-emit-object`
- `artifacts/bin/objc3c-frontend-c-api-runner.exe tests/tooling/fixtures/native/m268_b003_async_throws_rejected.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m268/b003/async-throws --emit-prefix module --no-emit-ir --no-emit-object`

Expected positive summary facts:

- `async_callable_sites = 3`
- `executor_affinity_sites = 3`
- `illegal_non_async_executor_sites = 0`
- `illegal_async_function_prototype_sites = 0`
- `illegal_async_throws_sites = 0`
- `compatibility_diagnostic_sites = 0`
- `supported_async_callable_sites = 3`
- `unsupported_topology_fail_closed = true`
- `deterministic = true`
- `ready_for_lowering_and_runtime = true`

Expected negative summary facts:

- sync executor fixture exits nonzero and contains `O3S224`
- async prototype fixture exits nonzero and contains `O3S225`
- async throws fixture exits nonzero and contains `O3S226`
