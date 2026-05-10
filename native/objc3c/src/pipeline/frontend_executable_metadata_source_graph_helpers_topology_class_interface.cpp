#include "pipeline/frontend_executable_metadata_source_graph_helpers_owners.h"

#include <algorithm>
#include <string>
#include <vector>

#include "pipeline/frontend_metadata_handoff_helpers.h"

namespace objc3c::pipeline::orchestration {

void PopulateExecutableMetadataInterfaceTopology(
    const Objc3Program &program,
    Objc3ExecutableMetadataSourceGraph &graph,
    ExecutableMetadataSourceGraphTopologyContext &context) {
  for (const auto &interface_decl : program.interfaces) {
    if (interface_decl.has_category) {
      const std::string category_owner_name =
          BuildCategoryOwnerName(interface_decl.name,
                                 interface_decl.category_name);
      ExecutableMetadataAggregatedCategorySurface &aggregate =
          context.aggregated_categories[category_owner_name];
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

      AddExecutableMetadataPropertyNodes(
          interface_decl.properties, "category-interface", category_owner_name,
          interface_decl.semantic_link_symbol,
          BuildRuntimeCategoryOwnerIdentity(interface_decl.name,
                                            interface_decl.category_name),
          context.property_synthesis_index, graph);
      AddExecutableMetadataMethodNodes(
          interface_decl.methods, "category-interface", category_owner_name,
          interface_decl.semantic_link_symbol,
          BuildRuntimeCategoryOwnerIdentity(interface_decl.name,
                                            interface_decl.category_name),
          BuildRuntimeCategoryOwnerIdentity(interface_decl.name,
                                            interface_decl.category_name),
          false, graph, context.method_edge_records);
      continue;
    }

    Objc3ExecutableMetadataInterfaceGraphNode node;
    node.class_name = interface_decl.name;
    node.owner_identity = interface_decl.semantic_link_symbol;
    node.class_owner_identity =
        BuildRuntimeClassOwnerIdentity(interface_decl.name);
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
        context.aggregated_classes[interface_decl.name];
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

    AddExecutableMetadataOwnerEdge(graph, "interface-to-class",
                                   node.owner_identity,
                                   node.class_owner_identity, node.line,
                                   node.column);
    AddExecutableMetadataOwnerEdge(graph, "interface-to-metaclass",
                                   node.owner_identity,
                                   node.metaclass_owner_identity, node.line,
                                   node.column);
    AddExecutableMetadataOwnerEdge(graph, "interface-to-instance-method-owner",
                                   node.owner_identity,
                                   node.instance_method_owner_identity,
                                   node.line, node.column);
    AddExecutableMetadataOwnerEdge(graph, "interface-to-class-method-owner",
                                   node.owner_identity,
                                   node.class_method_owner_identity, node.line,
                                   node.column);
    AddExecutableMetadataOwnerEdge(graph, "class-to-superclass",
                                   node.class_owner_identity,
                                   node.super_class_owner_identity, node.line,
                                   node.column);
    AddExecutableMetadataOwnerEdge(graph, "interface-to-superclass",
                                   node.owner_identity,
                                   node.super_class_owner_identity, node.line,
                                   node.column);
    AddExecutableMetadataOwnerEdge(graph, "interface-to-super-metaclass",
                                   node.owner_identity,
                                   node.super_metaclass_owner_identity,
                                   node.line, node.column);

    AddExecutableMetadataPropertyNodes(
        interface_decl.properties, "class-interface", interface_decl.name,
        interface_decl.semantic_link_symbol, node.class_owner_identity,
        context.property_synthesis_index, graph);
    AddExecutableMetadataMethodNodes(
        interface_decl.methods, "class-interface", interface_decl.name,
        interface_decl.semantic_link_symbol, node.class_owner_identity,
        node.metaclass_owner_identity, interface_decl.objc_direct_members_declared,
        graph, context.method_edge_records);
  }
}

void PopulateExecutableMetadataProtocolTopology(
    const Objc3Program &program,
    Objc3ExecutableMetadataSourceGraph &graph,
    ExecutableMetadataSourceGraphTopologyContext &context) {
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

    for (const auto &target :
         node.inherited_protocol_owner_identities_lexicographic) {
      AddExecutableMetadataOwnerEdge(
          graph, "protocol-to-inherited-protocol", node.owner_identity, target,
          node.line, node.column);
    }

    AddExecutableMetadataPropertyNodes(
        protocol_decl.properties, "protocol", protocol_decl.name,
        protocol_decl.semantic_link_symbol, protocol_decl.semantic_link_symbol,
        context.property_synthesis_index, graph);
    AddExecutableMetadataMethodNodes(
        protocol_decl.methods, "protocol", protocol_decl.name,
        protocol_decl.semantic_link_symbol, protocol_decl.semantic_link_symbol,
        protocol_decl.semantic_link_symbol, false, graph,
        context.method_edge_records);
  }
}

void FinalizeExecutableMetadataClassTopology(
    Objc3ExecutableMetadataSourceGraph &graph,
    const ExecutableMetadataSourceGraphTopologyContext &context) {
  std::vector<std::string> class_names;
  class_names.reserve(context.aggregated_classes.size());
  for (const auto &entry : context.aggregated_classes) {
    class_names.push_back(entry.first);
  }
  std::sort(class_names.begin(), class_names.end());

  graph.class_nodes_lexicographic.reserve(class_names.size());
  graph.metaclass_nodes_lexicographic.reserve(class_names.size());
  for (const std::string &class_name : class_names) {
    const ExecutableMetadataAggregatedClassSurface &aggregate =
        context.aggregated_classes.at(class_name);

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
    class_node.implementation_method_count =
        aggregate.implementation_method_count;
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
      metaclass_node.interface_owner_identity =
          aggregate.interface_owner_identity;
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

      AddExecutableMetadataOwnerEdge(graph, "class-to-metaclass",
                                     class_node.owner_identity,
                                     metaclass_node.owner_identity,
                                     class_node.line, class_node.column);
      AddExecutableMetadataOwnerEdge(
          graph, "metaclass-to-super-metaclass", metaclass_node.owner_identity,
          metaclass_node.super_metaclass_owner_identity, metaclass_node.line,
          metaclass_node.column);
    }
  }
}

}  // namespace objc3c::pipeline::orchestration
