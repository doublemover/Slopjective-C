#include "pipeline/frontend_executable_metadata_source_graph_helpers_owners.h"

namespace objc3c::pipeline::orchestration {

void PopulateExecutableMetadataSourceGraphTopology(
    const Objc3Program &program,
    Objc3ExecutableMetadataSourceGraph &graph) {
  ExecutableMetadataSourceGraphTopologyContext context;
  context.aggregated_classes.reserve(program.interfaces.size() +
                                     program.implementations.size());
  context.aggregated_categories.reserve(program.interfaces.size() +
                                        program.implementations.size());

  BuildExecutableMetadataPropertySynthesisIndex(
      program, context.property_synthesis_index);
  PopulateExecutableMetadataInterfaceTopology(program, graph, context);
  PopulateExecutableMetadataImplementationTopology(program, graph, context);
  PopulateExecutableMetadataProtocolTopology(program, graph, context);
  FinalizeExecutableMetadataClassTopology(graph, context);
  FinalizeExecutableMetadataCategoryTopology(graph, context);
  LinkExecutableMetadataPropertyAccessorEdges(graph, context.method_edge_records);
  LinkExecutableMetadataMethodOverrideEdges(graph);
}

}  // namespace objc3c::pipeline::orchestration
