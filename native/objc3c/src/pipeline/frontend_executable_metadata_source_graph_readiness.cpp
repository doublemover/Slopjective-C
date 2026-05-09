#include "pipeline/frontend_executable_metadata_source_graph_readiness.h"

#include <algorithm>
#include <cstddef>
#include <string>
#include <vector>

#include "pipeline/frontend_metadata_handoff_helpers.h"
#include "pipeline/frontend_metadata_handoff_ordering.h"
#include "pipeline/objc3_frontend_types.h"

namespace objc3c::pipeline::orchestration {

bool HasExecutableMetadataGraphEdge(
    const Objc3ExecutableMetadataSourceGraph &graph,
    const std::string &edge_kind,
    const std::string &source_owner_identity,
    const std::string &target_owner_identity) {
  return std::any_of(
      graph.owner_edges_lexicographic.begin(),
      graph.owner_edges_lexicographic.end(),
      [&edge_kind, &source_owner_identity, &target_owner_identity](
          const Objc3ExecutableMetadataGraphEdge &edge) {
        return edge.edge_kind == edge_kind &&
               edge.source_owner_identity == source_owner_identity &&
               edge.target_owner_identity == target_owner_identity;
      });
}

void SortExecutableMetadataSourceGraph(
    Objc3ExecutableMetadataSourceGraph &graph) {
  std::sort(graph.interface_nodes_lexicographic.begin(),
            graph.interface_nodes_lexicographic.end(),
            IsExecutableMetadataInterfaceNodeLess);
  std::sort(graph.implementation_nodes_lexicographic.begin(),
            graph.implementation_nodes_lexicographic.end(),
            IsExecutableMetadataImplementationNodeLess);
  std::sort(graph.class_nodes_lexicographic.begin(),
            graph.class_nodes_lexicographic.end(),
            IsExecutableMetadataClassNodeLess);
  std::sort(graph.metaclass_nodes_lexicographic.begin(),
            graph.metaclass_nodes_lexicographic.end(),
            IsExecutableMetadataMetaclassNodeLess);
  std::sort(graph.protocol_nodes_lexicographic.begin(),
            graph.protocol_nodes_lexicographic.end(),
            IsExecutableMetadataProtocolNodeLess);
  std::sort(graph.category_nodes_lexicographic.begin(),
            graph.category_nodes_lexicographic.end(),
            IsExecutableMetadataCategoryNodeLess);
  std::sort(graph.property_nodes_lexicographic.begin(),
            graph.property_nodes_lexicographic.end(),
            IsExecutableMetadataPropertyNodeLess);
  std::sort(graph.method_nodes_lexicographic.begin(),
            graph.method_nodes_lexicographic.end(),
            IsExecutableMetadataMethodNodeLess);
  std::sort(graph.ivar_nodes_lexicographic.begin(),
            graph.ivar_nodes_lexicographic.end(),
            IsExecutableMetadataIvarNodeLess);
  std::sort(graph.owner_edges_lexicographic.begin(),
            graph.owner_edges_lexicographic.end(),
            IsExecutableMetadataGraphEdgeLess);
}

bool IsExecutableMetadataSourceGraphDeterministic(
    const Objc3ExecutableMetadataSourceGraph &graph) {
  return std::is_sorted(graph.interface_nodes_lexicographic.begin(),
                        graph.interface_nodes_lexicographic.end(),
                        IsExecutableMetadataInterfaceNodeLess) &&
         std::is_sorted(graph.implementation_nodes_lexicographic.begin(),
                        graph.implementation_nodes_lexicographic.end(),
                        IsExecutableMetadataImplementationNodeLess) &&
         std::is_sorted(graph.class_nodes_lexicographic.begin(),
                        graph.class_nodes_lexicographic.end(),
                        IsExecutableMetadataClassNodeLess) &&
         std::is_sorted(graph.metaclass_nodes_lexicographic.begin(),
                        graph.metaclass_nodes_lexicographic.end(),
                        IsExecutableMetadataMetaclassNodeLess) &&
         std::is_sorted(graph.protocol_nodes_lexicographic.begin(),
                        graph.protocol_nodes_lexicographic.end(),
                        IsExecutableMetadataProtocolNodeLess) &&
         std::is_sorted(graph.category_nodes_lexicographic.begin(),
                        graph.category_nodes_lexicographic.end(),
                        IsExecutableMetadataCategoryNodeLess) &&
         std::is_sorted(graph.property_nodes_lexicographic.begin(),
                        graph.property_nodes_lexicographic.end(),
                        IsExecutableMetadataPropertyNodeLess) &&
         std::is_sorted(graph.method_nodes_lexicographic.begin(),
                        graph.method_nodes_lexicographic.end(),
                        IsExecutableMetadataMethodNodeLess) &&
         std::is_sorted(graph.ivar_nodes_lexicographic.begin(),
                        graph.ivar_nodes_lexicographic.end(),
                        IsExecutableMetadataIvarNodeLess) &&
         std::is_sorted(graph.owner_edges_lexicographic.begin(),
                        graph.owner_edges_lexicographic.end(),
                        IsExecutableMetadataGraphEdgeLess);
}

void MarkExecutableMetadataSourceGraphReadiness(
    const Objc3Program &program,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records,
    Objc3ExecutableMetadataSourceGraph &graph) {
  const std::size_t expected_class_interface_count = static_cast<std::size_t>(
      std::count_if(program.interfaces.begin(), program.interfaces.end(),
                    [](const Objc3InterfaceDecl &decl) {
                      return !decl.has_category;
                    }));
  const std::size_t expected_class_implementation_count =
      static_cast<std::size_t>(
          std::count_if(program.implementations.begin(),
                        program.implementations.end(),
                        [](const Objc3ImplementationDecl &decl) {
                          return !decl.has_category;
                        }));
  const bool interface_count_aligned =
      graph.interface_nodes_lexicographic.size() ==
      expected_class_interface_count;
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
          HasExecutableMetadataGraphEdge(graph, "interface-to-superclass",
                                         node.owner_identity,
                                         node.super_class_owner_identity) &&
          HasExecutableMetadataGraphEdge(graph, "interface-to-super-metaclass",
                                         node.owner_identity,
                                         node.super_metaclass_owner_identity)));
    graph.class_metaclass_method_owner_identity_closure_complete =
        graph.class_metaclass_method_owner_identity_closure_complete &&
        node.instance_method_owner_identity == node.class_owner_identity &&
        node.class_method_owner_identity == node.metaclass_owner_identity &&
        HasExecutableMetadataGraphEdge(graph, "interface-to-instance-method-owner",
                                       node.owner_identity,
                                       node.instance_method_owner_identity) &&
        HasExecutableMetadataGraphEdge(graph, "interface-to-class-method-owner",
                                       node.owner_identity,
                                       node.class_method_owner_identity);
    graph.class_metaclass_object_identity_closure_complete =
        graph.class_metaclass_object_identity_closure_complete &&
        !node.class_owner_identity.empty() &&
        !node.metaclass_owner_identity.empty() &&
        HasExecutableMetadataGraphEdge(graph, "interface-to-class",
                                       node.owner_identity,
                                       node.class_owner_identity) &&
        HasExecutableMetadataGraphEdge(graph, "interface-to-metaclass",
                                       node.owner_identity,
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
          HasExecutableMetadataGraphEdge(graph, "implementation-to-superclass",
                                         node.owner_identity,
                                         node.super_class_owner_identity) &&
          HasExecutableMetadataGraphEdge(graph,
                                         "implementation-to-super-metaclass",
                                         node.owner_identity,
                                         node.super_metaclass_owner_identity)));
    graph.class_metaclass_method_owner_identity_closure_complete =
        graph.class_metaclass_method_owner_identity_closure_complete &&
        node.instance_method_owner_identity == node.class_owner_identity &&
        node.class_method_owner_identity == node.metaclass_owner_identity &&
        HasExecutableMetadataGraphEdge(graph,
                                       "implementation-to-instance-method-owner",
                                       node.owner_identity,
                                       node.instance_method_owner_identity) &&
        HasExecutableMetadataGraphEdge(graph,
                                       "implementation-to-class-method-owner",
                                       node.owner_identity,
                                       node.class_method_owner_identity);
    graph.class_metaclass_object_identity_closure_complete =
        graph.class_metaclass_object_identity_closure_complete &&
        !node.class_owner_identity.empty() &&
        !node.metaclass_owner_identity.empty() &&
        HasExecutableMetadataGraphEdge(graph, "implementation-to-class",
                                       node.owner_identity,
                                       node.class_owner_identity) &&
        HasExecutableMetadataGraphEdge(graph, "implementation-to-metaclass",
                                       node.owner_identity,
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
          HasExecutableMetadataGraphEdge(graph, "class-to-superclass",
                                         node.owner_identity,
                                         node.super_class_owner_identity)));
    graph.class_metaclass_method_owner_identity_closure_complete =
        graph.class_metaclass_method_owner_identity_closure_complete &&
        node.instance_method_owner_identity == node.owner_identity &&
        node.class_method_owner_identity == node.metaclass_owner_identity;
    graph.class_metaclass_object_identity_closure_complete =
        graph.class_metaclass_object_identity_closure_complete &&
        !node.owner_identity.empty() &&
        !node.metaclass_owner_identity.empty() &&
        HasExecutableMetadataGraphEdge(graph, "class-to-metaclass",
                                       node.owner_identity,
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
          HasExecutableMetadataGraphEdge(graph, "metaclass-to-super-metaclass",
                                         node.owner_identity,
                                         node.super_metaclass_owner_identity)));
    graph.class_metaclass_object_identity_closure_complete =
        graph.class_metaclass_object_identity_closure_complete &&
        !node.owner_identity.empty() && !node.class_owner_identity.empty() &&
        HasExecutableMetadataGraphEdge(graph, "class-to-metaclass",
                                       node.class_owner_identity,
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
              return HasExecutableMetadataGraphEdge(
                  graph, "protocol-to-inherited-protocol",
                  node.owner_identity, target_owner_identity);
            });
  }

  for (const auto &node : graph.category_nodes_lexicographic) {
    graph.protocol_category_declaration_closure_complete =
        graph.protocol_category_declaration_closure_complete &&
        node.declaration_complete;
    graph.category_attachment_identity_closure_complete =
        graph.category_attachment_identity_closure_complete &&
        node.attachment_identity_complete &&
        HasExecutableMetadataGraphEdge(graph, "category-to-class",
                                       node.owner_identity,
                                       node.class_owner_identity) &&
        (!node.has_interface ||
         HasExecutableMetadataGraphEdge(graph, "category-to-interface",
                                        node.owner_identity,
                                        node.interface_owner_identity)) &&
        (!node.has_implementation ||
         HasExecutableMetadataGraphEdge(graph, "category-to-implementation",
                                        node.owner_identity,
                                        node.implementation_owner_identity));
    graph.protocol_category_conformance_identity_closure_complete =
        graph.protocol_category_conformance_identity_closure_complete &&
        node.conformance_identity_complete &&
        std::all_of(
            node.adopted_protocol_owner_identities_lexicographic.begin(),
            node.adopted_protocol_owner_identities_lexicographic.end(),
            [&](const std::string &target_owner_identity) {
              return HasExecutableMetadataGraphEdge(
                  graph, "category-to-protocol", node.owner_identity,
                  target_owner_identity);
            });
  }

  graph.deterministic = IsExecutableMetadataSourceGraphDeterministic(graph);
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
}

}  // namespace objc3c::pipeline::orchestration
