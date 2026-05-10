#include "pipeline/frontend_source_closure_replay_keys.h"

#include <sstream>

namespace objc3c::pipeline::orchestration {

std::string BuildToolingDiagnosticsMigratorSourceInventoryReplayKey(
    const Objc3FrontendToolingDiagnosticsMigratorSourceInventorySummary
        &summary) {
  std::ostringstream out;
  out << "families=" << summary.advanced_feature_family_count
      << ";dependencies=" << summary.dependency_surface_count
      << ";claims=" << summary.aggregated_source_only_claim_count
      << ";fail-closed=" << summary.fail_closed_construct_count
      << ";sites=" << summary.diagnostic_surface_sites << ":"
      << summary.fixit_surface_sites << ":" << summary.migrator_surface_sites
      << ":" << summary.canonicalization_hint_sites
      << ";parts=" << summary.error_surface_sites << ":"
      << summary.concurrency_surface_sites << ":" << summary.system_surface_sites
      << ":" << summary.dispatch_surface_sites << ":"
      << summary.metaprogramming_surface_sites << ":"
      << summary.interop_surface_sites << ";supported="
      << (summary.diagnostics_inventory_source_supported ? "true" : "false")
      << ":"
      << (summary.fixit_inventory_source_supported ? "true" : "false") << ":"
      << (summary.migrator_inventory_source_supported ? "true" : "false")
      << ";deterministic="
      << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildToolingMigrationCanonicalizationSourceCompletionReplayKey(
    const Objc3FrontendToolingMigrationCanonicalizationSourceCompletionSummary
        &summary) {
  std::ostringstream out;
  out << "compat=" << summary.language_profile
      << ";canonical-rejection-diagnostics="
      << (summary.canonical_literal_rejection_diagnostics_enabled ? "true"
                                                                  : "false")
      << ";canonical-literal-rejections=" << summary.legacy_yes_sites << ":"
      << summary.legacy_no_sites << ":" << summary.legacy_null_sites << ":"
      << summary.legacy_total_sites
      << ";canonical=" << summary.canonical_true_rewrite_sites << ":"
      << summary.canonical_false_rewrite_sites << ":"
      << summary.canonical_nil_rewrite_sites
      << ";candidates=" << summary.canonicalization_candidate_sites << ":"
      << summary.fixit_candidate_sites << ":" << summary.migrator_candidate_sites
      << ";ready="
      << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ";deterministic="
      << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildToolingDiagnosticTaxonomyPortabilityContractReplayKey(
    const Objc3ToolingDiagnosticTaxonomyPortabilityContractSummary &summary) {
  std::ostringstream out;
  out << "portability-dependencies=" << summary.portability_dependency_count
      << ";diagnostics=" << summary.diagnostics_total << ":"
      << summary.diagnostics_after_pass_final << ":"
      << summary.diagnostics_emitted_total
      << ";arc-fixit=" << summary.ownership_arc_diagnostic_candidate_sites
      << ":" << summary.ownership_arc_fixit_available_sites << ":"
      << summary.ownership_arc_profiled_sites << ":"
      << summary.ownership_arc_weak_unowned_conflict_diagnostic_sites << ":"
      << summary.ownership_arc_empty_fixit_hint_sites << ":"
      << summary.ownership_arc_contract_violation_sites
      << ";migration-candidates="
      << summary.migration_canonicalization_candidate_sites
      << ";ready=" << (summary.ready_for_lowering_and_runtime ? "true"
                                                              : "false")
      << ";deterministic="
      << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildToolingFeatureSpecificFixitSynthesisReplayKey(
    const Objc3ToolingFeatureSpecificFixitSynthesisSummary &summary) {
  std::ostringstream out;
  out << "families=" << summary.fixit_family_count
      << ";migration=" << summary.migration_fixit_candidate_sites << ":"
      << summary.migrator_candidate_sites
      << ";ownership-arc=" << summary.ownership_arc_fixit_available_sites << ":"
      << summary.ownership_arc_empty_fixit_hint_sites
      << ";ready=" << (summary.ready_for_lowering_and_runtime ? "true"
                                                              : "false")
      << ";deterministic="
      << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildSymbolGraphScopeResolutionHandoffKey(
    const Objc3FrontendSymbolGraphScopeResolutionSummary &summary) {
  std::ostringstream out;
  out << "symbol_graph_nodes=" << summary.global_symbol_nodes << ":"
      << summary.function_symbol_nodes << ":" << summary.interface_symbol_nodes
      << ":" << summary.implementation_symbol_nodes << ":"
      << summary.interface_property_symbol_nodes << ":"
      << summary.implementation_property_symbol_nodes << ":"
      << summary.interface_method_symbol_nodes << ":"
      << summary.implementation_method_symbol_nodes
      << ";scope_surface=" << summary.top_level_scope_symbols << ":"
      << summary.nested_scope_symbols << ":" << summary.scope_frames_total
      << ";resolution_surface="
      << summary.implementation_interface_resolution_sites << ":"
      << summary.implementation_interface_resolution_hits << ":"
      << summary.implementation_interface_resolution_misses << ":"
      << summary.method_resolution_sites << ":" << summary.method_resolution_hits
      << ":" << summary.method_resolution_misses
      << ";deterministic="
      << (summary.deterministic_symbol_graph_handoff ? "true" : "false")
      << ":"
      << (summary.deterministic_scope_resolution_handoff ? "true" : "false");
  return out.str();
}

}  // namespace objc3c::pipeline::orchestration
