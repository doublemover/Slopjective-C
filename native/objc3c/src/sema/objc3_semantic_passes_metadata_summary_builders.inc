  summary.pointer_declarator_sites = type_annotation_summary.pointer_declarator_sites;
  summary.normalized_protocol_composition_sites =
      std::min(summary.terminated_protocol_composition_sites, summary.object_pointer_type_sites);
  summary.contract_violation_sites =
      type_annotation_summary.invalid_generic_suffix_sites +
      (summary.protocol_composition_sites > summary.object_pointer_type_sites
           ? summary.protocol_composition_sites - summary.object_pointer_type_sites
           : 0u);
  summary.deterministic =
      type_annotation_summary.deterministic &&
      summary.contract_violation_sites == 0u &&
      summary.terminated_protocol_composition_sites <= summary.protocol_composition_sites &&
      summary.normalized_protocol_composition_sites <= summary.protocol_qualified_object_type_sites &&
      summary.contract_violation_sites <= summary.protocol_qualified_object_type_sites;
  return summary;
}

#include "sema/objc3_semantic_passes_type_metadata_module_summary_builders.inc"

static Objc3UnwindCleanupSummary
BuildUnwindCleanupSummaryFromThrowsAndResultSummaries(
    const Objc3ThrowsPropagationSummary &throws_summary,
    const Objc3ResultLikeLoweringSummary &result_summary) {
  Objc3UnwindCleanupSummary summary;
  summary.unwind_cleanup_sites = throws_summary.throws_propagation_sites;

  const auto saturating_add = [&](std::size_t lhs, std::size_t rhs) {
    if (rhs > std::numeric_limits<std::size_t>::max() - lhs) {
      summary.deterministic = false;
      return std::numeric_limits<std::size_t>::max();
    }
    return lhs + rhs;
  };

  summary.exceptional_exit_sites = std::min(
      summary.unwind_cleanup_sites,
      saturating_add(throws_summary.namespace_segment_sites,
                     throws_summary.import_edge_candidate_sites));
  summary.cleanup_action_sites = std::min(
      summary.unwind_cleanup_sites,
      saturating_add(throws_summary.cache_invalidation_candidate_sites,
                     result_summary.result_payload_sites));
  summary.cleanup_scope_sites = std::min(summary.unwind_cleanup_sites,
                                         result_summary.branch_merge_sites);
  summary.cleanup_resume_sites = std::min(summary.unwind_cleanup_sites,
                                          result_summary.result_success_sites);
  summary.normalized_sites =
      std::min(summary.unwind_cleanup_sites, throws_summary.normalized_sites);
  summary.fail_closed_sites =
      summary.unwind_cleanup_sites - summary.normalized_sites;
  summary.contract_violation_sites = std::min(
      summary.unwind_cleanup_sites,
      saturating_add(throws_summary.contract_violation_sites,
                     result_summary.contract_violation_sites));
  summary.deterministic =
      summary.deterministic &&
      throws_summary.deterministic &&
      result_summary.deterministic &&
      summary.exceptional_exit_sites <= summary.unwind_cleanup_sites &&
      summary.cleanup_action_sites <= summary.unwind_cleanup_sites &&
      summary.cleanup_scope_sites <= summary.unwind_cleanup_sites &&
      summary.cleanup_resume_sites <= summary.unwind_cleanup_sites &&
      summary.normalized_sites <= summary.unwind_cleanup_sites &&
      summary.fail_closed_sites <= summary.unwind_cleanup_sites &&
      summary.contract_violation_sites <= summary.unwind_cleanup_sites &&
      summary.normalized_sites + summary.fail_closed_sites ==
          summary.unwind_cleanup_sites;
  return summary;
}

#include "sema/objc3_semantic_passes_unsafe_intrinsic_summary_builders.inc"

static Objc3ResultLikeLoweringSummary BuildResultLikeLoweringSummaryFromProgramAst(
    const Objc3Program &ast) {
  Objc3ResultLikeLoweringSummary summary;

  const auto checked_add = [&](std::size_t &total, std::size_t value) {
    if (value > std::numeric_limits<std::size_t>::max() - total) {
      total = std::numeric_limits<std::size_t>::max();
      summary.deterministic = false;
      return;
    }
    total += value;
  };

  const auto is_partitioned = [](std::size_t lhs, std::size_t rhs, std::size_t total) {
    return lhs <= std::numeric_limits<std::size_t>::max() - rhs && lhs + rhs == total;
  };

  const auto accumulate_result_like_profile = [&](const auto &declaration) {
    checked_add(summary.result_like_sites, declaration.result_like_sites);
    checked_add(summary.result_success_sites, declaration.result_success_sites);
    checked_add(summary.result_failure_sites, declaration.result_failure_sites);
    checked_add(summary.result_branch_sites, declaration.result_branch_sites);
    checked_add(summary.result_payload_sites, declaration.result_payload_sites);
    checked_add(summary.normalized_sites, declaration.result_normalized_sites);
    checked_add(summary.branch_merge_sites, declaration.result_branch_merge_sites);
    checked_add(summary.contract_violation_sites, declaration.result_contract_violation_sites);

    const bool profile_contract_normalized =
        declaration.result_success_sites <= declaration.result_like_sites &&
        declaration.result_failure_sites <= declaration.result_like_sites &&
        declaration.result_branch_sites <= declaration.result_like_sites &&
        declaration.result_payload_sites <= declaration.result_like_sites &&
        declaration.result_normalized_sites <= declaration.result_like_sites &&
        declaration.result_branch_merge_sites <= declaration.result_like_sites &&
        declaration.result_contract_violation_sites <= declaration.result_like_sites &&
        is_partitioned(declaration.result_success_sites,
                       declaration.result_failure_sites,
                       declaration.result_normalized_sites) &&
        is_partitioned(declaration.result_normalized_sites,
                       declaration.result_branch_merge_sites,
                       declaration.result_like_sites);
    summary.deterministic =
        summary.deterministic && declaration.result_like_profile_is_normalized &&
        declaration.deterministic_result_like_lowering_handoff &&
        profile_contract_normalized;
  };

  for (const auto &function_decl : ast.functions) {
    accumulate_result_like_profile(function_decl);
  }

  for (const auto &interface_decl : ast.interfaces) {
    for (const auto &method_decl : interface_decl.methods) {
      accumulate_result_like_profile(method_decl);
    }
  }

  for (const auto &implementation_decl : ast.implementations) {
    for (const auto &method_decl : implementation_decl.methods) {
      accumulate_result_like_profile(method_decl);
    }
  }

  summary.deterministic =
      summary.deterministic &&
      summary.result_success_sites <= summary.result_like_sites &&
      summary.result_failure_sites <= summary.result_like_sites &&
      summary.result_branch_sites <= summary.result_like_sites &&
      summary.result_payload_sites <= summary.result_like_sites &&
      summary.normalized_sites <= summary.result_like_sites &&
      summary.branch_merge_sites <= summary.result_like_sites &&
      summary.contract_violation_sites <= summary.result_like_sites &&
      is_partitioned(summary.result_success_sites,
                     summary.result_failure_sites,
                     summary.normalized_sites) &&
      is_partitioned(summary.normalized_sites,
                     summary.branch_merge_sites,
                     summary.result_like_sites);
  return summary;
}

