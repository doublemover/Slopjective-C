#include "artifacts/objc3_frontend_runtime_metadata_section_artifacts.h"

#include <cstddef>
#include <sstream>
#include <string>

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

Objc3RuntimeMetadataSectionAbiFreezeSummary
BuildRuntimeMetadataSectionAbiFreezeSummary(
    const Objc3RuntimeMetadataSourceOwnershipBoundary
        &runtime_metadata_source_ownership,
    const Objc3RuntimeExportLegalityBoundary &runtime_export_legality,
    const Objc3RuntimeExportEnforcementSummary &runtime_export_enforcement) {
  Objc3RuntimeMetadataSectionAbiFreezeSummary summary;
  summary.boundary_frozen = true;
  summary.fail_closed = true;
  summary.object_file_section_inventory_frozen = true;
  summary.symbol_policy_frozen = true;
  summary.visibility_model_frozen = true;
  summary.retention_policy_frozen = true;
  summary.runtime_metadata_source_boundary_ready =
      IsReadyObjc3RuntimeMetadataSourceOwnershipBoundary(
          runtime_metadata_source_ownership);
  summary.runtime_export_legality_boundary_ready =
      IsReadyObjc3RuntimeExportLegalityBoundary(runtime_export_legality);
  summary.runtime_export_enforcement_ready =
      IsReadyObjc3RuntimeExportEnforcementSummary(runtime_export_enforcement);
  summary.ready_for_section_scaffold =
      summary.runtime_metadata_source_boundary_ready &&
      summary.runtime_export_legality_boundary_ready &&
      summary.runtime_export_enforcement_ready;
  if (!summary.ready_for_section_scaffold) {
    summary.failure_reason =
        "runtime metadata section ABI freeze prerequisites are not ready";
  }
  return summary;
}

Objc3RuntimeMetadataSectionPublicationSummary
BuildRuntimeMetadataSectionPublicationSummary(
    const Objc3RuntimeMetadataSectionAbiFreezeSummary
        &runtime_metadata_section_abi,
    const Objc3RuntimeExportLegalityBoundary &runtime_export_legality,
    const Objc3RuntimeExportEnforcementSummary &runtime_export_enforcement) {
  Objc3RuntimeMetadataSectionPublicationSummary summary;
  summary.fail_closed = true;
  if (!IsReadyObjc3RuntimeMetadataSectionAbiFreezeSummary(
          runtime_metadata_section_abi) ||
      !IsReadyObjc3RuntimeExportEnforcementSummary(
          runtime_export_enforcement)) {
    summary.failure_reason =
        "runtime metadata section publication prerequisites are not ready";
    return summary;
  }

  summary.publication_emitted = true;
  summary.uses_llvm_used = true;
  summary.image_info_emitted = true;
  summary.class_descriptor_count = runtime_export_legality.class_record_count;
  summary.protocol_descriptor_count =
      runtime_export_legality.protocol_record_count;
  summary.category_descriptor_count =
      runtime_export_legality.category_record_count;
  summary.property_descriptor_count =
      runtime_export_legality.property_record_count;
  summary.ivar_descriptor_count = runtime_export_legality.ivar_record_count;
  summary.total_descriptor_count =
      summary.class_descriptor_count + summary.protocol_descriptor_count +
      summary.category_descriptor_count + summary.property_descriptor_count +
      summary.ivar_descriptor_count;
  summary.total_retained_global_count = summary.total_descriptor_count + 6u;
  return summary;
}

Objc3RuntimeMetadataObjectInspectionHarnessSummary
BuildRuntimeMetadataObjectInspectionHarnessSummary(
    const Objc3RuntimeMetadataSectionAbiFreezeSummary
        &runtime_metadata_section_abi,
    const Objc3RuntimeMetadataSectionPublicationSummary
        &runtime_metadata_section_publication) {
  Objc3RuntimeMetadataObjectInspectionHarnessSummary summary;
  summary.fail_closed = true;
  if (!IsReadyObjc3RuntimeMetadataSectionAbiFreezeSummary(
          runtime_metadata_section_abi) ||
      !IsReadyObjc3RuntimeMetadataSectionPublicationSummary(
          runtime_metadata_section_publication)) {
    summary.failure_reason =
        "runtime metadata object inspection harness prerequisites are not ready";
    return summary;
  }

  summary.matrix_published = true;
  summary.uses_llvm_readobj = true;
  summary.uses_llvm_objdump = true;
  summary.matrix_row_count = 2u;
  return summary;
}

