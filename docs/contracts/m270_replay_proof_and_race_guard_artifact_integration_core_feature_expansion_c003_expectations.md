# M270 replay-proof and race-guard artifact integration expectations

Contract ID: `objc3c-part7-actor-replay-proof-and-race-guard-integration/m270-c003-v1`

This issue is complete when:

- actor-method `replay_proof_step()` lowers through `objc3_runtime_actor_record_replay_proof_i32`
- actor-method `race_guard_lock()` lowers through `objc3_runtime_actor_record_race_guard_i32`
- emitted IR still carries both actor-lowering and concurrency replay/race-guard lowering packets together
- `tests/tooling/runtime/m270_c003_actor_replay_race_guard_probe.cpp` proves the private actor runtime snapshot records replay-proof and race-guard helper traffic deterministically
- the next issue remains `M270-D001`