static Objc3AwaitLoweringSuspensionStateSummary
BuildAwaitLoweringSuspensionStateSummaryFromProgramAst(
    const Objc3Program &ast) {
  Objc3AwaitLoweringSuspensionStateSummary summary;

  const auto checked_add = [&](std::size_t &total, std::size_t value) {
    if (value > std::numeric_limits<std::size_t>::max() - total) {
      total = std::numeric_limits<std::size_t>::max();
      summary.deterministic = false;
      return;
    }
    total += value;
  };

  const auto is_partitioned = [](std::size_t lhs, std::size_t rhs, std::size_t total) {
    return lhs <= std::numeric_limits<std::size_t>::max() - rhs && lhs + rhs == total;
  };

  const auto accumulate_await_profile = [&](const auto &declaration) {
    checked_add(summary.await_suspension_sites, declaration.await_suspension_sites);
    checked_add(summary.await_keyword_sites, declaration.await_keyword_sites);
    checked_add(summary.await_suspension_point_sites,
                declaration.await_suspension_point_sites);
    checked_add(summary.await_resume_sites, declaration.await_resume_sites);
    checked_add(summary.await_state_machine_sites,
                declaration.await_state_machine_sites);
    checked_add(summary.await_continuation_sites,
                declaration.await_continuation_sites);
    checked_add(summary.normalized_sites,
                declaration.await_suspension_normalized_sites);
    checked_add(summary.gate_blocked_sites,
                declaration.await_suspension_gate_blocked_sites);
    checked_add(summary.contract_violation_sites,
                declaration.await_suspension_contract_violation_sites);

    const bool profile_contract_normalized =
        declaration.await_keyword_sites <= declaration.await_suspension_sites &&
        declaration.await_suspension_point_sites <=
            declaration.await_suspension_sites &&
        declaration.await_resume_sites <= declaration.await_suspension_sites &&
        declaration.await_state_machine_sites <=
            declaration.await_suspension_point_sites &&
        declaration.await_continuation_sites <=
            declaration.await_suspension_point_sites &&
        declaration.await_suspension_normalized_sites <=
            declaration.await_suspension_sites &&
        declaration.await_suspension_gate_blocked_sites <=
            declaration.await_suspension_sites &&
        declaration.await_suspension_contract_violation_sites <=
            declaration.await_suspension_sites &&
        is_partitioned(declaration.await_suspension_normalized_sites,
                       declaration.await_suspension_gate_blocked_sites,
                       declaration.await_suspension_sites);
    summary.deterministic =
        summary.deterministic && declaration.await_suspension_profile_is_normalized &&
        declaration.deterministic_await_suspension_handoff &&
        profile_contract_normalized;
  };

  for (const auto &function_decl : ast.functions) {
    accumulate_await_profile(function_decl);
  }

  for (const auto &interface_decl : ast.interfaces) {
    for (const auto &method_decl : interface_decl.methods) {
      accumulate_await_profile(method_decl);
    }
  }

  for (const auto &implementation_decl : ast.implementations) {
    for (const auto &method_decl : implementation_decl.methods) {
      accumulate_await_profile(method_decl);
    }
  }

  summary.deterministic =
      summary.deterministic &&
      summary.await_keyword_sites <= summary.await_suspension_sites &&
      summary.await_suspension_point_sites <= summary.await_suspension_sites &&
      summary.await_resume_sites <= summary.await_suspension_sites &&
      summary.await_state_machine_sites <= summary.await_suspension_point_sites &&
      summary.await_continuation_sites <= summary.await_suspension_point_sites &&
      summary.normalized_sites <= summary.await_suspension_sites &&
      summary.gate_blocked_sites <= summary.await_suspension_sites &&
      summary.contract_violation_sites <= summary.await_suspension_sites &&
      is_partitioned(summary.normalized_sites,
                     summary.gate_blocked_sites,
                     summary.await_suspension_sites);
  return summary;
}

