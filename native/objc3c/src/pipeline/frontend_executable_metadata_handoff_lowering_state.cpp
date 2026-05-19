#include "pipeline/frontend_executable_metadata_handoff_owners.h"

#include "runtime/metadata/class_metadata.h"
#include "sema/objc3_semantic_passes.h"

namespace objc3_frontend_executable_metadata_handoff {

void PopulateExecutableMetadataLoweringHandoffSurfaceState(
    Objc3ExecutableMetadataLoweringHandoffSurface &surface,
    const Objc3ExecutableMetadataSourceGraph &graph,
    const Objc3ExecutableMetadataSemanticConsistencyBoundary
        &semantic_consistency_boundary,
    const Objc3ExecutableMetadataSemanticValidationSurface
        &semantic_validation_surface,
    const Objc3SemanticTypeMetadataHandoff &sema_type_metadata_handoff,
    const Objc3FrontendProtocolCategorySummary &protocol_category_summary,
    const Objc3FrontendClassProtocolCategoryLinkingSummary
        &class_protocol_category_linking_summary,
    const Objc3FrontendSelectorNormalizationSummary
        &selector_normalization_summary,
    const Objc3FrontendPropertyAttributeSummary &property_attribute_summary,
    const Objc3FrontendSymbolGraphScopeResolutionSummary
        &symbol_graph_scope_resolution_summary,
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  surface.executable_metadata_source_graph_contract_id = graph.contract_id;
  surface.executable_metadata_semantic_consistency_contract_id =
      semantic_consistency_boundary.contract_id;
  surface.executable_metadata_semantic_validation_contract_id =
      semantic_validation_surface.contract_id;
  surface.source_graph_ready = IsReadyObjc3ExecutableMetadataSourceGraph(graph);
  surface.semantic_consistency_ready =
      IsReadyObjc3ExecutableMetadataSemanticConsistencyBoundary(
          semantic_consistency_boundary);
  surface.semantic_validation_ready =
      IsReadyObjc3ExecutableMetadataSemanticValidationSurface(
          semantic_validation_surface);
  surface.semantic_type_metadata_handoff_deterministic =
      IsDeterministicSemanticTypeMetadataHandoff(sema_type_metadata_handoff);
  surface.protocol_category_handoff_deterministic =
      protocol_category_summary.deterministic_protocol_category_handoff;
  surface.class_protocol_category_linking_handoff_deterministic =
      class_protocol_category_linking_summary
          .deterministic_class_protocol_category_linking_handoff;
  surface.selector_normalization_handoff_deterministic =
      selector_normalization_summary.deterministic_selector_normalization_handoff;
  surface.property_attribute_handoff_deterministic =
      property_attribute_summary.deterministic_property_attribute_handoff;
  surface.symbol_graph_scope_resolution_handoff_deterministic =
      symbol_graph_scope_resolution_summary.deterministic_symbol_graph_handoff &&
      symbol_graph_scope_resolution_summary
          .deterministic_scope_resolution_handoff;
  surface.property_synthesis_ivar_binding_handoff_deterministic =
      sema_parity_surface.property_synthesis_ivar_binding_summary.deterministic &&
      sema_parity_surface
          .deterministic_property_synthesis_ivar_binding_handoff;
  surface.interface_node_count = graph.interface_nodes_lexicographic.size();
  surface.implementation_node_count =
      graph.implementation_nodes_lexicographic.size();
  surface.class_node_count = graph.class_nodes_lexicographic.size();
  surface.metaclass_node_count = graph.metaclass_nodes_lexicographic.size();
  surface.protocol_node_count = graph.protocol_nodes_lexicographic.size();
  surface.category_node_count = graph.category_nodes_lexicographic.size();
  surface.property_node_count = graph.property_nodes_lexicographic.size();
  surface.method_node_count = graph.method_nodes_lexicographic.size();
  surface.ivar_node_count = graph.ivar_nodes_lexicographic.size();
  surface.owner_edge_count = graph.owner_edges_lexicographic.size();
}

}  // namespace objc3_frontend_executable_metadata_handoff
