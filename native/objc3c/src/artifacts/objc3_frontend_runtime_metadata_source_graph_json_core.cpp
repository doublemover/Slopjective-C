#include "artifacts/objc3_frontend_runtime_metadata_source_graph_json_families.h"

#include <cstddef>
#include <ostream>

#include "io/objc3_json.h"

namespace objc3::artifacts::frontend::runtime_metadata_source_graph_json {
namespace {

using objc3::io::EscapeJsonString;

}  // namespace

void WriteSourceGraphIdentityJsonFields(
    std::ostream &out, const Objc3ExecutableMetadataSourceGraph &graph) {
  out << "\"contract_id\":\"" << EscapeJsonString(graph.contract_id)
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
      << "\"";
}

void WriteSourceGraphNodeCountJsonFields(
    std::ostream &out, const Objc3ExecutableMetadataSourceGraph &graph) {
  out << ",\"interface_nodes\":"
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
      << ",\"method_nodes\":" << graph.method_nodes_lexicographic.size();
}

void WriteSourceGraphClosureJsonFields(
    std::ostream &out, const Objc3ExecutableMetadataSourceGraph &graph) {
  out << ",\"class_metaclass_declaration_closure_complete\":"
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
                                                                        : "false");
}

void WriteSourceGraphOwnerEdgeReadinessJsonFields(
    std::ostream &out, const Objc3ExecutableMetadataSourceGraph &graph) {
  out << ",\"owner_edges\":[";
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
      << (graph.ready_for_lowering ? "true" : "false");
}

}  // namespace objc3::artifacts::frontend::runtime_metadata_source_graph_json
