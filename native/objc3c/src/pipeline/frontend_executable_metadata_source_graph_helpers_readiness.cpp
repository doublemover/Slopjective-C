#include "pipeline/frontend_executable_metadata_source_graph_helpers_owners.h"

#include "pipeline/frontend_executable_metadata_source_graph_readiness.h"

namespace objc3c::pipeline::orchestration {

void FinalizeExecutableMetadataSourceGraphReadiness(
    const Objc3Program &program,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records,
  Objc3ExecutableMetadataSourceGraph &graph) {
  SortExecutableMetadataSourceGraph(graph);
  MarkExecutableMetadataSourceGraphReadiness(
      program, runtime_metadata_source_records, graph);
}

}  // namespace objc3c::pipeline::orchestration
