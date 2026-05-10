          handoff.concurrency_replay_race_guard_summary.deterministic &&
          handoff.concurrency_replay_race_guard_summary
                  .concurrency_replay_race_guard_sites ==
              concurrency_replay_race_guard_summary
                  .concurrency_replay_race_guard_sites &&
          handoff.concurrency_replay_race_guard_summary
                  .concurrency_replay_sites ==
              concurrency_replay_race_guard_summary.concurrency_replay_sites &&
          handoff.concurrency_replay_race_guard_summary.replay_proof_sites ==
              concurrency_replay_race_guard_summary.replay_proof_sites &&
          handoff.concurrency_replay_race_guard_summary.race_guard_sites ==
              concurrency_replay_race_guard_summary.race_guard_sites &&
          handoff.concurrency_replay_race_guard_summary.task_handoff_sites ==
              concurrency_replay_race_guard_summary.task_handoff_sites &&
          handoff.concurrency_replay_race_guard_summary.actor_isolation_sites ==
              concurrency_replay_race_guard_summary.actor_isolation_sites &&
          handoff.concurrency_replay_race_guard_summary
                  .deterministic_schedule_sites ==
              concurrency_replay_race_guard_summary
                  .deterministic_schedule_sites &&
          handoff.concurrency_replay_race_guard_summary.guard_blocked_sites ==
              concurrency_replay_race_guard_summary.guard_blocked_sites &&
          handoff.concurrency_replay_race_guard_summary
                  .contract_violation_sites ==
              concurrency_replay_race_guard_summary.contract_violation_sites &&
          handoff.concurrency_replay_race_guard_summary.concurrency_replay_sites <=
              handoff.concurrency_replay_race_guard_summary
                  .concurrency_replay_race_guard_sites &&
          handoff.concurrency_replay_race_guard_summary.replay_proof_sites <=
              handoff.concurrency_replay_race_guard_summary
                  .concurrency_replay_race_guard_sites &&
          handoff.concurrency_replay_race_guard_summary.race_guard_sites <=
              handoff.concurrency_replay_race_guard_summary
                  .concurrency_replay_race_guard_sites &&
          handoff.concurrency_replay_race_guard_summary.task_handoff_sites <=
              handoff.concurrency_replay_race_guard_summary
                  .concurrency_replay_race_guard_sites &&
          handoff.concurrency_replay_race_guard_summary.actor_isolation_sites <=
              handoff.concurrency_replay_race_guard_summary
                  .concurrency_replay_race_guard_sites &&
          handoff.concurrency_replay_race_guard_summary
                  .deterministic_schedule_sites <=
              handoff.concurrency_replay_race_guard_summary
                  .concurrency_replay_sites &&
          handoff.concurrency_replay_race_guard_summary.guard_blocked_sites <=
              handoff.concurrency_replay_race_guard_summary
                  .concurrency_replay_sites &&
          handoff.concurrency_replay_race_guard_summary
                  .contract_violation_sites <=
              handoff.concurrency_replay_race_guard_summary
                  .concurrency_replay_race_guard_sites &&
          handoff.concurrency_replay_race_guard_summary
                  .deterministic_schedule_sites +
                  handoff.concurrency_replay_race_guard_summary
                      .guard_blocked_sites ==
              handoff.concurrency_replay_race_guard_summary
                  .concurrency_replay_sites &&
