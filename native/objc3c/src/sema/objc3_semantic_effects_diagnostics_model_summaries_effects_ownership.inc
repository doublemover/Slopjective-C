Objc3EffectsOwnershipSemanticModelSummary BuildEffectsOwnershipSemanticModelSummary(
    const Objc3SemanticIntegrationSurface &surface,
    const Objc3ErrorHandlingErrorSemanticModelSummary &error_summary,
    const Objc3ConcurrencyAsyncEffectSuspensionSemanticModelSummary &async_summary,
    const Objc3ConcurrencyTaskExecutorCancellationSemanticModelSummary &task_summary,
    const Objc3ConcurrencyActorIsolationSendableSemanticModelSummary &actor_summary,
    const Objc3InteropInteropSemanticModelSummary &interop_summary) {
  Objc3EffectsOwnershipSemanticModelSummary summary;
  const auto &retain_release = surface.retain_release_operation_summary;
  const auto &weak_unowned = surface.weak_unowned_semantics_summary;
  const auto &autoreleasepool = surface.autoreleasepool_scope_summary;
  const auto &block_storage = surface.block_storage_escape_semantics_summary;
  const auto &block_copy_dispose = surface.block_copy_dispose_semantics_summary;
  const auto &throws_summary = surface.throws_propagation_summary;
  const auto &unwind_summary = surface.unwind_cleanup_summary;
  const auto &bridge_summary = surface.ns_error_bridging_summary;

  summary.arc_ownership_qualified_sites =
      retain_release.ownership_qualified_sites;
  summary.retain_insertion_sites = retain_release.retain_insertion_sites;
  summary.release_insertion_sites = retain_release.release_insertion_sites;
  summary.autorelease_insertion_sites =
      retain_release.autorelease_insertion_sites;
  summary.weak_zeroing_sites = weak_unowned.weak_reference_sites;
  summary.unowned_reference_sites = weak_unowned.unowned_reference_sites +
                                    weak_unowned.unowned_safe_reference_sites;
  summary.weak_unowned_conflict_sites =
      weak_unowned.weak_unowned_conflict_sites;
  summary.autoreleasepool_scope_sites = autoreleasepool.scope_sites;
  summary.cleanup_order_exit_sites = unwind_summary.cleanup_action_sites +
                                     autoreleasepool.scope_symbolized_sites;
  summary.block_literal_sites = block_copy_dispose.block_literal_sites;
  summary.stack_to_heap_promotion_sites = block_storage.escape_to_heap_sites;
  summary.byref_forwarding_cell_sites = block_storage.byref_slot_count_total;
  summary.copy_helper_required_sites =
      block_copy_dispose.copy_helper_required_sites;
  summary.dispose_helper_required_sites =
      block_copy_dispose.dispose_helper_required_sites;
  summary.copy_helper_symbolized_sites =
      block_copy_dispose.copy_helper_symbolized_sites;
  summary.dispose_helper_symbolized_sites =
      block_copy_dispose.dispose_helper_symbolized_sites;
  summary.captured_object_lifetime_sites =
      block_copy_dispose.capture_entries_total;
  summary.throws_propagation_sites =
      throws_summary.throws_propagation_sites +
      error_summary.throws_declaration_sites;
  summary.unwind_cleanup_sites = unwind_summary.unwind_cleanup_sites;
  summary.bridged_error_sites = bridge_summary.ns_error_bridging_sites +
                                bridge_summary.bridge_boundary_sites +
                                error_summary.ns_error_bridging_sites;
  summary.nested_cleanup_sites =
      unwind_summary.cleanup_scope_sites + unwind_summary.cleanup_resume_sites;
  summary.foreign_boundary_sites = interop_summary.foreign_callable_sites +
                                   interop_summary.bridge_callable_sites +
                                   interop_summary.async_executor_affinity_sites;
  summary.async_continuation_sites = async_summary.async_continuation_sites;
  summary.continuation_resume_sites = async_summary.continuation_resume_sites;
  summary.continuation_suspend_sites =
      async_summary.continuation_suspend_sites;
  summary.async_state_machine_sites = async_summary.async_state_machine_sites;
  summary.cancellation_propagation_sites =
      task_summary.cancellation_propagation_sites;
  summary.actor_isolation_sites =
      actor_summary.actor_isolation_sendability_sites;
  summary.actor_hop_sites = actor_summary.actor_hop_sites;
  summary.sendability_check_sites = actor_summary.sendable_annotation_sites +
                                    actor_summary.non_sendable_crossing_sites;
  summary.reentrancy_policy_sites =
      actor_summary.actor_member_executor_annotation_sites;
  summary.imported_actor_api_sites = actor_summary.actor_interface_sites;
  const std::size_t task_cancellation_violations =
      task_summary.cancellation_propagation_sites -
      std::min(task_summary.cancellation_propagation_sites,
               task_summary.cancellation_check_sites +
                   task_summary.cancellation_handler_sites);
  summary.contract_violation_sites =
      retain_release.contract_violation_sites +
      weak_unowned.contract_violation_sites +
      block_storage.contract_violation_sites +
      block_copy_dispose.contract_violation_sites +
      throws_summary.contract_violation_sites +
      unwind_summary.contract_violation_sites +
      bridge_summary.contract_violation_sites +
      (async_summary.failure_reason.empty() ? 0u : 1u) +
      task_cancellation_violations +
      actor_summary.contract_violation_sites;

  summary.arc_semantics_landed =
      summary.arc_ownership_qualified_sites > 0 ||
      summary.retain_insertion_sites > 0 || summary.release_insertion_sites > 0 ||
      summary.autorelease_insertion_sites > 0 ||
      summary.weak_zeroing_sites > 0 || summary.autoreleasepool_scope_sites > 0;
  summary.block_escape_semantics_landed =
      summary.block_literal_sites > 0 &&
      summary.copy_helper_required_sites <= summary.block_literal_sites &&
      summary.dispose_helper_required_sites <= summary.block_literal_sites;
  summary.throws_cleanup_semantics_landed =
      summary.throws_propagation_sites > 0 || summary.unwind_cleanup_sites > 0 ||
      summary.bridged_error_sites > 0;
  summary.async_task_semantics_landed =
      summary.async_continuation_sites > 0 ||
      summary.cancellation_propagation_sites > 0;
  summary.actor_semantics_landed =
      summary.actor_isolation_sites > 0 || summary.actor_hop_sites > 0 ||
      summary.sendability_check_sites > 0 ||
      summary.imported_actor_api_sites > 0 ||
      actor_summary.actor_member_metadata_sites > 0;
  summary.foreign_boundary_semantics_landed =
      summary.foreign_boundary_sites > 0 || interop_summary.deterministic;
  summary.deterministic =
      retain_release.deterministic && weak_unowned.deterministic &&
      autoreleasepool.deterministic && block_storage.deterministic &&
      block_copy_dispose.deterministic && throws_summary.deterministic &&
      unwind_summary.deterministic && bridge_summary.deterministic &&
      error_summary.deterministic && async_summary.deterministic &&
      task_summary.deterministic && actor_summary.deterministic &&
      interop_summary.deterministic &&
      summary.copy_helper_required_sites <= summary.block_literal_sites &&
      summary.dispose_helper_required_sites <= summary.block_literal_sites &&
      summary.stack_to_heap_promotion_sites <= summary.block_literal_sites &&
      summary.weak_unowned_conflict_sites <=
          weak_unowned.ownership_candidate_sites &&
      summary.contract_violation_sites == 0;
  summary.ready_for_lowering_and_runtime = summary.deterministic;
  if (!summary.deterministic) {
    summary.failure_reason =
        "effects/ownership semantic model is not deterministic or has contract violations";
  }

  std::ostringstream out;
  out << summary.contract_id
      << ";arc=" << summary.arc_ownership_qualified_sites << ":"
      << summary.retain_insertion_sites << ":" << summary.release_insertion_sites
      << ":" << summary.autorelease_insertion_sites
      << ";weak=" << summary.weak_zeroing_sites << ":"
      << summary.unowned_reference_sites << ":" << summary.weak_unowned_conflict_sites
      << ";autoreleasepool=" << summary.autoreleasepool_scope_sites << ":"
      << summary.cleanup_order_exit_sites
      << ";blocks=" << summary.block_literal_sites << ":"
      << summary.stack_to_heap_promotion_sites << ":"
      << summary.byref_forwarding_cell_sites << ":"
      << summary.copy_helper_required_sites << ":"
      << summary.dispose_helper_required_sites
      << ";throws=" << summary.throws_propagation_sites << ":"
      << summary.unwind_cleanup_sites << ":" << summary.bridged_error_sites
      << ":" << summary.nested_cleanup_sites
      << ";async=" << summary.async_continuation_sites << ":"
      << summary.continuation_resume_sites << ":"
      << summary.continuation_suspend_sites << ":"
      << summary.async_state_machine_sites << ":"
      << summary.cancellation_propagation_sites
      << ";actors=" << summary.actor_isolation_sites << ":"
      << summary.actor_hop_sites << ":" << summary.sendability_check_sites << ":"
      << summary.reentrancy_policy_sites << ":"
      << summary.imported_actor_api_sites
      << ";foreign=" << summary.foreign_boundary_sites
      << ";violations=" << summary.contract_violation_sites;
  summary.replay_key = out.str();
  return summary;
}
