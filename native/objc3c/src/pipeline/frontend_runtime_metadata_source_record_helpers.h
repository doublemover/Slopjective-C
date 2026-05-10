#pragma once

#include "pipeline/objc3_frontend_types.h"

namespace objc3c::pipeline::orchestration {

Objc3RuntimeMetadataSourceRecordSet BuildRuntimeMetadataSourceRecordSet(
    const Objc3Program &program);

}  // namespace objc3c::pipeline::orchestration
