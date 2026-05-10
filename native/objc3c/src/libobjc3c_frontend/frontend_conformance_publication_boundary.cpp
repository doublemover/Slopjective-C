#include "libobjc3c_frontend/frontend_conformance_publication_boundary.h"

#include "libobjc3c_frontend/objc3_cli_frontend.h"

namespace objc3c::frontend {

bool IsFrontendConformanceReportLoweringSummaryReady(
    const Objc3VersionedConformanceReportLoweringSummary &summary) {
  const bool language_profile_valid =
      summary.effective_language_profile == "canonical";
  return !summary.contract_id.empty() &&
         !summary.semantic_contract_id.empty() &&
         !summary.runnable_feature_claim_inventory_contract_id.empty() &&
         !summary.feature_claim_truth_surface_contract_id.empty() &&
         !summary.frontend_surface_path.empty() &&
         !summary.artifact_suffix.empty() &&
         !summary.artifact_schema_id.empty() &&
         !summary.payload_model.empty() &&
         !summary.authority_model.empty() &&
         !summary.known_unsupported_model.empty() &&
         !summary.selection_model.empty() &&
         !summary.canonical_interface_mode.empty() &&
         !summary.publication_model.empty() && language_profile_valid &&
         summary.runnable_feature_claim_count ==
             summary.runnable_feature_claim_ids.size() &&
         summary.source_only_feature_claim_count ==
             summary.source_only_feature_claim_ids.size() &&
         summary.unsupported_feature_claim_count ==
             summary.unsupported_feature_claim_ids.size() &&
         summary.live_unsupported_feature_family_count <=
             summary.unsupported_feature_claim_count &&
         summary.live_unsupported_feature_diagnostic_count ==
             summary.live_unsupported_feature_site_count &&
         summary.fail_closed && summary.semantic_surface_published &&
         summary.runnable_claim_inventory_ready &&
         summary.feature_claim_truth_surface_ready &&
         summary.semantic_boundary_ready &&
         summary.known_unsupported_surface_published &&
         summary.compatibility_selection_truthful &&
         summary.strictness_selection_fail_closed &&
         summary.strict_concurrency_selection_fail_closed &&
         summary.canonical_interface_truthful &&
         summary.feature_macro_truthful &&
         summary.ready_for_runtime_conformance_publication &&
         !summary.runnable_feature_claim_inventory_replay_key.empty() &&
         !summary.feature_claim_truth_surface_replay_key.empty() &&
         !summary.semantic_boundary_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

}  // namespace objc3c::frontend
