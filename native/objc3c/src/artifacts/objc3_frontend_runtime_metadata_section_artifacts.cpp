#include "artifacts/objc3_frontend_runtime_metadata_section_artifacts.h"

#include <cstddef>
#include <sstream>
#include <string>

#include "artifacts/objc3_runtime_metadata_section_publication_artifact_builders.h"
#include "ast/objc3_ast_contracts.h"
#include "io/objc3_json.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

}  // namespace

std::string BuildExecutableMetadataSourceGraphJson(
    const Objc3ExecutableMetadataSourceGraph &graph) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(graph.contract_id)
      << "\",\"owner_identity_model\":\""
      << EscapeJsonString(graph.owner_identity_model)
      << "\",\"metaclass_node_policy\":\""
      << EscapeJsonString(graph.metaclass_node_policy)
      << "\",\"edge_ordering_model\":\""
      << EscapeJsonString(graph.edge_ordering_model)
      << "\",\"class_metaclass_source_closure_contract_id\":\""
      << EscapeJsonString(graph.class_metaclass_source_closure_contract_id)
      << "\",\"class_metaclass_parent_identity_model\":\""
      << EscapeJsonString(graph.class_metaclass_parent_identity_model)
      << "\",\"class_metaclass_method_owner_identity_model\":\""
      << EscapeJsonString(graph.class_metaclass_method_owner_identity_model)
      << "\",\"class_metaclass_object_identity_model\":\""
      << EscapeJsonString(graph.class_metaclass_object_identity_model)
      << "\",\"protocol_category_source_closure_contract_id\":\""
      << EscapeJsonString(graph.protocol_category_source_closure_contract_id)
      << "\",\"protocol_inheritance_identity_model\":\""
      << EscapeJsonString(graph.protocol_inheritance_identity_model)
      << "\",\"category_attachment_identity_model\":\""
      << EscapeJsonString(graph.category_attachment_identity_model)
      << "\",\"protocol_category_conformance_identity_model\":\""
      << EscapeJsonString(graph.protocol_category_conformance_identity_model)
      << "\",\"interface_nodes\":"
      << graph.interface_nodes_lexicographic.size()
      << ",\"implementation_nodes\":"
      << graph.implementation_nodes_lexicographic.size()
      << ",\"class_nodes\":" << graph.class_nodes_lexicographic.size()
      << ",\"metaclass_nodes\":"
      << graph.metaclass_nodes_lexicographic.size()
      << ",\"protocol_nodes\":" << graph.protocol_nodes_lexicographic.size()
      << ",\"category_nodes\":" << graph.category_nodes_lexicographic.size()
      << ",\"property_nodes\":" << graph.property_nodes_lexicographic.size()
      << ",\"ivar_nodes\":" << graph.ivar_nodes_lexicographic.size()
      << ",\"method_nodes\":" << graph.method_nodes_lexicographic.size()
      << ",\"class_metaclass_declaration_closure_complete\":"
      << (graph.class_metaclass_declaration_closure_complete ? "true" : "false")
      << ",\"class_metaclass_parent_identity_closure_complete\":"
      << (graph.class_metaclass_parent_identity_closure_complete ? "true"
                                                                 : "false")
      << ",\"class_metaclass_method_owner_identity_closure_complete\":"
      << (graph.class_metaclass_method_owner_identity_closure_complete ? "true"
                                                                       : "false")
      << ",\"class_metaclass_object_identity_closure_complete\":"
      << (graph.class_metaclass_object_identity_closure_complete ? "true"
                                                                 : "false")
      << ",\"protocol_category_declaration_closure_complete\":"
      << (graph.protocol_category_declaration_closure_complete ? "true"
                                                               : "false")
      << ",\"protocol_inheritance_identity_closure_complete\":"
      << (graph.protocol_inheritance_identity_closure_complete ? "true"
                                                               : "false")
      << ",\"category_attachment_identity_closure_complete\":"
      << (graph.category_attachment_identity_closure_complete ? "true"
                                                              : "false")
      << ",\"protocol_category_conformance_identity_closure_complete\":"
      << (graph.protocol_category_conformance_identity_closure_complete ? "true"
                                                                        : "false")
      << ",\"interface_node_entries\":[";
  for (std::size_t i = 0; i < graph.interface_nodes_lexicographic.size(); ++i) {
    const auto &node = graph.interface_nodes_lexicographic[i];
    if (i != 0u) {
      out << ",";
    }
    out << "{\"class_name\":\"" << EscapeJsonString(node.class_name)
        << "\",\"owner_identity\":\"" << EscapeJsonString(node.owner_identity)
        << "\",\"class_owner_identity\":\""
        << EscapeJsonString(node.class_owner_identity)
        << "\",\"metaclass_owner_identity\":\""
        << EscapeJsonString(node.metaclass_owner_identity)
        << "\",\"super_class_owner_identity\":\""
        << EscapeJsonString(node.super_class_owner_identity)
        << "\",\"super_metaclass_owner_identity\":\""
        << EscapeJsonString(node.super_metaclass_owner_identity)
        << "\",\"instance_method_owner_identity\":\""
        << EscapeJsonString(node.instance_method_owner_identity)
        << "\",\"class_method_owner_identity\":\""
        << EscapeJsonString(node.class_method_owner_identity)
        << "\",\"has_super\":" << (node.has_super ? "true" : "false")
        << ",\"declaration_complete\":"
        << (node.declaration_complete ? "true" : "false")
        << ",\"property_count\":" << node.property_count
        << ",\"method_count\":" << node.method_count
        << ",\"class_method_count\":" << node.class_method_count
        << ",\"instance_method_count\":" << node.instance_method_count
        << ",\"line\":" << node.line << ",\"column\":" << node.column << "}";
  }
  out << "],\"implementation_node_entries\":[";
  for (std::size_t i = 0; i < graph.implementation_nodes_lexicographic.size();
       ++i) {
    const auto &node = graph.implementation_nodes_lexicographic[i];
    if (i != 0u) {
      out << ",";
    }
    out << "{\"class_name\":\"" << EscapeJsonString(node.class_name)
        << "\",\"owner_identity\":\"" << EscapeJsonString(node.owner_identity)
        << "\",\"interface_owner_identity\":\""
        << EscapeJsonString(node.interface_owner_identity)
        << "\",\"class_owner_identity\":\""
        << EscapeJsonString(node.class_owner_identity)
        << "\",\"metaclass_owner_identity\":\""
        << EscapeJsonString(node.metaclass_owner_identity)
        << "\",\"super_class_owner_identity\":\""
        << EscapeJsonString(node.super_class_owner_identity)
        << "\",\"super_metaclass_owner_identity\":\""
        << EscapeJsonString(node.super_metaclass_owner_identity)
        << "\",\"instance_method_owner_identity\":\""
        << EscapeJsonString(node.instance_method_owner_identity)
        << "\",\"class_method_owner_identity\":\""
        << EscapeJsonString(node.class_method_owner_identity)
        << "\",\"has_matching_interface\":"
        << (node.has_matching_interface ? "true" : "false")
        << ",\"has_super\":" << (node.has_super ? "true" : "false")
        << ",\"declaration_complete\":"
        << (node.declaration_complete ? "true" : "false")
        << ",\"property_count\":" << node.property_count
        << ",\"method_count\":" << node.method_count
        << ",\"class_method_count\":" << node.class_method_count
        << ",\"instance_method_count\":" << node.instance_method_count
        << ",\"line\":" << node.line << ",\"column\":" << node.column << "}";
  }
  out << "],\"class_node_entries\":[";
  for (std::size_t i = 0; i < graph.class_nodes_lexicographic.size(); ++i) {
    const auto &node = graph.class_nodes_lexicographic[i];
    if (i != 0u) {
      out << ",";
    }
    out << "{\"class_name\":\"" << EscapeJsonString(node.class_name)
        << "\",\"owner_identity\":\"" << EscapeJsonString(node.owner_identity)
        << "\",\"interface_owner_identity\":\""
        << EscapeJsonString(node.interface_owner_identity)
        << "\",\"implementation_owner_identity\":\""
        << EscapeJsonString(node.implementation_owner_identity)
        << "\",\"metaclass_owner_identity\":\""
        << EscapeJsonString(node.metaclass_owner_identity)
        << "\",\"super_class_owner_identity\":\""
        << EscapeJsonString(node.super_class_owner_identity)
        << "\",\"super_metaclass_owner_identity\":\""
        << EscapeJsonString(node.super_metaclass_owner_identity)
        << "\",\"adopted_protocol_owner_identities\":[";
    for (std::size_t adopted_index = 0;
         adopted_index <
         node.adopted_protocol_owner_identities_lexicographic.size();
         ++adopted_index) {
      if (adopted_index != 0u) {
        out << ",";
      }
      out << "\""
          << EscapeJsonString(
                 node.adopted_protocol_owner_identities_lexicographic
                     [adopted_index])
          << "\"";
    }
    out << "]"
        << ",\"instance_method_owner_identity\":\""
        << EscapeJsonString(node.instance_method_owner_identity)
        << "\",\"class_method_owner_identity\":\""
        << EscapeJsonString(node.class_method_owner_identity)
        << "\",\"has_interface\":" << (node.has_interface ? "true" : "false")
        << ",\"has_implementation\":"
        << (node.has_implementation ? "true" : "false")
        << ",\"has_super\":" << (node.has_super ? "true" : "false")
        << ",\"realization_identity_complete\":"
        << (node.realization_identity_complete ? "true" : "false")
        << ",\"interface_property_count\":" << node.interface_property_count
        << ",\"implementation_property_count\":"
        << node.implementation_property_count
        << ",\"interface_method_count\":" << node.interface_method_count
        << ",\"implementation_method_count\":"
        << node.implementation_method_count
        << ",\"interface_class_method_count\":"
        << node.interface_class_method_count
        << ",\"implementation_class_method_count\":"
        << node.implementation_class_method_count
        << ",\"interface_instance_method_count\":"
        << node.interface_instance_method_count
        << ",\"implementation_instance_method_count\":"
        << node.implementation_instance_method_count
        << ",\"line\":" << node.line << ",\"column\":" << node.column << "}";
  }
  out << "],\"metaclass_node_entries\":[";
  for (std::size_t i = 0; i < graph.metaclass_nodes_lexicographic.size(); ++i) {
    const auto &node = graph.metaclass_nodes_lexicographic[i];
    if (i != 0u) {
      out << ",";
    }
    out << "{\"class_name\":\"" << EscapeJsonString(node.class_name)
        << "\",\"owner_identity\":\"" << EscapeJsonString(node.owner_identity)
        << "\",\"class_owner_identity\":\""
        << EscapeJsonString(node.class_owner_identity)
        << "\",\"interface_owner_identity\":\""
        << EscapeJsonString(node.interface_owner_identity)
        << "\",\"implementation_owner_identity\":\""
        << EscapeJsonString(node.implementation_owner_identity)
        << "\",\"super_metaclass_owner_identity\":\""
        << EscapeJsonString(node.super_metaclass_owner_identity)
        << "\",\"derived_from_interface\":"
        << (node.derived_from_interface ? "true" : "false")
        << ",\"has_implementation\":"
        << (node.has_implementation ? "true" : "false")
        << ",\"has_super\":" << (node.has_super ? "true" : "false")
        << ",\"interface_class_method_count\":"
        << node.interface_class_method_count
        << ",\"implementation_class_method_count\":"
        << node.implementation_class_method_count
        << ",\"line\":" << node.line << ",\"column\":" << node.column << "}";
  }
  out << "],\"protocol_node_entries\":[";
  for (std::size_t i = 0; i < graph.protocol_nodes_lexicographic.size(); ++i) {
    const auto &node = graph.protocol_nodes_lexicographic[i];
    if (i != 0u) {
      out << ",";
    }
    out << "{\"protocol_name\":\"" << EscapeJsonString(node.protocol_name)
        << "\",\"owner_identity\":\"" << EscapeJsonString(node.owner_identity)
        << "\",\"inherited_protocol_owner_identities\":[";
    for (std::size_t inherited_index = 0;
         inherited_index <
         node.inherited_protocol_owner_identities_lexicographic.size();
         ++inherited_index) {
      if (inherited_index != 0u) {
        out << ",";
      }
      out << "\""
          << EscapeJsonString(
                 node.inherited_protocol_owner_identities_lexicographic
                     [inherited_index])
          << "\"";
    }
    out << "],\"property_count\":" << node.property_count
        << ",\"method_count\":" << node.method_count
        << ",\"is_forward_declaration\":"
        << (node.is_forward_declaration ? "true" : "false")
        << ",\"declaration_complete\":"
        << (node.declaration_complete ? "true" : "false")
        << ",\"inherited_protocol_identity_complete\":"
        << (node.inherited_protocol_identity_complete ? "true" : "false")
        << ",\"line\":" << node.line << ",\"column\":" << node.column << "}";
  }
  out << "],\"category_node_entries\":[";
  for (std::size_t i = 0; i < graph.category_nodes_lexicographic.size(); ++i) {
    const auto &node = graph.category_nodes_lexicographic[i];
    if (i != 0u) {
      out << ",";
    }
    out << "{\"class_name\":\"" << EscapeJsonString(node.class_name)
        << "\",\"category_name\":\"" << EscapeJsonString(node.category_name)
        << "\",\"owner_identity\":\"" << EscapeJsonString(node.owner_identity)
        << "\",\"interface_owner_identity\":\""
        << EscapeJsonString(node.interface_owner_identity)
        << "\",\"implementation_owner_identity\":\""
        << EscapeJsonString(node.implementation_owner_identity)
        << "\",\"class_owner_identity\":\""
        << EscapeJsonString(node.class_owner_identity)
        << "\",\"adopted_protocol_owner_identities\":[";
    for (std::size_t adopted_index = 0;
         adopted_index <
         node.adopted_protocol_owner_identities_lexicographic.size();
         ++adopted_index) {
      if (adopted_index != 0u) {
        out << ",";
      }
      out << "\""
          << EscapeJsonString(
                 node.adopted_protocol_owner_identities_lexicographic
                     [adopted_index])
          << "\"";
    }
    out << "],\"has_interface\":" << (node.has_interface ? "true" : "false")
        << ",\"has_implementation\":"
        << (node.has_implementation ? "true" : "false")
        << ",\"declaration_complete\":"
        << (node.declaration_complete ? "true" : "false")
        << ",\"attachment_identity_complete\":"
        << (node.attachment_identity_complete ? "true" : "false")
        << ",\"conformance_identity_complete\":"
        << (node.conformance_identity_complete ? "true" : "false")
        << ",\"interface_property_count\":" << node.interface_property_count
        << ",\"implementation_property_count\":"
        << node.implementation_property_count
        << ",\"interface_method_count\":" << node.interface_method_count
        << ",\"implementation_method_count\":"
        << node.implementation_method_count
        << ",\"interface_class_method_count\":"
        << node.interface_class_method_count
        << ",\"implementation_class_method_count\":"
        << node.implementation_class_method_count
        << ",\"line\":" << node.line << ",\"column\":" << node.column << "}";
  }
  out << "],\"property_node_entries\":[";
  for (std::size_t i = 0; i < graph.property_nodes_lexicographic.size(); ++i) {
    const auto &node = graph.property_nodes_lexicographic[i];
    if (i != 0u) {
      out << ",";
    }
    out << "{\"owner_kind\":\"" << EscapeJsonString(node.owner_kind)
        << "\",\"owner_name\":\"" << EscapeJsonString(node.owner_name)
        << "\",\"owner_identity\":\"" << EscapeJsonString(node.owner_identity)
        << "\",\"declaration_owner_identity\":\""
        << EscapeJsonString(node.declaration_owner_identity)
        << "\",\"export_owner_identity\":\""
        << EscapeJsonString(node.export_owner_identity)
        << "\",\"property_name\":\"" << EscapeJsonString(node.property_name)
        << "\",\"type_name\":\"" << EscapeJsonString(node.type_name)
        << "\",\"has_getter\":" << (node.has_getter ? "true" : "false")
        << ",\"getter_selector\":\"" << EscapeJsonString(node.getter_selector)
        << "\",\"has_setter\":" << (node.has_setter ? "true" : "false")
        << ",\"setter_selector\":\"" << EscapeJsonString(node.setter_selector)
        << "\",\"ivar_binding_symbol\":\""
        << EscapeJsonString(node.ivar_binding_symbol)
        << "\",\"executable_synthesized_binding_kind\":\""
        << EscapeJsonString(node.executable_synthesized_binding_kind)
        << "\",\"executable_synthesized_binding_symbol\":\""
        << EscapeJsonString(node.executable_synthesized_binding_symbol)
        << "\",\"property_attribute_profile\":\""
        << EscapeJsonString(node.property_attribute_profile)
        << "\",\"ownership_lifetime_profile\":\""
        << EscapeJsonString(node.ownership_lifetime_profile)
        << "\",\"ownership_runtime_hook_profile\":\""
        << EscapeJsonString(node.ownership_runtime_hook_profile)
        << "\",\"effective_getter_selector\":\""
        << EscapeJsonString(node.effective_getter_selector)
        << "\",\"effective_setter_available\":"
        << (node.effective_setter_available ? "true" : "false")
        << ",\"effective_setter_selector\":\""
        << EscapeJsonString(node.effective_setter_selector)
        << "\",\"accessor_ownership_profile\":\""
        << EscapeJsonString(node.accessor_ownership_profile)
        << "\",\"synthesizes_executable_accessors\":"
        << (node.synthesizes_executable_accessors ? "true" : "false")
        << ",\"getter_storage_runtime_helper_symbol\":\""
        << EscapeJsonString(node.getter_storage_runtime_helper_symbol)
        << "\",\"setter_storage_runtime_helper_symbol\":\""
        << EscapeJsonString(node.setter_storage_runtime_helper_symbol)
        << "\",\"executable_ivar_layout_symbol\":\""
        << EscapeJsonString(node.executable_ivar_layout_symbol)
        << "\",\"executable_ivar_layout_slot_index\":"
        << node.executable_ivar_layout_slot_index
        << ",\"executable_ivar_layout_size_bytes\":"
        << node.executable_ivar_layout_size_bytes
        << ",\"executable_ivar_layout_alignment_bytes\":"
        << node.executable_ivar_layout_alignment_bytes
        << ",\"executable_ivar_init_order_index\":"
        << node.executable_ivar_init_order_index
        << ",\"executable_ivar_destroy_order_index\":"
        << node.executable_ivar_destroy_order_index
        << ",\"line\":" << node.line << ",\"column\":" << node.column << "}";
  }
  out << "],\"method_node_entries\":[";
  for (std::size_t i = 0; i < graph.method_nodes_lexicographic.size(); ++i) {
    const auto &node = graph.method_nodes_lexicographic[i];
    if (i != 0u) {
      out << ",";
    }
    out << "{\"owner_kind\":\"" << EscapeJsonString(node.owner_kind)
        << "\",\"owner_name\":\"" << EscapeJsonString(node.owner_name)
        << "\",\"owner_identity\":\"" << EscapeJsonString(node.owner_identity)
        << "\",\"declaration_owner_identity\":\""
        << EscapeJsonString(node.declaration_owner_identity)
        << "\",\"export_owner_identity\":\""
        << EscapeJsonString(node.export_owner_identity)
        << "\",\"selector\":\"" << EscapeJsonString(node.selector)
        << "\",\"is_class_method\":"
        << (node.is_class_method ? "true" : "false")
        << ",\"has_body\":" << (node.has_body ? "true" : "false")
        << ",\"parameter_count\":" << node.parameter_count
        << ",\"return_type_name\":\""
        << EscapeJsonString(node.return_type_name)
        << "\",\"line\":" << node.line << ",\"column\":" << node.column << "}";
  }
  out << "],\"ivar_node_entries\":[";
  for (std::size_t i = 0; i < graph.ivar_nodes_lexicographic.size(); ++i) {
    const auto &node = graph.ivar_nodes_lexicographic[i];
    if (i != 0u) {
      out << ",";
    }
    out << "{\"owner_kind\":\"" << EscapeJsonString(node.owner_kind)
        << "\",\"owner_name\":\"" << EscapeJsonString(node.owner_name)
        << "\",\"owner_identity\":\"" << EscapeJsonString(node.owner_identity)
        << "\",\"declaration_owner_identity\":\""
        << EscapeJsonString(node.declaration_owner_identity)
        << "\",\"export_owner_identity\":\""
        << EscapeJsonString(node.export_owner_identity)
        << "\",\"property_owner_identity\":\""
        << EscapeJsonString(node.property_owner_identity)
        << "\",\"property_name\":\"" << EscapeJsonString(node.property_name)
        << "\",\"ivar_binding_symbol\":\""
        << EscapeJsonString(node.ivar_binding_symbol)
        << "\",\"executable_synthesized_binding_kind\":\""
        << EscapeJsonString(node.executable_synthesized_binding_kind)
        << "\",\"executable_synthesized_binding_symbol\":\""
        << EscapeJsonString(node.executable_synthesized_binding_symbol)
        << "\",\"executable_ivar_layout_symbol\":\""
        << EscapeJsonString(node.executable_ivar_layout_symbol)
        << "\",\"executable_ivar_layout_slot_index\":"
        << node.executable_ivar_layout_slot_index
        << ",\"executable_ivar_layout_size_bytes\":"
        << node.executable_ivar_layout_size_bytes
        << ",\"executable_ivar_layout_alignment_bytes\":"
        << node.executable_ivar_layout_alignment_bytes
        << ",\"executable_ivar_init_order_index\":"
        << node.executable_ivar_init_order_index
        << ",\"executable_ivar_destroy_order_index\":"
        << node.executable_ivar_destroy_order_index
        << ",\"line\":" << node.line << ",\"column\":" << node.column << "}";
  }
  out << "],\"owner_edges\":[";
  for (std::size_t i = 0; i < graph.owner_edges_lexicographic.size(); ++i) {
    const auto &edge = graph.owner_edges_lexicographic[i];
    if (i != 0u) {
      out << ",";
    }
    out << "{\"edge_kind\":\"" << EscapeJsonString(edge.edge_kind)
        << "\",\"source_owner_identity\":\""
        << EscapeJsonString(edge.source_owner_identity)
        << "\",\"target_owner_identity\":\""
        << EscapeJsonString(edge.target_owner_identity)
        << "\",\"line\":" << edge.line << ",\"column\":" << edge.column
        << "}";
  }
  out << "],\"lexicographic_owner_edge_ordering\":"
      << (graph.deterministic ? "true" : "false")
      << ",\"source_graph_complete\":"
      << (graph.source_graph_complete ? "true" : "false")
      << ",\"ready_for_semantic_closure\":"
      << (graph.ready_for_semantic_closure ? "true" : "false")
      << ",\"ready_for_lowering\":"
      << (graph.ready_for_lowering ? "true" : "false") << "}";
  return out.str();
}