std::string BuildRuntimeMetadataSourceToSectionMatrixReplayKey(
    const Objc3RuntimeMetadataSourceToSectionMatrixSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";source_graph_contract=" << summary.source_graph_contract_id
      << ";section_abi_contract=" << summary.section_abi_contract_id
      << ";section_publication_contract="
      << summary.section_publication_contract_id
      << ";object_inspection_contract="
      << summary.object_inspection_contract_id
      << ";surface_path=" << summary.manifest_surface_path
      << ";row_ordering=" << summary.row_ordering_model
      << ";row_count=" << summary.matrix_row_count;
  for (const auto &row : summary.rows) {
    out << ";row=" << row.row_key << "|" << row.graph_node_kind << "|"
        << row.emission_mode << "|" << row.logical_section << "|"
        << row.payload_role << "|" << row.descriptor_symbol_family << "|"
        << row.aggregate_symbol << "|" << row.relocation_behavior << "|"
        << row.proof_fixture_path << "|" << row.proof_mode << "|"
        << row.section_inventory_command << "|" << row.symbol_inventory_command;
  }
  return out.str();
}

Objc3RuntimeMetadataSourceToSectionMatrixSummary
BuildRuntimeMetadataSourceToSectionMatrixSummary(
    const Objc3ExecutableMetadataSourceGraph &executable_metadata_source_graph,
    const Objc3RuntimeMetadataSectionAbiFreezeSummary
        &runtime_metadata_section_abi,
    const Objc3RuntimeMetadataSectionPublicationSummary
        &runtime_metadata_section_publication,
    const Objc3RuntimeMetadataObjectInspectionHarnessSummary
        &runtime_metadata_object_inspection) {
  Objc3RuntimeMetadataSourceToSectionMatrixSummary summary;
  summary.matrix_published = true;
  summary.fail_closed = true;
  summary.source_graph_ready =
      IsReadyObjc3ExecutableMetadataSourceGraph(executable_metadata_source_graph);
  summary.section_abi_ready =
      IsReadyObjc3RuntimeMetadataSectionAbiFreezeSummary(
          runtime_metadata_section_abi);
  summary.section_publication_ready =
      IsReadyObjc3RuntimeMetadataSectionPublicationSummary(
          runtime_metadata_section_publication);
  summary.object_inspection_ready =
      IsReadyObjc3RuntimeMetadataObjectInspectionHarnessSummary(
          runtime_metadata_object_inspection);
  summary.supported_node_coverage_complete = true;
  summary.explicit_non_goals_published = true;
  summary.row_ordering_frozen = true;
  summary.rows[0] = Objc3RuntimeMetadataSourceToSectionMatrixRow{
      kObjc3RuntimeMetadataSourceToSectionInterfaceRowKey,
      "interface",
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneEmissionMode,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      "no standalone emitted interface payload yet",
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneRelocationBehavior,
      kObjc3ExecutableMetadataDebugProjectionClassFixturePath,
      kObjc3RuntimeMetadataSourceToSectionSourceGraphFixtureProofMode,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue};
  summary.rows[1] = Objc3RuntimeMetadataSourceToSectionMatrixRow{
      kObjc3RuntimeMetadataSourceToSectionImplementationRowKey,
      "implementation",
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneEmissionMode,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      "no standalone emitted implementation payload yet",
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneRelocationBehavior,
      kObjc3ExecutableMetadataDebugProjectionClassFixturePath,
      kObjc3RuntimeMetadataSourceToSectionSourceGraphFixtureProofMode,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue};
  summary.rows[2] = Objc3RuntimeMetadataSourceToSectionMatrixRow{
      kObjc3RuntimeMetadataSourceToSectionClassRowKey,
      "class",
      kObjc3RuntimeMetadataSourceToSectionStandaloneEmissionMode,
      kObjc3RuntimeMetadataLogicalClassDescriptorSection,
      "standalone class descriptor payload",
      "__objc3_meta_class_####",
      kObjc3RuntimeMetadataClassDescriptorAggregateSymbol,
      kObjc3RuntimeMetadataSourceToSectionAggregateRelocationBehavior,
      kObjc3ExecutableMetadataDebugProjectionClassFixturePath,
      kObjc3RuntimeMetadataSourceToSectionFixturePlusObjectInspectionProofMode,
      kObjc3RuntimeMetadataObjectInspectionSectionCommand,
      kObjc3RuntimeMetadataObjectInspectionSymbolCommand};
  summary.rows[3] = Objc3RuntimeMetadataSourceToSectionMatrixRow{
      kObjc3RuntimeMetadataSourceToSectionMetaclassRowKey,
      "metaclass",
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneEmissionMode,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      "no standalone emitted metaclass payload yet",
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneRelocationBehavior,
      kObjc3ExecutableMetadataDebugProjectionClassFixturePath,
      kObjc3RuntimeMetadataSourceToSectionSourceGraphFixtureProofMode,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue};
  summary.rows[4] = Objc3RuntimeMetadataSourceToSectionMatrixRow{
      kObjc3RuntimeMetadataSourceToSectionProtocolRowKey,
      "protocol",
      kObjc3RuntimeMetadataSourceToSectionStandaloneEmissionMode,
      kObjc3RuntimeMetadataLogicalProtocolDescriptorSection,
      "standalone protocol descriptor payload",
      "__objc3_meta_protocol_####",
      kObjc3RuntimeMetadataProtocolDescriptorAggregateSymbol,
      kObjc3RuntimeMetadataSourceToSectionAggregateRelocationBehavior,
      kObjc3ExecutableMetadataDebugProjectionClassFixturePath,
      kObjc3RuntimeMetadataSourceToSectionFixturePlusObjectInspectionProofMode,
      kObjc3RuntimeMetadataObjectInspectionSectionCommand,
      kObjc3RuntimeMetadataObjectInspectionSymbolCommand};
  summary.rows[5] = Objc3RuntimeMetadataSourceToSectionMatrixRow{
      kObjc3RuntimeMetadataSourceToSectionCategoryRowKey,
      "category",
      kObjc3RuntimeMetadataSourceToSectionStandaloneEmissionMode,
      kObjc3RuntimeMetadataLogicalCategoryDescriptorSection,
      "standalone category descriptor payload",
      "__objc3_meta_category_####",
      kObjc3RuntimeMetadataCategoryDescriptorAggregateSymbol,
      kObjc3RuntimeMetadataSourceToSectionAggregateRelocationBehavior,
      kObjc3ExecutableMetadataDebugProjectionCategoryFixturePath,
      kObjc3RuntimeMetadataSourceToSectionFixturePlusObjectInspectionProofMode,
      kObjc3RuntimeMetadataObjectInspectionSectionCommand,
      kObjc3RuntimeMetadataObjectInspectionSymbolCommand};
  summary.rows[6] = Objc3RuntimeMetadataSourceToSectionMatrixRow{
      kObjc3RuntimeMetadataSourceToSectionPropertyRowKey,
      "property",
      kObjc3RuntimeMetadataSourceToSectionStandaloneEmissionMode,
      kObjc3RuntimeMetadataLogicalPropertyDescriptorSection,
      "standalone property descriptor payload",
      "__objc3_meta_property_####",
      kObjc3RuntimeMetadataPropertyDescriptorAggregateSymbol,
      kObjc3RuntimeMetadataSourceToSectionAggregateRelocationBehavior,
      kObjc3ExecutableMetadataDebugProjectionClassFixturePath,
      kObjc3RuntimeMetadataSourceToSectionFixturePlusObjectInspectionProofMode,
      kObjc3RuntimeMetadataObjectInspectionSectionCommand,
      kObjc3RuntimeMetadataObjectInspectionSymbolCommand};
  summary.rows[7] = Objc3RuntimeMetadataSourceToSectionMatrixRow{
      kObjc3RuntimeMetadataSourceToSectionMethodRowKey,
      "method",
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneEmissionMode,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      "no standalone emitted method payload yet",
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneRelocationBehavior,
      kObjc3ExecutableMetadataDebugProjectionClassFixturePath,
      kObjc3RuntimeMetadataSourceToSectionSourceGraphFixtureProofMode,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue};
  summary.rows[8] = Objc3RuntimeMetadataSourceToSectionMatrixRow{
      kObjc3RuntimeMetadataSourceToSectionIvarRowKey,
      "ivar",
      kObjc3RuntimeMetadataSourceToSectionStandaloneEmissionMode,
      kObjc3RuntimeMetadataLogicalIvarDescriptorSection,
      "standalone ivar descriptor payload",
      "__objc3_meta_ivar_####",
      kObjc3RuntimeMetadataIvarDescriptorAggregateSymbol,
      kObjc3RuntimeMetadataSourceToSectionAggregateRelocationBehavior,
      kObjc3ExecutableMetadataDebugProjectionClassFixturePath,
      kObjc3RuntimeMetadataSourceToSectionFixturePlusObjectInspectionProofMode,
      kObjc3RuntimeMetadataObjectInspectionSectionCommand,
      kObjc3RuntimeMetadataObjectInspectionSymbolCommand};
  summary.matrix_row_count = summary.rows.size();
  summary.replay_key =
      BuildRuntimeMetadataSourceToSectionMatrixReplayKey(summary);
  if (!IsReadyObjc3RuntimeMetadataSourceToSectionMatrixSummary(summary)) {
    summary.failure_reason =
        "runtime metadata source-to-section completeness matrix is incomplete";
  }
  return summary;
}

