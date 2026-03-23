# Packet: `M270-C002`

- Issue: `#7313`
- Milestone: `M270`
- Lane: `C`
- Title: `Actor thunk, hop, and isolation lowering - Core feature implementation`
- Contract ID: `objc3c-part7-actor-thunk-hop-and-isolation-lowering/m270-c002-v1`

## Scope

Implement the narrow live lowering slice for actor methods:

- `actor_enter_isolation_thunk()` lowers to `objc3_runtime_actor_enter_isolation_thunk_i32`
- `actor_nonisolated_entry(value)` lowers to `objc3_runtime_actor_enter_nonisolated_i32`
- `actor_hop_to_executor(value)` lowers to `objc3_runtime_actor_hop_to_executor_i32`

## Proof

- positive fixture: `tests/tooling/fixtures/native/m270_c002_actor_lowering_runtime_positive.objc3`
- runtime probe: `tests/tooling/runtime/m270_c002_actor_lowering_runtime_probe.cpp`
- checker: `scripts/check_m270_c002_actor_thunk_hop_and_isolation_lowering_core_feature_implementation.py`

## Continuity

- depends on `M270-C001` and `M270-B003`
- next issue remains `M270-C003`