static Objc3ErrorDiagnosticsRecoverySummary
BuildErrorDiagnosticsRecoverySummaryFromProgramAst(const Objc3Program &ast) {
  Objc3ErrorDiagnosticsRecoverySummary summary;

  const auto checked_add = [&](std::size_t &total, std::size_t value) {
    if (value > std::numeric_limits<std::size_t>::max() - total) {
      total = std::numeric_limits<std::size_t>::max();
      summary.deterministic = false;
      return;
    }
    total += value;
  };

  const auto is_partitioned = [](std::size_t lhs, std::size_t rhs,
                                 std::size_t total) {
    return lhs <= std::numeric_limits<std::size_t>::max() - rhs &&
           lhs + rhs == total;
  };

  const auto accumulate_error_diagnostics_recovery_profile =
      [&](const auto &declaration) {
        checked_add(summary.error_diagnostics_recovery_sites,
                    declaration.error_diagnostics_recovery_sites);
        checked_add(summary.diagnostic_emit_sites,
                    declaration.diagnostic_emit_sites);
        checked_add(summary.recovery_anchor_sites,
                    declaration.recovery_anchor_sites);
        checked_add(summary.recovery_boundary_sites,
                    declaration.recovery_boundary_sites);
        checked_add(summary.fail_closed_diagnostic_sites,
                    declaration.fail_closed_diagnostic_sites);
        checked_add(summary.normalized_sites,
                    declaration.error_diagnostics_recovery_normalized_sites);
        checked_add(summary.gate_blocked_sites,
                    declaration.error_diagnostics_recovery_gate_blocked_sites);
        checked_add(
            summary.contract_violation_sites,
            declaration.error_diagnostics_recovery_contract_violation_sites);

        const bool profile_contract_normalized =
            declaration.diagnostic_emit_sites <=
                declaration.error_diagnostics_recovery_sites &&
            declaration.recovery_anchor_sites <=
                declaration.error_diagnostics_recovery_sites &&
            declaration.recovery_boundary_sites <=
                declaration.error_diagnostics_recovery_sites &&
            declaration.fail_closed_diagnostic_sites <=
                declaration.error_diagnostics_recovery_sites &&
            declaration.error_diagnostics_recovery_normalized_sites <=
                declaration.error_diagnostics_recovery_sites &&
            declaration.error_diagnostics_recovery_gate_blocked_sites <=
                declaration.error_diagnostics_recovery_sites &&
            declaration.error_diagnostics_recovery_contract_violation_sites <=
                declaration.error_diagnostics_recovery_sites &&
            is_partitioned(
                declaration.error_diagnostics_recovery_normalized_sites,
                declaration.error_diagnostics_recovery_gate_blocked_sites,
                declaration.error_diagnostics_recovery_sites);
        summary.deterministic =
            summary.deterministic &&
            declaration.error_diagnostics_recovery_profile_is_normalized &&
            declaration.deterministic_error_diagnostics_recovery_handoff &&
            profile_contract_normalized;
      };

  for (const auto &function_decl : ast.functions) {
    accumulate_error_diagnostics_recovery_profile(function_decl);
  }

  for (const auto &interface_decl : ast.interfaces) {
    for (const auto &method_decl : interface_decl.methods) {
      accumulate_error_diagnostics_recovery_profile(method_decl);
    }
  }

  for (const auto &implementation_decl : ast.implementations) {
    for (const auto &method_decl : implementation_decl.methods) {
      accumulate_error_diagnostics_recovery_profile(method_decl);
    }
  }

  summary.deterministic =
      summary.deterministic &&
      summary.diagnostic_emit_sites <= summary.error_diagnostics_recovery_sites &&
      summary.recovery_anchor_sites <= summary.error_diagnostics_recovery_sites &&
      summary.recovery_boundary_sites <=
          summary.error_diagnostics_recovery_sites &&
      summary.fail_closed_diagnostic_sites <=
          summary.error_diagnostics_recovery_sites &&
      summary.normalized_sites <= summary.error_diagnostics_recovery_sites &&
      summary.gate_blocked_sites <= summary.error_diagnostics_recovery_sites &&
      summary.contract_violation_sites <=
          summary.error_diagnostics_recovery_sites &&
      is_partitioned(summary.normalized_sites, summary.gate_blocked_sites,
                     summary.error_diagnostics_recovery_sites);
  return summary;
}

template <typename SiteMetadata>
static void AccumulateAsyncContinuationSummaryFromSiteMetadata(
    const SiteMetadata &metadata,
    Objc3AsyncContinuationSummary &summary) {
  summary.async_continuation_sites += metadata.async_continuation_sites;
  summary.async_keyword_sites += metadata.async_keyword_sites;
  summary.async_function_sites += metadata.async_function_sites;
  summary.continuation_allocation_sites += metadata.continuation_allocation_sites;
  summary.continuation_resume_sites += metadata.continuation_resume_sites;
  summary.continuation_suspend_sites += metadata.continuation_suspend_sites;
  summary.async_state_machine_sites += metadata.async_state_machine_sites;
  summary.normalized_sites += metadata.async_continuation_normalized_sites;
  summary.gate_blocked_sites += metadata.async_continuation_gate_blocked_sites;
  summary.contract_violation_sites += metadata.async_continuation_contract_violation_sites;
  summary.deterministic =
      summary.deterministic &&
      metadata.deterministic_async_continuation_handoff &&
      (metadata.async_continuation_sites == 0u ||
       metadata.async_continuation_profile_is_normalized);
}

static void FinalizeAsyncContinuationSummaryDeterminism(
    Objc3AsyncContinuationSummary &summary) {
  summary.deterministic =
      summary.deterministic &&
      summary.async_keyword_sites <= summary.async_continuation_sites &&
      summary.async_function_sites <= summary.async_continuation_sites &&
      summary.continuation_allocation_sites <= summary.async_continuation_sites &&
      summary.continuation_resume_sites <= summary.async_continuation_sites &&
      summary.continuation_suspend_sites <= summary.async_continuation_sites &&
      summary.async_state_machine_sites <= summary.async_continuation_sites &&
      summary.normalized_sites <= summary.async_continuation_sites &&
      summary.gate_blocked_sites <= summary.async_continuation_sites &&
      summary.contract_violation_sites <= summary.async_continuation_sites &&
      summary.normalized_sites + summary.gate_blocked_sites ==
          summary.async_continuation_sites;
}

