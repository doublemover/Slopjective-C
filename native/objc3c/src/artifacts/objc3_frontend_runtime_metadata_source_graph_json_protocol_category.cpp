#include "artifacts/objc3_frontend_runtime_metadata_source_graph_json_families.h"

#include <cstddef>
#include <ostream>

#include "io/objc3_json.h"

namespace objc3::artifacts::frontend::runtime_metadata_source_graph_json {
namespace {

using objc3::io::EscapeJsonString;

}  // namespace

void WriteSourceGraphProtocolCategoryNodeJsonFields(
    std::ostream &out, const Objc3ExecutableMetadataSourceGraph &graph) {
  out << ",\"protocol_node_entries\":[";
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
  out << "]";
}

}  // namespace objc3::artifacts::frontend::runtime_metadata_source_graph_json