std::string BuildExecutableMetadataSemanticConsistencyBoundaryJson(
    const Objc3ExecutableMetadataSemanticConsistencyBoundary &boundary) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(boundary.contract_id)
      << "\",\"executable_metadata_source_graph_contract_id\":\""
      << EscapeJsonString(boundary.executable_metadata_source_graph_contract_id)
      << "\",\"semantic_boundary_frozen\":"
      << (boundary.semantic_boundary_frozen ? "true" : "false")
      << ",\"lowering_admission_ready\":"
      << (boundary.lowering_admission_ready ? "true" : "false")
      << ",\"fail_closed\":" << (boundary.fail_closed ? "true" : "false")
      << ",\"ready\":"
      << (IsReadyObjc3ExecutableMetadataSemanticConsistencyBoundary(boundary)
              ? "true"
              : "false")
      << ",\"source_graph_ready\":"
      << (boundary.source_graph_ready ? "true" : "false")
      << ",\"protocol_category_handoff_deterministic\":"
      << (boundary.protocol_category_handoff_deterministic ? "true" : "false")
      << ",\"class_protocol_category_linking_deterministic\":"
      << (boundary.class_protocol_category_linking_deterministic ? "true"
                                                                 : "false")
      << ",\"selector_normalization_deterministic\":"
      << (boundary.selector_normalization_deterministic ? "true" : "false")
      << ",\"property_attribute_deterministic\":"
      << (boundary.property_attribute_deterministic ? "true" : "false")
      << ",\"symbol_graph_scope_resolution_deterministic\":"
      << (boundary.symbol_graph_scope_resolution_deterministic ? "true"
                                                               : "false")
      << ",\"protocol_inheritance_edges_complete\":"
      << (boundary.protocol_inheritance_edges_complete ? "true" : "false")
      << ",\"category_attachment_edges_complete\":"
      << (boundary.category_attachment_edges_complete ? "true" : "false")
      << ",\"declaration_export_owner_split_complete\":"
      << (boundary.declaration_export_owner_split_complete ? "true" : "false")
      << ",\"property_method_ivar_owner_edges_complete\":"
      << (boundary.property_method_ivar_owner_edges_complete ? "true" : "false")
      << ",\"semantic_conflict_diagnostics_enforcement_pending\":"
      << (boundary.semantic_conflict_diagnostics_enforcement_pending ? "true"
                                                                     : "false")
      << ",\"duplicate_export_owner_enforcement_pending\":"
      << (boundary.duplicate_export_owner_enforcement_pending ? "true"
                                                              : "false")
      << ",\"lowering_admission_pending\":"
      << (boundary.lowering_admission_pending ? "true" : "false")
      << ",\"protocol_node_count\":" << boundary.protocol_node_count
      << ",\"category_node_count\":" << boundary.category_node_count
      << ",\"property_node_count\":" << boundary.property_node_count
      << ",\"method_node_count\":" << boundary.method_node_count
      << ",\"ivar_node_count\":" << boundary.ivar_node_count
      << ",\"owner_edge_count\":" << boundary.owner_edge_count
      << ",\"failure_reason\":\"" << EscapeJsonString(boundary.failure_reason)
      << "\"}";
  return out.str();
}

