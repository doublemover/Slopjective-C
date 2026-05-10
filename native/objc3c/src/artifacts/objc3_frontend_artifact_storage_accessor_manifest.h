#pragma once

#include <iosfwd>
#include <string>

struct Objc3PropertySynthesisIvarBindingContract;
struct Objc3RuntimeBootstrapApiSummary;
struct Objc3RuntimeLinkHostLinkContract;
struct Objc3RuntimeRegistrationDescriptorFrontendClosureSummary;
struct Objc3RuntimeTranslationUnitRegistrationManifestSummary;

namespace objc3::artifacts::frontend {

void WriteStorageAccessorRuntimeAbiSurface(
    std::ostream &manifest,
    const Objc3RuntimeBootstrapApiSummary &runtime_bootstrap_api,
    const Objc3RuntimeLinkHostLinkContract &runtime_link_host_link_contract,
    const Objc3PropertySynthesisIvarBindingContract
        &property_synthesis_ivar_binding_contract);

void WriteRuntimePropertyIvarStorageAccessorSourceSurface(
    std::ostream &manifest,
    const std::string &runtime_state_publication_emit_prefix,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
        &runtime_registration_descriptor_frontend_closure);

void WriteRuntimePropertyIvarAccessorReflectionImplementationSurface(
    std::ostream &manifest,
    const std::string &runtime_state_publication_emit_prefix,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
        &runtime_registration_descriptor_frontend_closure,
    const Objc3RuntimeBootstrapApiSummary &runtime_bootstrap_api);

}  // namespace objc3::artifacts::frontend
