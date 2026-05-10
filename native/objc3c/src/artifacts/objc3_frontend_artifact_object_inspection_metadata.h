#pragma once

#include "artifacts/objc3_frontend_artifact_metadata_dtos.h"

namespace objc3::artifacts::frontend {

void ApplyObjc3FrontendObjectInspectionMetadata(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const Objc3RuntimeMetadataObjectInspectionHarnessSummary
        &runtime_metadata_object_inspection);

}  // namespace objc3::artifacts::frontend
