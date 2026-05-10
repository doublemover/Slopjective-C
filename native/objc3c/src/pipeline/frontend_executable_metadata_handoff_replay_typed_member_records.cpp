#include "pipeline/frontend_executable_metadata_handoff_replay_owners.h"

namespace objc3_frontend_executable_metadata_handoff_replay {

void AppendExecutableMetadataTypedMemberReplayRecords(
    std::ostringstream &out,
    const Objc3ExecutableMetadataSourceGraph &graph) {
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
}

}  // namespace objc3_frontend_executable_metadata_handoff_replay
