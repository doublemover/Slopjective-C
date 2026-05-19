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
