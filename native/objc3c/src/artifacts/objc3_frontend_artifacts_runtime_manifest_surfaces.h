#pragma once

#include <iosfwd>

#include "artifacts/objc3_frontend_artifact_runtime_object_manifest_contracts.h"

struct Objc3DispatchSurfaceClassificationContract;
struct Objc3ExecutableMetadataSourceGraph;
struct Objc3FrontendOptions;
struct Objc3MessageSendSelectorLoweringContract;
struct Objc3PropertySynthesisIvarBindingContract;
struct Objc3RuntimeBootstrapApiSummary;
struct Objc3RuntimeBootstrapFailureRestartSemanticsSummary;
struct Objc3RuntimeBootstrapLegalitySemanticsSummary;
struct Objc3RuntimeBootstrapLoweringSummary;
struct Objc3RuntimeBootstrapSemanticsSummary;
struct Objc3RuntimeLinkHostLinkContract;
struct Objc3RuntimeMetadataSectionPublicationSummary;
struct Objc3RuntimeMetadataSourceRecordSet;
struct Objc3RuntimeRegistrationDescriptorFrontendClosureSummary;
struct Objc3RuntimeRegistrationDescriptorImageRootSourceSurfaceSummary;
struct Objc3RuntimeSupportLibraryLinkWiringSummary;
struct Objc3RuntimeTranslationUnitRegistrationManifestSummary;

namespace objc3::artifacts::frontend {

void WriteObjc3FrontendRuntimeManifestSurfaces(
    std::ostream &manifest,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records,
    const Objc3ExecutableMetadataSourceGraph &executable_metadata_source_graph,
    const Objc3RuntimeLinkHostLinkContract &runtime_link_host_link_contract,
    const Objc3FrontendOptions &options,
    const Objc3RuntimeSupportLibraryLinkWiringSummary
        &runtime_support_library_link_wiring,
    const Objc3DispatchSurfaceClassificationContract
        &dispatch_surface_classification_contract,
    const Objc3MessageSendSelectorLoweringContract
        &message_send_selector_lowering_contract,
    const Objc3RuntimeDispatchTableReflectionRecordLoweringFields
        &runtime_dispatch_table_reflection_record_lowering_fields,
    const Objc3PropertySynthesisIvarBindingContract
        &property_synthesis_ivar_binding_contract,
    const Objc3RuntimeMetadataSectionPublicationSummary
        &runtime_metadata_section_publication,
    const Objc3RuntimeBootstrapApiSummary &runtime_bootstrap_api,
    const Objc3RuntimeBootstrapSemanticsSummary &runtime_bootstrap_semantics,
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
        &runtime_registration_descriptor_frontend_closure,
    const Objc3RuntimeRegistrationDescriptorImageRootSourceSurfaceSummary
        &runtime_registration_descriptor_image_root_source_surface,
    const Objc3RuntimeBootstrapLoweringSummary &runtime_bootstrap_lowering,
    const Objc3RuntimeBootstrapLegalitySemanticsSummary
        &runtime_bootstrap_legality_semantics,
    const Objc3RuntimeBootstrapFailureRestartSemanticsSummary
        &runtime_bootstrap_failure_restart_semantics);

}  // namespace objc3::artifacts::frontend
