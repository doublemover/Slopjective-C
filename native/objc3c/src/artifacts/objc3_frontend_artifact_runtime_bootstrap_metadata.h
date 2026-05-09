#pragma once

#include "artifacts/objc3_frontend_runtime_bootstrap_artifacts.h"
#include "artifacts/objc3_frontend_runtime_descriptor_artifacts.h"
#include "artifacts/objc3_frontend_runtime_registration_artifacts.h"
#include "ir/objc3_ir_frontend_metadata.h"

namespace objc3::artifacts::frontend {

void ApplyObjc3FrontendRuntimeBootstrapMetadata(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const Objc3RuntimeBootstrapLoweringSummary &runtime_bootstrap_lowering,
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
        &runtime_registration_descriptor_frontend_closure,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest);

}  // namespace objc3::artifacts::frontend
