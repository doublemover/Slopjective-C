#pragma once

#include "pipeline/frontend_executable_metadata_source_graph_readiness.h"

namespace objc3c::pipeline::orchestration {

struct ExecutableMetadataSourceGraphReadinessCounts {
  bool interface_count_aligned = false;
  bool implementation_count_aligned = false;
  bool metaclass_count_aligned = false;
  bool class_node_floor_satisfied = false;
  bool protocol_count_aligned = false;
  bool category_count_aligned = false;
  bool property_count_aligned = false;
  bool method_count_aligned = false;
  bool ivar_count_aligned = false;
};

ExecutableMetadataSourceGraphReadinessCounts
BuildExecutableMetadataSourceGraphReadinessCounts(
    const Objc3Program &program,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records,
    const Objc3ExecutableMetadataSourceGraph &graph);

void InitializeExecutableMetadataSourceGraphReadinessPublication(
    Objc3ExecutableMetadataSourceGraph &graph);

void PublishExecutableMetadataClassMetaclassReadiness(
    Objc3ExecutableMetadataSourceGraph &graph);

void PublishExecutableMetadataProtocolCategoryReadiness(
    Objc3ExecutableMetadataSourceGraph &graph);

void PublishExecutableMetadataSourceGraphFinalReadiness(
    const ExecutableMetadataSourceGraphReadinessCounts &readiness_counts,
    Objc3ExecutableMetadataSourceGraph &graph);

}  // namespace objc3c::pipeline::orchestration