std::string BuildExecutableMetadataSemanticValidationSurfaceJson(
    const Objc3ExecutableMetadataSemanticValidationSurface &surface) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(surface.contract_id)
      << "\",\"executable_metadata_semantic_consistency_contract_id\":\""
      << EscapeJsonString(
             surface.executable_metadata_semantic_consistency_contract_id)
      << "\",\"semantic_consistency_ready\":"
      << (surface.semantic_consistency_ready ? "true" : "false")
      << ",\"ready\":"
      << (IsReadyObjc3ExecutableMetadataSemanticValidationSurface(surface)
              ? "true"
              : "false")
      << ",\"method_lookup_override_conflict_handoff_deterministic\":"
      << (surface.method_lookup_override_conflict_handoff_deterministic
              ? "true"
              : "false")
      << ",\"class_protocol_category_linking_deterministic\":"
      << (surface.class_protocol_category_linking_deterministic ? "true"
                                                                : "false")
      << ",\"class_inheritance_edges_complete\":"
      << (surface.class_inheritance_edges_complete ? "true" : "false")
      << ",\"protocol_inheritance_edges_complete\":"
      << (surface.protocol_inheritance_edges_complete ? "true" : "false")
      << ",\"metaclass_edges_complete\":"
      << (surface.metaclass_edges_complete ? "true" : "false")
      << ",\"inheritance_chain_cycle_free\":"
      << (surface.inheritance_chain_cycle_free ? "true" : "false")
      << ",\"superclass_targets_resolved\":"
      << (surface.superclass_targets_resolved ? "true" : "false")
      << ",\"protocol_inheritance_targets_resolved\":"
      << (surface.protocol_inheritance_targets_resolved ? "true" : "false")
      << ",\"metaclass_targets_resolved\":"
      << (surface.metaclass_targets_resolved ? "true" : "false")
      << ",\"metaclass_lineage_aligned\":"
      << (surface.metaclass_lineage_aligned ? "true" : "false")
      << ",\"method_override_edges_complete\":"
      << (surface.method_override_edges_complete ? "true" : "false")
      << ",\"override_lookup_complete\":"
      << (surface.override_lookup_complete ? "true" : "false")
      << ",\"override_conflicts_absent\":"
      << (surface.override_conflicts_absent ? "true" : "false")
      << ",\"protocol_composition_valid\":"
      << (surface.protocol_composition_valid ? "true" : "false")
      << ",\"inheritance_validation_ready\":"
      << (surface.inheritance_validation_ready ? "true" : "false")
      << ",\"override_validation_ready\":"
      << (surface.override_validation_ready ? "true" : "false")
      << ",\"protocol_composition_validation_ready\":"
      << (surface.protocol_composition_validation_ready ? "true" : "false")
      << ",\"metaclass_relationship_validation_ready\":"
      << (surface.metaclass_relationship_validation_ready ? "true" : "false")
      << ",\"semantic_validation_complete\":"
      << (surface.semantic_validation_complete ? "true" : "false")
      << ",\"lowering_admission_ready\":"
      << (surface.lowering_admission_ready ? "true" : "false")
      << ",\"fail_closed\":" << (surface.fail_closed ? "true" : "false")
      << ",\"class_inheritance_edge_count\":"
      << surface.class_inheritance_edge_count
      << ",\"protocol_inheritance_edge_count\":"
      << surface.protocol_inheritance_edge_count
      << ",\"metaclass_super_edge_count\":"
      << surface.metaclass_super_edge_count
      << ",\"override_edge_count\":" << surface.override_edge_count
      << ",\"class_method_override_edge_count\":"
      << surface.class_method_override_edge_count
      << ",\"instance_method_override_edge_count\":"
      << surface.instance_method_override_edge_count
      << ",\"override_lookup_sites\":" << surface.override_lookup_sites
      << ",\"override_lookup_hits\":" << surface.override_lookup_hits
      << ",\"override_lookup_misses\":" << surface.override_lookup_misses
      << ",\"override_conflicts\":" << surface.override_conflicts
      << ",\"unresolved_base_interfaces\":"
      << surface.unresolved_base_interfaces
      << ",\"protocol_composition_sites\":"
      << surface.protocol_composition_sites
      << ",\"protocol_composition_symbols\":"
      << surface.protocol_composition_symbols
      << ",\"category_composition_sites\":"
      << surface.category_composition_sites
      << ",\"category_composition_symbols\":"
      << surface.category_composition_symbols
      << ",\"invalid_protocol_composition_sites\":"
      << surface.invalid_protocol_composition_sites
      << ",\"failure_reason\":\""
      << EscapeJsonString(surface.failure_reason) << "\"}";
  return out.str();
}

