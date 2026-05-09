#pragma once

#include "artifacts/objc3_frontend_runtime_registration_artifacts.h"
#include "ir/objc3_ir_frontend_metadata.h"

namespace objc3::artifacts::frontend {

void ApplyObjc3FrontendRuntimeSupportLibraryMetadata(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const Objc3RuntimeSupportLibraryContractSummary &runtime_support_library,
    const Objc3RuntimeSupportLibraryCoreFeatureSummary
        &runtime_support_library_core_feature,
    const Objc3RuntimeSupportLibraryLinkWiringSummary
        &runtime_support_library_link_wiring);

}  // namespace objc3::artifacts::frontend
