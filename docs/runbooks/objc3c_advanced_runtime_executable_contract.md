# Advanced Runtime Executable Contract

This runbook owns the bounded #8214/#8215 executable-runtime contract slice
that follows the #8213 native link/run closure work.

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

## Fail-Closed Rules

- Invalid or unknown executable-contract surface queries return rejected status.
- `compatibility_shim_allowed` is always false.
- Typed dispatch and native registration are required for both supported rows.
- Metadata preservation alone cannot satisfy executable runtime support.
- Public docs must keep broad scheduler and Swift/C++ runtime claims reserved
  until a real runtime owner adds executable behavior and negative fixtures.
