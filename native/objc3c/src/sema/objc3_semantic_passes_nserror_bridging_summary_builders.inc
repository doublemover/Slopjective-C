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
