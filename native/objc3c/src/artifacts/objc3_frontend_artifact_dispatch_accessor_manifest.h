#pragma once

#include "artifacts/objc3_frontend_artifact_dispatch_accessor_abi_manifest.h"

#include <iosfwd>
#include <string>

struct Objc3DispatchSurfaceClassificationContract;
struct Objc3FrontendOptions;
struct Objc3MessageSendSelectorLoweringContract;
struct Objc3PropertySynthesisIvarBindingContract;
struct Objc3RuntimeLinkHostLinkContract;
struct Objc3RuntimeMetadataSectionPublicationSummary;
struct Objc3RuntimeSupportLibraryLinkWiringSummary;
struct Objc3RuntimeTranslationUnitRegistrationManifestSummary;

namespace objc3::artifacts::frontend {

struct Objc3AccessorStorageLoweringMetadataSummary;

void WriteDispatchAndSynthesizedAccessorLoweringSurface(
    std::ostream &manifest,
    const std::string &runtime_state_publication_emit_prefix,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    const Objc3RuntimeLinkHostLinkContract &runtime_link_host_link_contract,
    const Objc3FrontendOptions &options,
    const Objc3RuntimeSupportLibraryLinkWiringSummary
        &runtime_support_library_link_wiring,
    const Objc3DispatchSurfaceClassificationContract
        &dispatch_surface_classification_contract,
    const Objc3MessageSendSelectorLoweringContract
        &message_send_selector_lowering_contract,
    const Objc3PropertySynthesisIvarBindingContract
        &property_synthesis_ivar_binding_contract,
    const Objc3AccessorStorageLoweringMetadataSummary
        &accessor_storage_lowering_metadata_summary,
    const Objc3RuntimeMetadataSectionPublicationSummary
        &runtime_metadata_section_publication);

}  // namespace objc3::artifacts::frontend
