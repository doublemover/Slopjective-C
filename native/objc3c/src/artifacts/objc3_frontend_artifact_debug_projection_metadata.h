#pragma once

#include "artifacts/objc3_frontend_artifact_metadata_dtos.h"

namespace objc3::artifacts::frontend {

void ApplyObjc3FrontendDebugProjectionMetadata(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const Objc3ExecutableMetadataDebugProjectionSummary
        &executable_metadata_debug_projection);

}  // namespace objc3::artifacts::frontend
