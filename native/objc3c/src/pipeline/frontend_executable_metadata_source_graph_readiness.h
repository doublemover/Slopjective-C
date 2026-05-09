#pragma once

#include <string>

#include "pipeline/objc3_frontend_types.h"

namespace objc3c::pipeline::orchestration {

bool HasExecutableMetadataGraphEdge(
    const Objc3ExecutableMetadataSourceGraph &graph,
    const std::string &edge_kind,
    const std::string &source_owner_identity,
    const std::string &target_owner_identity);

void SortExecutableMetadataSourceGraph(
    Objc3ExecutableMetadataSourceGraph &graph);

bool IsExecutableMetadataSourceGraphDeterministic(
    const Objc3ExecutableMetadataSourceGraph &graph);

void MarkExecutableMetadataSourceGraphReadiness(
    const Objc3Program &program,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records,
    Objc3ExecutableMetadataSourceGraph &graph);

}  // namespace objc3c::pipeline::orchestration
