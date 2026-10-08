#include "pipeline/frontend_executable_metadata_source_graph_helpers_owners.h"

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "pipeline/frontend_metadata_handoff_helpers.h"
#include "sema/objc3_typed_throws_effect_contract.h"

namespace objc3c::pipeline::orchestration {

void AddExecutableMetadataMethodNodes(
    const std::vector<Objc3MethodDecl> &methods,
    const std::string &owner_kind,
    const std::string &owner_name,
    const std::string &declaration_owner_identity,
    const std::string &instance_export_owner_identity,
    const std::string &class_export_owner_identity,
    bool direct_members_declared,
    Objc3ExecutableMetadataSourceGraph &graph,
    std::vector<ExecutableMetadataMethodEdgeRecord> &method_edge_records) {
  for (const auto &method : methods) {
    Objc3ExecutableMetadataMethodGraphNode node;
    node.owner_kind = owner_kind;
    node.owner_name = owner_name;
    node.declaration_owner_identity = declaration_owner_identity;
    node.export_owner_identity =
        method.is_class_method ? class_export_owner_identity
                               : instance_export_owner_identity;
    node.owner_identity =
        BuildMethodNodeOwnerIdentity(declaration_owner_identity, method);
    node.selector = method.selector;
    node.is_class_method = method.is_class_method;
    node.has_body = method.has_body;
    node.effective_direct_dispatch =
        method.objc_direct_declared ||
        (direct_members_declared && !method.objc_dynamic_declared);
    node.objc_final_declared = method.objc_final_declared;
    node.throws_error_out_abi_ready =
        Objc3TypedThrowsAbiLoweringReady(
            method.throws_declared, method.typed_throws_declared,
            method.typed_throws_payload.canonical_spelling);
    node.parameter_count = method.params.size();
    node.return_type_name = RuntimeMetadataTypeName(method.return_type);
    node.line = method.line;
    node.column = method.column;
    graph.method_nodes_lexicographic.push_back(node);

    AddExecutableMetadataOwnerEdge(
        graph, "method-to-declaration-owner", node.owner_identity,
        declaration_owner_identity, node.line, node.column);
    AddExecutableMetadataOwnerEdge(
        graph, "method-to-export-owner", node.owner_identity,
        node.export_owner_identity, node.line, node.column);

    method_edge_records.push_back({node.owner_identity,
                                   node.export_owner_identity,
                                   node.selector,
                                   node.is_class_method});
  }
}

void LinkExecutableMetadataMethodOverrideEdges(
    Objc3ExecutableMetadataSourceGraph &graph) {
  std::unordered_map<std::string, const Objc3ExecutableMetadataClassGraphNode *>
      class_nodes_by_owner_identity;
  class_nodes_by_owner_identity.reserve(graph.class_nodes_lexicographic.size());
  for (const auto &class_node : graph.class_nodes_lexicographic) {
    class_nodes_by_owner_identity.emplace(class_node.owner_identity,
                                          &class_node);
  }

  std::unordered_map<std::string,
                     const Objc3ExecutableMetadataInterfaceGraphNode *>
      interface_nodes_by_owner_identity;
  interface_nodes_by_owner_identity.reserve(
      graph.interface_nodes_lexicographic.size());
  for (const auto &interface_node : graph.interface_nodes_lexicographic) {
    interface_nodes_by_owner_identity.emplace(interface_node.owner_identity,
                                              &interface_node);
  }

  std::unordered_map<std::string, std::string>
      class_interface_method_owner_identity_by_key;
  class_interface_method_owner_identity_by_key.reserve(
      graph.method_nodes_lexicographic.size());
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
    class_interface_method_owner_identity_by_key.emplace(
        build_interface_method_key(method_node.declaration_owner_identity,
                                   method_node.selector,
                                   method_node.is_class_method),
        method_node.owner_identity);
  }

  const auto find_overridden_interface_method_owner_identity =
      [&](const Objc3ExecutableMetadataInterfaceGraphNode &interface_node,
          const Objc3ExecutableMetadataMethodGraphNode &method_node) {
        std::string next_super_owner_identity =
            interface_node.super_class_owner_identity;
        std::unordered_set<std::string> visited;
        while (!next_super_owner_identity.empty()) {
          if (!visited.insert(next_super_owner_identity).second) {
            return std::string{};
          }
          const auto class_it =
              class_nodes_by_owner_identity.find(next_super_owner_identity);
          if (class_it == class_nodes_by_owner_identity.end() ||
              class_it->second->interface_owner_identity.empty()) {
            return std::string{};
          }
          const std::string key = build_interface_method_key(
              class_it->second->interface_owner_identity, method_node.selector,
              method_node.is_class_method);
          const auto method_it =
              class_interface_method_owner_identity_by_key.find(key);
          if (method_it !=
              class_interface_method_owner_identity_by_key.end()) {
            return method_it->second;
          }
          next_super_owner_identity =
              class_it->second->super_class_owner_identity;
        }
        return std::string{};
      };

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
    const std::string overridden_method_owner_identity =
        find_overridden_interface_method_owner_identity(*interface_it->second,
                                                        method_node);
    if (!overridden_method_owner_identity.empty()) {
      AddExecutableMetadataOwnerEdge(
          graph, "method-to-overridden-method", method_node.owner_identity,
          overridden_method_owner_identity, method_node.line,
          method_node.column);
    }
  }
}

}  // namespace objc3c::pipeline::orchestration
