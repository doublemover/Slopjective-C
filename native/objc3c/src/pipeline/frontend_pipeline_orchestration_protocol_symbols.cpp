#include "pipeline/frontend_pipeline_orchestration_owners.h"

#include <cstddef>

#include "parse/objc3_ast_builder_contract.h"
#include "pipeline/frontend_semantic_metadata_summary_helpers.h"
#include "pipeline/frontend_source_closure_replay_keys.h"

namespace {

Objc3FrontendSymbolGraphScopeResolutionSummary BuildSymbolGraphScopeResolutionSummary(
    const Objc3SemanticIntegrationSurface &integration_surface,
    const Objc3SemanticTypeMetadataHandoff &type_metadata_handoff) {
  Objc3FrontendSymbolGraphScopeResolutionSummary summary;
  const Objc3SymbolGraphScopeResolutionSummary &integration_summary =
      integration_surface.symbol_graph_scope_resolution_summary;
  const Objc3SymbolGraphScopeResolutionSummary &type_metadata_summary =
      type_metadata_handoff.symbol_graph_scope_resolution_summary;

  const auto select_value = [&](std::size_t integration_value,
                                std::size_t type_metadata_value) {
    if (integration_surface.built) {
      return integration_value;
    }
    return type_metadata_value;
  };

  summary.global_symbol_nodes = select_value(
      integration_summary.global_symbol_nodes,
      type_metadata_summary.global_symbol_nodes);
  summary.function_symbol_nodes = select_value(
      integration_summary.function_symbol_nodes,
      type_metadata_summary.function_symbol_nodes);
  summary.interface_symbol_nodes = select_value(
      integration_summary.interface_symbol_nodes,
      type_metadata_summary.interface_symbol_nodes);
  summary.implementation_symbol_nodes = select_value(
      integration_summary.implementation_symbol_nodes,
      type_metadata_summary.implementation_symbol_nodes);
  summary.interface_property_symbol_nodes = select_value(
      integration_summary.interface_property_symbol_nodes,
      type_metadata_summary.interface_property_symbol_nodes);
  summary.implementation_property_symbol_nodes = select_value(
      integration_summary.implementation_property_symbol_nodes,
      type_metadata_summary.implementation_property_symbol_nodes);
  summary.interface_method_symbol_nodes = select_value(
      integration_summary.interface_method_symbol_nodes,
      type_metadata_summary.interface_method_symbol_nodes);
  summary.implementation_method_symbol_nodes = select_value(
      integration_summary.implementation_method_symbol_nodes,
      type_metadata_summary.implementation_method_symbol_nodes);
  summary.top_level_scope_symbols = select_value(
      integration_summary.top_level_scope_symbols,
      type_metadata_summary.top_level_scope_symbols);
  summary.nested_scope_symbols = select_value(
      integration_summary.nested_scope_symbols,
      type_metadata_summary.nested_scope_symbols);
  summary.scope_frames_total = select_value(
      integration_summary.scope_frames_total,
      type_metadata_summary.scope_frames_total);
  summary.implementation_interface_resolution_sites =
      select_value(integration_summary.implementation_interface_resolution_sites,
                   type_metadata_summary.implementation_interface_resolution_sites);
  summary.implementation_interface_resolution_hits =
      select_value(integration_summary.implementation_interface_resolution_hits,
                   type_metadata_summary.implementation_interface_resolution_hits);
  summary.implementation_interface_resolution_misses =
      select_value(integration_summary.implementation_interface_resolution_misses,
                   type_metadata_summary.implementation_interface_resolution_misses);
  summary.method_resolution_sites = select_value(
      integration_summary.method_resolution_sites,
      type_metadata_summary.method_resolution_sites);
  summary.method_resolution_hits = select_value(
      integration_summary.method_resolution_hits,
      type_metadata_summary.method_resolution_hits);
  summary.method_resolution_misses = select_value(
      integration_summary.method_resolution_misses,
      type_metadata_summary.method_resolution_misses);

  const bool symbol_graph_fields_match =
      integration_summary.global_symbol_nodes == type_metadata_summary.global_symbol_nodes &&
      integration_summary.function_symbol_nodes == type_metadata_summary.function_symbol_nodes &&
      integration_summary.interface_symbol_nodes == type_metadata_summary.interface_symbol_nodes &&
      integration_summary.implementation_symbol_nodes == type_metadata_summary.implementation_symbol_nodes &&
      integration_summary.interface_property_symbol_nodes == type_metadata_summary.interface_property_symbol_nodes &&
      integration_summary.implementation_property_symbol_nodes ==
          type_metadata_summary.implementation_property_symbol_nodes &&
      integration_summary.interface_method_symbol_nodes == type_metadata_summary.interface_method_symbol_nodes &&
      integration_summary.implementation_method_symbol_nodes ==
          type_metadata_summary.implementation_method_symbol_nodes;
  const bool scope_resolution_fields_match =
      integration_summary.top_level_scope_symbols == type_metadata_summary.top_level_scope_symbols &&
      integration_summary.nested_scope_symbols == type_metadata_summary.nested_scope_symbols &&
      integration_summary.scope_frames_total == type_metadata_summary.scope_frames_total &&
      integration_summary.implementation_interface_resolution_sites ==
          type_metadata_summary.implementation_interface_resolution_sites &&
      integration_summary.implementation_interface_resolution_hits ==
          type_metadata_summary.implementation_interface_resolution_hits &&
      integration_summary.implementation_interface_resolution_misses ==
          type_metadata_summary.implementation_interface_resolution_misses &&
      integration_summary.method_resolution_sites == type_metadata_summary.method_resolution_sites &&
      integration_summary.method_resolution_hits == type_metadata_summary.method_resolution_hits &&
      integration_summary.method_resolution_misses == type_metadata_summary.method_resolution_misses;

  summary.deterministic_symbol_graph_handoff =
      integration_summary.deterministic &&
      type_metadata_summary.deterministic &&
      symbol_graph_fields_match &&
      summary.symbol_nodes_total() == summary.top_level_scope_symbols + summary.nested_scope_symbols;
  summary.deterministic_scope_resolution_handoff =
      integration_summary.deterministic &&
      type_metadata_summary.deterministic &&
      scope_resolution_fields_match &&
      summary.resolution_hits_total() <= summary.resolution_sites_total() &&
      summary.resolution_hits_total() + summary.resolution_misses_total() == summary.resolution_sites_total();
  summary.deterministic_handoff_key =
      objc3c::pipeline::orchestration::BuildSymbolGraphScopeResolutionHandoffKey(summary);
  return summary;
}

}  // namespace

namespace objc3_frontend_pipeline_orchestration {

void RefreshProtocolSymbolReadiness(Objc3FrontendPipelineResult &result) {
  result.protocol_category_summary =
      objc3c::pipeline::orchestration::BuildProtocolCategorySummary(
          Objc3ParsedProgramAst(result.program),
          result.integration_surface,
          result.sema_type_metadata_handoff);
  result.class_protocol_category_linking_summary =
      objc3c::pipeline::orchestration::BuildClassProtocolCategoryLinkingSummary(
          result.sema_type_metadata_handoff.interface_implementation_summary,
          result.protocol_category_summary,
          result.integration_surface,
          result.sema_type_metadata_handoff);
  result.symbol_graph_scope_resolution_summary =
      BuildSymbolGraphScopeResolutionSummary(result.integration_surface,
                                             result.sema_type_metadata_handoff);
}

}  // namespace objc3_frontend_pipeline_orchestration
