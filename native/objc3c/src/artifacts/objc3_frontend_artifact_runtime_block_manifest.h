#pragma once

#include <iosfwd>
#include <string>

struct Objc3RuntimeRegistrationDescriptorFrontendClosureSummary;
struct Objc3RuntimeTranslationUnitRegistrationManifestSummary;
struct Objc3RuntimeBootstrapApiSummary;

namespace objc3::artifacts::frontend {

void WriteRuntimeBlockArcUnifiedSourceSurface(
    std::ostream &manifest,
    const std::string &runtime_state_publication_emit_prefix,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
        &runtime_registration_descriptor_frontend_closure);

void WriteRuntimeOwnershipTransferCaptureFamilySourceSurface(
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

void WriteRuntimeBlockArcRuntimeAbiSurface(
    std::ostream &manifest,
    const Objc3RuntimeBootstrapApiSummary &runtime_bootstrap_api);

}  // namespace objc3::artifacts::frontend
