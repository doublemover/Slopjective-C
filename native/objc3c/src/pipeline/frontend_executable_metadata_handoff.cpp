#include "pipeline/frontend_executable_metadata_handoff.h"

#include "pipeline/frontend_executable_metadata_handoff_replay.h"
#include "runtime/metadata/class_metadata.h"
#include "sema/objc3_semantic_passes.h"

Objc3ExecutableMetadataLoweringHandoffSurface
BuildExecutableMetadataLoweringHandoffSurface(
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
  Objc3ExecutableMetadataLoweringHandoffSurface surface;
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

  if (surface.contract_id.empty()) {
    surface.failure_reason =
        "metadata lowering handoff contract id is empty";
  } else if (surface.executable_metadata_source_graph_contract_id.empty()) {
    surface.failure_reason =
        "metadata lowering handoff graph contract id is empty";
  } else if (surface.executable_metadata_semantic_consistency_contract_id.empty()) {
    surface.failure_reason =
        "metadata lowering handoff semantic consistency contract id is empty";
  } else if (surface.executable_metadata_semantic_validation_contract_id.empty()) {
    surface.failure_reason =
        "metadata lowering handoff semantic validation contract id is empty";
  } else if (!surface.source_graph_ready) {
    surface.failure_reason =
        "executable metadata source graph is not ready for lowering handoff";
  } else if (!surface.semantic_consistency_ready) {
    surface.failure_reason =
        "executable metadata semantic consistency boundary is not ready";
  } else if (!surface.semantic_validation_ready) {
    surface.failure_reason =
        "executable metadata semantic validation surface is not ready";
  } else if (!surface.semantic_type_metadata_handoff_deterministic) {
    surface.failure_reason =
        "semantic type metadata handoff is not deterministic";
  } else if (!surface.protocol_category_handoff_deterministic) {
    surface.failure_reason =
        "protocol/category metadata handoff is not deterministic";
  } else if (!surface.class_protocol_category_linking_handoff_deterministic) {
    surface.failure_reason =
        "class/protocol/category metadata linking handoff is not deterministic";
  } else if (!surface.selector_normalization_handoff_deterministic) {
    surface.failure_reason =
        "selector normalization handoff is not deterministic";
  } else if (!surface.property_attribute_handoff_deterministic) {
    surface.failure_reason =
        "property attribute handoff is not deterministic";
  } else if (!surface.symbol_graph_scope_resolution_handoff_deterministic) {
    surface.failure_reason =
        "symbol graph and scope resolution handoff is not deterministic";
  } else if (!surface.property_synthesis_ivar_binding_handoff_deterministic) {
    surface.failure_reason =
        "property synthesis and ivar binding handoff is not deterministic";
  }

  surface.lowering_schema_frozen = surface.failure_reason.empty();
  surface.ready_for_lowering = false;
  surface.fail_closed =
      surface.lowering_schema_frozen && !surface.ready_for_lowering;
  surface.replay_key = BuildExecutableMetadataLoweringHandoffReplayKey(surface);
  if (surface.failure_reason.empty() && !surface.fail_closed) {
    surface.failure_reason =
        "metadata lowering handoff freeze is not fail-closed";
  }
  if (!surface.failure_reason.empty()) {
    surface.replay_key.clear();
  }
  return surface;
}