static Objc3AsyncContinuationSummary
BuildAsyncContinuationSummaryFromIntegrationSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  Objc3AsyncContinuationSummary summary;
  for (const auto &entry : surface.functions) {
    AccumulateAsyncContinuationSummaryFromSiteMetadata(entry.second, summary);
  }
  for (const auto &interface_entry : surface.interfaces) {
    for (const auto &method_entry : interface_entry.second.methods) {
      AccumulateAsyncContinuationSummaryFromSiteMetadata(method_entry.second,
                                                         summary);
    }
  }
  for (const auto &implementation_entry : surface.implementations) {
    for (const auto &method_entry : implementation_entry.second.methods) {
      AccumulateAsyncContinuationSummaryFromSiteMetadata(method_entry.second,
                                                         summary);
    }
  }
  FinalizeAsyncContinuationSummaryDeterminism(summary);
  return summary;
}

static Objc3AsyncContinuationSummary
BuildAsyncContinuationSummaryFromTypeMetadataHandoff(
    const Objc3SemanticTypeMetadataHandoff &handoff) {
  Objc3AsyncContinuationSummary summary;
  for (const auto &metadata : handoff.functions_lexicographic) {
    AccumulateAsyncContinuationSummaryFromSiteMetadata(metadata, summary);
  }
  for (const auto &interface_metadata : handoff.interfaces_lexicographic) {
    for (const auto &method_metadata :
         interface_metadata.methods_lexicographic) {
      AccumulateAsyncContinuationSummaryFromSiteMetadata(method_metadata,
                                                         summary);
    }
  }
  for (const auto &implementation_metadata :
       handoff.implementations_lexicographic) {
    for (const auto &method_metadata :
         implementation_metadata.methods_lexicographic) {
      AccumulateAsyncContinuationSummaryFromSiteMetadata(method_metadata,
                                                         summary);
    }
  }
  FinalizeAsyncContinuationSummaryDeterminism(summary);
  return summary;
}

template <typename SiteMetadata>
static void AccumulateActorIsolationSendabilitySummaryFromSiteMetadata(
    const SiteMetadata &metadata,
    Objc3ActorIsolationSendabilitySummary &summary) {
  summary.actor_isolation_sendability_sites +=
      metadata.actor_isolation_sendability_sites;
  summary.actor_isolation_decl_sites += metadata.actor_isolation_decl_sites;
  summary.actor_hop_sites += metadata.actor_hop_sites;
  summary.sendable_annotation_sites += metadata.sendable_annotation_sites;
  summary.non_sendable_crossing_sites += metadata.non_sendable_crossing_sites;
  summary.isolation_boundary_sites += metadata.isolation_boundary_sites;
  summary.normalized_sites +=
      metadata.actor_isolation_sendability_normalized_sites;
  summary.gate_blocked_sites +=
      metadata.actor_isolation_sendability_gate_blocked_sites;
  summary.contract_violation_sites +=
      metadata.actor_isolation_sendability_contract_violation_sites;
  summary.deterministic =
      summary.deterministic &&
      metadata.deterministic_actor_isolation_sendability_handoff &&
      (metadata.actor_isolation_sendability_sites == 0u ||
       metadata.actor_isolation_sendability_profile_is_normalized);
}

static void FinalizeActorIsolationSendabilitySummaryDeterminism(
    Objc3ActorIsolationSendabilitySummary &summary) {
  summary.deterministic =
      summary.deterministic &&
      summary.actor_isolation_decl_sites <=
          summary.actor_isolation_sendability_sites &&
      summary.actor_hop_sites <= summary.actor_isolation_sendability_sites &&
      summary.sendable_annotation_sites <=
          summary.actor_isolation_sendability_sites &&
      summary.non_sendable_crossing_sites <=
          summary.actor_isolation_sendability_sites &&
      summary.isolation_boundary_sites <=
          summary.actor_isolation_sendability_sites &&
      summary.normalized_sites <= summary.actor_isolation_sendability_sites &&
      summary.gate_blocked_sites <= summary.actor_isolation_sendability_sites &&
      summary.gate_blocked_sites <= summary.non_sendable_crossing_sites &&
      summary.contract_violation_sites <=
          summary.actor_isolation_sendability_sites &&
      summary.normalized_sites + summary.gate_blocked_sites ==
          summary.actor_isolation_sendability_sites;
}

static Objc3ActorIsolationSendabilitySummary
BuildActorIsolationSendabilitySummaryFromIntegrationSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  Objc3ActorIsolationSendabilitySummary summary;
  for (const auto &entry : surface.functions) {
    AccumulateActorIsolationSendabilitySummaryFromSiteMetadata(entry.second,
                                                               summary);
  }
  for (const auto &interface_entry : surface.interfaces) {
    for (const auto &method_entry : interface_entry.second.methods) {
      AccumulateActorIsolationSendabilitySummaryFromSiteMetadata(
          method_entry.second, summary);
    }
  }
  for (const auto &implementation_entry : surface.implementations) {
    for (const auto &method_entry : implementation_entry.second.methods) {
      AccumulateActorIsolationSendabilitySummaryFromSiteMetadata(
          method_entry.second, summary);
    }
  }
  FinalizeActorIsolationSendabilitySummaryDeterminism(summary);
  return summary;
}

static Objc3ActorIsolationSendabilitySummary
BuildActorIsolationSendabilitySummaryFromTypeMetadataHandoff(
    const Objc3SemanticTypeMetadataHandoff &handoff) {
  Objc3ActorIsolationSendabilitySummary summary;
  for (const auto &metadata : handoff.functions_lexicographic) {
    AccumulateActorIsolationSendabilitySummaryFromSiteMetadata(metadata,
                                                               summary);
  }
  for (const auto &interface_metadata : handoff.interfaces_lexicographic) {
    for (const auto &method_metadata :
         interface_metadata.methods_lexicographic) {
      AccumulateActorIsolationSendabilitySummaryFromSiteMetadata(
          method_metadata, summary);
    }
  }
  for (const auto &implementation_metadata :
       handoff.implementations_lexicographic) {
    for (const auto &method_metadata :
         implementation_metadata.methods_lexicographic) {
      AccumulateActorIsolationSendabilitySummaryFromSiteMetadata(
          method_metadata, summary);
    }
  }
  FinalizeActorIsolationSendabilitySummaryDeterminism(summary);
  return summary;
}

