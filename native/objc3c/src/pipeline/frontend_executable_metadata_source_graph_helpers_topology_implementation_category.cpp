#include "pipeline/frontend_executable_metadata_source_graph_helpers_owners.h"

#include <algorithm>
#include <cstddef>
#include <string>
#include <vector>

#include "pipeline/frontend_metadata_handoff_helpers.h"

namespace objc3c::pipeline::orchestration {

void PopulateExecutableMetadataImplementationTopology(
    const Objc3Program &program,
    Objc3ExecutableMetadataSourceGraph &graph,
    ExecutableMetadataSourceGraphTopologyContext &context) {
  for (const auto &implementation_decl : program.implementations) {
    if (implementation_decl.has_category) {
      const std::string category_owner_name =
          BuildCategoryOwnerName(implementation_decl.name,
                                 implementation_decl.category_name);
      ExecutableMetadataAggregatedCategorySurface &aggregate =
          context.aggregated_categories[category_owner_name];
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
      aggregate.implementation_method_count =
          implementation_decl.methods.size();
      aggregate.implementation_class_method_count =
          CountClassMethods(implementation_decl.methods);

      AddExecutableMetadataPropertyNodes(
          implementation_decl.properties, "category-implementation",
          category_owner_name, implementation_decl.semantic_link_symbol,
          BuildRuntimeCategoryOwnerIdentity(implementation_decl.name,
                                            implementation_decl.category_name),
          context.property_synthesis_index, graph);
      AddExecutableMetadataMethodNodes(
          implementation_decl.methods, "category-implementation",
          category_owner_name, implementation_decl.semantic_link_symbol,
          BuildRuntimeCategoryOwnerIdentity(implementation_decl.name,
                                            implementation_decl.category_name),
          BuildRuntimeCategoryOwnerIdentity(implementation_decl.name,
                                            implementation_decl.category_name),
          false, graph, context.method_edge_records);
      continue;
    }

    ExecutableMetadataAggregatedClassSurface &aggregate =
        context.aggregated_classes[implementation_decl.name];
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
    aggregate.implementation_owner_identity =
        implementation_decl.semantic_link_symbol;
    aggregate.implementation_property_count = node.property_count;
    aggregate.implementation_method_count = node.method_count;
    aggregate.implementation_class_method_count = node.class_method_count;

    AddExecutableMetadataOwnerEdge(graph, "implementation-to-class",
                                   node.owner_identity,
                                   node.class_owner_identity, node.line,
                                   node.column);
    AddExecutableMetadataOwnerEdge(graph, "implementation-to-metaclass",
                                   node.owner_identity,
                                   node.metaclass_owner_identity, node.line,
                                   node.column);
    AddExecutableMetadataOwnerEdge(graph, "implementation-to-interface",
                                   node.owner_identity,
                                   node.interface_owner_identity, node.line,
                                   node.column);
    AddExecutableMetadataOwnerEdge(
        graph, "implementation-to-instance-method-owner", node.owner_identity,
        node.instance_method_owner_identity, node.line, node.column);
    AddExecutableMetadataOwnerEdge(graph, "implementation-to-class-method-owner",
                                   node.owner_identity,
                                   node.class_method_owner_identity, node.line,
                                   node.column);
    AddExecutableMetadataOwnerEdge(graph, "implementation-to-superclass",
                                   node.owner_identity,
                                   node.super_class_owner_identity, node.line,
                                   node.column);
    AddExecutableMetadataOwnerEdge(graph, "implementation-to-super-metaclass",
                                   node.owner_identity,
                                   node.super_metaclass_owner_identity,
                                   node.line, node.column);

    AddExecutableMetadataPropertyNodes(
        implementation_decl.properties, "class-implementation",
        implementation_decl.name, implementation_decl.semantic_link_symbol,
        node.class_owner_identity, context.property_synthesis_index, graph);
    AddExecutableMetadataMethodNodes(
        implementation_decl.methods, "class-implementation",
        implementation_decl.name, implementation_decl.semantic_link_symbol,
        node.class_owner_identity, node.metaclass_owner_identity,
        aggregate.objc_direct_members_declared, graph,
        context.method_edge_records);
  }
}

void FinalizeExecutableMetadataCategoryTopology(
    Objc3ExecutableMetadataSourceGraph &graph,
    const ExecutableMetadataSourceGraphTopologyContext &context) {
  std::vector<std::string> category_names;
  category_names.reserve(context.aggregated_categories.size());
  for (const auto &entry : context.aggregated_categories) {
    category_names.push_back(entry.first);
  }
  std::sort(category_names.begin(), category_names.end());

  graph.category_nodes_lexicographic.reserve(category_names.size());
  for (const std::string &category_owner_name : category_names) {
    const ExecutableMetadataAggregatedCategorySurface &aggregate =
        context.aggregated_categories.at(category_owner_name);
    const std::size_t open_paren = category_owner_name.find('(');
    const std::string class_name =
        open_paren == std::string::npos
            ? category_owner_name
            : category_owner_name.substr(0u, open_paren);
    const std::string category_name =
        open_paren == std::string::npos
            ? std::string{}
            : category_owner_name.substr(
                  open_paren + 1u,
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

    AddExecutableMetadataOwnerEdge(graph, "category-to-class",
                                   node.owner_identity,
                                   node.class_owner_identity, node.line,
                                   node.column);
    AddExecutableMetadataOwnerEdge(graph, "category-to-interface",
                                   node.owner_identity,
                                   node.interface_owner_identity, node.line,
                                   node.column);
    AddExecutableMetadataOwnerEdge(graph, "category-to-implementation",
                                   node.owner_identity,
                                   node.implementation_owner_identity,
                                   node.line, node.column);
    for (const auto &target :
         node.adopted_protocol_owner_identities_lexicographic) {
      AddExecutableMetadataOwnerEdge(graph, "category-to-protocol",
                                     node.owner_identity, target, node.line,
                                     node.column);
    }
  }
}

}  // namespace objc3c::pipeline::orchestration
