# M270 actor thunk, hop, and isolation lowering expectations

Contract ID: `objc3c-part7-actor-thunk-hop-and-isolation-lowering/m270-c002-v1`

This issue is complete when:

- actor helper spellings inside actor methods lower through the private runtime helpers
- emitted IR contains direct calls to `objc3_runtime_actor_enter_isolation_thunk_i32`, `objc3_runtime_actor_enter_nonisolated_i32`, and `objc3_runtime_actor_hop_to_executor_i32`
- the generated object path remains real and uses the normal object backend publication
- `tests/tooling/runtime/m270_c002_actor_lowering_runtime_probe.cpp` proves the runtime helper state snapshot deterministically
- the next issue remains `M270-C003`
