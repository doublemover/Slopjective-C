#include "pipeline/frontend_executable_metadata_source_graph_helpers_owners.h"

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "pipeline/frontend_metadata_handoff_helpers.h"
#include "support/objc3_property_storage_profile_helpers.h"

namespace objc3c::pipeline::orchestration {

void BuildExecutableMetadataPropertySynthesisIndex(
    const Objc3Program &program,
    ExecutableMetadataPropertySynthesisIndex &property_synthesis_index) {
  property_synthesis_index.class_implementation_names.reserve(
      program.implementations.size());
  for (const auto &implementation_decl : program.implementations) {
    if (!implementation_decl.has_category) {
      property_synthesis_index.class_implementation_names.insert(
          implementation_decl.name);
    }
    const std::string owner_name =
        implementation_decl.has_category
            ? BuildCategoryOwnerName(implementation_decl.name,
                                     implementation_decl.category_name)
            : implementation_decl.name;
    for (const auto &property : implementation_decl.properties) {
      property_synthesis_index.implementation_property_keys.insert(
          BuildExecutablePropertyOwnerKey(owner_name, property.name));
    }
  }
}

void AddExecutableMetadataOwnerEdge(
    Objc3ExecutableMetadataSourceGraph &graph,
    const std::string &edge_kind,
    const std::string &source_owner_identity,
    const std::string &target_owner_identity,
    unsigned line,
    unsigned column) {
  if (source_owner_identity.empty() || target_owner_identity.empty()) {
    return;
  }
  Objc3ExecutableMetadataGraphEdge edge;
  edge.edge_kind = edge_kind;
  edge.source_owner_identity = source_owner_identity;
  edge.target_owner_identity = target_owner_identity;
  edge.line = line;
  edge.column = column;
  graph.owner_edges_lexicographic.push_back(std::move(edge));
}

void AddExecutableMetadataPropertyNodes(
    const std::vector<Objc3PropertyDecl> &properties,
    const std::string &owner_kind,
    const std::string &owner_name,
    const std::string &declaration_owner_identity,
    const std::string &export_owner_identity,
    const ExecutableMetadataPropertySynthesisIndex &property_synthesis_index,
    Objc3ExecutableMetadataSourceGraph &graph) {
  for (const auto &property : properties) {
    Objc3ExecutableMetadataPropertyGraphNode node;
    node.owner_kind = owner_kind;
    node.owner_name = owner_name;
    node.declaration_owner_identity = declaration_owner_identity;
    node.export_owner_identity = export_owner_identity;
    node.owner_identity =
        BuildPropertyNodeOwnerIdentity(declaration_owner_identity, property);
    node.property_name = property.name;
    node.type_name = RuntimeMetadataTypeName(property.type);
    node.has_getter = property.has_getter;
    node.getter_selector = property.getter_selector;
    node.has_setter = property.has_setter;
    node.setter_selector = property.setter_selector;
    node.ivar_binding_symbol = property.ivar_binding_symbol;
    node.executable_synthesized_binding_kind =
        property.executable_synthesized_binding_kind;
    node.executable_synthesized_binding_symbol =
        property.executable_synthesized_binding_symbol;
    node.property_attribute_profile = property.property_attribute_profile;
    node.ownership_lifetime_profile = property.ownership_lifetime_profile;
    node.ownership_runtime_hook_profile =
        property.ownership_runtime_hook_profile;
    node.effective_getter_selector = property.effective_getter_selector;
    node.effective_setter_available = property.effective_setter_available;
    node.effective_setter_selector = property.effective_setter_selector;
    node.accessor_ownership_profile = property.accessor_ownership_profile;
    objc3c::support::ApplyPropertyOwnershipProfileFallback(node, false);
    objc3c::support::RebuildPropertyAccessorOwnershipProfileIfNeeded(node);
    node.synthesizes_executable_accessors =
        ShouldSynthesizeExecutablePropertyAccessors(
            owner_kind, owner_name, property.name,
            property_synthesis_index.class_implementation_names,
            property_synthesis_index.implementation_property_keys);
    node.getter_storage_runtime_helper_symbol =
        BuildGetterStorageRuntimeHelperSymbol(
            node.synthesizes_executable_accessors,
            node.ownership_runtime_hook_profile);
    node.setter_storage_runtime_helper_symbol =
        BuildSetterStorageRuntimeHelperSymbol(
            node.synthesizes_executable_accessors,
            node.effective_setter_available, node.ownership_lifetime_profile,
            node.ownership_runtime_hook_profile,
            node.accessor_ownership_profile);
    node.executable_ivar_layout_symbol = property.executable_ivar_layout_symbol;
    node.executable_ivar_layout_slot_index =
        property.executable_ivar_layout_slot_index;
    node.executable_ivar_layout_size_bytes =
        property.executable_ivar_layout_size_bytes;
    node.executable_ivar_layout_alignment_bytes =
        property.executable_ivar_layout_alignment_bytes;
    node.executable_ivar_layout_offset_bytes =
        property.executable_ivar_layout_offset_bytes;
    node.executable_ivar_layout_padding_bytes =
        property.executable_ivar_layout_padding_bytes;
    node.executable_ivar_layout_inherited_slot_count =
        property.executable_ivar_layout_inherited_slot_count;
    node.executable_ivar_layout_inherited_size_bytes =
        property.executable_ivar_layout_inherited_size_bytes;
    node.executable_ivar_layout_owner_size_bytes =
        property.executable_ivar_layout_owner_size_bytes;
    node.executable_ivar_init_order_index =
        property.executable_ivar_init_order_index;
    node.executable_ivar_destroy_order_index =
        property.executable_ivar_destroy_order_index;
    node.executable_ivar_layout_valid = property.executable_ivar_layout_valid;
    node.executable_ivar_layout_replay_key =
        property.executable_ivar_layout_replay_key;
    node.line = property.line;
    node.column = property.column;
    graph.property_nodes_lexicographic.push_back(node);

    AddExecutableMetadataOwnerEdge(
        graph, "property-to-declaration-owner", node.owner_identity,
        declaration_owner_identity, node.line, node.column);
    AddExecutableMetadataOwnerEdge(
        graph, "property-to-export-owner", node.owner_identity,
        export_owner_identity, node.line, node.column);

    if (!property.ivar_binding_symbol.empty()) {
      Objc3ExecutableMetadataIvarGraphNode ivar_node;
      ivar_node.owner_kind = owner_kind;
      ivar_node.owner_name = owner_name;
      ivar_node.declaration_owner_identity = declaration_owner_identity;
      ivar_node.export_owner_identity = export_owner_identity;
      ivar_node.property_owner_identity = node.owner_identity;
      ivar_node.owner_identity =
          BuildIvarNodeOwnerIdentity(declaration_owner_identity, property);
      ivar_node.property_name = property.name;
      ivar_node.ivar_binding_symbol = property.ivar_binding_symbol;
      ivar_node.executable_synthesized_binding_kind =
          property.executable_synthesized_binding_kind;
      ivar_node.executable_synthesized_binding_symbol =
          property.executable_synthesized_binding_symbol;
      ivar_node.executable_ivar_layout_symbol =
          property.executable_ivar_layout_symbol;
      ivar_node.executable_ivar_layout_slot_index =
          property.executable_ivar_layout_slot_index;
      ivar_node.executable_ivar_layout_size_bytes =
          property.executable_ivar_layout_size_bytes;
      ivar_node.executable_ivar_layout_alignment_bytes =
          property.executable_ivar_layout_alignment_bytes;
      ivar_node.executable_ivar_layout_offset_bytes =
          property.executable_ivar_layout_offset_bytes;
      ivar_node.executable_ivar_layout_padding_bytes =
          property.executable_ivar_layout_padding_bytes;
      ivar_node.executable_ivar_layout_inherited_slot_count =
          property.executable_ivar_layout_inherited_slot_count;
      ivar_node.executable_ivar_layout_inherited_size_bytes =
          property.executable_ivar_layout_inherited_size_bytes;
      ivar_node.executable_ivar_layout_owner_size_bytes =
          property.executable_ivar_layout_owner_size_bytes;
      ivar_node.executable_ivar_init_order_index =
          property.executable_ivar_init_order_index;
      ivar_node.executable_ivar_destroy_order_index =
          property.executable_ivar_destroy_order_index;
      ivar_node.executable_ivar_layout_valid =
          property.executable_ivar_layout_valid;
      ivar_node.executable_ivar_layout_replay_key =
          property.executable_ivar_layout_replay_key;
      ivar_node.line = property.line;
      ivar_node.column = property.column;
      graph.ivar_nodes_lexicographic.push_back(ivar_node);

      AddExecutableMetadataOwnerEdge(
          graph, "ivar-to-declaration-owner", ivar_node.owner_identity,
          declaration_owner_identity, ivar_node.line, ivar_node.column);
      AddExecutableMetadataOwnerEdge(
          graph, "ivar-to-export-owner", ivar_node.owner_identity,
          export_owner_identity, ivar_node.line, ivar_node.column);
      AddExecutableMetadataOwnerEdge(
          graph, "ivar-to-property", ivar_node.owner_identity,
          node.owner_identity, ivar_node.line, ivar_node.column);
      AddExecutableMetadataOwnerEdge(
          graph, "property-to-ivar", node.owner_identity,
          ivar_node.owner_identity, node.line, node.column);
    }
  }
}

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

void LinkExecutableMetadataPropertyAccessorEdges(
    Objc3ExecutableMetadataSourceGraph &graph,
    const std::vector<ExecutableMetadataMethodEdgeRecord> &method_edge_records) {
  for (const auto &property_node : graph.property_nodes_lexicographic) {
    if (property_node.has_getter) {
      for (const auto &method_record : method_edge_records) {
        if (method_record.export_owner_identity ==
                property_node.export_owner_identity &&
            method_record.selector == property_node.getter_selector &&
            !method_record.is_class_method) {
          AddExecutableMetadataOwnerEdge(
              graph, "property-to-getter-method", property_node.owner_identity,
              method_record.owner_identity, property_node.line,
              property_node.column);
        }
      }
    }

    if (property_node.has_setter) {
      for (const auto &method_record : method_edge_records) {
        if (method_record.export_owner_identity ==
                property_node.export_owner_identity &&
            method_record.selector == property_node.setter_selector &&
            !method_record.is_class_method) {
          AddExecutableMetadataOwnerEdge(
              graph, "property-to-setter-method", property_node.owner_identity,
              method_record.owner_identity, property_node.line,
              property_node.column);
        }
      }
    }
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
