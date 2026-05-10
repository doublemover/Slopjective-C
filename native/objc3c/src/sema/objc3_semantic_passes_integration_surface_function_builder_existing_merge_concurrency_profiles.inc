    existing.async_continuation_profile_is_normalized =
        existing.async_continuation_profile_is_normalized ||
        fn.async_continuation_profile_is_normalized;
    existing.deterministic_async_continuation_handoff =
        existing.deterministic_async_continuation_handoff ||
        fn.deterministic_async_continuation_handoff;
    existing.async_continuation_sites =
        std::max(existing.async_continuation_sites, fn.async_continuation_sites);
    existing.async_keyword_sites =
        std::max(existing.async_keyword_sites, fn.async_keyword_sites);
    existing.async_function_sites =
        std::max(existing.async_function_sites, fn.async_function_sites);
    existing.continuation_allocation_sites = std::max(
        existing.continuation_allocation_sites, fn.continuation_allocation_sites);
    existing.continuation_resume_sites = std::max(
        existing.continuation_resume_sites, fn.continuation_resume_sites);
    existing.continuation_suspend_sites = std::max(
        existing.continuation_suspend_sites, fn.continuation_suspend_sites);
    existing.async_state_machine_sites = std::max(
        existing.async_state_machine_sites, fn.async_state_machine_sites);
    existing.async_continuation_normalized_sites = std::max(
        existing.async_continuation_normalized_sites,
        fn.async_continuation_normalized_sites);
    existing.async_continuation_gate_blocked_sites = std::max(
        existing.async_continuation_gate_blocked_sites,
        fn.async_continuation_gate_blocked_sites);
    existing.async_continuation_contract_violation_sites = std::max(
        existing.async_continuation_contract_violation_sites,
        fn.async_continuation_contract_violation_sites);
    existing.actor_isolation_sendability_profile_is_normalized =
        existing.actor_isolation_sendability_profile_is_normalized ||
        fn.actor_isolation_sendability_profile_is_normalized;
    existing.deterministic_actor_isolation_sendability_handoff =
        existing.deterministic_actor_isolation_sendability_handoff ||
        fn.deterministic_actor_isolation_sendability_handoff;
    existing.actor_isolation_sendability_sites = std::max(
        existing.actor_isolation_sendability_sites,
        fn.actor_isolation_sendability_sites);
    existing.actor_isolation_decl_sites = std::max(
        existing.actor_isolation_decl_sites, fn.actor_isolation_decl_sites);
    existing.actor_hop_sites =
        std::max(existing.actor_hop_sites, fn.actor_hop_sites);
    existing.sendable_annotation_sites = std::max(
        existing.sendable_annotation_sites, fn.sendable_annotation_sites);
    existing.non_sendable_crossing_sites = std::max(
        existing.non_sendable_crossing_sites, fn.non_sendable_crossing_sites);
    existing.isolation_boundary_sites = std::max(
        existing.isolation_boundary_sites, fn.isolation_boundary_sites);
    existing.actor_isolation_sendability_normalized_sites = std::max(
        existing.actor_isolation_sendability_normalized_sites,
        fn.actor_isolation_sendability_normalized_sites);
    existing.actor_isolation_sendability_gate_blocked_sites = std::max(
        existing.actor_isolation_sendability_gate_blocked_sites,
        fn.actor_isolation_sendability_gate_blocked_sites);
    existing.actor_isolation_sendability_contract_violation_sites = std::max(
        existing.actor_isolation_sendability_contract_violation_sites,
        fn.actor_isolation_sendability_contract_violation_sites);
    existing.task_runtime_cancellation_profile_is_normalized =
        existing.task_runtime_cancellation_profile_is_normalized ||
        fn.task_runtime_cancellation_profile_is_normalized;
    existing.deterministic_task_runtime_cancellation_handoff =
        existing.deterministic_task_runtime_cancellation_handoff ||
        fn.deterministic_task_runtime_cancellation_handoff;
    existing.task_runtime_interop_sites = std::max(
        existing.task_runtime_interop_sites, fn.task_runtime_interop_sites);
    existing.runtime_hook_sites =
        std::max(existing.runtime_hook_sites, fn.runtime_hook_sites);
    existing.cancellation_check_sites = std::max(
        existing.cancellation_check_sites, fn.cancellation_check_sites);
    existing.cancellation_handler_sites = std::max(
        existing.cancellation_handler_sites, fn.cancellation_handler_sites);
    existing.suspension_point_sites = std::max(
        existing.suspension_point_sites, fn.suspension_point_sites);
    existing.cancellation_propagation_sites = std::max(
        existing.cancellation_propagation_sites,
        fn.cancellation_propagation_sites);
    existing.task_runtime_cancellation_normalized_sites = std::max(
        existing.task_runtime_cancellation_normalized_sites,
        fn.task_runtime_cancellation_normalized_sites);
    existing.task_runtime_cancellation_gate_blocked_sites = std::max(
        existing.task_runtime_cancellation_gate_blocked_sites,
        fn.task_runtime_cancellation_gate_blocked_sites);
    existing.task_runtime_cancellation_contract_violation_sites = std::max(
        existing.task_runtime_cancellation_contract_violation_sites,
        fn.task_runtime_cancellation_contract_violation_sites);
    existing.concurrency_replay_race_guard_profile_is_normalized =
        existing.concurrency_replay_race_guard_profile_is_normalized ||
        fn.concurrency_replay_race_guard_profile_is_normalized;
    existing.deterministic_concurrency_replay_race_guard_handoff =
        existing.deterministic_concurrency_replay_race_guard_handoff ||
        fn.deterministic_concurrency_replay_race_guard_handoff;
    existing.concurrency_replay_race_guard_sites = std::max(
        existing.concurrency_replay_race_guard_sites,
        fn.concurrency_replay_race_guard_sites);
    existing.concurrency_replay_sites =
        std::max(existing.concurrency_replay_sites, fn.concurrency_replay_sites);
    existing.replay_proof_sites =
        std::max(existing.replay_proof_sites, fn.replay_proof_sites);
    existing.race_guard_sites =
        std::max(existing.race_guard_sites, fn.race_guard_sites);
    existing.task_handoff_sites =
        std::max(existing.task_handoff_sites, fn.task_handoff_sites);
    existing.actor_isolation_sites =
        std::max(existing.actor_isolation_sites, fn.actor_isolation_sites);
    existing.deterministic_schedule_sites = std::max(
        existing.deterministic_schedule_sites, fn.deterministic_schedule_sites);
    existing.concurrency_replay_guard_blocked_sites = std::max(
        existing.concurrency_replay_guard_blocked_sites,
        fn.concurrency_replay_guard_blocked_sites);
    existing.concurrency_replay_contract_violation_sites = std::max(
        existing.concurrency_replay_contract_violation_sites,
        fn.concurrency_replay_contract_violation_sites);
