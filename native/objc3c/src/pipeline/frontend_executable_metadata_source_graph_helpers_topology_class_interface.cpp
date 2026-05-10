#include "pipeline/frontend_executable_metadata_source_graph_helpers_owners.h"

#include <string>

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

}  // namespace objc3c::pipeline::orchestration
