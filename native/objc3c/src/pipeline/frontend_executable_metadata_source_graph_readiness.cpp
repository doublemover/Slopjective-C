#include "pipeline/frontend_executable_metadata_source_graph_readiness.h"

#include "pipeline/frontend_executable_metadata_source_graph_readiness_owners.h"

namespace objc3c::pipeline::orchestration {

void MarkExecutableMetadataSourceGraphReadiness(
    const Objc3Program &program,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records,
    Objc3ExecutableMetadataSourceGraph &graph) {
  const ExecutableMetadataSourceGraphReadinessCounts readiness_counts =
      BuildExecutableMetadataSourceGraphReadinessCounts(
          program, runtime_metadata_source_records, graph);

  InitializeExecutableMetadataSourceGraphReadinessPublication(graph);
  PublishExecutableMetadataClassMetaclassReadiness(graph);
  PublishExecutableMetadataProtocolCategoryReadiness(graph);
  PublishExecutableMetadataSourceGraphFinalReadiness(readiness_counts, graph);
}

}  // namespace objc3c::pipeline::orchestration
