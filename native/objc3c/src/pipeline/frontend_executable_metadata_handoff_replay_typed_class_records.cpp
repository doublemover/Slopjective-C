#include "pipeline/frontend_executable_metadata_handoff_replay_owners.h"

namespace objc3_frontend_executable_metadata_handoff_replay {

void AppendExecutableMetadataTypedClassReplayRecords(
    std::ostringstream &out,
    const Objc3ExecutableMetadataSourceGraph &graph) {
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
}

}  // namespace objc3_frontend_executable_metadata_handoff_replay
