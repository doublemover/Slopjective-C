#include "pipeline/frontend_executable_metadata_source_graph_helpers.h"

#include "pipeline/frontend_executable_metadata_source_graph_helpers_owners.h"

namespace objc3c::pipeline::orchestration {

Objc3ExecutableMetadataSourceGraph BuildExecutableMetadataSourceGraph(
    const Objc3Program &program,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records) {
  Objc3ExecutableMetadataSourceGraph graph;
  PopulateExecutableMetadataSourceGraphTopology(program, graph);
  FinalizeExecutableMetadataSourceGraphReadiness(
      program, runtime_metadata_source_records, graph);
  return graph;
}

}  // namespace objc3c::pipeline::orchestration