std::string BuildExecutableMetadataLoweringHandoffSurfaceJson(
    const Objc3ExecutableMetadataLoweringHandoffSurface &surface) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(surface.contract_id)
      << "\",\"executable_metadata_source_graph_contract_id\":\""
      << EscapeJsonString(surface.executable_metadata_source_graph_contract_id)
      << "\",\"executable_metadata_semantic_consistency_contract_id\":\""
      << EscapeJsonString(
             surface.executable_metadata_semantic_consistency_contract_id)
      << "\",\"executable_metadata_semantic_validation_contract_id\":\""
      << EscapeJsonString(
             surface.executable_metadata_semantic_validation_contract_id)
      << "\",\"ready\":"
      << (IsReadyObjc3ExecutableMetadataLoweringHandoffSurface(surface)
              ? "true"
              : "false")
      << ",\"source_graph_ready\":"
      << (surface.source_graph_ready ? "true" : "false")
      << ",\"semantic_consistency_ready\":"
      << (surface.semantic_consistency_ready ? "true" : "false")
      << ",\"semantic_validation_ready\":"
      << (surface.semantic_validation_ready ? "true" : "false")
      << ",\"semantic_type_metadata_handoff_deterministic\":"
      << (surface.semantic_type_metadata_handoff_deterministic ? "true"
                                                               : "false")
      << ",\"protocol_category_handoff_deterministic\":"
      << (surface.protocol_category_handoff_deterministic ? "true" : "false")
      << ",\"class_protocol_category_linking_handoff_deterministic\":"
      << (surface.class_protocol_category_linking_handoff_deterministic ? "true"
                                                                        : "false")
      << ",\"selector_normalization_handoff_deterministic\":"
      << (surface.selector_normalization_handoff_deterministic ? "true"
                                                               : "false")
      << ",\"property_attribute_handoff_deterministic\":"
      << (surface.property_attribute_handoff_deterministic ? "true" : "false")
      << ",\"symbol_graph_scope_resolution_handoff_deterministic\":"
      << (surface.symbol_graph_scope_resolution_handoff_deterministic ? "true"
                                                                      : "false")
      << ",\"property_synthesis_ivar_binding_handoff_deterministic\":"
      << (surface.property_synthesis_ivar_binding_handoff_deterministic ? "true"
                                                                        : "false")
      << ",\"lowering_schema_frozen\":"
      << (surface.lowering_schema_frozen ? "true" : "false")
      << ",\"fail_closed\":" << (surface.fail_closed ? "true" : "false")
      << ",\"ready_for_lowering\":"
      << (surface.ready_for_lowering ? "true" : "false")
      << ",\"interface_node_count\":" << surface.interface_node_count
      << ",\"implementation_node_count\":"
      << surface.implementation_node_count
      << ",\"class_node_count\":" << surface.class_node_count
      << ",\"metaclass_node_count\":" << surface.metaclass_node_count
      << ",\"protocol_node_count\":" << surface.protocol_node_count
      << ",\"category_node_count\":" << surface.category_node_count
      << ",\"property_node_count\":" << surface.property_node_count
      << ",\"method_node_count\":" << surface.method_node_count
      << ",\"ivar_node_count\":" << surface.ivar_node_count
      << ",\"owner_edge_count\":" << surface.owner_edge_count
      << ",\"replay_key\":\"" << EscapeJsonString(surface.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(surface.failure_reason) << "\"}";
  return out.str();
}

