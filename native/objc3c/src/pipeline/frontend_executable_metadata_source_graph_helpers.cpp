#include "pipeline/frontend_executable_metadata_source_graph_helpers.h"

#include <algorithm>
#include <cstddef>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "pipeline/frontend_metadata_handoff_helpers.h"
#include "pipeline/frontend_metadata_handoff_ordering.h"
#include "pipeline/frontend_executable_metadata_source_graph_aggregation.h"
#include "pipeline/frontend_executable_metadata_source_graph_readiness.h"
#include "pipeline/objc3_frontend_types.h"
#include "support/objc3_property_storage_profile_helpers.h"

namespace objc3c::pipeline::orchestration {

Objc3ExecutableMetadataSourceGraph BuildExecutableMetadataSourceGraph(
    const Objc3Program &program,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records) {
  Objc3ExecutableMetadataSourceGraph graph;

  std::unordered_map<std::string, ExecutableMetadataAggregatedClassSurface>
      aggregated_classes;
  aggregated_classes.reserve(program.interfaces.size() + program.implementations.size());
  std::unordered_map<std::string, ExecutableMetadataAggregatedCategorySurface>
      aggregated_categories;
  aggregated_categories.reserve(program.interfaces.size() + program.implementations.size());

  auto &owner_edges = graph.owner_edges_lexicographic;
  const auto add_owner_edge = [&owner_edges](const std::string &edge_kind,
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
    owner_edges.push_back(std::move(edge));
  };

  std::vector<ExecutableMetadataMethodEdgeRecord> method_edge_records;
  std::unordered_set<std::string> class_implementation_names;
  class_implementation_names.reserve(program.implementations.size());
  std::unordered_set<std::string> implementation_property_keys;
  for (const auto &implementation_decl : program.implementations) {
    if (!implementation_decl.has_category) {
      class_implementation_names.insert(implementation_decl.name);
    }
    const std::string owner_name =
        implementation_decl.has_category
            ? BuildCategoryOwnerName(implementation_decl.name,
                                     implementation_decl.category_name)
            : implementation_decl.name;
    for (const auto &property : implementation_decl.properties) {
      implementation_property_keys.insert(
          BuildExecutablePropertyOwnerKey(owner_name, property.name));
    }
  }

  const auto apply_arc_property_interaction_metadata =
      [](Objc3ExecutableMetadataPropertyGraphNode &node) {
        objc3c::support::ApplyPropertyOwnershipProfileFallback(node, false);
        objc3c::support::RebuildPropertyAccessorOwnershipProfileIfNeeded(node);
      };

  const auto add_property_nodes =
      [&graph, &add_owner_edge, &apply_arc_property_interaction_metadata,
       &class_implementation_names, &implementation_property_keys](
          const auto &properties, const std::string &owner_kind,
          const std::string &owner_name,
          const std::string &declaration_owner_identity,
          const std::string &export_owner_identity) {
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
          node.ownership_lifetime_profile =
              property.ownership_lifetime_profile;
          node.ownership_runtime_hook_profile =
              property.ownership_runtime_hook_profile;
          node.effective_getter_selector =
              property.effective_getter_selector;
          node.effective_setter_available =
              property.effective_setter_available;
          node.effective_setter_selector =
              property.effective_setter_selector;
          node.accessor_ownership_profile =
              property.accessor_ownership_profile;
          apply_arc_property_interaction_metadata(node);
          node.synthesizes_executable_accessors =
              ShouldSynthesizeExecutablePropertyAccessors(
                  owner_kind, owner_name, property.name,
                  class_implementation_names, implementation_property_keys);
          node.getter_storage_runtime_helper_symbol =
              BuildGetterStorageRuntimeHelperSymbol(
                  node.synthesizes_executable_accessors,
                  node.ownership_runtime_hook_profile);
          node.setter_storage_runtime_helper_symbol =
              BuildSetterStorageRuntimeHelperSymbol(
                  node.synthesizes_executable_accessors,
                  node.effective_setter_available,
                  node.ownership_lifetime_profile,
                  node.ownership_runtime_hook_profile,
                  node.accessor_ownership_profile);
          node.executable_ivar_layout_symbol =
              property.executable_ivar_layout_symbol;
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
          node.executable_ivar_layout_valid =
              property.executable_ivar_layout_valid;
          node.executable_ivar_layout_replay_key =
              property.executable_ivar_layout_replay_key;
          node.line = property.line;
          node.column = property.column;
          graph.property_nodes_lexicographic.push_back(node);

          add_owner_edge("property-to-declaration-owner", node.owner_identity,
                         declaration_owner_identity, node.line, node.column);
          add_owner_edge("property-to-export-owner", node.owner_identity,
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

            add_owner_edge("ivar-to-declaration-owner", ivar_node.owner_identity,
                           declaration_owner_identity, ivar_node.line,
                           ivar_node.column);
            add_owner_edge("ivar-to-export-owner", ivar_node.owner_identity,
                           export_owner_identity, ivar_node.line, ivar_node.column);
            add_owner_edge("ivar-to-property", ivar_node.owner_identity,
                           node.owner_identity, ivar_node.line, ivar_node.column);
            add_owner_edge("property-to-ivar", node.owner_identity,
                           ivar_node.owner_identity, node.line, node.column);
          }
        }
      };

