#include "artifacts/objc3_frontend_artifact_semantic_closure_metadata.h"

namespace objc3::artifacts::frontend {

void ApplyObjc3FrontendSemanticClosureMetadata(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const Objc3FrontendObjectPointerNullabilityGenericsSummary
        &object_pointer_nullability_generics_summary,
    const Objc3FrontendSymbolGraphScopeResolutionSummary
        &symbol_graph_scope_resolution_summary,
    bool deterministic_interface_implementation_handoff,
    const Objc3InterfaceImplementationSummary &interface_implementation_summary,
    const Objc3FrontendProtocolCategorySummary &protocol_category_summary,
    const Objc3FrontendClassProtocolCategoryLinkingSummary
        &class_protocol_category_linking_summary,
    const Objc3FrontendSelectorNormalizationSummary
        &selector_normalization_summary,
    const Objc3FrontendPropertyAttributeSummary &property_attribute_summary) {
  ir_frontend_metadata.object_pointer_type_spellings =
      object_pointer_nullability_generics_summary.object_pointer_type_spellings;
  ir_frontend_metadata.pointer_declarator_entries =
      object_pointer_nullability_generics_summary.pointer_declarator_entries;
  ir_frontend_metadata.pointer_declarator_depth_total =
      object_pointer_nullability_generics_summary.pointer_declarator_depth_total;
  ir_frontend_metadata.pointer_declarator_token_entries =
      object_pointer_nullability_generics_summary.pointer_declarator_token_entries;
  ir_frontend_metadata.nullability_suffix_entries =
      object_pointer_nullability_generics_summary.nullability_suffix_entries;
  ir_frontend_metadata.generic_suffix_entries =
      object_pointer_nullability_generics_summary.generic_suffix_entries;
  ir_frontend_metadata.terminated_generic_suffix_entries =
      object_pointer_nullability_generics_summary
          .terminated_generic_suffix_entries;
  ir_frontend_metadata.unterminated_generic_suffix_entries =
      object_pointer_nullability_generics_summary
          .unterminated_generic_suffix_entries;
  ir_frontend_metadata
      .deterministic_object_pointer_nullability_generics_handoff =
      object_pointer_nullability_generics_summary
          .deterministic_object_pointer_nullability_generics_handoff;

  ir_frontend_metadata.global_symbol_nodes =
      symbol_graph_scope_resolution_summary.global_symbol_nodes;
  ir_frontend_metadata.function_symbol_nodes =
      symbol_graph_scope_resolution_summary.function_symbol_nodes;
  ir_frontend_metadata.interface_symbol_nodes =
      symbol_graph_scope_resolution_summary.interface_symbol_nodes;
  ir_frontend_metadata.implementation_symbol_nodes =
      symbol_graph_scope_resolution_summary.implementation_symbol_nodes;
  ir_frontend_metadata.interface_property_symbol_nodes =
      symbol_graph_scope_resolution_summary.interface_property_symbol_nodes;
  ir_frontend_metadata.implementation_property_symbol_nodes =
      symbol_graph_scope_resolution_summary.implementation_property_symbol_nodes;
  ir_frontend_metadata.interface_method_symbol_nodes =
      symbol_graph_scope_resolution_summary.interface_method_symbol_nodes;
  ir_frontend_metadata.implementation_method_symbol_nodes =
      symbol_graph_scope_resolution_summary.implementation_method_symbol_nodes;
  ir_frontend_metadata.top_level_scope_symbols =
      symbol_graph_scope_resolution_summary.top_level_scope_symbols;
  ir_frontend_metadata.nested_scope_symbols =
      symbol_graph_scope_resolution_summary.nested_scope_symbols;
  ir_frontend_metadata.scope_frames_total =
      symbol_graph_scope_resolution_summary.scope_frames_total;
  ir_frontend_metadata.implementation_interface_resolution_sites =
      symbol_graph_scope_resolution_summary
          .implementation_interface_resolution_sites;
  ir_frontend_metadata.implementation_interface_resolution_hits =
      symbol_graph_scope_resolution_summary
          .implementation_interface_resolution_hits;
  ir_frontend_metadata.implementation_interface_resolution_misses =
      symbol_graph_scope_resolution_summary
          .implementation_interface_resolution_misses;
  ir_frontend_metadata.method_resolution_sites =
      symbol_graph_scope_resolution_summary.method_resolution_sites;
  ir_frontend_metadata.method_resolution_hits =
      symbol_graph_scope_resolution_summary.method_resolution_hits;
  ir_frontend_metadata.method_resolution_misses =
      symbol_graph_scope_resolution_summary.method_resolution_misses;
  ir_frontend_metadata.deterministic_symbol_graph_handoff =
      symbol_graph_scope_resolution_summary.deterministic_symbol_graph_handoff;
  ir_frontend_metadata.deterministic_scope_resolution_handoff =
      symbol_graph_scope_resolution_summary.deterministic_scope_resolution_handoff;
  ir_frontend_metadata.deterministic_symbol_graph_scope_resolution_handoff_key =
      symbol_graph_scope_resolution_summary.deterministic_handoff_key;

  ir_frontend_metadata.deterministic_interface_implementation_handoff =
      deterministic_interface_implementation_handoff &&
      interface_implementation_summary.deterministic;
  ir_frontend_metadata.deterministic_protocol_category_handoff =
      protocol_category_summary.deterministic_protocol_category_handoff;
  ir_frontend_metadata.deterministic_class_protocol_category_linking_handoff =
      class_protocol_category_linking_summary
          .deterministic_class_protocol_category_linking_handoff;
  ir_frontend_metadata.deterministic_selector_normalization_handoff =
      selector_normalization_summary
          .deterministic_selector_normalization_handoff;
  ir_frontend_metadata.deterministic_property_attribute_handoff =
      property_attribute_summary.deterministic_property_attribute_handoff;
}

}  // namespace objc3::artifacts::frontend