template <typename SiteMetadata>
static void AccumulateTaskRuntimeCancellationSummaryFromSiteMetadata(
    const SiteMetadata &metadata,
    Objc3TaskRuntimeCancellationSummary &summary) {
  summary.task_runtime_interop_sites += metadata.task_runtime_interop_sites;
  summary.runtime_hook_sites += metadata.runtime_hook_sites;
  summary.cancellation_check_sites += metadata.cancellation_check_sites;
  summary.cancellation_handler_sites += metadata.cancellation_handler_sites;
  summary.suspension_point_sites += metadata.suspension_point_sites;
  summary.cancellation_propagation_sites +=
      metadata.cancellation_propagation_sites;
  summary.normalized_sites += metadata.task_runtime_cancellation_normalized_sites;
  summary.gate_blocked_sites +=
      metadata.task_runtime_cancellation_gate_blocked_sites;
  summary.contract_violation_sites +=
      metadata.task_runtime_cancellation_contract_violation_sites;
  summary.deterministic =
      summary.deterministic &&
      metadata.deterministic_task_runtime_cancellation_handoff &&
      (metadata.task_runtime_interop_sites == 0u ||
       metadata.task_runtime_cancellation_profile_is_normalized);
}

static void FinalizeTaskRuntimeCancellationSummaryDeterminism(
    Objc3TaskRuntimeCancellationSummary &summary) {
  summary.deterministic =
      summary.deterministic &&
      summary.runtime_hook_sites <= summary.task_runtime_interop_sites &&
      summary.cancellation_check_sites <= summary.task_runtime_interop_sites &&
      summary.cancellation_handler_sites <= summary.task_runtime_interop_sites &&
      summary.suspension_point_sites <= summary.task_runtime_interop_sites &&
      summary.cancellation_propagation_sites <=
          summary.cancellation_check_sites &&
      summary.cancellation_propagation_sites <=
          summary.cancellation_handler_sites &&
      summary.normalized_sites <= summary.task_runtime_interop_sites &&
      summary.gate_blocked_sites <= summary.task_runtime_interop_sites &&
      summary.gate_blocked_sites <= summary.cancellation_propagation_sites &&
      summary.contract_violation_sites <= summary.task_runtime_interop_sites &&
      summary.normalized_sites + summary.gate_blocked_sites ==
          summary.task_runtime_interop_sites;
}

static Objc3TaskRuntimeCancellationSummary
BuildTaskRuntimeCancellationSummaryFromIntegrationSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  Objc3TaskRuntimeCancellationSummary summary;
  for (const auto &entry : surface.functions) {
    AccumulateTaskRuntimeCancellationSummaryFromSiteMetadata(entry.second,
                                                             summary);
  }
  for (const auto &interface_entry : surface.interfaces) {
    for (const auto &method_entry : interface_entry.second.methods) {
      AccumulateTaskRuntimeCancellationSummaryFromSiteMetadata(
          method_entry.second, summary);
    }
  }
  for (const auto &implementation_entry : surface.implementations) {
    for (const auto &method_entry : implementation_entry.second.methods) {
      AccumulateTaskRuntimeCancellationSummaryFromSiteMetadata(
          method_entry.second, summary);
    }
  }
  FinalizeTaskRuntimeCancellationSummaryDeterminism(summary);
  return summary;
}

static Objc3TaskRuntimeCancellationSummary
BuildTaskRuntimeCancellationSummaryFromTypeMetadataHandoff(
    const Objc3SemanticTypeMetadataHandoff &handoff) {
  Objc3TaskRuntimeCancellationSummary summary;
  for (const auto &metadata : handoff.functions_lexicographic) {
    AccumulateTaskRuntimeCancellationSummaryFromSiteMetadata(metadata, summary);
  }
  for (const auto &interface_metadata : handoff.interfaces_lexicographic) {
    for (const auto &method_metadata :
         interface_metadata.methods_lexicographic) {
      AccumulateTaskRuntimeCancellationSummaryFromSiteMetadata(method_metadata,
                                                               summary);
    }
  }
  for (const auto &implementation_metadata :
       handoff.implementations_lexicographic) {
    for (const auto &method_metadata :
         implementation_metadata.methods_lexicographic) {
      AccumulateTaskRuntimeCancellationSummaryFromSiteMetadata(method_metadata,
                                                               summary);
    }
  }
  FinalizeTaskRuntimeCancellationSummaryDeterminism(summary);
  return summary;
}

template <typename SiteMetadata>
static void AccumulateConcurrencyReplayRaceGuardSummaryFromSiteMetadata(
    const SiteMetadata &metadata,
    Objc3ConcurrencyReplayRaceGuardSummary &summary) {
  const std::size_t aggregate_sites =
      std::max({metadata.concurrency_replay_race_guard_sites,
                metadata.concurrency_replay_sites,
                metadata.replay_proof_sites,
                metadata.race_guard_sites,
                metadata.task_handoff_sites,
                metadata.actor_isolation_sites});
  summary.concurrency_replay_race_guard_sites += aggregate_sites;
  summary.concurrency_replay_sites += metadata.concurrency_replay_sites;
  summary.replay_proof_sites += metadata.replay_proof_sites;
  summary.race_guard_sites += metadata.race_guard_sites;
  summary.task_handoff_sites += metadata.task_handoff_sites;
  summary.actor_isolation_sites += metadata.actor_isolation_sites;
  summary.deterministic_schedule_sites += metadata.deterministic_schedule_sites;
  summary.guard_blocked_sites += metadata.concurrency_replay_guard_blocked_sites;
  summary.contract_violation_sites +=
      metadata.concurrency_replay_contract_violation_sites;
  summary.deterministic =
      summary.deterministic &&
      metadata.deterministic_concurrency_replay_race_guard_handoff &&
      (aggregate_sites == 0u ||
       metadata.concurrency_replay_race_guard_profile_is_normalized);
}

