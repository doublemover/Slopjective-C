#pragma once

#include <iosfwd>

#include "artifacts/objc3_frontend_artifact_dispatch_accessor_manifest_contracts.h"
#include "artifacts/objc3_frontend_artifact_runtime_object_manifest_contracts.h"
#include "artifacts/objc3_frontend_artifact_storage_accessor_manifest_contracts.h"

struct Objc3ExecutableMetadataSourceGraph;
struct Objc3RuntimeBootstrapApiSummary;
struct Objc3RuntimeBootstrapFailureRestartSemanticsSummary;
struct Objc3RuntimeBootstrapLegalitySemanticsSummary;
struct Objc3RuntimeBootstrapLoweringSummary;
struct Objc3RuntimeBootstrapSemanticsSummary;
struct Objc3RuntimeMetadataSectionPublicationSummary;
struct Objc3RuntimeMetadataSourceRecordSet;
struct Objc3RuntimeRegistrationDescriptorFrontendClosureSummary;
struct Objc3RuntimeRegistrationDescriptorImageRootSourceSurfaceSummary;
struct Objc3RuntimeTranslationUnitRegistrationManifestSummary;

namespace objc3::artifacts::frontend {

void WriteObjc3FrontendRuntimeManifestSurfaces(
    std::ostream &manifest,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records,
    const Objc3ExecutableMetadataSourceGraph &executable_metadata_source_graph,
    const Objc3DispatchAndSynthesizedAccessorLoweringFields
        &dispatch_and_synthesized_accessor_lowering_fields,
    const Objc3DispatchAccessorRuntimeAbiFields
        &dispatch_accessor_runtime_abi_fields,
    const Objc3RuntimeDispatchTableReflectionRecordLoweringFields
        &runtime_dispatch_table_reflection_record_lowering_fields,
    const Objc3StorageAccessorRuntimeAbiFields
        &storage_accessor_runtime_abi_fields,
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
