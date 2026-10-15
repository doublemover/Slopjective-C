#include "artifacts/reports/frontend_conformance_report_contracts.h"

#include <cstddef>
#include <sstream>

#include "artifacts/reports/frontend_conformance_report_contracts_json_helpers.h"
#include "io/objc3_json.h"

namespace objc3::artifacts::reports {

using objc3::artifacts::reports::detail::BuildStringArrayJson;
using objc3::io::EscapeJsonString;

std::string BuildFrontendCompatibilityStrictnessClaimSemanticsReplayKey(
    const Objc3FrontendCompatibilityStrictnessClaimSemanticsSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";inventory_contract_id="
      << summary.runnable_feature_claim_inventory_contract_id
      << ";truth_surface_contract_id="
      << summary.feature_claim_truth_surface_contract_id
      << ";frontend_surface_path=" << summary.frontend_surface_path
      << ";semantic_model=" << summary.semantic_model
      << ";downgrade_model=" << summary.downgrade_model
      << ";rejection_model=" << summary.rejection_model
      << ";canonical_interface_truth_model="
      << summary.canonical_interface_truth_model
      << ";separate_compilation_macro_truth_model="
      << summary.separate_compilation_macro_truth_model
      << ";canonical_interface_payload_mode="
      << summary.canonical_interface_payload_mode
      << ";language_profile=" << summary.effective_language_profile
      << ";canonical_literal_rejection_diagnostics="
      << (summary.canonical_literal_rejection_diagnostics_enabled ? "true"
                                                                  : "false")
      << ";live_unsupported_feature_families="
      << summary.live_unsupported_feature_family_count
      << ";live_unsupported_feature_sites="
      << summary.live_unsupported_feature_site_count
      << ";live_unsupported_feature_diagnostics="
      << summary.live_unsupported_feature_diagnostic_count
      << ";throws_source_rejections="
      << summary.throws_source_rejection_site_count
      << ";blocks_source_rejections="
      << summary.blocks_source_rejection_site_count
      << ";arc_source_rejections=" << summary.arc_source_rejection_site_count
      << ";suppressed_macro_claim_ids=";
  for (std::size_t index = 0; index < summary.suppressed_macro_claim_ids.size();
       ++index) {
    if (index > 0u) {
      out << ",";
    }
    out << summary.suppressed_macro_claim_ids[index];
  }
  out << ";semantic_boundary_replay_key="
      << summary.semantic_boundary_replay_key;
  return out.str();
}

Objc3FrontendCompatibilityStrictnessClaimSemanticsSummary
BuildFrontendCompatibilityStrictnessClaimSemanticsSummary(
    const Objc3CompatibilityStrictnessClaimSemanticsSummary
        &semantic_boundary) {
  Objc3FrontendCompatibilityStrictnessClaimSemanticsSummary summary;
  summary.semantic_boundary_ready =
      IsReadyObjc3CompatibilityStrictnessClaimSemanticsSummary(
          semantic_boundary);
  if (summary.semantic_boundary_ready) {
    summary.effective_language_profile =
        semantic_boundary.effective_language_profile;
    summary.canonical_literal_rejection_diagnostics_enabled = true;
    summary.valid_language_profile_count =
        semantic_boundary.valid_language_profile_count;
    summary.live_selection_surface_count =
        semantic_boundary.live_selection_surface_count;
    summary.valid_selection_combination_count =
        semantic_boundary.valid_selection_combination_count;
    summary.runnable_feature_claim_count =
        semantic_boundary.runnable_feature_claim_count;
    summary.downgraded_source_only_claim_count =
        semantic_boundary.downgraded_source_only_claim_count;
    summary.rejected_unsupported_feature_claim_count =
        semantic_boundary.rejected_unsupported_feature_claim_count;
    summary.rejected_selection_surface_count =
        semantic_boundary.rejected_selection_surface_count;
    summary.suppressed_macro_claim_count =
        semantic_boundary.suppressed_macro_claim_count;
    summary.live_unsupported_feature_family_count =
        semantic_boundary.live_unsupported_feature_family_count;
    summary.live_unsupported_feature_site_count =
        semantic_boundary.live_unsupported_feature_site_count;
    summary.live_unsupported_feature_diagnostic_count =
        semantic_boundary.live_unsupported_feature_diagnostic_count;
    summary.throws_source_rejection_site_count =
        semantic_boundary.throws_source_rejection_site_count;
    summary.blocks_source_rejection_site_count =
        semantic_boundary.blocks_source_rejection_site_count;
    summary.arc_source_rejection_site_count =
        semantic_boundary.arc_source_rejection_site_count;
    summary.canonical_interface_truth_model =
        semantic_boundary.canonical_interface_truth_model;
    summary.separate_compilation_macro_truth_model =
        semantic_boundary.separate_compilation_macro_truth_model;
    summary.canonical_interface_payload_mode =
        semantic_boundary.canonical_interface_payload_mode;
    summary.suppressed_macro_claim_ids =
        semantic_boundary.suppressed_macro_claim_ids;
    summary.fail_closed = semantic_boundary.fail_closed;
    summary.language_profile_semantics_landed =
        semantic_boundary.language_profile_semantics_landed;
    summary.canonical_literal_rejection_semantics_landed =
        semantic_boundary.canonical_literal_rejection_semantics_landed;
    summary.source_only_claim_downgrade_semantics_landed =
        semantic_boundary.source_only_claim_downgrade_semantics_landed;
    summary.unsupported_feature_claim_rejection_semantics_landed =
        semantic_boundary.unsupported_feature_claim_rejection_semantics_landed;
    summary.live_unsupported_feature_source_rejection_landed =
        semantic_boundary.live_unsupported_feature_source_rejection_landed;
    summary.strictness_selection_semantics_landed =
        semantic_boundary.strictness_selection_semantics_landed;
    summary.strict_concurrency_selection_semantics_landed =
        semantic_boundary.strict_concurrency_selection_semantics_landed;
    summary.feature_macro_claim_suppression_semantics_landed =
        semantic_boundary.feature_macro_claim_suppression_semantics_landed;
    summary.canonical_interface_truth_semantics_landed =
        semantic_boundary.canonical_interface_truth_semantics_landed;
    summary.separate_compilation_macro_truth_semantics_landed =
        semantic_boundary.separate_compilation_macro_truth_semantics_landed;
    summary.selected_configuration_valid =
        semantic_boundary.selected_configuration_valid;
    summary.selected_configuration_downgraded =
        semantic_boundary.selected_configuration_downgraded;
    summary.selected_configuration_rejected =
        semantic_boundary.selected_configuration_rejected;
    summary.ready_for_lowering_and_runtime =
        semantic_boundary.ready_for_lowering_and_runtime;
    summary.semantic_boundary_replay_key = semantic_boundary.replay_key;
    summary.replay_key =
        BuildFrontendCompatibilityStrictnessClaimSemanticsReplayKey(summary);
  }
  if (!IsReadyObjc3FrontendCompatibilityStrictnessClaimSemanticsSummary(
          summary)) {
    summary.failure_reason =
        "frontend canonical selection claim semantics summary is incomplete";
  }
  return summary;
}

std::string BuildFrontendCompatibilityStrictnessClaimSemanticsSummaryJson(
    const Objc3FrontendCompatibilityStrictnessClaimSemanticsSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"runnable_feature_claim_inventory_contract_id\":\""
      << EscapeJsonString(summary.runnable_feature_claim_inventory_contract_id)
      << "\",\"feature_claim_truth_surface_contract_id\":\""
      << EscapeJsonString(summary.feature_claim_truth_surface_contract_id)
      << "\",\"frontend_surface_path\":\""
      << EscapeJsonString(summary.frontend_surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"downgrade_model\":\"" << EscapeJsonString(summary.downgrade_model)
      << "\",\"rejection_model\":\"" << EscapeJsonString(summary.rejection_model)
      << "\",\"canonical_interface_truth_model\":\""
      << EscapeJsonString(summary.canonical_interface_truth_model)
      << "\",\"separate_compilation_macro_truth_model\":\""
      << EscapeJsonString(summary.separate_compilation_macro_truth_model)
      << "\",\"canonical_interface_payload_mode\":\""
      << EscapeJsonString(summary.canonical_interface_payload_mode)
      << "\",\"effective_language_profile\":\""
      << EscapeJsonString(summary.effective_language_profile)
      << "\",\"canonical_literal_rejection_diagnostics_enabled\":"
      << (summary.canonical_literal_rejection_diagnostics_enabled ? "true"
                                                                  : "false")
      << ",\"valid_language_profile_count\":"
      << summary.valid_language_profile_count
      << ",\"live_selection_surface_count\":"
      << summary.live_selection_surface_count
      << ",\"valid_selection_combination_count\":"
      << summary.valid_selection_combination_count
      << ",\"runnable_feature_claim_count\":"
      << summary.runnable_feature_claim_count
      << ",\"downgraded_source_only_claim_count\":"
      << summary.downgraded_source_only_claim_count
      << ",\"rejected_unsupported_feature_claim_count\":"
      << summary.rejected_unsupported_feature_claim_count
      << ",\"live_unsupported_feature_family_count\":"
      << summary.live_unsupported_feature_family_count
      << ",\"live_unsupported_feature_site_count\":"
      << summary.live_unsupported_feature_site_count
      << ",\"live_unsupported_feature_diagnostic_count\":"
      << summary.live_unsupported_feature_diagnostic_count
      << ",\"throws_source_rejection_site_count\":"
      << summary.throws_source_rejection_site_count
      << ",\"blocks_source_rejection_site_count\":"
      << summary.blocks_source_rejection_site_count
      << ",\"arc_source_rejection_site_count\":"
      << summary.arc_source_rejection_site_count
      << ",\"rejected_selection_surface_count\":"
      << summary.rejected_selection_surface_count
      << ",\"suppressed_macro_claim_count\":"
      << summary.suppressed_macro_claim_count
      << ",\"suppressed_macro_claim_ids\":"
      << BuildStringArrayJson(summary.suppressed_macro_claim_ids)
      << ",\"fail_closed\":" << (summary.fail_closed ? "true" : "false")
      << ",\"semantic_boundary_ready\":"
      << (summary.semantic_boundary_ready ? "true" : "false")
      << ",\"language_profile_semantics_landed\":"
      << (summary.language_profile_semantics_landed ? "true" : "false")
      << ",\"canonical_literal_rejection_semantics_landed\":"
      << (summary.canonical_literal_rejection_semantics_landed ? "true"
                                                               : "false")
      << ",\"source_only_claim_downgrade_semantics_landed\":"
      << (summary.source_only_claim_downgrade_semantics_landed ? "true"
                                                               : "false")
      << ",\"unsupported_feature_claim_rejection_semantics_landed\":"
      << (summary.unsupported_feature_claim_rejection_semantics_landed ? "true"
                                                                       : "false")
      << ",\"live_unsupported_feature_source_rejection_landed\":"
      << (summary.live_unsupported_feature_source_rejection_landed ? "true"
                                                                   : "false")
      << ",\"strictness_selection_semantics_landed\":"
      << (summary.strictness_selection_semantics_landed ? "true" : "false")
      << ",\"strict_concurrency_selection_semantics_landed\":"
      << (summary.strict_concurrency_selection_semantics_landed ? "true"
                                                                : "false")
      << ",\"feature_macro_claim_suppression_semantics_landed\":"
      << (summary.feature_macro_claim_suppression_semantics_landed ? "true"
                                                                   : "false")
      << ",\"canonical_interface_truth_semantics_landed\":"
      << (summary.canonical_interface_truth_semantics_landed ? "true"
                                                             : "false")
      << ",\"separate_compilation_macro_truth_semantics_landed\":"
      << (summary.separate_compilation_macro_truth_semantics_landed ? "true"
                                                                    : "false")
      << ",\"selected_configuration_valid\":"
      << (summary.selected_configuration_valid ? "true" : "false")
      << ",\"selected_configuration_downgraded\":"
      << (summary.selected_configuration_downgraded ? "true" : "false")
      << ",\"selected_configuration_rejected\":"
      << (summary.selected_configuration_rejected ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"semantic_boundary_replay_key\":\""
      << EscapeJsonString(summary.semantic_boundary_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"ready\":"
      << (IsReadyObjc3FrontendCompatibilityStrictnessClaimSemanticsSummary(
              summary)
              ? "true"
              : "false")
      << "}";
  return out.str();
}

}  // namespace objc3::artifacts::reports
