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