  const auto add_method_nodes =
      [&graph, &add_owner_edge, &method_edge_records](const auto &methods,
                                                      const std::string &owner_kind,
                                                      const std::string &owner_name,
                                                      const std::string &declaration_owner_identity,
                                                      const std::string &instance_export_owner_identity,
                                                      const std::string &class_export_owner_identity,
                                                      bool direct_members_declared) {
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

          add_owner_edge("method-to-declaration-owner", node.owner_identity,
                         declaration_owner_identity, node.line, node.column);
          add_owner_edge("method-to-export-owner", node.owner_identity,
                         node.export_owner_identity, node.line, node.column);

          method_edge_records.push_back(
              {node.owner_identity, node.export_owner_identity, node.selector,
               node.is_class_method});
        }
      };

  for (const auto &interface_decl : program.interfaces) {
    if (interface_decl.has_category) {
      const std::string category_owner_name =
          BuildCategoryOwnerName(interface_decl.name, interface_decl.category_name);
      ExecutableMetadataAggregatedCategorySurface &aggregate =
          aggregated_categories[category_owner_name];
      if (!aggregate.has_interface && !aggregate.has_implementation) {
        aggregate.line = interface_decl.line;
        aggregate.column = interface_decl.column;
      }
      aggregate.has_interface = true;
      aggregate.interface_owner_identity = interface_decl.semantic_link_symbol;
      aggregate.class_owner_identity =
          BuildRuntimeClassOwnerIdentity(interface_decl.name);
      aggregate.adopted_protocol_owner_identities_lexicographic =
          interface_decl.adopted_protocols_lexicographic;
      aggregate.interface_property_count = interface_decl.properties.size();
      aggregate.interface_method_count = interface_decl.methods.size();
      aggregate.interface_class_method_count =
          CountClassMethods(interface_decl.methods);

      add_property_nodes(interface_decl.properties, "category-interface",
                         category_owner_name, interface_decl.semantic_link_symbol,
                         BuildRuntimeCategoryOwnerIdentity(
                             interface_decl.name, interface_decl.category_name));
      add_method_nodes(interface_decl.methods, "category-interface",
                       category_owner_name, interface_decl.semantic_link_symbol,
                       BuildRuntimeCategoryOwnerIdentity(
                           interface_decl.name, interface_decl.category_name),
                       BuildRuntimeCategoryOwnerIdentity(
                           interface_decl.name, interface_decl.category_name),
                       false);
      continue;
    }

    Objc3ExecutableMetadataInterfaceGraphNode node;
    node.class_name = interface_decl.name;
    node.owner_identity = interface_decl.semantic_link_symbol;
    node.class_owner_identity = BuildRuntimeClassOwnerIdentity(interface_decl.name);
    node.metaclass_owner_identity =
        BuildRuntimeMetaclassOwnerIdentity(interface_decl.name);
    node.super_class_owner_identity =
        interface_decl.super_name.empty()
            ? std::string{}
            : BuildRuntimeClassOwnerIdentity(interface_decl.super_name);
    node.super_metaclass_owner_identity =
        interface_decl.super_name.empty()
            ? std::string{}
            : BuildRuntimeMetaclassOwnerIdentity(interface_decl.super_name);
    node.instance_method_owner_identity = node.class_owner_identity;
    node.class_method_owner_identity = node.metaclass_owner_identity;
    node.has_super = !interface_decl.super_name.empty();
    node.declaration_complete =
        !node.owner_identity.empty() && !node.class_owner_identity.empty() &&
        !node.metaclass_owner_identity.empty() &&
        !node.instance_method_owner_identity.empty() &&
        !node.class_method_owner_identity.empty() &&
        (!node.has_super ||
         (!node.super_class_owner_identity.empty() &&
          !node.super_metaclass_owner_identity.empty()));
    node.property_count = interface_decl.properties.size();
    node.method_count = interface_decl.methods.size();
    node.class_method_count = CountClassMethods(interface_decl.methods);
    node.instance_method_count = node.method_count - node.class_method_count;
    node.line = interface_decl.line;
    node.column = interface_decl.column;
    graph.interface_nodes_lexicographic.push_back(node);

    ExecutableMetadataAggregatedClassSurface &aggregate =
        aggregated_classes[interface_decl.name];
    if (!aggregate.has_interface && !aggregate.has_implementation) {
      aggregate.line = interface_decl.line;
      aggregate.column = interface_decl.column;
    }
    aggregate.has_interface = true;
    aggregate.interface_owner_identity = interface_decl.semantic_link_symbol;
    aggregate.super_class_owner_identity = node.super_class_owner_identity;
    aggregate.adopted_protocol_owner_identities_lexicographic =
        interface_decl.adopted_protocols_lexicographic;
    aggregate.objc_direct_members_declared =
        interface_decl.objc_direct_members_declared;
    aggregate.objc_final_declared = interface_decl.objc_final_declared;
    aggregate.objc_sealed_declared = interface_decl.objc_sealed_declared;
    aggregate.interface_property_count = node.property_count;
    aggregate.interface_method_count = node.method_count;
    aggregate.interface_class_method_count = node.class_method_count;

    add_owner_edge("interface-to-class", node.owner_identity,
                   node.class_owner_identity, node.line, node.column);
    add_owner_edge("interface-to-metaclass", node.owner_identity,
                   node.metaclass_owner_identity, node.line, node.column);
    add_owner_edge("interface-to-instance-method-owner", node.owner_identity,
                   node.instance_method_owner_identity, node.line, node.column);
    add_owner_edge("interface-to-class-method-owner", node.owner_identity,
                   node.class_method_owner_identity, node.line, node.column);
    add_owner_edge("class-to-superclass", node.class_owner_identity,
                   node.super_class_owner_identity, node.line, node.column);
    add_owner_edge("interface-to-superclass", node.owner_identity,
                   node.super_class_owner_identity, node.line, node.column);
    add_owner_edge("interface-to-super-metaclass", node.owner_identity,
                   node.super_metaclass_owner_identity, node.line, node.column);

    add_property_nodes(interface_decl.properties, "class-interface",
                       interface_decl.name, interface_decl.semantic_link_symbol,
                       node.class_owner_identity);
    add_method_nodes(interface_decl.methods, "class-interface", interface_decl.name,
                     interface_decl.semantic_link_symbol, node.class_owner_identity,
                     node.metaclass_owner_identity,
                     interface_decl.objc_direct_members_declared);
  }

  for (const auto &implementation_decl : program.implementations) {
    if (implementation_decl.has_category) {
      const std::string category_owner_name =
          BuildCategoryOwnerName(implementation_decl.name,
                                 implementation_decl.category_name);
      ExecutableMetadataAggregatedCategorySurface &aggregate =
          aggregated_categories[category_owner_name];
      if (!aggregate.has_interface && !aggregate.has_implementation) {
        aggregate.line = implementation_decl.line;
        aggregate.column = implementation_decl.column;
      }
      aggregate.has_implementation = true;
      aggregate.implementation_owner_identity =
          implementation_decl.semantic_link_symbol;
      aggregate.class_owner_identity =
          BuildRuntimeClassOwnerIdentity(implementation_decl.name);
      aggregate.implementation_property_count =
          implementation_decl.properties.size();
      aggregate.implementation_method_count = implementation_decl.methods.size();
      aggregate.implementation_class_method_count =
          CountClassMethods(implementation_decl.methods);

      add_property_nodes(
          implementation_decl.properties, "category-implementation",
          category_owner_name, implementation_decl.semantic_link_symbol,
          BuildRuntimeCategoryOwnerIdentity(implementation_decl.name,
                                            implementation_decl.category_name));
      add_method_nodes(
          implementation_decl.methods, "category-implementation",
          category_owner_name, implementation_decl.semantic_link_symbol,
          BuildRuntimeCategoryOwnerIdentity(implementation_decl.name,
                                            implementation_decl.category_name),
          BuildRuntimeCategoryOwnerIdentity(implementation_decl.name,
                                            implementation_decl.category_name),
          false);
      continue;
    }

    ExecutableMetadataAggregatedClassSurface &aggregate =
        aggregated_classes[implementation_decl.name];
    Objc3ExecutableMetadataImplementationGraphNode node;
    node.class_name = implementation_decl.name;
    node.owner_identity = implementation_decl.semantic_link_symbol;
    node.interface_owner_identity =
        implementation_decl.semantic_link_interface_symbol;
    node.class_owner_identity =
        BuildRuntimeClassOwnerIdentity(implementation_decl.name);
    node.metaclass_owner_identity =
        BuildRuntimeMetaclassOwnerIdentity(implementation_decl.name);
    node.super_class_owner_identity = aggregate.super_class_owner_identity;
    node.super_metaclass_owner_identity =
        aggregate.super_class_owner_identity.empty()
            ? std::string{}
            : BuildRuntimeMetaclassOwnerIdentity(
                  aggregate.super_class_owner_identity.substr(6u));
    node.instance_method_owner_identity = node.class_owner_identity;
    node.class_method_owner_identity = node.metaclass_owner_identity;
    node.has_matching_interface = !node.interface_owner_identity.empty();
    node.has_super = !node.super_class_owner_identity.empty();
    node.declaration_complete =
        !node.owner_identity.empty() && !node.class_owner_identity.empty() &&
        !node.metaclass_owner_identity.empty() &&
        !node.instance_method_owner_identity.empty() &&
        !node.class_method_owner_identity.empty() &&
        (!node.has_super ||
         (!node.super_class_owner_identity.empty() &&
          !node.super_metaclass_owner_identity.empty()));
    node.property_count = implementation_decl.properties.size();
    node.method_count = implementation_decl.methods.size();
    node.class_method_count = CountClassMethods(implementation_decl.methods);
    node.instance_method_count = node.method_count - node.class_method_count;
    node.line = implementation_decl.line;
    node.column = implementation_decl.column;
    graph.implementation_nodes_lexicographic.push_back(node);

    if (!aggregate.has_interface && !aggregate.has_implementation) {
      aggregate.line = implementation_decl.line;
      aggregate.column = implementation_decl.column;
    }
    aggregate.has_implementation = true;
    aggregate.implementation_owner_identity = implementation_decl.semantic_link_symbol;
    aggregate.implementation_property_count = node.property_count;
    aggregate.implementation_method_count = node.method_count;
    aggregate.implementation_class_method_count = node.class_method_count;

    add_owner_edge("implementation-to-class", node.owner_identity,
                   node.class_owner_identity, node.line, node.column);
    add_owner_edge("implementation-to-metaclass", node.owner_identity,
                   node.metaclass_owner_identity, node.line, node.column);
    add_owner_edge("implementation-to-interface", node.owner_identity,
                   node.interface_owner_identity, node.line, node.column);
    add_owner_edge("implementation-to-instance-method-owner",
                   node.owner_identity, node.instance_method_owner_identity,
                   node.line, node.column);
    add_owner_edge("implementation-to-class-method-owner", node.owner_identity,
                   node.class_method_owner_identity, node.line, node.column);
    add_owner_edge("implementation-to-superclass", node.owner_identity,
                   node.super_class_owner_identity, node.line, node.column);
    add_owner_edge("implementation-to-super-metaclass", node.owner_identity,
                   node.super_metaclass_owner_identity, node.line, node.column);

    add_property_nodes(implementation_decl.properties, "class-implementation",
                       implementation_decl.name,
                       implementation_decl.semantic_link_symbol,
                       node.class_owner_identity);
    add_method_nodes(implementation_decl.methods, "class-implementation",
                     implementation_decl.name,
                     implementation_decl.semantic_link_symbol,
                     node.class_owner_identity, node.metaclass_owner_identity,
                     aggregate.objc_direct_members_declared);
  }

  for (const auto &protocol_decl : program.protocols) {
    Objc3ExecutableMetadataProtocolGraphNode node;
    node.protocol_name = protocol_decl.name;
    node.owner_identity = protocol_decl.semantic_link_symbol;
    node.inherited_protocol_owner_identities_lexicographic =
        protocol_decl.inherited_protocols_lexicographic;
    node.property_count = protocol_decl.properties.size();
    node.method_count = protocol_decl.methods.size();
    node.is_forward_declaration = protocol_decl.is_forward_declaration;
    node.declaration_complete =
        !node.protocol_name.empty() && !node.owner_identity.empty();
    node.line = protocol_decl.line;
    node.column = protocol_decl.column;
    std::sort(node.inherited_protocol_owner_identities_lexicographic.begin(),
              node.inherited_protocol_owner_identities_lexicographic.end());
    node.inherited_protocol_owner_identities_lexicographic.erase(
        std::unique(node.inherited_protocol_owner_identities_lexicographic.begin(),
                    node.inherited_protocol_owner_identities_lexicographic.end()),
        node.inherited_protocol_owner_identities_lexicographic.end());
    node.inherited_protocol_identity_complete =
        std::all_of(node.inherited_protocol_owner_identities_lexicographic.begin(),
                    node.inherited_protocol_owner_identities_lexicographic.end(),
                    [](const std::string &owner_identity) {
                      return !owner_identity.empty();
                    });
    graph.protocol_nodes_lexicographic.push_back(node);

    for (const auto &target : node.inherited_protocol_owner_identities_lexicographic) {
      add_owner_edge("protocol-to-inherited-protocol", node.owner_identity,
                     target, node.line, node.column);
    }

    add_property_nodes(protocol_decl.properties, "protocol", protocol_decl.name,
                       protocol_decl.semantic_link_symbol,
                       protocol_decl.semantic_link_symbol);
    add_method_nodes(protocol_decl.methods, "protocol", protocol_decl.name,
                     protocol_decl.semantic_link_symbol,
                     protocol_decl.semantic_link_symbol,
                     protocol_decl.semantic_link_symbol, false);
  }

  std::vector<std::string> class_names;
  class_names.reserve(aggregated_classes.size());
  for (const auto &entry : aggregated_classes) {
    class_names.push_back(entry.first);
  }
  std::sort(class_names.begin(), class_names.end());

  graph.class_nodes_lexicographic.reserve(class_names.size());
  graph.metaclass_nodes_lexicographic.reserve(class_names.size());
  for (const std::string &class_name : class_names) {
    const ExecutableMetadataAggregatedClassSurface &aggregate =
        aggregated_classes.at(class_name);

    Objc3ExecutableMetadataClassGraphNode class_node;
    class_node.class_name = class_name;
    class_node.owner_identity = BuildRuntimeClassOwnerIdentity(class_name);
    class_node.interface_owner_identity = aggregate.interface_owner_identity;
    class_node.implementation_owner_identity =
        aggregate.implementation_owner_identity;
    class_node.metaclass_owner_identity =
        BuildRuntimeMetaclassOwnerIdentity(class_name);
    class_node.super_class_owner_identity = aggregate.super_class_owner_identity;
    class_node.super_metaclass_owner_identity =
        aggregate.super_class_owner_identity.empty()
            ? std::string{}
            : BuildRuntimeMetaclassOwnerIdentity(
                  aggregate.super_class_owner_identity.substr(6u));
    class_node.adopted_protocol_owner_identities_lexicographic =
        aggregate.adopted_protocol_owner_identities_lexicographic;
    class_node.instance_method_owner_identity = class_node.owner_identity;
    class_node.class_method_owner_identity = class_node.metaclass_owner_identity;
    class_node.has_interface = aggregate.has_interface;
    class_node.has_implementation = aggregate.has_implementation;
    class_node.has_super = !aggregate.super_class_owner_identity.empty();
    class_node.objc_final_declared = aggregate.objc_final_declared;
    class_node.objc_sealed_declared = aggregate.objc_sealed_declared;
    class_node.realization_identity_complete =
        !class_node.owner_identity.empty() &&
        !class_node.metaclass_owner_identity.empty() &&
        !class_node.instance_method_owner_identity.empty() &&
        !class_node.class_method_owner_identity.empty() &&
        (!class_node.has_super ||
         (!class_node.super_class_owner_identity.empty() &&
          !class_node.super_metaclass_owner_identity.empty()));
    class_node.interface_property_count = aggregate.interface_property_count;
    class_node.implementation_property_count =
        aggregate.implementation_property_count;
    class_node.interface_method_count = aggregate.interface_method_count;
    class_node.implementation_method_count = aggregate.implementation_method_count;
    class_node.interface_class_method_count =
        aggregate.interface_class_method_count;
    class_node.implementation_class_method_count =
        aggregate.implementation_class_method_count;
    class_node.interface_instance_method_count =
        aggregate.interface_method_count -
        aggregate.interface_class_method_count;
    class_node.implementation_instance_method_count =
        aggregate.implementation_method_count -
        aggregate.implementation_class_method_count;
    class_node.line = aggregate.line;
    class_node.column = aggregate.column;
    graph.class_nodes_lexicographic.push_back(class_node);

    if (aggregate.has_interface) {
      Objc3ExecutableMetadataMetaclassGraphNode metaclass_node;
      metaclass_node.class_name = class_name;
      metaclass_node.owner_identity =
          BuildRuntimeMetaclassOwnerIdentity(class_name);
      metaclass_node.class_owner_identity = class_node.owner_identity;
      metaclass_node.interface_owner_identity = aggregate.interface_owner_identity;
      metaclass_node.implementation_owner_identity =
          aggregate.implementation_owner_identity;
      metaclass_node.super_metaclass_owner_identity =
          class_node.has_super
              ? BuildRuntimeMetaclassOwnerIdentity(
                    class_node.super_class_owner_identity.substr(6u))
              : std::string{};
      metaclass_node.derived_from_interface = true;
      metaclass_node.has_implementation = aggregate.has_implementation;
      metaclass_node.has_super = class_node.has_super;
      metaclass_node.interface_class_method_count =
          aggregate.interface_class_method_count;
      metaclass_node.implementation_class_method_count =
          aggregate.implementation_class_method_count;
      metaclass_node.line = aggregate.line;
      metaclass_node.column = aggregate.column;
      graph.metaclass_nodes_lexicographic.push_back(metaclass_node);

      add_owner_edge("class-to-metaclass", class_node.owner_identity,
                     metaclass_node.owner_identity, class_node.line,
                     class_node.column);
      add_owner_edge("metaclass-to-super-metaclass",
                     metaclass_node.owner_identity,
                     metaclass_node.super_metaclass_owner_identity,
                     metaclass_node.line, metaclass_node.column);
    }
  }

  std::vector<std::string> category_names;
  category_names.reserve(aggregated_categories.size());
  for (const auto &entry : aggregated_categories) {
    category_names.push_back(entry.first);
  }
  std::sort(category_names.begin(), category_names.end());

  graph.category_nodes_lexicographic.reserve(category_names.size());
  for (const std::string &category_owner_name : category_names) {
    const ExecutableMetadataAggregatedCategorySurface &aggregate =
        aggregated_categories.at(category_owner_name);
    const std::size_t open_paren = category_owner_name.find('(');
    const std::string class_name =
        open_paren == std::string::npos
            ? category_owner_name
            : category_owner_name.substr(0u, open_paren);
    const std::string category_name =
        open_paren == std::string::npos
            ? std::string{}
            : category_owner_name.substr(open_paren + 1u,
                                         category_owner_name.size() - open_paren - 2u);

    Objc3ExecutableMetadataCategoryGraphNode node;
    node.class_name = class_name;
    node.category_name = category_name;
    node.owner_identity =
        BuildRuntimeCategoryOwnerIdentity(class_name, category_name);
    node.interface_owner_identity = aggregate.interface_owner_identity;
    node.implementation_owner_identity = aggregate.implementation_owner_identity;
    node.class_owner_identity = aggregate.class_owner_identity;
    node.adopted_protocol_owner_identities_lexicographic =
        aggregate.adopted_protocol_owner_identities_lexicographic;
    node.has_interface = aggregate.has_interface;
    node.has_implementation = aggregate.has_implementation;
    node.declaration_complete =
        !node.class_name.empty() && !node.category_name.empty() &&
        !node.owner_identity.empty() && !node.class_owner_identity.empty() &&
        (!node.has_interface || !node.interface_owner_identity.empty()) &&
        (!node.has_implementation || !node.implementation_owner_identity.empty());
    node.attachment_identity_complete =
        !node.class_owner_identity.empty() &&
        (!node.has_interface || !node.interface_owner_identity.empty()) &&
        (!node.has_implementation || !node.implementation_owner_identity.empty());
    node.interface_property_count = aggregate.interface_property_count;
    node.implementation_property_count = aggregate.implementation_property_count;
    node.interface_method_count = aggregate.interface_method_count;
    node.implementation_method_count = aggregate.implementation_method_count;
    node.interface_class_method_count = aggregate.interface_class_method_count;
    node.implementation_class_method_count =
        aggregate.implementation_class_method_count;
    node.line = aggregate.line;
    node.column = aggregate.column;
    std::sort(node.adopted_protocol_owner_identities_lexicographic.begin(),
              node.adopted_protocol_owner_identities_lexicographic.end());
    node.adopted_protocol_owner_identities_lexicographic.erase(
        std::unique(node.adopted_protocol_owner_identities_lexicographic.begin(),
                    node.adopted_protocol_owner_identities_lexicographic.end()),
        node.adopted_protocol_owner_identities_lexicographic.end());
    node.conformance_identity_complete =
        std::all_of(node.adopted_protocol_owner_identities_lexicographic.begin(),
                    node.adopted_protocol_owner_identities_lexicographic.end(),
                    [](const std::string &owner_identity) {
                      return !owner_identity.empty();
                    });
    graph.category_nodes_lexicographic.push_back(node);

    add_owner_edge("category-to-class", node.owner_identity,
                   node.class_owner_identity, node.line, node.column);
    add_owner_edge("category-to-interface", node.owner_identity,
                   node.interface_owner_identity, node.line, node.column);
    add_owner_edge("category-to-implementation", node.owner_identity,
                   node.implementation_owner_identity, node.line, node.column);
    for (const auto &target : node.adopted_protocol_owner_identities_lexicographic) {
      add_owner_edge("category-to-protocol", node.owner_identity, target,
                     node.line, node.column);
    }
  }

  for (const auto &property_node : graph.property_nodes_lexicographic) {
    if (property_node.has_getter) {
      for (const auto &method_record : method_edge_records) {
        if (method_record.export_owner_identity == property_node.export_owner_identity &&
            method_record.selector == property_node.getter_selector &&
            !method_record.is_class_method) {
          add_owner_edge("property-to-getter-method", property_node.owner_identity,
                         method_record.owner_identity, property_node.line,
                         property_node.column);
        }
      }
    }

    if (property_node.has_setter) {
      for (const auto &method_record : method_edge_records) {
        if (method_record.export_owner_identity == property_node.export_owner_identity &&
            method_record.selector == property_node.setter_selector &&
            !method_record.is_class_method) {
          add_owner_edge("property-to-setter-method", property_node.owner_identity,
                         method_record.owner_identity, property_node.line,
                         property_node.column);
        }
      }
    }
  }

  std::unordered_map<std::string, const Objc3ExecutableMetadataClassGraphNode *>
      class_nodes_by_owner_identity;
  class_nodes_by_owner_identity.reserve(graph.class_nodes_lexicographic.size());
  for (const auto &class_node : graph.class_nodes_lexicographic) {
    class_nodes_by_owner_identity.emplace(class_node.owner_identity, &class_node);
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
      add_owner_edge("method-to-overridden-method", method_node.owner_identity,
                     overridden_method_owner_identity, method_node.line,
                     method_node.column);
    }
  }

  SortExecutableMetadataSourceGraph(graph);

  const auto has_graph_edge = [&graph](const std::string &edge_kind,
                                       const std::string &source_owner_identity,
                                       const std::string &target_owner_identity) {
    return HasExecutableMetadataGraphEdge(graph, edge_kind,
                                          source_owner_identity,
                                          target_owner_identity);
  };

  const std::size_t expected_class_interface_count = static_cast<std::size_t>(
      std::count_if(program.interfaces.begin(), program.interfaces.end(),
                    [](const Objc3InterfaceDecl &decl) {
                      return !decl.has_category;
                    }));
  const std::size_t expected_class_implementation_count = static_cast<std::size_t>(
      std::count_if(program.implementations.begin(), program.implementations.end(),
                    [](const Objc3ImplementationDecl &decl) {
                      return !decl.has_category;
                    }));
  const bool interface_count_aligned =
      graph.interface_nodes_lexicographic.size() == expected_class_interface_count;
  const bool implementation_count_aligned =
      graph.implementation_nodes_lexicographic.size() ==
      expected_class_implementation_count;
  const bool metaclass_count_aligned =
      graph.metaclass_nodes_lexicographic.size() ==
      graph.interface_nodes_lexicographic.size();
  const bool class_node_floor_satisfied =
      graph.class_nodes_lexicographic.size() >=
          graph.interface_nodes_lexicographic.size() &&
      graph.class_nodes_lexicographic.size() >=
          graph.implementation_nodes_lexicographic.size();
  const bool protocol_count_aligned =
      graph.protocol_nodes_lexicographic.size() ==
      runtime_metadata_source_records.protocols_lexicographic.size();
  const bool property_count_aligned =
      graph.property_nodes_lexicographic.size() ==
      runtime_metadata_source_records.properties_lexicographic.size();
  const bool method_count_aligned =
      graph.method_nodes_lexicographic.size() ==
      runtime_metadata_source_records.methods_lexicographic.size();
  const bool ivar_count_aligned =
      graph.ivar_nodes_lexicographic.size() ==
      runtime_metadata_source_records.ivars_lexicographic.size();
  std::vector<std::string> category_record_owner_names;
  category_record_owner_names.reserve(
      runtime_metadata_source_records.categories_lexicographic.size());
  for (const auto &record :
       runtime_metadata_source_records.categories_lexicographic) {
    category_record_owner_names.push_back(
        BuildCategoryOwnerName(record.class_name, record.category_name));
  }
  std::sort(category_record_owner_names.begin(),
            category_record_owner_names.end());
  category_record_owner_names.erase(
      std::unique(category_record_owner_names.begin(),
                  category_record_owner_names.end()),
      category_record_owner_names.end());
  const bool category_count_aligned =
      graph.category_nodes_lexicographic.size() ==
      category_record_owner_names.size();

  graph.class_metaclass_declaration_closure_complete = true;
  graph.class_metaclass_parent_identity_closure_complete = true;
  graph.class_metaclass_method_owner_identity_closure_complete = true;
  graph.class_metaclass_object_identity_closure_complete = true;
  graph.protocol_category_declaration_closure_complete = true;
  graph.protocol_inheritance_identity_closure_complete = true;
  graph.category_attachment_identity_closure_complete = true;
  graph.protocol_category_conformance_identity_closure_complete = true;

  for (const auto &node : graph.interface_nodes_lexicographic) {
    graph.class_metaclass_declaration_closure_complete =
        graph.class_metaclass_declaration_closure_complete &&
        node.declaration_complete;
    graph.class_metaclass_parent_identity_closure_complete =
        graph.class_metaclass_parent_identity_closure_complete &&
        (!node.has_super ||
         (!node.super_class_owner_identity.empty() &&
          !node.super_metaclass_owner_identity.empty() &&
          has_graph_edge("interface-to-superclass", node.owner_identity,
                         node.super_class_owner_identity) &&
          has_graph_edge("interface-to-super-metaclass", node.owner_identity,
                         node.super_metaclass_owner_identity)));
    graph.class_metaclass_method_owner_identity_closure_complete =
        graph.class_metaclass_method_owner_identity_closure_complete &&
        node.instance_method_owner_identity == node.class_owner_identity &&
        node.class_method_owner_identity == node.metaclass_owner_identity &&
        has_graph_edge("interface-to-instance-method-owner", node.owner_identity,
                       node.instance_method_owner_identity) &&
        has_graph_edge("interface-to-class-method-owner", node.owner_identity,
                       node.class_method_owner_identity);
    graph.class_metaclass_object_identity_closure_complete =
        graph.class_metaclass_object_identity_closure_complete &&
        !node.class_owner_identity.empty() && !node.metaclass_owner_identity.empty() &&
        has_graph_edge("interface-to-class", node.owner_identity,
                       node.class_owner_identity) &&
        has_graph_edge("interface-to-metaclass", node.owner_identity,
                       node.metaclass_owner_identity);
  }

  for (const auto &node : graph.implementation_nodes_lexicographic) {
    graph.class_metaclass_declaration_closure_complete =
        graph.class_metaclass_declaration_closure_complete &&
        node.declaration_complete;
    graph.class_metaclass_parent_identity_closure_complete =
        graph.class_metaclass_parent_identity_closure_complete &&
        (!node.has_super ||
         (!node.super_class_owner_identity.empty() &&
          !node.super_metaclass_owner_identity.empty() &&
          has_graph_edge("implementation-to-superclass", node.owner_identity,
                         node.super_class_owner_identity) &&
          has_graph_edge("implementation-to-super-metaclass", node.owner_identity,
                         node.super_metaclass_owner_identity)));
    graph.class_metaclass_method_owner_identity_closure_complete =
        graph.class_metaclass_method_owner_identity_closure_complete &&
        node.instance_method_owner_identity == node.class_owner_identity &&
        node.class_method_owner_identity == node.metaclass_owner_identity &&
        has_graph_edge("implementation-to-instance-method-owner",
                       node.owner_identity, node.instance_method_owner_identity) &&
        has_graph_edge("implementation-to-class-method-owner",
                       node.owner_identity, node.class_method_owner_identity);
    graph.class_metaclass_object_identity_closure_complete =
        graph.class_metaclass_object_identity_closure_complete &&
        !node.class_owner_identity.empty() && !node.metaclass_owner_identity.empty() &&
        has_graph_edge("implementation-to-class", node.owner_identity,
                       node.class_owner_identity) &&
        has_graph_edge("implementation-to-metaclass", node.owner_identity,
                       node.metaclass_owner_identity);
  }

  for (const auto &node : graph.class_nodes_lexicographic) {
    graph.class_metaclass_declaration_closure_complete =
        graph.class_metaclass_declaration_closure_complete &&
        node.realization_identity_complete;
    graph.class_metaclass_parent_identity_closure_complete =
        graph.class_metaclass_parent_identity_closure_complete &&
        (!node.has_super ||
         (!node.super_class_owner_identity.empty() &&
          !node.super_metaclass_owner_identity.empty() &&
          has_graph_edge("class-to-superclass", node.owner_identity,
                         node.super_class_owner_identity)));
    graph.class_metaclass_method_owner_identity_closure_complete =
        graph.class_metaclass_method_owner_identity_closure_complete &&
        node.instance_method_owner_identity == node.owner_identity &&
        node.class_method_owner_identity == node.metaclass_owner_identity;
    graph.class_metaclass_object_identity_closure_complete =
        graph.class_metaclass_object_identity_closure_complete &&
        !node.owner_identity.empty() && !node.metaclass_owner_identity.empty() &&
        has_graph_edge("class-to-metaclass", node.owner_identity,
                       node.metaclass_owner_identity);
  }

  for (const auto &node : graph.metaclass_nodes_lexicographic) {
    graph.class_metaclass_declaration_closure_complete =
        graph.class_metaclass_declaration_closure_complete &&
        !node.owner_identity.empty() && !node.class_owner_identity.empty();
    graph.class_metaclass_parent_identity_closure_complete =
        graph.class_metaclass_parent_identity_closure_complete &&
        (!node.has_super ||
         (!node.super_metaclass_owner_identity.empty() &&
          has_graph_edge("metaclass-to-super-metaclass", node.owner_identity,
                         node.super_metaclass_owner_identity)));
    graph.class_metaclass_object_identity_closure_complete =
        graph.class_metaclass_object_identity_closure_complete &&
        !node.owner_identity.empty() && !node.class_owner_identity.empty() &&
        has_graph_edge("class-to-metaclass", node.class_owner_identity,
                       node.owner_identity);
  }

  for (const auto &node : graph.protocol_nodes_lexicographic) {
    graph.protocol_category_declaration_closure_complete =
        graph.protocol_category_declaration_closure_complete &&
        node.declaration_complete;
    graph.protocol_inheritance_identity_closure_complete =
        graph.protocol_inheritance_identity_closure_complete &&
        node.inherited_protocol_identity_complete &&
        std::all_of(
            node.inherited_protocol_owner_identities_lexicographic.begin(),
            node.inherited_protocol_owner_identities_lexicographic.end(),
            [&](const std::string &target_owner_identity) {
              return has_graph_edge("protocol-to-inherited-protocol",
                                    node.owner_identity,
                                    target_owner_identity);
            });
  }

  for (const auto &node : graph.category_nodes_lexicographic) {
    graph.protocol_category_declaration_closure_complete =
        graph.protocol_category_declaration_closure_complete &&
        node.declaration_complete;
    graph.category_attachment_identity_closure_complete =
        graph.category_attachment_identity_closure_complete &&
        node.attachment_identity_complete &&
        has_graph_edge("category-to-class", node.owner_identity,
                       node.class_owner_identity) &&
        (!node.has_interface ||
         has_graph_edge("category-to-interface", node.owner_identity,
                        node.interface_owner_identity)) &&
        (!node.has_implementation ||
         has_graph_edge("category-to-implementation", node.owner_identity,
                        node.implementation_owner_identity));
    graph.protocol_category_conformance_identity_closure_complete =
        graph.protocol_category_conformance_identity_closure_complete &&
        node.conformance_identity_complete &&
        std::all_of(
            node.adopted_protocol_owner_identities_lexicographic.begin(),
            node.adopted_protocol_owner_identities_lexicographic.end(),
            [&](const std::string &target_owner_identity) {
              return has_graph_edge("category-to-protocol", node.owner_identity,
                                    target_owner_identity);
            });
  }

  graph.deterministic =
      IsExecutableMetadataSourceGraphDeterministic(graph);
  graph.source_graph_complete =
      graph.deterministic && interface_count_aligned &&
      implementation_count_aligned && metaclass_count_aligned &&
      class_node_floor_satisfied && protocol_count_aligned &&
      category_count_aligned && property_count_aligned &&
      method_count_aligned && ivar_count_aligned &&
      graph.class_metaclass_declaration_closure_complete &&
      graph.class_metaclass_parent_identity_closure_complete &&
      graph.class_metaclass_method_owner_identity_closure_complete &&
      graph.class_metaclass_object_identity_closure_complete &&
      graph.protocol_category_declaration_closure_complete &&
      graph.protocol_inheritance_identity_closure_complete &&
      graph.category_attachment_identity_closure_complete &&
      graph.protocol_category_conformance_identity_closure_complete;
  graph.ready_for_semantic_closure = graph.source_graph_complete;
  graph.ready_for_lowering = false;
  return graph;
}

}  // namespace objc3c::pipeline::orchestration
