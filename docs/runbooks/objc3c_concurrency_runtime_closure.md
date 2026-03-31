# Objective-C 3 Concurrency Runtime Closure

This runbook freezes the concurrency runtime closure boundary.

Canonical checked-in boundary and contract surfaces:

- `tests/tooling/fixtures/concurrency_runtime_closure/boundary_inventory.json`
- `tests/tooling/fixtures/concurrency_runtime_closure/unified_concurrency_runtime_semantic_model.json`
- `tests/tooling/fixtures/concurrency_runtime_closure/scheduler_executor_cancellation_policy.json`
- `tests/tooling/fixtures/concurrency_runtime_closure/task_continuation_lifecycle_contract.json`
- `tests/tooling/fixtures/concurrency_runtime_closure/actor_isolation_sendability_hop_contract.json`
- `tests/tooling/fixtures/concurrency_runtime_closure/lowering_runtime_abi_contract.json`
- `tests/tooling/fixtures/concurrency_runtime_closure/executable_proof_abi_contract.json`

Replayable generators and validators:

- `python scripts/build_concurrency_runtime_closure_boundary_inventory_summary.py`
- `python scripts/build_concurrency_runtime_closure_semantic_summary.py`
- `python scripts/build_concurrency_runtime_closure_scheduler_policy_summary.py`
- `python scripts/build_concurrency_runtime_closure_task_lifecycle_summary.py`
- `python scripts/build_concurrency_runtime_closure_actor_semantics_summary.py`
- `python scripts/build_concurrency_runtime_closure_artifact_summary.py`
- `python scripts/check_concurrency_runtime_closure_task_lowering.py`
- `python scripts/check_concurrency_runtime_closure_actor_lowering.py`
- `python scripts/build_concurrency_runtime_closure_executable_proof_summary.py`
- `python scripts/check_concurrency_runtime_closure_live_task_runtime.py`
- `python scripts/check_concurrency_runtime_closure_live_actor_runtime.py`
- `python scripts/check_m322_concurrency_runtime_closeout_gate.py`

Current closure scope:

- continuation allocation, executor handoff, and async resume behavior on the live compiler/runtime path
- task lifecycle execution for spawn, task-group scope, add/wait/cancel, cancellation polling, and executor hops
- actor isolation entry, nonisolated entry, replay proof, race guard, executor binding, mailbox enqueue, and mailbox drain over the private runtime helper boundary
- packaged and cross-module replay proof for the currently emitted concurrency runtime surfaces

Current closure constraints:

- the public runtime ABI remains registration, selector lookup, dispatch, and reset; concurrency behavior stays on the private runtime-owned helper and snapshot surfaces
- shared conformance and packaged e2e proof already exist, but the milestone still needs one truthful closure boundary tying task runtime, actor runtime, scheduler/executor behavior, and isolation semantics together
- ARC, thrown-error cleanup, and broader interop interaction claims must stay narrower than the evidence published today

Unified concurrency runtime semantic model:

- scheduler, executor, continuation, task, and actor semantics are runtime responsibilities and must not be reduced to helper-token placeholders in public claims
- task runtime and actor runtime are separate internal tracks that close under one milestone and share the same private runtime helper boundary
- emitted lowering, runtime metadata, runtime helper behavior, and packaged replay evidence must describe the same observable concurrency behavior

Scheduler, executor, and cancellation policy:

- executor selection, cancellation polling, cancellation callbacks, and executor hops are supported only through the current private runtime helper cluster and shared acceptance evidence
- scheduler and executor claims remain bounded by the existing task-runtime and actor-runtime probes plus the packaged runnable concurrency workflow
- deterministic runtime snapshots are required for task, continuation, and actor state before broader public ABI claims can be made

Task and continuation lifecycle contract:

- continuation allocation, handoff, resume, task spawn, task-group scope, task-group add/wait/cancel, and on-cancel behavior remain supported only through the emitted lowering packets and private runtime-owned helper ABI
- task runtime claims must stay grounded in the live continuation and task probes plus the packaged runnable concurrency path
- interaction with ARC and thrown-error cleanup remains a successor concern until dedicated runtime evidence lands

Actor isolation, sendability, and hop semantics:

- actor isolation entry, nonisolated entry, executor hops, replay proof, race guard, executor binding, and mailbox ownership remain supported only through the private actor helper cluster and runtime snapshots
- sendability and isolation enforcement claims must stay bounded by the current lowering contracts, actor probes, and cross-module replay proof
- broader interop and public actor runtime ABI claims remain out of scope for this milestone

Lowering and runtime artifact contract:

- the canonical compile-manifest and runtime-registration surface for this milestone is the shared acceptance output published by `scripts/check_objc3c_runtime_acceptance.py`
- the canonical concurrency surfaces for this milestone are `runtime_unified_concurrency_source_surface`, `runtime_async_task_actor_normalization_completion_surface`, `runtime_unified_concurrency_lowering_metadata_surface`, and `runtime_unified_concurrency_runtime_abi_surface`
- milestone-local checks must consume those emitted surfaces instead of recreating parallel concurrency manifest truth

Executable proof and ABI contract:

- the public command surface for this milestone remains `test:objc3c:concurrency-conformance` and `test:objc3c:runnable-concurrency`
- the public workflow surface remains `validate-concurrency-conformance` and `validate-runnable-concurrency`
- concurrency closure still relies on the private runtime-owned helper ABI and snapshot surfaces; the public runtime header is not widened by this milestone

Explicit non-goals:

- public runtime ABI widening for continuation, task, executor, actor, mailbox, or sendability helpers
- claims that ARC, thrown-error cleanup, or broader interop interaction is complete beyond the currently published concurrency-runtime evidence
- milestone-local concurrency runtime scaffolding parallel to the shared runtime acceptance and runnable package path
- claims that scheduler fairness, distributed actor transport, or external executor integration are complete beyond the current manifest/runtime-registration/replay proof

Follow-on tracks:

- throws, cleanup, bridged errors, and executable propagation closure
- metaprogramming, property-behavior runtime materialization, and interop closure
- full-envelope conformance, stability, and production claimability

Authoritative live surfaces:

- runtime:
  - `native/objc3c/src/runtime/objc3_runtime.cpp`
  - `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`
- sema and lowering:
  - `native/objc3c/src/sema/objc3_semantic_passes.cpp`
  - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
  - `native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp`
- acceptance and public workflow:
  - `scripts/check_objc3c_runtime_acceptance.py`
  - `scripts/check_objc3c_runnable_concurrency_conformance.py`
  - `scripts/check_objc3c_runnable_concurrency_end_to_end.py`
  - `scripts/objc3c_public_workflow_runner.py`
- public claims:
  - `README.md`
  - `docs/objc3c-native.md`
  - `tests/tooling/runtime/README.md`
  - `docs/runbooks/objc3c_public_command_surface.md`
