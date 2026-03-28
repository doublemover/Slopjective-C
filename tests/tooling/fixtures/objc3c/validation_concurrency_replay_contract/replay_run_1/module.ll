; ModuleID = 'm190_validation_concurrency_replay_race_guard'
source_filename = "m190_validation_concurrency_replay_race_guard.objc3"

; concurrency_replay_race_guard_lowering = concurrency_replay_sites=8;replay_proof_sites=5;race_guard_sites=4;task_handoff_sites=3;actor_isolation_sites=4;deterministic_schedule_sites=6;guard_blocked_sites=2;contract_violation_sites=0;deterministic=true;lane_contract=m190-concurrency-replay-race-guard-lowering-v1
; frontend_objc_concurrency_replay_race_guard_lowering_profile = concurrency_replay_sites=8, replay_proof_sites=5, race_guard_sites=4, task_handoff_sites=3, actor_isolation_sites=4, deterministic_schedule_sites=6, guard_blocked_sites=2, contract_violation_sites=0, deterministic_concurrency_replay_race_guard_lowering_handoff=true
!objc3.objc_concurrency_replay_race_guard_lowering = !{!39}
!39 = !{i64 8, i64 5, i64 4, i64 3, i64 4, i64 6, i64 2, i64 0, i1 1}
