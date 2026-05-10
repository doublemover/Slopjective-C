#include "pipeline/frontend_executable_metadata_semantic_surface_helpers_owners.h"

#include <algorithm>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace objc3c::pipeline::orchestration {

void PopulateExecutableMetadataOverrideValidation(
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
      std::string, const Objc3ExecutableMetadataInterfaceGraphNode *>
      interface_nodes_by_owner_identity;
  interface_nodes_by_owner_identity.reserve(
      graph.interface_nodes_lexicographic.size());
  for (const auto &interface_node : graph.interface_nodes_lexicographic) {
    interface_nodes_by_owner_identity.emplace(interface_node.owner_identity,
                                              &interface_node);
  }

  std::unordered_map<std::string, const Objc3ExecutableMetadataMethodGraphNode *>
      class_interface_methods_by_key;
  class_interface_methods_by_key.reserve(graph.method_nodes_lexicographic.size());
  const auto build_interface_method_key =
      [](const std::string &declaration_owner_identity,
         const std::string &selector, bool is_class_method) {
        return declaration_owner_identity + "::" +
               (is_class_method ? "class" : "instance") + "::" + selector;
      };
  for (const auto &method_node : graph.method_nodes_lexicographic) {
    if (method_node.owner_kind != "class-interface") {
      continue;
    }
    class_interface_methods_by_key.emplace(
        build_interface_method_key(method_node.declaration_owner_identity,
                                   method_node.selector,
                                   method_node.is_class_method),
        &method_node);
  }

  surface.method_override_edges_complete = true;
  for (const auto &method_node : graph.method_nodes_lexicographic) {
    if (method_node.owner_kind != "class-interface") {
      continue;
    }
    const auto interface_it = interface_nodes_by_owner_identity.find(
        method_node.declaration_owner_identity);
    if (interface_it == interface_nodes_by_owner_identity.end() ||
        !interface_it->second->has_super) {
      continue;
    }

    std::string next_super_owner_identity =
        interface_it->second->super_class_owner_identity;
    const Objc3ExecutableMetadataMethodGraphNode *overridden_method = nullptr;
    std::unordered_set<std::string> visited;
    while (!next_super_owner_identity.empty()) {
      if (!visited.insert(next_super_owner_identity).second) {
        break;
      }
      const auto class_it =
          class_nodes_by_owner_identity.find(next_super_owner_identity);
      if (class_it == class_nodes_by_owner_identity.end() ||
          class_it->second->interface_owner_identity.empty()) {
        break;
      }
      const auto base_method_it = class_interface_methods_by_key.find(
          build_interface_method_key(class_it->second->interface_owner_identity,
                                     method_node.selector,
                                     method_node.is_class_method));
      if (base_method_it != class_interface_methods_by_key.end()) {
        overridden_method = base_method_it->second;
        break;
      }
      next_super_owner_identity = class_it->second->super_class_owner_identity;
    }
    if (overridden_method == nullptr) {
      continue;
    }
    if (!has_edge("method-to-overridden-method", method_node.owner_identity,
                  overridden_method->owner_identity)) {
      surface.method_override_edges_complete = false;
    } else if (method_node.is_class_method) {
      ++surface.class_method_override_edge_count;
    } else {
      ++surface.instance_method_override_edge_count;
    }
  }
}

}  // namespace objc3c::pipeline::orchestration
