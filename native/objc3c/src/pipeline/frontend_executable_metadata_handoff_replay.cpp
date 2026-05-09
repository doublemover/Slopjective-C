#include "pipeline/frontend_executable_metadata_handoff_replay.h"

#include <sstream>

#include "runtime/metadata/class_metadata.h"

std::string BuildExecutableMetadataLoweringHandoffReplayKey(
    const Objc3ExecutableMetadataLoweringHandoffSurface &surface) {
  std::ostringstream out;
  out << "executable-metadata-lowering-handoff:v1"
      << ";source_graph_ready=" << (surface.source_graph_ready ? "true" : "false")
      << ";semantic_consistency_ready="
      << (surface.semantic_consistency_ready ? "true" : "false")
      << ";semantic_validation_ready="
      << (surface.semantic_validation_ready ? "true" : "false")
      << ";semantic_type_metadata_handoff_deterministic="
      << (surface.semantic_type_metadata_handoff_deterministic ? "true"
                                                               : "false")
      << ";protocol_category_handoff_deterministic="
      << (surface.protocol_category_handoff_deterministic ? "true" : "false")
      << ";class_protocol_category_linking_handoff_deterministic="
      << (surface.class_protocol_category_linking_handoff_deterministic ? "true"
                                                                        : "false")
      << ";selector_normalization_handoff_deterministic="
      << (surface.selector_normalization_handoff_deterministic ? "true"
                                                               : "false")
      << ";property_attribute_handoff_deterministic="
      << (surface.property_attribute_handoff_deterministic ? "true" : "false")
      << ";symbol_graph_scope_resolution_handoff_deterministic="
      << (surface.symbol_graph_scope_resolution_handoff_deterministic ? "true"
                                                                      : "false")
      << ";property_synthesis_ivar_binding_handoff_deterministic="
      << (surface.property_synthesis_ivar_binding_handoff_deterministic ? "true"
                                                                        : "false")
      << ";interface_node_count=" << surface.interface_node_count
      << ";implementation_node_count=" << surface.implementation_node_count
      << ";class_node_count=" << surface.class_node_count
      << ";metaclass_node_count=" << surface.metaclass_node_count
      << ";protocol_node_count=" << surface.protocol_node_count
      << ";category_node_count=" << surface.category_node_count
      << ";property_node_count=" << surface.property_node_count
      << ";method_node_count=" << surface.method_node_count
      << ";ivar_node_count=" << surface.ivar_node_count
      << ";owner_edge_count=" << surface.owner_edge_count
      << ";ready_for_lowering=" << (surface.ready_for_lowering ? "true"
                                                               : "false");
  return out.str();
}

