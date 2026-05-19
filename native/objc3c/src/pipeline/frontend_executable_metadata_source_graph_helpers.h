#pragma once

#include "pipeline/objc3_frontend_types.h"

namespace objc3c::pipeline::orchestration {

Objc3ExecutableMetadataSourceGraph BuildExecutableMetadataSourceGraph(
    const Objc3Program &program,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records);

}  // namespace objc3c::pipeline::orchestration
