#include "artifacts/objc3_frontend_tooling_source_artifacts.h"

#include <sstream>
#include <string>
#include <vector>

#include "io/json/json_writer.h"
#include "io/objc3_json.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

std::string BuildStringArrayJson(const std::vector<std::string> &values) {
  return objc3::io::json::RenderJsonStringArray(values);
}

}  // namespace

std::string BuildToolingDiagnosticsMigratorSourceInventorySummaryJson(
    const Objc3FrontendToolingDiagnosticsMigratorSourceInventorySummary
        &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"frontend_surface_path\":\""
      << EscapeJsonString(summary.frontend_surface_path)
      << "\",\"source_model\":\"" << EscapeJsonString(summary.source_model)
      << "\",\"failure_model\":\"" << EscapeJsonString(summary.failure_model)
      << "\",\"dependency_contract_ids\":"
      << BuildStringArrayJson(summary.dependency_contract_ids)
      << ",\"source_only_claim_ids\":"
      << BuildStringArrayJson(summary.source_only_claim_ids)
      << ",\"advanced_feature_family_count\":"
      << summary.advanced_feature_family_count
      << ",\"dependency_surface_count\":"
      << summary.dependency_surface_count
      << ",\"aggregated_source_only_claim_count\":"
      << summary.aggregated_source_only_claim_count
      << ",\"fail_closed_construct_count\":"
      << summary.fail_closed_construct_count
      << ",\"diagnostic_surface_sites\":"
      << summary.diagnostic_surface_sites
      << ",\"fixit_surface_sites\":" << summary.fixit_surface_sites
      << ",\"migrator_surface_sites\":" << summary.migrator_surface_sites
      << ",\"canonicalization_hint_sites\":"
      << summary.canonicalization_hint_sites
      << ",\"error_surface_sites\":" << summary.error_surface_sites
      << ",\"concurrency_surface_sites\":"
      << summary.concurrency_surface_sites
      << ",\"system_surface_sites\":" << summary.system_surface_sites
      << ",\"dispatch_surface_sites\":" << summary.dispatch_surface_sites
      << ",\"metaprogramming_surface_sites\":"
      << summary.metaprogramming_surface_sites
      << ",\"interop_surface_sites\":" << summary.interop_surface_sites
      << ",\"diagnostics_inventory_source_supported\":"
      << (summary.diagnostics_inventory_source_supported ? "true" : "false")
      << ",\"fixit_inventory_source_supported\":"
      << (summary.fixit_inventory_source_supported ? "true" : "false")
      << ",\"migrator_inventory_source_supported\":"
      << (summary.migrator_inventory_source_supported ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_semantic_expansion\":"
      << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildToolingMigrationCanonicalizationSourceCompletionSummaryJson(
    const Objc3FrontendToolingMigrationCanonicalizationSourceCompletionSummary
        &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"frontend_surface_path\":\""
      << EscapeJsonString(summary.frontend_surface_path)
      << "\",\"source_model\":\"" << EscapeJsonString(summary.source_model)
      << "\",\"completion_model\":\""
      << EscapeJsonString(summary.completion_model)
      << "\",\"failure_model\":\""
      << EscapeJsonString(summary.failure_model)
      << "\",\"source_only_claim_ids\":"
      << BuildStringArrayJson(summary.source_only_claim_ids)
      << ",\"language_profile\":\""
      << EscapeJsonString(summary.language_profile)
      << "\",\"canonical_literal_rejection_diagnostics_enabled\":"
      << (summary.canonical_literal_rejection_diagnostics_enabled ? "true"
                                                                  : "false")
      << ",\"canonical_literal_rejection_diagnostics_required\":"
      << (summary.canonical_literal_rejection_diagnostics_required ? "true"
                                                                  : "false")
      << ",\"canonical_yes_literal_rejection_sites\":"
      << summary.legacy_yes_sites
      << ",\"canonical_no_literal_rejection_sites\":"
      << summary.legacy_no_sites
      << ",\"canonical_null_literal_rejection_sites\":"
      << summary.legacy_null_sites
      << ",\"canonical_literal_rejection_total_sites\":"
      << summary.legacy_total_sites
      << ",\"canonical_true_rewrite_sites\":"
      << summary.canonical_true_rewrite_sites
      << ",\"canonical_false_rewrite_sites\":"
      << summary.canonical_false_rewrite_sites
      << ",\"canonical_nil_rewrite_sites\":"
      << summary.canonical_nil_rewrite_sites
      << ",\"canonicalization_candidate_sites\":"
      << summary.canonicalization_candidate_sites
      << ",\"fixit_candidate_sites\":" << summary.fixit_candidate_sites
      << ",\"migrator_candidate_sites\":"
      << summary.migrator_candidate_sites
      << ",\"dependency_inventory_ready\":"
      << (summary.dependency_inventory_ready ? "true" : "false")
      << ",\"canonicalization_surface_supported\":"
      << (summary.canonicalization_surface_supported ? "true" : "false")
      << ",\"fixit_migration_surface_supported\":"
      << (summary.fixit_migration_surface_supported ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_semantic_expansion\":"
      << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildToolingDiagnosticTaxonomyPortabilityContractSummaryJson(
    const Objc3ToolingDiagnosticTaxonomyPortabilityContractSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"frontend_surface_path\":\""
      << EscapeJsonString(summary.frontend_surface_path)
      << "\",\"semantic_model\":\""
      << EscapeJsonString(summary.semantic_model)
      << "\",\"portability_model\":\""
      << EscapeJsonString(summary.portability_model)
      << "\",\"portability_dependency_surface_ids\":"
      << BuildStringArrayJson(summary.portability_dependency_surface_ids)
      << ",\"diagnostic_namespace\":\""
      << EscapeJsonString(summary.diagnostic_namespace)
      << "\",\"portability_dependency_count\":"
      << summary.portability_dependency_count
      << ",\"diagnostics_total\":" << summary.diagnostics_total
      << ",\"diagnostics_after_pass_final\":"
      << summary.diagnostics_after_pass_final
      << ",\"diagnostics_emitted_total\":"
      << summary.diagnostics_emitted_total
      << ",\"ownership_arc_diagnostic_candidate_sites\":"
      << summary.ownership_arc_diagnostic_candidate_sites
      << ",\"ownership_arc_fixit_available_sites\":"
      << summary.ownership_arc_fixit_available_sites
      << ",\"ownership_arc_profiled_sites\":"
      << summary.ownership_arc_profiled_sites
      << ",\"ownership_arc_weak_unowned_conflict_diagnostic_sites\":"
      << summary.ownership_arc_weak_unowned_conflict_diagnostic_sites
      << ",\"ownership_arc_empty_fixit_hint_sites\":"
      << summary.ownership_arc_empty_fixit_hint_sites
      << ",\"ownership_arc_contract_violation_sites\":"
      << summary.ownership_arc_contract_violation_sites
      << ",\"migration_canonicalization_candidate_sites\":"
      << summary.migration_canonicalization_candidate_sites
      << ",\"migration_surface_ready\":"
      << (summary.migration_surface_ready ? "true" : "false")
      << ",\"portability_dependencies_frozen\":"
      << (summary.portability_dependencies_frozen ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"recovery_replay_key\":\""
      << EscapeJsonString(summary.recovery_replay_key)
      << "\",\"arc_diagnostics_fixit_replay_key\":\""
      << EscapeJsonString(summary.arc_diagnostics_fixit_replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildToolingFeatureSpecificFixitSynthesisSummaryJson(
    const Objc3ToolingFeatureSpecificFixitSynthesisSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"frontend_surface_path\":\""
      << EscapeJsonString(summary.frontend_surface_path)
      << "\",\"semantic_model\":\""
      << EscapeJsonString(summary.semantic_model)
      << "\",\"fixit_family_ids\":"
      << BuildStringArrayJson(summary.fixit_family_ids)
      << ",\"fixit_family_count\":" << summary.fixit_family_count
      << ",\"migration_fixit_candidate_sites\":"
      << summary.migration_fixit_candidate_sites
      << ",\"migrator_candidate_sites\":" << summary.migrator_candidate_sites
      << ",\"ownership_arc_fixit_available_sites\":"
      << summary.ownership_arc_fixit_available_sites
      << ",\"ownership_arc_empty_fixit_hint_sites\":"
      << summary.ownership_arc_empty_fixit_hint_sites
      << ",\"diagnostic_taxonomy_ready\":"
      << (summary.diagnostic_taxonomy_ready ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildToolingLegacyCanonicalMigrationSemanticsReplayKey(
    const Objc3ToolingLegacyCanonicalMigrationSemanticsSummary &summary) {
  std::ostringstream out;
  out << "mode=" << summary.effective_language_profile
      << ";canonical-rejection-diagnostics="
      << (summary.canonical_literal_rejection_diagnostics_enabled ? "true"
                                                                  : "false")
      << ";canonical-literal-rejection-sites="
      << summary.current_run_legacy_literal_sites
      << ";candidates=" << summary.current_run_canonicalization_candidate_sites
      << ";families=" << summary.fixit_family_count
      << ";fail-closed=" << (summary.fail_closed ? "true" : "false")
      << ";rejection-code=" << summary.canonical_mode_rejection_code
      << ";ready="
      << (summary.ready_for_lowering_and_runtime ? "true" : "false");
  return out.str();
}

Objc3ToolingLegacyCanonicalMigrationSemanticsSummary
BuildToolingLegacyCanonicalMigrationSemanticsSummary(
    const Objc3FrontendCompatibilityStrictnessClaimSemanticsSummary
        &compatibility_summary,
    const Objc3ToolingFeatureSpecificFixitSynthesisSummary &fixit_summary) {
  Objc3ToolingLegacyCanonicalMigrationSemanticsSummary summary;
  summary.effective_language_profile =
      compatibility_summary.effective_language_profile;
  summary.canonical_literal_rejection_diagnostics_enabled =
      compatibility_summary.canonical_literal_rejection_diagnostics_enabled;
  summary.current_run_legacy_literal_sites =
      fixit_summary.migration_fixit_candidate_sites;
  summary.current_run_canonicalization_candidate_sites =
      fixit_summary.migration_fixit_candidate_sites;
  summary.fixit_family_count = fixit_summary.fixit_family_count;
  summary.fail_closed = compatibility_summary.fail_closed;
  summary.selected_configuration_valid =
      compatibility_summary.selected_configuration_valid;
  summary.language_profile_semantics_landed =
      compatibility_summary.language_profile_semantics_landed;
  summary.canonical_literal_rejection_semantics_landed =
      compatibility_summary.canonical_literal_rejection_semantics_landed;
  summary.canonical_mode_rejection_ready =
      compatibility_summary.fail_closed &&
      compatibility_summary.language_profile_semantics_landed &&
      compatibility_summary.canonical_literal_rejection_semantics_landed &&
      summary.canonical_mode_rejection_code ==
          kObjc3ToolingLegacyCanonicalMigrationDiagnosticCode;
  summary.feature_specific_fixit_surface_ready =
      fixit_summary.ready_for_lowering_and_runtime;
  summary.deterministic_handoff =
      IsReadyObjc3FrontendCompatibilityStrictnessClaimSemanticsSummary(
          compatibility_summary) &&
      summary.feature_specific_fixit_surface_ready &&
      summary.selected_configuration_valid &&
      !compatibility_summary.selected_configuration_downgraded &&
      !compatibility_summary.selected_configuration_rejected &&
      summary.canonical_mode_rejection_ready;
  summary.ready_for_lowering_and_runtime = summary.deterministic_handoff;
  if (!summary.feature_specific_fixit_surface_ready) {
    summary.failure_reason =
        "tooling feature-specific fix-it synthesis prerequisite is not ready";
  } else if (!IsReadyObjc3FrontendCompatibilityStrictnessClaimSemanticsSummary(
                 compatibility_summary)) {
    summary.failure_reason =
        "frontend compatibility/strictness semantics prerequisite is not ready";
  }
  summary.replay_key =
      BuildToolingLegacyCanonicalMigrationSemanticsReplayKey(summary);
  return summary;
}

std::string BuildToolingLegacyCanonicalMigrationSemanticsSummaryJson(
    const Objc3ToolingLegacyCanonicalMigrationSemanticsSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"compatibility_semantics_contract_id\":\""
      << EscapeJsonString(summary.compatibility_semantics_contract_id)
      << "\",\"frontend_surface_path\":\""
      << EscapeJsonString(summary.frontend_surface_path)
      << "\",\"semantic_model\":\""
      << EscapeJsonString(summary.semantic_model)
      << "\",\"language_profile_model\":\""
      << EscapeJsonString(summary.language_profile_model)
      << "\",\"effective_language_profile\":\""
      << EscapeJsonString(summary.effective_language_profile)
      << "\",\"canonical_literal_rejection_diagnostics_enabled\":"
      << (summary.canonical_literal_rejection_diagnostics_enabled ? "true"
                                                                  : "false")
      << ",\"current_run_canonical_literal_rejection_sites\":"
      << summary.current_run_legacy_literal_sites
      << ",\"current_run_canonicalization_candidate_sites\":"
      << summary.current_run_canonicalization_candidate_sites
      << ",\"fixit_family_count\":" << summary.fixit_family_count
      << ",\"fail_closed\":" << (summary.fail_closed ? "true" : "false")
      << ",\"selected_configuration_valid\":"
      << (summary.selected_configuration_valid ? "true" : "false")
      << ",\"language_profile_semantics_landed\":"
      << (summary.language_profile_semantics_landed ? "true" : "false")
      << ",\"canonical_literal_rejection_semantics_landed\":"
      << (summary.canonical_literal_rejection_semantics_landed ? "true"
                                                               : "false")
      << ",\"canonical_mode_rejection_code\":\""
      << EscapeJsonString(summary.canonical_mode_rejection_code)
      << "\",\"canonical_mode_rejection_ready\":"
      << (summary.canonical_mode_rejection_ready ? "true" : "false")
      << ",\"feature_specific_fixit_surface_ready\":"
      << (summary.feature_specific_fixit_surface_ready ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

}  // namespace objc3::artifacts::frontend