std::string BuildExecutableMetadataTypedLoweringHandoffReplayKey(
    const Objc3ExecutableMetadataTypedLoweringHandoff &surface) {
  std::ostringstream out;
  out << "executable-metadata-typed-lowering-handoff:v1"
      << ";graph_contract=" << surface.executable_metadata_source_graph_contract_id
      << ";protocol_category_source_closure_contract="
      << surface.source_graph.protocol_category_source_closure_contract_id
      << ";protocol_inheritance_identity_model="
      << surface.source_graph.protocol_inheritance_identity_model
      << ";category_attachment_identity_model="
      << surface.source_graph.category_attachment_identity_model
      << ";protocol_category_conformance_identity_model="
      << surface.source_graph.protocol_category_conformance_identity_model
      << ";semantic_consistency_contract="
      << surface.executable_metadata_semantic_consistency_contract_id
      << ";semantic_validation_contract="
      << surface.executable_metadata_semantic_validation_contract_id
      << ";lowering_handoff_contract="
      << surface.executable_metadata_lowering_handoff_contract_id
      << ";schema_ordering_model=" << surface.manifest_schema_ordering_model
      << ";deterministic=" << (surface.deterministic ? "true" : "false")
      << ";ready_for_lowering="
      << (surface.ready_for_lowering ? "true" : "false");

  const Objc3ExecutableMetadataSourceGraph &graph = surface.source_graph;
  for (const auto &node : graph.interface_nodes_lexicographic) {
    out << ";interface=" << node.class_name << "|" << node.owner_identity << "|"
        << node.class_owner_identity << "|" << node.metaclass_owner_identity
        << "|" << node.super_class_owner_identity << "|"
        << (node.has_super ? "true" : "false") << "|" << node.property_count
        << "|" << node.method_count << "|" << node.class_method_count << "|"
        << node.instance_method_count << "|" << node.line << "|"
        << node.column;
  }
  for (const auto &node : graph.implementation_nodes_lexicographic) {
    out << ";implementation=" << node.class_name << "|" << node.owner_identity
        << "|" << node.interface_owner_identity << "|"
        << node.class_owner_identity << "|" << node.metaclass_owner_identity
        << "|" << (node.has_matching_interface ? "true" : "false") << "|"
        << node.property_count << "|" << node.method_count << "|"
        << node.class_method_count << "|" << node.instance_method_count << "|"
        << node.line << "|" << node.column;
  }
  for (const auto &node : graph.class_nodes_lexicographic) {
    out << ";class=" << node.class_name << "|" << node.owner_identity << "|"
        << node.interface_owner_identity << "|"
        << node.implementation_owner_identity << "|"
        << node.metaclass_owner_identity << "|" << node.super_class_owner_identity
        << "|" << (node.has_interface ? "true" : "false") << "|"
        << (node.has_implementation ? "true" : "false") << "|"
        << (node.has_super ? "true" : "false") << "|"
        << node.interface_property_count << "|"
        << node.implementation_property_count << "|"
        << node.interface_method_count << "|" << node.implementation_method_count
        << "|" << node.interface_class_method_count << "|"
        << node.implementation_class_method_count << "|"
        << node.interface_instance_method_count << "|"
        << node.implementation_instance_method_count << "|" << node.line << "|"
        << node.column;
  }
  for (const auto &node : graph.metaclass_nodes_lexicographic) {
    out << ";metaclass=" << node.class_name << "|" << node.owner_identity << "|"
        << node.class_owner_identity << "|" << node.interface_owner_identity
        << "|" << node.implementation_owner_identity << "|"
        << node.super_metaclass_owner_identity << "|"
        << (node.derived_from_interface ? "true" : "false") << "|"
        << (node.has_implementation ? "true" : "false") << "|"
        << (node.has_super ? "true" : "false") << "|"
        << node.interface_class_method_count << "|"
        << node.implementation_class_method_count << "|" << node.line << "|"
        << node.column;
  }
  for (const auto &node : graph.protocol_nodes_lexicographic) {
    out << ";protocol=" << node.protocol_name << "|" << node.owner_identity << "|";
    for (const auto &owner :
         node.inherited_protocol_owner_identities_lexicographic) {
      out << owner << ",";
    }
    out << "|" << node.property_count << "|" << node.method_count << "|"
        << (node.is_forward_declaration ? "true" : "false") << "|"
        << (node.declaration_complete ? "true" : "false") << "|"
        << (node.inherited_protocol_identity_complete ? "true" : "false")
        << "|" << node.line << "|" << node.column;
  }
  for (const auto &node : graph.category_nodes_lexicographic) {
    out << ";category=" << node.class_name << "|" << node.category_name << "|"
        << node.owner_identity << "|" << node.interface_owner_identity << "|"
        << node.implementation_owner_identity << "|" << node.class_owner_identity
        << "|";
    for (const auto &owner :
         node.adopted_protocol_owner_identities_lexicographic) {
      out << owner << ",";
    }
    out << "|" << (node.has_interface ? "true" : "false") << "|"
        << (node.has_implementation ? "true" : "false") << "|"
        << (node.declaration_complete ? "true" : "false") << "|"
        << (node.attachment_identity_complete ? "true" : "false") << "|"
        << (node.conformance_identity_complete ? "true" : "false") << "|"
        << node.interface_property_count << "|"
        << node.implementation_property_count << "|"
        << node.interface_method_count << "|" << node.implementation_method_count
        << "|" << node.interface_class_method_count << "|"
        << node.implementation_class_method_count << "|" << node.line << "|"
        << node.column;
  }
  for (const auto &node : graph.property_nodes_lexicographic) {
    out << ";property=" << node.owner_kind << "|" << node.owner_name << "|"
        << node.owner_identity << "|" << node.declaration_owner_identity << "|"
        << node.export_owner_identity << "|" << node.property_name << "|"
        << node.type_name << "|" << (node.has_getter ? "true" : "false") << "|"
        << node.getter_selector << "|" << (node.has_setter ? "true" : "false")
        << "|" << node.setter_selector << "|" << node.ivar_binding_symbol
        << "|" << node.line << "|" << node.column;
  }
  for (const auto &node : graph.method_nodes_lexicographic) {
    out << ";method=" << node.owner_kind << "|" << node.owner_name << "|"
        << node.owner_identity << "|" << node.declaration_owner_identity << "|"
        << node.export_owner_identity << "|" << node.selector << "|"
        << (node.is_class_method ? "true" : "false") << "|"
        << (node.has_body ? "true" : "false") << "|" << node.parameter_count
        << "|" << node.return_type_name << "|" << node.line << "|"
        << node.column;
  }
  for (const auto &node : graph.ivar_nodes_lexicographic) {
    out << ";ivar=" << node.owner_kind << "|" << node.owner_name << "|"
        << node.owner_identity << "|" << node.declaration_owner_identity << "|"
        << node.export_owner_identity << "|" << node.property_owner_identity
        << "|" << node.property_name << "|" << node.ivar_binding_symbol << "|"
        << node.line << "|" << node.column;
  }
  for (const auto &edge : graph.owner_edges_lexicographic) {
    out << ";edge=" << edge.edge_kind << "|" << edge.source_owner_identity << "|"
        << edge.target_owner_identity << "|" << edge.line << "|" << edge.column;
  }
  return out.str();
}
