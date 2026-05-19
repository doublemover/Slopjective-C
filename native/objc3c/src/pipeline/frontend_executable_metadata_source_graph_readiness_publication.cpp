#include "pipeline/frontend_executable_metadata_source_graph_readiness_owners.h"

#include <algorithm>
#include <string>

namespace objc3c::pipeline::orchestration {

void InitializeExecutableMetadataSourceGraphReadinessPublication(
    Objc3ExecutableMetadataSourceGraph &graph) {
  graph.class_metaclass_declaration_closure_complete = true;
  graph.class_metaclass_parent_identity_closure_complete = true;
  graph.class_metaclass_method_owner_identity_closure_complete = true;
  graph.class_metaclass_object_identity_closure_complete = true;
  graph.protocol_category_declaration_closure_complete = true;
  graph.protocol_inheritance_identity_closure_complete = true;
  graph.category_attachment_identity_closure_complete = true;
  graph.protocol_category_conformance_identity_closure_complete = true;
}

void PublishExecutableMetadataClassMetaclassReadiness(
    Objc3ExecutableMetadataSourceGraph &graph) {
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
}

void PublishExecutableMetadataProtocolCategoryReadiness(
    Objc3ExecutableMetadataSourceGraph &graph) {
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
}

void PublishExecutableMetadataSourceGraphFinalReadiness(
    const ExecutableMetadataSourceGraphReadinessCounts &readiness_counts,
    Objc3ExecutableMetadataSourceGraph &graph) {
  graph.deterministic = IsExecutableMetadataSourceGraphDeterministic(graph);
  graph.source_graph_complete =
      graph.deterministic && readiness_counts.interface_count_aligned &&
      readiness_counts.implementation_count_aligned &&
      readiness_counts.metaclass_count_aligned &&
      readiness_counts.class_node_floor_satisfied &&
      readiness_counts.protocol_count_aligned &&
      readiness_counts.category_count_aligned &&
      readiness_counts.property_count_aligned &&
      readiness_counts.method_count_aligned &&
      readiness_counts.ivar_count_aligned &&
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
