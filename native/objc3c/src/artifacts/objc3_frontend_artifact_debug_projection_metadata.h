#pragma once

#include "artifacts/objc3_frontend_runtime_metadata_section_artifacts.h"
#include "ir/objc3_ir_frontend_metadata.h"

namespace objc3::artifacts::frontend {

void ApplyObjc3FrontendDebugProjectionMetadata(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const Objc3ExecutableMetadataDebugProjectionSummary
        &executable_metadata_debug_projection);

}  // namespace objc3::artifacts::frontend
