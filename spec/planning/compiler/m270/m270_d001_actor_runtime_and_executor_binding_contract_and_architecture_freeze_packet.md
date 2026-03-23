# Packet: `M270-D001`

- Issue: `#7315`
- Milestone: `M270`
- Lane: `D`
- Title: `Actor runtime and executor binding contract - Contract and architecture freeze`
- Contract ID: `objc3c-part7-actor-runtime-and-executor-binding/m270-d001-v1`

## Scope

Freeze the truthful lane-D runtime surface for actors:

- private actor helper entrypoints for thunk, nonisolated entry, executor hop, replay proof, and race guard
- private actor state snapshot publication through `objc3_runtime_copy_actor_runtime_state_for_testing`
- packaged runtime archive path continuity for actor helper traffic in mixed-module link planning

## Proof

- positive fixture: `tests/tooling/fixtures/native/m270_d001_actor_runtime_executor_contract_positive.objc3`
- runtime probe: `tests/tooling/runtime/m270_d001_actor_runtime_executor_contract_probe.cpp`
- checker: `scripts/check_m270_d001_actor_runtime_and_executor_binding_contract_and_architecture_freeze.py`

## Continuity

- depends on `M270-C002`
- next issue remains `M270-D002`
