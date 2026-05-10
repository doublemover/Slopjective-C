#include "pipeline/frontend_executable_metadata_handoff_replay_owners.h"

namespace objc3_frontend_executable_metadata_handoff_replay {

void AppendExecutableMetadataTypedProtocolCategoryReplayRecords(
    std::ostringstream &out,
    const Objc3ExecutableMetadataSourceGraph &graph) {
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
}

}  // namespace objc3_frontend_executable_metadata_handoff_replay
