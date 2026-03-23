# Packet: `M270-C003`

- Issue: `#7314`
- Milestone: `M270`
- Lane: `C`
- Title: `Replay-proof and race-guard artifact integration - Core feature expansion`
- Contract ID: `objc3c-part7-actor-replay-proof-and-race-guard-integration/m270-c003-v1`

## Scope

Integrate the strict-concurrency replay-proof and race-guard artifact slice into the runnable actor lowering path.

- `replay_proof_step()` lowers to `objc3_runtime_actor_record_replay_proof_i32`
- `race_guard_lock()` lowers to `objc3_runtime_actor_record_race_guard_i32`
- emitted actor lowering still carries the existing actor thunk/hop/nonisolated helper-backed surface from `M270-C002`

## Proof

- positive fixture: `tests/tooling/fixtures/native/m270_c003_actor_replay_race_guard_positive.objc3`
- runtime probe: `tests/tooling/runtime/m270_c003_actor_replay_race_guard_probe.cpp`
- checker: `scripts/check_m270_c003_replay_proof_and_race_guard_artifact_integration_core_feature_expansion.py`

## Continuity

- depends on `M270-C002` and `M270-B003`
- next issue remains `M270-D001`