std::string BuildRuntimeMetadataSourceToSectionMatrixSummaryJson(
    const Objc3RuntimeMetadataSourceToSectionMatrixSummary &summary) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"source_graph_contract_id\":\""
      << EscapeJsonString(summary.source_graph_contract_id)
      << "\",\"section_abi_contract_id\":\""
      << EscapeJsonString(summary.section_abi_contract_id)
      << "\",\"section_publication_contract_id\":\""
      << EscapeJsonString(summary.section_publication_contract_id)
      << "\",\"object_inspection_contract_id\":\""
      << EscapeJsonString(summary.object_inspection_contract_id)
      << "\",\"manifest_surface_path\":\""
      << EscapeJsonString(summary.manifest_surface_path)
      << "\",\"row_ordering_model\":\""
      << EscapeJsonString(summary.row_ordering_model)
      << "\",\"ready\":"
      << (IsReadyObjc3RuntimeMetadataSourceToSectionMatrixSummary(summary)
              ? "true"
              : "false")
      << ",\"matrix_published\":"
      << (summary.matrix_published ? "true" : "false")
      << ",\"fail_closed\":" << (summary.fail_closed ? "true" : "false")
      << ",\"source_graph_ready\":"
      << (summary.source_graph_ready ? "true" : "false")
      << ",\"section_abi_ready\":"
      << (summary.section_abi_ready ? "true" : "false")
      << ",\"section_publication_ready\":"
      << (summary.section_publication_ready ? "true" : "false")
      << ",\"object_inspection_ready\":"
      << (summary.object_inspection_ready ? "true" : "false")
      << ",\"supported_node_coverage_complete\":"
      << (summary.supported_node_coverage_complete ? "true" : "false")
      << ",\"explicit_non_goals_published\":"
      << (summary.explicit_non_goals_published ? "true" : "false")
      << ",\"row_ordering_frozen\":"
      << (summary.row_ordering_frozen ? "true" : "false")
      << ",\"matrix_row_count\":" << summary.matrix_row_count
      << ",\"rows\":[";
  for (std::size_t index = 0; index < summary.rows.size(); ++index) {
    const auto &row = summary.rows[index];
    if (index != 0u) {
      out << ",";
    }
    out << "{\"row_key\":\"" << EscapeJsonString(row.row_key)
        << "\",\"graph_node_kind\":\""
        << EscapeJsonString(row.graph_node_kind)
        << "\",\"emission_mode\":\""
        << EscapeJsonString(row.emission_mode)
        << "\",\"logical_section\":\""
        << EscapeJsonString(row.logical_section)
        << "\",\"payload_role\":\""
        << EscapeJsonString(row.payload_role)
        << "\",\"descriptor_symbol_family\":\""
        << EscapeJsonString(row.descriptor_symbol_family)
        << "\",\"aggregate_symbol\":\""
        << EscapeJsonString(row.aggregate_symbol)
        << "\",\"relocation_behavior\":\""
        << EscapeJsonString(row.relocation_behavior)
        << "\",\"proof_fixture_path\":\""
        << EscapeJsonString(row.proof_fixture_path)
        << "\",\"proof_mode\":\""
        << EscapeJsonString(row.proof_mode)
        << "\",\"section_inventory_command\":\""
        << EscapeJsonString(row.section_inventory_command)
        << "\",\"symbol_inventory_command\":\""
        << EscapeJsonString(row.symbol_inventory_command) << "\"}";
  }
  out << "],\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

}  // namespace objc3::artifacts::frontend
