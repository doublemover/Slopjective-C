#include "pipeline/frontend_executable_metadata_semantic_surface_helpers_owners.h"

#include <algorithm>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace objc3c::pipeline::orchestration {

void PopulateExecutableMetadataInheritanceValidation(
    Objc3ExecutableMetadataSemanticValidationSurface &surface,
    const Objc3ExecutableMetadataSourceGraph &graph) {
  const auto has_edge = [&](const std::string &edge_kind,
                            const std::string &source_owner_identity,
                            const std::string &target_owner_identity) {
    return std::any_of(
        graph.owner_edges_lexicographic.begin(),
        graph.owner_edges_lexicographic.end(),
        [&](const Objc3ExecutableMetadataGraphEdge &edge) {
          return edge.edge_kind == edge_kind &&
                 edge.source_owner_identity == source_owner_identity &&
                 edge.target_owner_identity == target_owner_identity;
        });
  };

  std::unordered_map<std::string, const Objc3ExecutableMetadataClassGraphNode *>
      class_nodes_by_owner_identity;
  class_nodes_by_owner_identity.reserve(graph.class_nodes_lexicographic.size());
  for (const auto &class_node : graph.class_nodes_lexicographic) {
    class_nodes_by_owner_identity.emplace(class_node.owner_identity, &class_node);
  }

  std::unordered_map<
      std::string, const Objc3ExecutableMetadataProtocolGraphNode *>
      protocol_nodes_by_owner_identity;
  protocol_nodes_by_owner_identity.reserve(
      graph.protocol_nodes_lexicographic.size());
  for (const auto &protocol_node : graph.protocol_nodes_lexicographic) {
    protocol_nodes_by_owner_identity.emplace(protocol_node.owner_identity,
                                             &protocol_node);
  }

  surface.class_inheritance_edges_complete = true;
  surface.inheritance_chain_cycle_free = true;
  surface.superclass_targets_resolved = true;
  for (const auto &class_node : graph.class_nodes_lexicographic) {
    if (!class_node.has_super) {
      continue;
    }
    if (!has_edge("class-to-superclass", class_node.owner_identity,
                  class_node.super_class_owner_identity)) {
      surface.class_inheritance_edges_complete = false;
    }
    std::string next_super_owner_identity = class_node.super_class_owner_identity;
    std::unordered_set<std::string> visited;
    while (!next_super_owner_identity.empty()) {
      if (!visited.insert(next_super_owner_identity).second) {
        surface.inheritance_chain_cycle_free = false;
        break;
      }
      const auto class_it =
          class_nodes_by_owner_identity.find(next_super_owner_identity);
      if (class_it == class_nodes_by_owner_identity.end()) {
        surface.superclass_targets_resolved = false;
        break;
      }
      next_super_owner_identity = class_it->second->super_class_owner_identity;
    }
  }

  surface.protocol_inheritance_edges_complete = true;
  surface.protocol_inheritance_targets_resolved = true;
  for (const auto &protocol_node : graph.protocol_nodes_lexicographic) {
    for (const auto &target_owner_identity :
         protocol_node.inherited_protocol_owner_identities_lexicographic) {
      if (!has_edge("protocol-to-inherited-protocol",
                    protocol_node.owner_identity, target_owner_identity)) {
        surface.protocol_inheritance_edges_complete = false;
      }
      if (protocol_nodes_by_owner_identity.find(target_owner_identity) ==
          protocol_nodes_by_owner_identity.end()) {
        surface.protocol_inheritance_targets_resolved = false;
      }
    }
  }
}

}  // namespace objc3c::pipeline::orchestration
