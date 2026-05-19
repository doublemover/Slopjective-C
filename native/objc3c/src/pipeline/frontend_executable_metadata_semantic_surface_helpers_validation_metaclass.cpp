#include "pipeline/frontend_executable_metadata_semantic_surface_helpers_owners.h"

#include <algorithm>
#include <string>
#include <unordered_map>

namespace objc3c::pipeline::orchestration {

void PopulateExecutableMetadataMetaclassValidation(
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
      std::string, const Objc3ExecutableMetadataMetaclassGraphNode *>
      metaclass_nodes_by_owner_identity;
  metaclass_nodes_by_owner_identity.reserve(
      graph.metaclass_nodes_lexicographic.size());
  for (const auto &metaclass_node : graph.metaclass_nodes_lexicographic) {
    metaclass_nodes_by_owner_identity.emplace(metaclass_node.owner_identity,
                                              &metaclass_node);
  }

  surface.metaclass_edges_complete = true;
  surface.metaclass_targets_resolved = true;
  surface.metaclass_lineage_aligned = true;
  for (const auto &metaclass_node : graph.metaclass_nodes_lexicographic) {
    const auto class_it =
        class_nodes_by_owner_identity.find(metaclass_node.class_owner_identity);
    if (class_it == class_nodes_by_owner_identity.end()) {
      surface.metaclass_targets_resolved = false;
      surface.metaclass_lineage_aligned = false;
      continue;
    }
    if (!has_edge("class-to-metaclass", metaclass_node.class_owner_identity,
                  metaclass_node.owner_identity)) {
      surface.metaclass_edges_complete = false;
    }
    if (class_it->second->metaclass_owner_identity !=
        metaclass_node.owner_identity) {
      surface.metaclass_lineage_aligned = false;
    }
    if (metaclass_node.has_super) {
      if (!has_edge("metaclass-to-super-metaclass", metaclass_node.owner_identity,
                    metaclass_node.super_metaclass_owner_identity)) {
        surface.metaclass_edges_complete = false;
      }
      const auto super_metaclass_it =
          metaclass_nodes_by_owner_identity.find(
              metaclass_node.super_metaclass_owner_identity);
      if (super_metaclass_it == metaclass_nodes_by_owner_identity.end()) {
        surface.metaclass_targets_resolved = false;
        surface.metaclass_lineage_aligned = false;
      } else if (class_it->second->has_super &&
                 super_metaclass_it->second->class_owner_identity !=
                     class_it->second->super_class_owner_identity) {
        surface.metaclass_lineage_aligned = false;
      }
    } else if (!metaclass_node.super_metaclass_owner_identity.empty()) {
      surface.metaclass_lineage_aligned = false;
    }
  }
}

}  // namespace objc3c::pipeline::orchestration
