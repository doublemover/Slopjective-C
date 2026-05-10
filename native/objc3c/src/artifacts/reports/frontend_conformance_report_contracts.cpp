#include "artifacts/reports/frontend_conformance_report_contracts.h"

#include <sstream>
#include <vector>

#include "artifacts/objc3_frontend_conformance_artifacts.h"
#include "artifacts/objc3_frontend_artifacts.h"
#include "artifacts/objc3_frontend_feature_claim_artifacts.h"
#include "artifacts/objc3_frontend_feature_claim_truth_artifacts.h"
#include "artifacts/objc3_frontend_runtime_capability_artifacts.h"
#include "io/objc3_json.h"

namespace objc3::artifacts::reports {
namespace {

using objc3::io::EscapeJsonString;

std::string BuildStringArrayJson(const std::vector<std::string> &values) {
  std::ostringstream out;
  out << "[";
  for (std::size_t index = 0; index < values.size(); ++index) {
    if (index > 0u) {
      out << ",";
    }
    out << "\"" << EscapeJsonString(values[index]) << "\"";
  }
  out << "]";
  return out.str();
}

}  // namespace

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
    summary.strictness_selection_rejection_semantics_landed =
        semantic_boundary.strictness_selection_rejection_semantics_landed;
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
        "frontend compatibility/strictness/claim semantics summary is incomplete";
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
      << ",\"strictness_selection_rejection_semantics_landed\":"
      << (summary.strictness_selection_rejection_semantics_landed ? "true"
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
    summary.strictness_selection_fail_closed =
        semantic_summary.strictness_selection_rejection_semantics_landed;
    summary.strict_concurrency_selection_fail_closed =
        semantic_summary.strictness_selection_rejection_semantics_landed;
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
      << ",\"strictness_selection_fail_closed\":"
      << (summary.strictness_selection_fail_closed ? "true" : "false")
      << ",\"strict_concurrency_selection_fail_closed\":"
      << (summary.strict_concurrency_selection_fail_closed ? "true" : "false")
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

std::string BuildToolingMachineReadableConformanceReportContractReplayKey(
    const Objc3ToolingMachineReadableConformanceReportContractSummary
        &summary) {
  std::ostringstream out;
  out << "mode=" << summary.effective_language_profile
      << ";migration=" << (summary.migration_semantics_ready ? "true" : "false")
      << ";lowering=" << (summary.lowering_contract_ready ? "true" : "false")
      << ";runtime-capability="
      << (summary.runtime_capability_surface_published ? "true" : "false")
      << ";ready="
      << (summary.ready_for_runtime_publication ? "true" : "false");
  return out.str();
}

Objc3ToolingMachineReadableConformanceReportContractSummary
BuildToolingMachineReadableConformanceReportContractSummary(
    const Objc3ToolingLegacyCanonicalMigrationSemanticsSummary
        &migration_summary,
    const Objc3VersionedConformanceReportLoweringSummary &lowering_summary) {
  Objc3ToolingMachineReadableConformanceReportContractSummary summary;
  summary.effective_language_profile =
      lowering_summary.effective_language_profile;
  summary.migration_semantics_ready =
      migration_summary.ready_for_lowering_and_runtime;
  summary.lowering_contract_ready =
      IsReadyObjc3VersionedConformanceReportLoweringSummary(lowering_summary);
  summary.runtime_capability_surface_published =
      summary.lowering_contract_ready &&
      lowering_summary.ready_for_runtime_conformance_publication;
  summary.deterministic_handoff =
      summary.migration_semantics_ready && summary.lowering_contract_ready &&
      summary.runtime_capability_surface_published &&
      lowering_summary.artifact_suffix ==
          kObjc3VersionedConformanceReportLoweringArtifactSuffix &&
      lowering_summary.artifact_schema_id ==
          kObjc3VersionedConformanceReportLoweringArtifactSchemaId;
  summary.ready_for_runtime_publication = summary.deterministic_handoff;
  if (!summary.migration_semantics_ready) {
    summary.failure_reason =
        "tooling legacy/canonical migration semantics prerequisite is not ready";
  } else if (!summary.lowering_contract_ready) {
    summary.failure_reason =
        "versioned conformance-report lowering prerequisite is not ready";
  }
  summary.replay_key =
      BuildToolingMachineReadableConformanceReportContractReplayKey(summary);
  return summary;
}

std::string BuildToolingMachineReadableConformanceReportContractSummaryJson(
    const Objc3ToolingMachineReadableConformanceReportContractSummary
        &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"lowering_contract_id\":\""
      << EscapeJsonString(summary.lowering_contract_id)
      << "\",\"runtime_capability_contract_id\":\""
      << EscapeJsonString(summary.runtime_capability_contract_id)
      << "\",\"frontend_surface_path\":\""
      << EscapeJsonString(summary.frontend_surface_path)
      << "\",\"payload_model\":\"" << EscapeJsonString(summary.payload_model)
      << "\",\"authority_model\":\""
      << EscapeJsonString(summary.authority_model)
      << "\",\"artifact_suffix\":\"" << EscapeJsonString(summary.artifact_suffix)
      << "\",\"artifact_schema_id\":\""
      << EscapeJsonString(summary.artifact_schema_id)
      << "\",\"runtime_capability_schema_id\":\""
      << EscapeJsonString(summary.runtime_capability_schema_id)
      << "\",\"effective_language_profile\":\""
      << EscapeJsonString(summary.effective_language_profile)
      << "\",\"migration_semantics_ready\":"
      << (summary.migration_semantics_ready ? "true" : "false")
      << ",\"lowering_contract_ready\":"
      << (summary.lowering_contract_ready ? "true" : "false")
      << ",\"runtime_capability_surface_published\":"
      << (summary.runtime_capability_surface_published ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_runtime_publication\":"
      << (summary.ready_for_runtime_publication ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildToolingFeatureAwareConformanceReportEmissionReplayKey(
    const Objc3ToolingFeatureAwareConformanceReportEmissionSummary &summary) {
  std::ostringstream out;
  out << "mode=" << summary.effective_language_profile
      << ";canonical-rejection-diagnostics="
      << (summary.canonical_literal_rejection_diagnostics_enabled ? "true"
                                                                  : "false")
      << ";families=" << summary.fixit_family_count
      << ";canonical-literal-rejection-sites="
      << summary.current_run_canonical_literal_rejection_sites
      << ";ready="
      << (summary.ready_for_runtime_publication ? "true" : "false");
  return out.str();
}

Objc3ToolingFeatureAwareConformanceReportEmissionSummary
BuildToolingFeatureAwareConformanceReportEmissionSummary(
    const Objc3ToolingFeatureSpecificFixitSynthesisSummary &fixit_summary,
    const Objc3ToolingLegacyCanonicalMigrationSemanticsSummary
        &migration_summary,
    const Objc3ToolingMachineReadableConformanceReportContractSummary
        &report_summary) {
  Objc3ToolingFeatureAwareConformanceReportEmissionSummary summary;
  summary.effective_language_profile =
      migration_summary.effective_language_profile;
  summary.canonical_literal_rejection_diagnostics_enabled =
      migration_summary.canonical_literal_rejection_diagnostics_enabled;
  summary.fixit_family_ids = fixit_summary.fixit_family_ids;
  summary.fixit_family_count = fixit_summary.fixit_family_count;
  summary.current_run_canonical_literal_rejection_sites =
      migration_summary.current_run_legacy_literal_sites;
  summary.canonical_mode_rejection_code =
      migration_summary.canonical_mode_rejection_code;
  summary.report_payload_emitted =
      report_summary.ready_for_runtime_publication;
  summary.deterministic_handoff =
      fixit_summary.ready_for_lowering_and_runtime &&
      migration_summary.ready_for_lowering_and_runtime &&
      report_summary.ready_for_runtime_publication &&
      summary.fixit_family_count == summary.fixit_family_ids.size();
  summary.ready_for_runtime_publication = summary.deterministic_handoff;
  if (!report_summary.ready_for_runtime_publication) {
    summary.failure_reason =
        "tooling machine-readable conformance/report contract prerequisite is not ready";
  }
  summary.replay_key =
      BuildToolingFeatureAwareConformanceReportEmissionReplayKey(summary);
  return summary;
}

std::string BuildToolingFeatureAwareConformanceReportEmissionSummaryJson(
    const Objc3ToolingFeatureAwareConformanceReportEmissionSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"machine_readable_report_contract_id\":\""
      << EscapeJsonString(summary.machine_readable_report_contract_id)
      << "\",\"fixit_contract_id\":\""
      << EscapeJsonString(summary.fixit_contract_id)
      << "\",\"migration_semantics_contract_id\":\""
      << EscapeJsonString(summary.migration_semantics_contract_id)
      << "\",\"frontend_surface_path\":\""
      << EscapeJsonString(summary.frontend_surface_path)
      << "\",\"payload_model\":\"" << EscapeJsonString(summary.payload_model)
      << "\",\"authority_model\":\""
      << EscapeJsonString(summary.authority_model)
      << "\",\"effective_language_profile\":\""
      << EscapeJsonString(summary.effective_language_profile)
      << "\",\"canonical_literal_rejection_diagnostics_enabled\":"
      << (summary.canonical_literal_rejection_diagnostics_enabled ? "true"
                                                                  : "false")
      << ",\"fixit_family_ids\":"
      << BuildStringArrayJson(summary.fixit_family_ids)
      << ",\"fixit_family_count\":" << summary.fixit_family_count
      << ",\"current_run_canonical_literal_rejection_sites\":"
      << summary.current_run_canonical_literal_rejection_sites
      << ",\"canonical_mode_rejection_code\":\""
      << EscapeJsonString(summary.canonical_mode_rejection_code)
      << "\",\"report_payload_emitted\":"
      << (summary.report_payload_emitted ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_runtime_publication\":"
      << (summary.ready_for_runtime_publication ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildToolingAdvancedFeatureReportingJson(
    const Objc3ToolingFeatureAwareConformanceReportEmissionSummary &summary) {
  return BuildToolingFeatureAwareConformanceReportEmissionSummaryJson(summary);
}

std::string BuildToolingCorpusShardingReleaseEvidencePackagingReplayKey(
    const Objc3ToolingCorpusShardingReleaseEvidencePackagingSummary &summary) {
  std::ostringstream out;
  out << "mode=" << summary.effective_language_profile
      << ";canonical-rejection-diagnostics="
      << (summary.canonical_literal_rejection_diagnostics_enabled ? "true"
                                                                  : "false")
      << ";profiles=" << summary.targeted_profile_count
      << ";corpus-shards=" << summary.corpus_shard_count
      << ";release-artifacts=" << summary.release_evidence_artifact_count
      << ";ready="
      << (summary.ready_for_release_evidence_packaging ? "true" : "false");
  return out.str();
}

Objc3ToolingCorpusShardingReleaseEvidencePackagingSummary
BuildToolingCorpusShardingReleaseEvidencePackagingSummary(
    const Objc3ToolingFeatureAwareConformanceReportEmissionSummary
        &feature_summary) {
  Objc3ToolingCorpusShardingReleaseEvidencePackagingSummary summary;
  summary.effective_language_profile =
      feature_summary.effective_language_profile;
  summary.canonical_literal_rejection_diagnostics_enabled =
      feature_summary.canonical_literal_rejection_diagnostics_enabled;
  summary.feature_report_payload_emitted =
      feature_summary.report_payload_emitted;
  summary.deterministic_handoff =
      IsReadyObjc3ToolingFeatureAwareConformanceReportEmissionSummary(
          feature_summary) &&
      summary.targeted_profile_count == summary.targeted_profile_ids.size() &&
      summary.corpus_shard_count == summary.corpus_shard_ids.size() &&
      summary.corpus_shard_count ==
          summary.corpus_shard_manifest_paths.size() &&
      summary.release_evidence_artifact_count ==
          summary.release_evidence_artifact_ids.size();
  summary.ready_for_release_evidence_packaging =
      summary.deterministic_handoff;
  if (!summary.feature_report_payload_emitted) {
    summary.failure_reason =
        "tooling feature-aware conformance report emission prerequisite is not ready";
  }
  summary.replay_key =
      BuildToolingCorpusShardingReleaseEvidencePackagingReplayKey(summary);
  return summary;
}

std::string BuildToolingCorpusShardingReleaseEvidencePackagingSummaryJson(
    const Objc3ToolingCorpusShardingReleaseEvidencePackagingSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"feature_aware_report_contract_id\":\""
      << EscapeJsonString(summary.feature_aware_report_contract_id)
      << "\",\"machine_readable_report_contract_id\":\""
      << EscapeJsonString(summary.machine_readable_report_contract_id)
      << "\",\"frontend_surface_path\":\""
      << EscapeJsonString(summary.frontend_surface_path)
      << "\",\"payload_model\":\"" << EscapeJsonString(summary.payload_model)
      << "\",\"authority_model\":\""
      << EscapeJsonString(summary.authority_model)
      << "\",\"targeted_profile_ids\":"
      << BuildStringArrayJson(summary.targeted_profile_ids)
      << ",\"targeted_profile_count\":" << summary.targeted_profile_count
      << ",\"corpus_shard_ids\":"
      << BuildStringArrayJson(summary.corpus_shard_ids)
      << ",\"corpus_shard_manifest_paths\":"
      << BuildStringArrayJson(summary.corpus_shard_manifest_paths)
      << ",\"corpus_shard_count\":" << summary.corpus_shard_count
      << ",\"release_evidence_artifact_ids\":"
      << BuildStringArrayJson(summary.release_evidence_artifact_ids)
      << ",\"release_evidence_artifact_count\":"
      << summary.release_evidence_artifact_count
      << ",\"release_evidence_checklist_path\":\""
      << EscapeJsonString(summary.release_evidence_checklist_path)
      << "\",\"release_evidence_schema_path\":\""
      << EscapeJsonString(summary.release_evidence_schema_path)
      << "\",\"report_artifact_suffix\":\""
      << EscapeJsonString(summary.report_artifact_suffix)
      << "\",\"effective_language_profile\":\""
      << EscapeJsonString(summary.effective_language_profile)
      << "\",\"canonical_literal_rejection_diagnostics_enabled\":"
      << (summary.canonical_literal_rejection_diagnostics_enabled ? "true"
                                                                  : "false")
      << ",\"feature_report_payload_emitted\":"
      << (summary.feature_report_payload_emitted ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_release_evidence_packaging\":"
      << (summary.ready_for_release_evidence_packaging ? "true" : "false")
      << ",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildToolingAdvancedFeatureReleaseEvidenceJson(
    const Objc3ToolingCorpusShardingReleaseEvidencePackagingSummary &summary) {
  return BuildToolingCorpusShardingReleaseEvidencePackagingSummaryJson(summary);
}

std::string BuildVersionedConformanceReportArtifactJson(
    const Objc3VersionedConformanceReportLoweringSummary &summary,
    const Objc3FrontendOptions &options,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendCompatibilityStrictnessClaimSemanticsSummary
        &semantic_summary,
    const Objc3ToolingFeatureAwareConformanceReportEmissionSummary
        &feature_summary,
    const Objc3ToolingCorpusShardingReleaseEvidencePackagingSummary
        &packaging_summary) {
  return ::objc3::artifacts::frontend::RenderVersionedConformanceReportArtifactJson(
      summary,
      static_cast<unsigned>(options.language_version),
      ::objc3::artifacts::BuildRunnableFeatureClaimInventoryJson(
          options, pipeline_result),
      ::objc3::artifacts::frontend::BuildFeatureClaimStrictnessTruthSurfaceJson(
          options, pipeline_result),
      BuildFrontendCompatibilityStrictnessClaimSemanticsSummaryJson(
          semantic_summary),
      ::objc3::artifacts::frontend::BuildRuntimeCapabilityReportJson(summary),
      ::objc3::artifacts::frontend::BuildPublicConformanceReportJson(summary),
      BuildToolingAdvancedFeatureReportingJson(feature_summary),
      BuildToolingAdvancedFeatureReleaseEvidenceJson(packaging_summary));
}

void PopulateObjc3FrontendVersionedConformanceReportOutput(
    Objc3FrontendArtifactBundle &bundle,
    const Objc3VersionedConformanceReportLoweringSummary &summary,
    const Objc3FrontendOptions &options,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendCompatibilityStrictnessClaimSemanticsSummary
        &semantic_summary,
    const Objc3ToolingFeatureAwareConformanceReportEmissionSummary
        &feature_summary,
    const Objc3ToolingCorpusShardingReleaseEvidencePackagingSummary
        &packaging_summary) {
  if (!IsReadyObjc3VersionedConformanceReportLoweringSummary(summary)) {
    return;
  }

  bundle.versioned_conformance_report_artifact_json =
      BuildVersionedConformanceReportArtifactJson(
          summary, options, pipeline_result, semantic_summary, feature_summary,
          packaging_summary);
}

}  // namespace objc3::artifacts::reports
