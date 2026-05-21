# objc3c public concurrency usability

This runbook describes the currently claimable public concurrency slice for
`objc3.concurrency`. It is intentionally narrower than the full async roadmap.

## Claimable surface

- `objc3_task_spawn`, `objc3_task_spawn_child`, and
  `objc3_task_spawn_detached` expose structured and detached task-spawn tokens
  over `objc3_runtime_spawn_task_i32`.
- `objc3_task_join`, `objc3_task_group_run_two`,
  `objc3_task_group_run_bounded`,
  `objc3_task_is_cancelled`, and `objc3_task_cancel_if_needed` expose the
  checked join, task-group, and cancellation helper cluster over the runtime
  task-group entrypoints. The bounded task-group wrapper accepts the caller's
  child count and inherits the fail-closed negative-count cancellation path.
- `objc3_executor_hop` exposes deterministic executor-hop routing over
  `objc3_runtime_executor_hop_i32`.
- `objc3_actor_mailbox_send_and_drain` exposes single-message actor mailbox
  bind/enqueue/drain behavior over the runtime actor mailbox helper cluster.

## Evidence anchors

- Public module contract:
  `stdlib/modules/objc3.concurrency/module.json`
- Public module source and smoke source:
  `stdlib/modules/objc3.concurrency/module.objc3` and
  `stdlib/modules/objc3.concurrency/smoke.objc3`
- Public claim contract:
  `tests/tooling/fixtures/stdlib_concurrency/public_concurrency_usability_claims_contract.json`
- Runtime acceptance probe:
  `tests/tooling/runtime/stdlib_concurrency_runtime_probe.cpp`
- Runtime acceptance case:
  `scripts/objc3c_runtime_acceptance/domains/stdlib_runtime_cases.py`
- Diagnostic anchors:
  `tests/tooling/fixtures/native/recovery/negative/negative_language_semantics_await_outside_async.objc3`,
  `tests/tooling/fixtures/native/recovery/negative/negative_language_semantics_executor_on_sync_function.objc3`,
  `tests/tooling/fixtures/native/non_actor_actor_hop_rejected.objc3`, and
  `tests/tooling/fixtures/native/non_actor_non_sendable_crossing_rejected.objc3`

## Validation

Run the focused claim-shape test after changing the public stdlib surface:

```powershell
python -m pytest tests/tooling/test_stdlib_concurrency_public_usability_layer.py tests/tooling/test_concurrency_usability_support_claim_groundwork.py
```

Run the targeted runtime acceptance suite after changing runtime helper wiring:

```powershell
npm run objc3c -- test-runtime-acceptance-concurrency
```

## Explicit non-goals

This slice does not claim scheduler fairness, distributed actors, Swift
concurrency ABI compatibility, generic `Task` ABI completeness, OS scheduler
integration, arbitrary task group result typing, or broad async closure.
