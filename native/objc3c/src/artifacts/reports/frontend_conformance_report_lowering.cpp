#include "artifacts/reports/frontend_conformance_report_contracts.h"

#include <cstddef>
#include <sstream>

#include "artifacts/objc3_frontend_feature_claim_artifacts.h"
#include "artifacts/objc3_frontend_feature_claim_truth_artifacts.h"
#include "artifacts/reports/frontend_conformance_report_contracts_json_helpers.h"
#include "io/objc3_json.h"

namespace objc3::artifacts::reports {

using objc3::artifacts::reports::detail::BuildStringArrayJson;
using objc3::io::EscapeJsonString;

std::string BuildVersionedConformanceReportLoweringReplayKey(
    const Objc3VersionedConformanceReportLoweringSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";semantic_contract_id=" << summary.semantic_contract_id
      << ";artifact_schema_id=" << summary.artifact_schema_id
      << ";effective_language_profile=" << summary.effective_language_profile
      << ";canonical_literal_rejection_diagnostics="
      << (summary.canonical_literal_rejection_diagnostics_enabled ? "true"
                                                                  : "false")
      << ";runnable_claim_count=" << summary.runnable_feature_claim_count
      << ";source_only_claim_count=" << summary.source_only_feature_claim_count
      << ";unsupported_claim_count="
      << summary.unsupported_feature_claim_count
      << ";live_unsupported_feature_families="
      << summary.live_unsupported_feature_family_count
      << ";live_unsupported_feature_sites="
      << summary.live_unsupported_feature_site_count
      << ";suppressed_macro_claim_ids=";
  for (std::size_t index = 0;
       index < summary.suppressed_macro_claim_ids.size(); ++index) {
    if (index > 0u) {
      out << ",";
    }
    out << summary.suppressed_macro_claim_ids[index];
  }
  out << ";semantic_boundary_replay_key="
      << summary.semantic_boundary_replay_key;
  return out.str();
}

Objc3VersionedConformanceReportLoweringSummary
BuildVersionedConformanceReportLoweringSummary(
    const Objc3FrontendOptions &options,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendCompatibilityStrictnessClaimSemanticsSummary
        &semantic_summary) {
  Objc3VersionedConformanceReportLoweringSummary summary;
  summary.effective_language_profile =
      semantic_summary.effective_language_profile;
  summary.canonical_literal_rejection_diagnostics_enabled = true;
  summary.runnable_feature_claim_ids =
      ::objc3::artifacts::BuildRunnableFeatureClaimIds();
  summary.source_only_feature_claim_ids =
      ::objc3::artifacts::BuildSourceOnlyFeatureClaimIds();
  summary.unsupported_feature_claim_ids =
      ::objc3::artifacts::BuildUnsupportedFeatureClaimIds();
  summary.runnable_feature_claim_count =
      summary.runnable_feature_claim_ids.size();
  summary.source_only_feature_claim_count =
      summary.source_only_feature_claim_ids.size();
  summary.unsupported_feature_claim_count =
      summary.unsupported_feature_claim_ids.size();
  summary.runnable_feature_claim_inventory_replay_key =
      ::objc3::artifacts::BuildRunnableFeatureClaimInventoryReplayKey(
          options, pipeline_result);
  summary.feature_claim_truth_surface_replay_key =
      ::objc3::artifacts::frontend::BuildFeatureClaimStrictnessTruthSurfaceReplayKey(
          options, pipeline_result);
  if (IsReadyObjc3FrontendCompatibilityStrictnessClaimSemanticsSummary(
          semantic_summary)) {
    summary.suppressed_macro_claim_ids =
        semantic_summary.suppressed_macro_claim_ids;
    summary.live_unsupported_feature_family_count =
        semantic_summary.live_unsupported_feature_family_count;
    summary.live_unsupported_feature_site_count =
        semantic_summary.live_unsupported_feature_site_count;
    summary.live_unsupported_feature_diagnostic_count =
        semantic_summary.live_unsupported_feature_diagnostic_count;
    summary.fail_closed = semantic_summary.fail_closed;
    summary.semantic_surface_published = true;
    summary.runnable_claim_inventory_ready = true;
    summary.feature_claim_truth_surface_ready = true;
    summary.semantic_boundary_ready =
        semantic_summary.semantic_boundary_ready;
    summary.known_unsupported_surface_published =
        semantic_summary.live_unsupported_feature_source_rejection_landed;
    summary.compatibility_selection_truthful =
        semantic_summary.language_profile_semantics_landed;
    summary.strictness_selection_supported =
        semantic_summary.strictness_selection_semantics_landed;
    summary.strict_concurrency_selection_supported =
        semantic_summary.strict_concurrency_selection_semantics_landed;
    summary.canonical_interface_truthful =
        semantic_summary.canonical_interface_truth_semantics_landed;
    summary.feature_macro_truthful =
        semantic_summary.feature_macro_claim_suppression_semantics_landed &&
        semantic_summary.separate_compilation_macro_truth_semantics_landed;
    summary.ready_for_runtime_conformance_publication =
        semantic_summary.ready_for_lowering_and_runtime;
    summary.semantic_boundary_replay_key =
        semantic_summary.semantic_boundary_replay_key;
    summary.replay_key =
        BuildVersionedConformanceReportLoweringReplayKey(summary);
  }
  if (!IsReadyObjc3VersionedConformanceReportLoweringSummary(summary)) {
    summary.failure_reason =
        "versioned conformance-report lowering summary is incomplete";
  }
  return summary;
}

std::string BuildVersionedConformanceReportLoweringSummaryJson(
    const Objc3VersionedConformanceReportLoweringSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"semantic_contract_id\":\""
      << EscapeJsonString(summary.semantic_contract_id)
      << "\",\"runnable_feature_claim_inventory_contract_id\":\""
      << EscapeJsonString(summary.runnable_feature_claim_inventory_contract_id)
      << "\",\"feature_claim_truth_surface_contract_id\":\""
      << EscapeJsonString(summary.feature_claim_truth_surface_contract_id)
      << "\",\"frontend_surface_path\":\""
      << EscapeJsonString(summary.frontend_surface_path)
      << "\",\"artifact_suffix\":\"" << EscapeJsonString(summary.artifact_suffix)
      << "\",\"artifact_schema_id\":\""
      << EscapeJsonString(summary.artifact_schema_id)
      << "\",\"payload_model\":\"" << EscapeJsonString(summary.payload_model)
      << "\",\"authority_model\":\""
      << EscapeJsonString(summary.authority_model)
      << "\",\"known_unsupported_model\":\""
      << EscapeJsonString(summary.known_unsupported_model)
      << "\",\"selection_model\":\""
      << EscapeJsonString(summary.selection_model)
      << "\",\"canonical_interface_mode\":\""
      << EscapeJsonString(summary.canonical_interface_mode)
      << "\",\"publication_model\":\""
      << EscapeJsonString(summary.publication_model)
      << "\",\"effective_language_profile\":\""
      << EscapeJsonString(summary.effective_language_profile)
      << "\",\"canonical_literal_rejection_diagnostics_enabled\":"
      << (summary.canonical_literal_rejection_diagnostics_enabled ? "true"
                                                                  : "false")
      << ",\"runnable_feature_claim_count\":"
      << summary.runnable_feature_claim_count
      << ",\"source_only_feature_claim_count\":"
      << summary.source_only_feature_claim_count
      << ",\"unsupported_feature_claim_count\":"
      << summary.unsupported_feature_claim_count
      << ",\"live_unsupported_feature_family_count\":"
      << summary.live_unsupported_feature_family_count
      << ",\"live_unsupported_feature_site_count\":"
      << summary.live_unsupported_feature_site_count
      << ",\"live_unsupported_feature_diagnostic_count\":"
      << summary.live_unsupported_feature_diagnostic_count
      << ",\"fail_closed\":" << (summary.fail_closed ? "true" : "false")
      << ",\"semantic_surface_published\":"
      << (summary.semantic_surface_published ? "true" : "false")
      << ",\"runnable_claim_inventory_ready\":"
      << (summary.runnable_claim_inventory_ready ? "true" : "false")
      << ",\"feature_claim_truth_surface_ready\":"
      << (summary.feature_claim_truth_surface_ready ? "true" : "false")
      << ",\"semantic_boundary_ready\":"
      << (summary.semantic_boundary_ready ? "true" : "false")
      << ",\"known_unsupported_surface_published\":"
      << (summary.known_unsupported_surface_published ? "true" : "false")
      << ",\"compatibility_selection_truthful\":"
      << (summary.compatibility_selection_truthful ? "true" : "false")
      << ",\"strictness_selection_supported\":"
      << (summary.strictness_selection_supported ? "true" : "false")
      << ",\"strict_concurrency_selection_supported\":"
      << (summary.strict_concurrency_selection_supported ? "true" : "false")
      << ",\"canonical_interface_truthful\":"
      << (summary.canonical_interface_truthful ? "true" : "false")
      << ",\"feature_macro_truthful\":"
      << (summary.feature_macro_truthful ? "true" : "false")
      << ",\"ready_for_runtime_conformance_publication\":"
      << (summary.ready_for_runtime_conformance_publication ? "true" : "false")
      << ",\"runnable_feature_claim_ids\":"
      << BuildStringArrayJson(summary.runnable_feature_claim_ids)
      << ",\"source_only_feature_claim_ids\":"
      << BuildStringArrayJson(summary.source_only_feature_claim_ids)
      << ",\"unsupported_feature_claim_ids\":"
      << BuildStringArrayJson(summary.unsupported_feature_claim_ids)
      << ",\"suppressed_macro_claim_ids\":"
      << BuildStringArrayJson(summary.suppressed_macro_claim_ids)
      << ",\"runnable_feature_claim_inventory_replay_key\":\""
      << EscapeJsonString(summary.runnable_feature_claim_inventory_replay_key)
      << "\",\"feature_claim_truth_surface_replay_key\":\""
      << EscapeJsonString(summary.feature_claim_truth_surface_replay_key)
      << "\",\"semantic_boundary_replay_key\":\""
      << EscapeJsonString(summary.semantic_boundary_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"ready\":"
      << (IsReadyObjc3VersionedConformanceReportLoweringSummary(summary)
              ? "true"
              : "false")
      << "}";
  return out.str();
}

}  // namespace objc3::artifacts::reports