std::string BuildExecutableMetadataTypedLoweringHandoffJson(
    const Objc3ExecutableMetadataTypedLoweringHandoff &surface) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(surface.contract_id)
      << "\",\"executable_metadata_lowering_handoff_contract_id\":\""
      << EscapeJsonString(
             surface.executable_metadata_lowering_handoff_contract_id)
      << "\",\"executable_metadata_source_graph_contract_id\":\""
      << EscapeJsonString(surface.executable_metadata_source_graph_contract_id)
      << "\",\"executable_metadata_semantic_consistency_contract_id\":\""
      << EscapeJsonString(
             surface.executable_metadata_semantic_consistency_contract_id)
      << "\",\"executable_metadata_semantic_validation_contract_id\":\""
      << EscapeJsonString(
             surface.executable_metadata_semantic_validation_contract_id)
      << "\",\"manifest_schema_ordering_model\":\""
      << EscapeJsonString(surface.manifest_schema_ordering_model)
      << "\",\"ready\":"
      << (IsReadyObjc3ExecutableMetadataTypedLoweringHandoff(surface) ? "true"
                                                                     : "false")
      << ",\"source_graph_ready\":"
      << (surface.source_graph_ready ? "true" : "false")
      << ",\"semantic_consistency_ready\":"
      << (surface.semantic_consistency_ready ? "true" : "false")
      << ",\"semantic_validation_ready\":"
      << (surface.semantic_validation_ready ? "true" : "false")
      << ",\"lowering_handoff_surface_ready\":"
      << (surface.lowering_handoff_surface_ready ? "true" : "false")
      << ",\"deterministic\":" << (surface.deterministic ? "true" : "false")
      << ",\"manifest_schema_frozen\":"
      << (surface.manifest_schema_frozen ? "true" : "false")
      << ",\"fail_closed\":" << (surface.fail_closed ? "true" : "false")
      << ",\"ready_for_lowering\":"
      << (surface.ready_for_lowering ? "true" : "false")
      << ",\"source_graph\":"
      << BuildExecutableMetadataSourceGraphJson(surface.source_graph)
      << ",\"replay_key\":\"" << EscapeJsonString(surface.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(surface.failure_reason) << "\"}";
  return out.str();
}

