#include "pipeline/frontend_executable_metadata_source_graph_helpers_owners.h"

#include <string>
#include <utility>
#include <vector>

#include "pipeline/frontend_metadata_handoff_helpers.h"
#include "support/objc3_property_storage_profile_helpers.h"

namespace objc3c::pipeline::orchestration {

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
    node.property_behavior_declared = property.property_behavior_declared;
    node.property_behavior_name = property.property_behavior_name;
    node.ownership_lifetime_profile = property.ownership_lifetime_profile;
    node.ownership_runtime_hook_profile =
        property.ownership_runtime_hook_profile;
    node.effective_getter_selector = property.effective_getter_selector;
    node.effective_setter_available = property.effective_setter_available;
    node.effective_setter_selector = property.effective_setter_selector;
    node.accessor_ownership_profile = property.accessor_ownership_profile;
    objc3c::support::ApplyPropertyOwnershipProfileRetiredRoute(node, false);
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

}  // namespace objc3c::pipeline::orchestration