static void FinalizeConcurrencyReplayRaceGuardSummaryDeterminism(
    Objc3ConcurrencyReplayRaceGuardSummary &summary) {
  summary.deterministic =
      summary.deterministic &&
      summary.concurrency_replay_sites <=
          summary.concurrency_replay_race_guard_sites &&
      summary.replay_proof_sites <=
          summary.concurrency_replay_race_guard_sites &&
      summary.race_guard_sites <= summary.concurrency_replay_race_guard_sites &&
      summary.task_handoff_sites <= summary.concurrency_replay_race_guard_sites &&
      summary.actor_isolation_sites <=
          summary.concurrency_replay_race_guard_sites &&
      summary.deterministic_schedule_sites <= summary.concurrency_replay_sites &&
      summary.guard_blocked_sites <= summary.concurrency_replay_sites &&
      summary.contract_violation_sites <=
          summary.concurrency_replay_race_guard_sites &&
      summary.deterministic_schedule_sites + summary.guard_blocked_sites ==
          summary.concurrency_replay_sites;
}

static Objc3ConcurrencyReplayRaceGuardSummary
BuildConcurrencyReplayRaceGuardSummaryFromIntegrationSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  Objc3ConcurrencyReplayRaceGuardSummary summary;
  for (const auto &entry : surface.functions) {
    AccumulateConcurrencyReplayRaceGuardSummaryFromSiteMetadata(entry.second,
                                                                summary);
  }
  for (const auto &interface_entry : surface.interfaces) {
    for (const auto &method_entry : interface_entry.second.methods) {
      AccumulateConcurrencyReplayRaceGuardSummaryFromSiteMetadata(
          method_entry.second, summary);
    }
  }
  for (const auto &implementation_entry : surface.implementations) {
    for (const auto &method_entry : implementation_entry.second.methods) {
      AccumulateConcurrencyReplayRaceGuardSummaryFromSiteMetadata(
          method_entry.second, summary);
    }
  }
  FinalizeConcurrencyReplayRaceGuardSummaryDeterminism(summary);
  return summary;
}

static Objc3ConcurrencyReplayRaceGuardSummary
BuildConcurrencyReplayRaceGuardSummaryFromTypeMetadataHandoff(
    const Objc3SemanticTypeMetadataHandoff &handoff) {
  Objc3ConcurrencyReplayRaceGuardSummary summary;
  for (const auto &metadata : handoff.functions_lexicographic) {
    AccumulateConcurrencyReplayRaceGuardSummaryFromSiteMetadata(metadata,
                                                                summary);
  }
  for (const auto &interface_metadata : handoff.interfaces_lexicographic) {
    for (const auto &method_metadata :
         interface_metadata.methods_lexicographic) {
      AccumulateConcurrencyReplayRaceGuardSummaryFromSiteMetadata(
          method_metadata, summary);
    }
  }
  for (const auto &implementation_metadata :
       handoff.implementations_lexicographic) {
    for (const auto &method_metadata :
         implementation_metadata.methods_lexicographic) {
      AccumulateConcurrencyReplayRaceGuardSummaryFromSiteMetadata(
          method_metadata, summary);
    }
  }
  FinalizeConcurrencyReplayRaceGuardSummaryDeterminism(summary);
  return summary;
}

template <typename SiteMetadata>
static void AccumulateNSErrorBridgingSummaryFromSiteMetadata(
    const SiteMetadata &metadata,
    Objc3NSErrorBridgingSummary &summary) {
  summary.ns_error_bridging_sites += metadata.ns_error_bridging_sites;
  summary.ns_error_parameter_sites += metadata.ns_error_parameter_sites;
  summary.ns_error_out_parameter_sites += metadata.ns_error_out_parameter_sites;
  summary.ns_error_bridge_path_sites += metadata.ns_error_bridge_path_sites;
  summary.failable_call_sites += metadata.failable_call_sites;
  summary.normalized_sites += metadata.ns_error_bridging_normalized_sites;
  summary.bridge_boundary_sites += metadata.ns_error_bridge_boundary_sites;
  summary.contract_violation_sites += metadata.ns_error_bridging_contract_violation_sites;
  summary.deterministic =
      summary.deterministic &&
      metadata.deterministic_ns_error_bridging_lowering_handoff &&
      (metadata.ns_error_bridging_sites == 0u ||
       metadata.ns_error_bridging_profile_is_normalized);
}

static void FinalizeNSErrorBridgingSummaryDeterminism(
    Objc3NSErrorBridgingSummary &summary) {
  summary.deterministic =
      summary.deterministic &&
      summary.ns_error_parameter_sites <= summary.ns_error_bridging_sites &&
      summary.ns_error_out_parameter_sites <= summary.ns_error_bridging_sites &&
      summary.ns_error_bridge_path_sites <= summary.ns_error_bridging_sites &&
      summary.failable_call_sites <= summary.ns_error_bridging_sites &&
      summary.normalized_sites <= summary.ns_error_bridging_sites &&
      summary.bridge_boundary_sites <= summary.ns_error_bridging_sites &&
      summary.normalized_sites + summary.bridge_boundary_sites ==
          summary.ns_error_bridging_sites &&
      summary.contract_violation_sites <= summary.ns_error_bridging_sites;
}

static Objc3NSErrorBridgingSummary
BuildNSErrorBridgingSummaryFromIntegrationSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  Objc3NSErrorBridgingSummary summary;
  for (const auto &entry : surface.functions) {
    AccumulateNSErrorBridgingSummaryFromSiteMetadata(entry.second, summary);
  }
  for (const auto &interface_entry : surface.interfaces) {
    for (const auto &method_entry : interface_entry.second.methods) {
      AccumulateNSErrorBridgingSummaryFromSiteMetadata(method_entry.second,
                                                       summary);
    }
  }
  for (const auto &implementation_entry : surface.implementations) {
    for (const auto &method_entry : implementation_entry.second.methods) {
      AccumulateNSErrorBridgingSummaryFromSiteMetadata(method_entry.second,
                                                       summary);
    }
  }
  FinalizeNSErrorBridgingSummaryDeterminism(summary);
  return summary;
}

