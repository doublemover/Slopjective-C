#pragma once

#include "pipeline/objc3_frontend_types.h"

namespace objc3c::pipeline::orchestration {

void PopulateExecutableMetadataSourceGraphTopology(
    const Objc3Program &program,
    Objc3ExecutableMetadataSourceGraph &graph);

void FinalizeExecutableMetadataSourceGraphReadiness(
    const Objc3Program &program,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records,
    Objc3ExecutableMetadataSourceGraph &graph);

}  // namespace objc3c::pipeline::orchestration
