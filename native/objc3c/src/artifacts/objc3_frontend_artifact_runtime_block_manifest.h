#pragma once

#include "artifacts/objc3_frontend_artifact_runtime_block_abi_manifest.h"
#include "artifacts/objc3_frontend_artifact_runtime_block_ownership_manifest.h"

#include <iosfwd>
#include <string>

struct Objc3RuntimeRegistrationDescriptorFrontendClosureSummary;
struct Objc3RuntimeTranslationUnitRegistrationManifestSummary;
namespace objc3::artifacts::frontend {

void WriteRuntimeBlockArcUnifiedSourceSurface(
    std::ostream &manifest,
    const std::string &runtime_state_publication_emit_prefix,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
        &runtime_registration_descriptor_frontend_closure);

void WriteRuntimeBlockArcLoweringHelperSurface(
    std::ostream &manifest,
    const std::string &runtime_state_publication_emit_prefix,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
        &runtime_registration_descriptor_frontend_closure);

}  // namespace objc3::artifacts::frontend