static Objc3NSErrorBridgingSummary
BuildNSErrorBridgingSummaryFromTypeMetadataHandoff(
    const Objc3SemanticTypeMetadataHandoff &handoff) {
  Objc3NSErrorBridgingSummary summary;
  for (const auto &metadata : handoff.functions_lexicographic) {
    AccumulateNSErrorBridgingSummaryFromSiteMetadata(metadata, summary);
  }
  for (const auto &interface_metadata : handoff.interfaces_lexicographic) {
    for (const auto &method_metadata : interface_metadata.methods_lexicographic) {
      AccumulateNSErrorBridgingSummaryFromSiteMetadata(method_metadata, summary);
    }
  }
  for (const auto &implementation_metadata : handoff.implementations_lexicographic) {
    for (const auto &method_metadata : implementation_metadata.methods_lexicographic) {
      AccumulateNSErrorBridgingSummaryFromSiteMetadata(method_metadata, summary);
    }
  }
  FinalizeNSErrorBridgingSummaryDeterminism(summary);
  return summary;
}

static Objc3SymbolGraphScopeResolutionSummary BuildSymbolGraphScopeResolutionSummaryFromIntegrationSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  Objc3SymbolGraphScopeResolutionSummary summary;
  summary.global_symbol_nodes = surface.globals.size();
  summary.function_symbol_nodes = surface.functions.size();
  summary.interface_symbol_nodes = surface.interfaces.size();
  summary.implementation_symbol_nodes = surface.implementations.size();
  summary.top_level_scope_symbols = summary.global_symbol_nodes + summary.function_symbol_nodes +
                                    summary.interface_symbol_nodes + summary.implementation_symbol_nodes;
  summary.scope_frames_total =
      static_cast<std::size_t>(1u) + summary.function_symbol_nodes + summary.interface_symbol_nodes +
      summary.implementation_symbol_nodes;

  for (const auto &entry : surface.interfaces) {
    summary.interface_property_symbol_nodes += entry.second.properties.size();
    summary.interface_method_symbol_nodes += entry.second.methods.size();
  }
  for (const auto &entry : surface.implementations) {
    summary.implementation_property_symbol_nodes += entry.second.properties.size();
    summary.implementation_method_symbol_nodes += entry.second.methods.size();
    if (entry.second.has_matching_interface) {
      ++summary.implementation_interface_resolution_hits;
    }
  }

  summary.nested_scope_symbols = summary.interface_property_symbol_nodes + summary.implementation_property_symbol_nodes +
                                 summary.interface_method_symbol_nodes + summary.implementation_method_symbol_nodes;
  summary.implementation_interface_resolution_sites = summary.implementation_symbol_nodes;
  if (summary.implementation_interface_resolution_hits > summary.implementation_interface_resolution_sites) {
    summary.deterministic = false;
    summary.implementation_interface_resolution_misses = 0;
  } else {
    summary.implementation_interface_resolution_misses =
        summary.implementation_interface_resolution_sites - summary.implementation_interface_resolution_hits;
  }

  summary.method_resolution_sites = summary.implementation_method_symbol_nodes;
  summary.method_resolution_hits = surface.interface_implementation_summary.linked_implementation_symbols;
  if (summary.method_resolution_hits > summary.method_resolution_sites) {
    summary.deterministic = false;
    summary.method_resolution_misses = 0;
  } else {
    summary.method_resolution_misses = summary.method_resolution_sites - summary.method_resolution_hits;
  }

  summary.deterministic = summary.deterministic &&
                          summary.interface_method_symbol_nodes ==
                              surface.interface_implementation_summary.interface_method_symbols &&
                          summary.implementation_method_symbol_nodes ==
                              surface.interface_implementation_summary.implementation_method_symbols &&
                          summary.symbol_nodes_total() == summary.top_level_scope_symbols + summary.nested_scope_symbols &&
                          summary.implementation_interface_resolution_hits <=
                              summary.implementation_interface_resolution_sites &&
                          summary.implementation_interface_resolution_hits +
                                  summary.implementation_interface_resolution_misses ==
                              summary.implementation_interface_resolution_sites &&
                          summary.method_resolution_hits <= summary.method_resolution_sites &&
                          summary.method_resolution_hits + summary.method_resolution_misses ==
                              summary.method_resolution_sites &&
                          summary.resolution_hits_total() <= summary.resolution_sites_total() &&
                          summary.resolution_hits_total() + summary.resolution_misses_total() ==
                              summary.resolution_sites_total();
  return summary;
}

