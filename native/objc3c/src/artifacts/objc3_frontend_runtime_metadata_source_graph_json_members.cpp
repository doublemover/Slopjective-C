#include "artifacts/objc3_frontend_runtime_metadata_source_graph_json_families.h"

#include <cstddef>
#include <ostream>

#include "io/objc3_json.h"

namespace objc3::artifacts::frontend::runtime_metadata_source_graph_json {
namespace {

using objc3::io::EscapeJsonString;

}  // namespace

void WriteSourceGraphMemberNodeJsonFields(
    std::ostream &out, const Objc3ExecutableMetadataSourceGraph &graph) {
  out << ",\"property_node_entries\":[";
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
        << "\",\"property_behavior_declared\":"
        << (node.property_behavior_declared ? "true" : "false")
        << ",\"property_behavior_name\":\""
        << EscapeJsonString(node.property_behavior_name)
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
        << ",\"throws_error_out_abi_ready\":"
        << (node.throws_error_out_abi_ready ? "true" : "false")
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
  out << "]";
}

}  // namespace objc3::artifacts::frontend::runtime_metadata_source_graph_json
