#include "pipeline/frontend_executable_metadata_source_graph_readiness.h"

#include <algorithm>

#include "pipeline/frontend_metadata_handoff_ordering.h"

namespace objc3c::pipeline::orchestration {

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

}  // namespace objc3c::pipeline::orchestration