static Objc3SymbolGraphScopeResolutionSummary BuildSymbolGraphScopeResolutionSummaryFromTypeMetadataHandoff(
    const Objc3SemanticTypeMetadataHandoff &handoff) {
  Objc3SymbolGraphScopeResolutionSummary summary;
  summary.global_symbol_nodes = handoff.global_names_lexicographic.size();
  summary.function_symbol_nodes = handoff.functions_lexicographic.size();
  summary.interface_symbol_nodes = handoff.interfaces_lexicographic.size();
  summary.implementation_symbol_nodes = handoff.implementations_lexicographic.size();
  summary.top_level_scope_symbols = summary.global_symbol_nodes + summary.function_symbol_nodes +
                                    summary.interface_symbol_nodes + summary.implementation_symbol_nodes;
  summary.scope_frames_total =
      static_cast<std::size_t>(1u) + summary.function_symbol_nodes + summary.interface_symbol_nodes +
      summary.implementation_symbol_nodes;

  for (const auto &metadata : handoff.interfaces_lexicographic) {
    summary.interface_property_symbol_nodes += metadata.properties_lexicographic.size();
    summary.interface_method_symbol_nodes += metadata.methods_lexicographic.size();
  }
  for (const auto &metadata : handoff.implementations_lexicographic) {
    summary.implementation_property_symbol_nodes += metadata.properties_lexicographic.size();
    summary.implementation_method_symbol_nodes += metadata.methods_lexicographic.size();
    if (metadata.has_matching_interface) {
      ++summary.implementation_interface_resolution_hits;
    }
  }

  summary.nested_scope_symbols = summary.interface_property_symbol_nodes + summary.implementation_property_symbol_nodes +
                                 summary.interface_method_symbol_nodes + summary.implementation_method_symbol_nodes;
  summary.implementation_interface_resolution_sites = summary.implementation_symbol_nodes;
  if (summary.implementation_interface_resolution_hits > summary.implementation_interface_resolution_sites) {
    summary.deterministic = false;
    summary.implementation_interface_resolution_misses = 0;
  } else {
    summary.implementation_interface_resolution_misses =
        summary.implementation_interface_resolution_sites - summary.implementation_interface_resolution_hits;
  }

  summary.method_resolution_sites = summary.implementation_method_symbol_nodes;
  summary.method_resolution_hits = handoff.interface_implementation_summary.linked_implementation_symbols;
  if (summary.method_resolution_hits > summary.method_resolution_sites) {
    summary.deterministic = false;
    summary.method_resolution_misses = 0;
  } else {
    summary.method_resolution_misses = summary.method_resolution_sites - summary.method_resolution_hits;
  }

  summary.deterministic = summary.deterministic &&
                          summary.interface_method_symbol_nodes ==
                              handoff.interface_implementation_summary.interface_method_symbols &&
                          summary.implementation_method_symbol_nodes ==
                              handoff.interface_implementation_summary.implementation_method_symbols &&
                          summary.symbol_nodes_total() == summary.top_level_scope_symbols + summary.nested_scope_symbols &&
                          summary.implementation_interface_resolution_hits <=
                              summary.implementation_interface_resolution_sites &&
                          summary.implementation_interface_resolution_hits +
                                  summary.implementation_interface_resolution_misses ==
                              summary.implementation_interface_resolution_sites &&
                          summary.method_resolution_hits <= summary.method_resolution_sites &&
                          summary.method_resolution_hits + summary.method_resolution_misses ==
                              summary.method_resolution_sites &&
                          summary.resolution_hits_total() <= summary.resolution_sites_total() &&
                          summary.resolution_hits_total() + summary.resolution_misses_total() ==
                              summary.resolution_sites_total();
  return summary;
}

static const Objc3MethodInfo *FindMethodInfoWithKind(
    const std::unordered_map<std::string, Objc3MethodInfo> &methods,
    const std::string &selector,
    bool is_class_method) {
  const auto method_it = methods.find(selector);
  if (method_it == methods.end() || method_it->second.is_class_method != is_class_method) {
    return nullptr;
  }
  return &method_it->second;
}

static const Objc3MethodInfo *FindSurfaceMethodInSuperChain(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &interface_name,
    const std::string &selector,
    bool is_class_method,
    bool &missing_base,
    bool &cycle_detected) {
  missing_base = false;
  cycle_detected = false;
  const auto interface_it = surface.interfaces.find(interface_name);
  if (interface_it == surface.interfaces.end()) {
    missing_base = true;
    return nullptr;
  }

  std::string next_super = interface_it->second.super_name;
  std::unordered_set<std::string> visited;
  while (!next_super.empty()) {
    if (!visited.insert(next_super).second) {
      cycle_detected = true;
      return nullptr;
    }
    const auto super_it = surface.interfaces.find(next_super);
    if (super_it == surface.interfaces.end()) {
      missing_base = true;
      return nullptr;
    }
    const Objc3MethodInfo *method =
        FindMethodInfoWithKind(super_it->second.methods, selector, is_class_method);
    if (method != nullptr) {
      return method;
    }
    next_super = super_it->second.super_name;
  }
  return nullptr;
}

static const Objc3MethodInfo *ResolveMethodInOwnerInterface(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &owner_name,
    const std::string &selector,
    bool is_class_method) {
  const auto interface_it = surface.interfaces.find(owner_name);
  if (interface_it == surface.interfaces.end()) {
    return nullptr;
  }
  return FindMethodInfoWithKind(interface_it->second.methods, selector,
                                is_class_method);
}

static const Objc3MethodInfo *ResolveMethodInOwnerImplementation(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &owner_name,
    const std::string &selector,
    bool is_class_method) {
  const auto implementation_it = surface.implementations.find(owner_name);
  if (implementation_it == surface.implementations.end()) {
    return nullptr;
  }
  return FindMethodInfoWithKind(implementation_it->second.methods, selector,
                                is_class_method);
}

static const Objc3MethodInfo *ResolveMethodInCategoryInterface(
    const std::unordered_map<std::string, Objc3InterfaceInfo> &category_interfaces,
    const std::string &owner_identity,
    const std::string &selector,
    bool is_class_method) {
  const auto interface_it = category_interfaces.find(owner_identity);
  if (interface_it == category_interfaces.end()) {
    return nullptr;
  }
  return FindMethodInfoWithKind(interface_it->second.methods, selector,
                                is_class_method);
}

static const Objc3MethodInfo *ResolveMethodInCategoryImplementation(
    const std::unordered_map<std::string, Objc3ImplementationInfo>
        &category_implementations,
    const std::string &owner_identity,
    const std::string &selector,
    bool is_class_method) {
  const auto implementation_it = category_implementations.find(owner_identity);
  if (implementation_it == category_implementations.end()) {
    return nullptr;
  }
  return FindMethodInfoWithKind(implementation_it->second.methods, selector,
                                is_class_method);
}

static const Objc3MethodInfo *ResolveMethodInMergedCategorySurface(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &owner_name,
    const std::string &selector,
    bool is_class_method) {
  const auto merge_it = surface.category_merge_surfaces.find(owner_name);
  if (merge_it == surface.category_merge_surfaces.end()) {
    return nullptr;
  }
  return FindMethodInfoWithKind(merge_it->second.merged_methods, selector,
