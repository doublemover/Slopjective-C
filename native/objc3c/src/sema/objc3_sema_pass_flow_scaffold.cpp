#include "sema/objc3_sema_pass_flow_scaffold.h"

#include <cstddef>

void MarkObjc3SemaPassExecuted(Objc3SemaPassFlowSummary &summary, Objc3SemaPassId pass) {
  const std::size_t pass_index = static_cast<std::size_t>(pass);
  if (pass_index >= summary.pass_executed.size()) {
    return;
  }
  if (!summary.pass_executed[pass_index]) {
    summary.pass_executed[pass_index] = true;
    ++summary.executed_pass_count;
  }
}

void FinalizeObjc3SemaPassFlowSummary(
    Objc3SemaPassFlowSummary &summary,
    const Objc3SemanticIntegrationSurface &integration_surface,
    const Objc3SemanticTypeMetadataHandoff &type_metadata_handoff,
    bool diagnostics_after_pass_monotonic,
    bool pass_order_matches_contract,
    bool deterministic_semantic_diagnostics,
    bool deterministic_type_metadata_handoff) {
  summary.pass_order_matches_contract =
      pass_order_matches_contract && summary.configured_pass_order == kObjc3SemaPassOrder;
  summary.diagnostics_after_pass_monotonic = diagnostics_after_pass_monotonic;

  summary.symbol_globals_count = integration_surface.globals.size();
  summary.symbol_functions_count = integration_surface.functions.size();
  summary.symbol_interfaces_count = integration_surface.interfaces.size();
  summary.symbol_implementations_count = integration_surface.implementations.size();

  summary.type_metadata_global_entries = type_metadata_handoff.global_names_lexicographic.size();
  summary.type_metadata_function_entries = type_metadata_handoff.functions_lexicographic.size();
  summary.type_metadata_interface_entries = type_metadata_handoff.interfaces_lexicographic.size();
  summary.type_metadata_implementation_entries = type_metadata_handoff.implementations_lexicographic.size();

  summary.symbol_flow_counts_consistent =
      summary.symbol_globals_count == summary.type_metadata_global_entries &&
      summary.symbol_functions_count == summary.type_metadata_function_entries &&
      summary.symbol_interfaces_count == summary.type_metadata_interface_entries &&
      summary.symbol_implementations_count == summary.type_metadata_implementation_entries;

  summary.deterministic =
      summary.pass_order_matches_contract &&
      summary.configured_pass_count == kObjc3SemaPassOrder.size() &&
      summary.executed_pass_count == summary.configured_pass_count &&
      summary.diagnostics_after_pass_monotonic &&
      summary.symbol_flow_counts_consistent &&
      deterministic_semantic_diagnostics &&
      deterministic_type_metadata_handoff;
}