Objc3RuntimeMetadataSectionAbiFreezeSummary
BuildRuntimeMetadataSectionAbiFreezeSummary(
    const Objc3RuntimeMetadataSourceOwnershipBoundary
        &runtime_metadata_source_ownership,
    const Objc3RuntimeExportLegalityBoundary &runtime_export_legality,
    const Objc3RuntimeExportEnforcementSummary &runtime_export_enforcement) {
  return runtime_metadata_section_publication::
      RuntimeMetadataSectionPublicationArtifactBuilder::BuildAbiFreeze(
          runtime_metadata_source_ownership,
          runtime_export_legality,
          runtime_export_enforcement);
}

Objc3RuntimeMetadataSectionPublicationSummary
BuildRuntimeMetadataSectionPublicationSummary(
    const Objc3RuntimeMetadataSectionAbiFreezeSummary
        &runtime_metadata_section_abi,
    const Objc3RuntimeExportLegalityBoundary &runtime_export_legality,
    const Objc3RuntimeExportEnforcementSummary &runtime_export_enforcement) {
  return runtime_metadata_section_publication::
      RuntimeMetadataSectionPublicationArtifactBuilder::BuildPublication(
          runtime_metadata_section_abi,
          runtime_export_legality,
          runtime_export_enforcement);
}

Objc3RuntimeMetadataObjectInspectionHarnessSummary
BuildRuntimeMetadataObjectInspectionHarnessSummary(
    const Objc3RuntimeMetadataSectionAbiFreezeSummary
        &runtime_metadata_section_abi,
    const Objc3RuntimeMetadataSectionPublicationSummary
        &runtime_metadata_section_publication) {
  return runtime_metadata_section_publication::
      RuntimeMetadataSectionPublicationArtifactBuilder::
          BuildObjectInspectionHarness(runtime_metadata_section_abi,
                                       runtime_metadata_section_publication);
}

}  // namespace objc3::artifacts::frontend
