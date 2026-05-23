# Advanced Runtime Executable Contract

This runbook owns the bounded #8214/#8215/#8216/#8217 executable-runtime
contract slice that follows the #8213 native link/run closure work.

## Contract Surface

- Runtime API: `native/objc3c/src/runtime/public/objc3_runtime_executable_contract.h`
- Runtime implementation: `native/objc3c/src/runtime/public/objc3_runtime_executable_contract.cpp`
- Schema: `schemas/objc3c-advanced-runtime-executable-contract-v1.schema.json`
- Fixture: `tests/tooling/fixtures/advanced_runtime_closure/executable_runtime_contract_surfaces.json`

The contract is intentionally query-only. It reports which executable runtime
surfaces are supported, reserved, or rejected; it does not provide fallback
execution, metadata promotion, or compatibility shims.

## #8214 Scheduler And Task Runtime Guarantees

Supported behavior is limited to the deterministic single-process helper
contract: task creation/start/completion, cooperative cancellation checkpoints,
continuation lifecycle records, executor-hop ordering, per-actor FIFO mailbox
enqueue/dequeue, and bounded runtime snapshots.

Rejected or reserved behavior includes scheduler fairness, priority
inheritance, work stealing, distributed scheduling, cross-process actor
transport, and external executor integration. The canonical negative fixture is
`tests/native/runtime/advanced_closure/negative_scheduler_priority_fairness_overclaim.objc3`.

## #8215 Foreign ABI Runtime Closure

Supported behavior is limited to a narrow C ABI package/replay boundary with
stable ownership, nullability, error, and async metadata normalized before
lowering. The contract still requires typed dispatch and native registration
surfaces before a runtime behavior claim can be published.

Rejected or reserved behavior includes Swift runtime ABI mirroring, C++
exception propagation, template/object-layout mirroring, actor-owned Swift
callables, async foreign callbacks, and arbitrary foreign runtime mirroring.
The canonical negative fixture is
`tests/native/runtime/advanced_closure/negative_foreign_abi_runtime_mirroring.objc3`.

## #8216 Actor Mailbox Runtime Expansion

Supported behavior is still local and single-process: stable logical mailbox
message ids, FIFO enqueue/dequeue/completion sequence records, cancellation,
deterministic actor method error recording, and shutdown/drain state through
the private actor runtime snapshot.

Rejected or reserved behavior includes distributed actor syntax without a
transport, cross-process mailboxes, remote serialization, stale actor identity
reuse after shutdown, and treating cancelled queued messages as delivered work.
The canonical negative fixtures are
`tests/native/runtime/advanced_closure/negative_distributed_actor_missing_transport.objc3`
and `tests/native/runtime/advanced_closure/negative_actor_mailbox_stale_identity.objc3`.

## #8217 Macro Host And Package Replay Runtime Closure

Supported behavior is limited to content-addressed macro host cache/package
replay handoff records: package identity, lock identity, trust identity, host
identity, input/output content identity, replay generation, cache validation
status, and runtime consumption identity must be present before cross-module
link-plan consumption.

Rejected behavior includes arbitrary host I/O, impure macro execution, unsigned
or stale package replay, host identity mismatch, and tampered runtime metadata.
The canonical negative fixtures are
`tests/native/runtime/advanced_closure/negative_macro_package_replay_stale_cache.objc3`
and `tests/native/runtime/advanced_closure/negative_macro_package_replay_host_mismatch.objc3`.

## #8218 Promotion Boundary

`language.advanced-runtime-closure` is promoted only by the integrated
executable fixture, negative matrix, schema-backed surfaces, evidence map, and
umbrella readiness agreeing through the public command without using temp or
generated reports as source truth. It is not a shortcut for broad scheduler,
Swift/C++ ABI, distributed actor, or arbitrary macro-host behavior.

## Fail-Closed Rules

- Invalid or unknown executable-contract surface queries return rejected status.
- `compatibility_shim_allowed` is always false.
- Typed dispatch and native registration are required for every supported row.
- Native object artifacts, link, run, and umbrella promotion require the
  source-owned #8232 `llc --filetype=obj` contract; missing `llc` records
  `native_object_emission_missing_llc` and cannot publish object, package,
  execution, or conformance-minima success.
- Metadata preservation alone cannot satisfy executable runtime support.
- Public docs must keep broad scheduler, Swift/C++ runtime, distributed actor,
  and arbitrary macro-host claims reserved until real runtime owners add
  executable behavior and negative fixtures.