Objc3ExecutableMetadataTypedLoweringHandoff
BuildExecutableMetadataTypedLoweringHandoff(
    const Objc3ExecutableMetadataSourceGraph &graph,
    const Objc3ExecutableMetadataSemanticConsistencyBoundary
        &semantic_consistency_boundary,
    const Objc3ExecutableMetadataSemanticValidationSurface
        &semantic_validation_surface,
    const Objc3ExecutableMetadataLoweringHandoffSurface
        &lowering_handoff_surface) {
  Objc3ExecutableMetadataTypedLoweringHandoff surface;
  surface.executable_metadata_lowering_handoff_contract_id =
      lowering_handoff_surface.contract_id;
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
  surface.lowering_handoff_surface_ready =
      IsReadyObjc3ExecutableMetadataLoweringHandoffSurface(
          lowering_handoff_surface);
  surface.source_graph = graph;

  if (surface.contract_id.empty()) {
    surface.failure_reason =
        "typed lowering handoff contract id is empty";
  } else if (surface.executable_metadata_lowering_handoff_contract_id.empty()) {
    surface.failure_reason =
        "typed lowering handoff contract id is empty";
  } else if (surface.executable_metadata_source_graph_contract_id.empty()) {
    surface.failure_reason =
        "typed lowering handoff source graph contract id is empty";
  } else if (surface.executable_metadata_semantic_consistency_contract_id.empty()) {
    surface.failure_reason =
        "typed lowering handoff semantic consistency contract id is empty";
  } else if (surface.executable_metadata_semantic_validation_contract_id.empty()) {
    surface.failure_reason =
        "typed lowering handoff semantic validation contract id is empty";
  } else if (surface.manifest_schema_ordering_model.empty()) {
    surface.failure_reason =
        "typed lowering handoff manifest schema ordering model is empty";
  } else if (!surface.source_graph_ready) {
    surface.failure_reason =
        "typed lowering handoff source graph is not ready";
  } else if (!surface.semantic_consistency_ready) {
    surface.failure_reason =
        "typed lowering handoff semantic consistency boundary is not ready";
  } else if (!surface.semantic_validation_ready) {
    surface.failure_reason =
        "typed lowering handoff semantic validation surface is not ready";
  } else if (!surface.lowering_handoff_surface_ready) {
    surface.failure_reason =
        "typed lowering handoff freeze surface is not ready";
  } else if (graph.interface_nodes_lexicographic.size() !=
             lowering_handoff_surface.interface_node_count) {
    surface.failure_reason =
        "typed lowering handoff interface node count drifted from the freeze surface";
  } else if (graph.implementation_nodes_lexicographic.size() !=
             lowering_handoff_surface.implementation_node_count) {
    surface.failure_reason =
        "typed lowering handoff implementation node count drifted from the freeze surface";
  } else if (graph.class_nodes_lexicographic.size() !=
             lowering_handoff_surface.class_node_count) {
    surface.failure_reason =
        "typed lowering handoff class node count drifted from the freeze surface";
  } else if (graph.metaclass_nodes_lexicographic.size() !=
             lowering_handoff_surface.metaclass_node_count) {
    surface.failure_reason =
        "typed lowering handoff metaclass node count drifted from the freeze surface";
  } else if (graph.protocol_nodes_lexicographic.size() !=
             lowering_handoff_surface.protocol_node_count) {
    surface.failure_reason =
        "typed lowering handoff protocol node count drifted from the freeze surface";
  } else if (graph.category_nodes_lexicographic.size() !=
             lowering_handoff_surface.category_node_count) {
    surface.failure_reason =
        "typed lowering handoff category node count drifted from the freeze surface";
  } else if (graph.property_nodes_lexicographic.size() !=
             lowering_handoff_surface.property_node_count) {
    surface.failure_reason =
        "typed lowering handoff property node count drifted from the freeze surface";
  } else if (graph.method_nodes_lexicographic.size() !=
             lowering_handoff_surface.method_node_count) {
    surface.failure_reason =
        "typed lowering handoff method node count drifted from the freeze surface";
  } else if (graph.ivar_nodes_lexicographic.size() !=
             lowering_handoff_surface.ivar_node_count) {
    surface.failure_reason =
        "typed lowering handoff ivar node count drifted from the freeze surface";
  } else if (graph.owner_edges_lexicographic.size() !=
             lowering_handoff_surface.owner_edge_count) {
    surface.failure_reason =
        "typed lowering handoff owner edge count drifted from the freeze surface";
  }

  surface.deterministic =
      surface.failure_reason.empty() && graph.deterministic &&
      !lowering_handoff_surface.replay_key.empty();
  surface.manifest_schema_frozen = surface.failure_reason.empty();
  surface.fail_closed = surface.manifest_schema_frozen;
  surface.ready_for_lowering =
      surface.manifest_schema_frozen && surface.deterministic &&
      surface.fail_closed;
  if (surface.failure_reason.empty()) {
    surface.replay_key =
        BuildExecutableMetadataTypedLoweringHandoffReplayKey(surface);
  }
  if (!surface.failure_reason.empty() || !surface.ready_for_lowering) {
    if (surface.failure_reason.empty()) {
      surface.failure_reason =
          "typed lowering handoff did not become lowering-ready";
    }
    surface.replay_key.clear();
  }
  return surface;
}
