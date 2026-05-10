#pragma once

#include <iosfwd>
#include <string>

#include "artifacts/objc3_frontend_artifact_runtime_object_manifest_contracts.h"

struct Objc3RuntimeMetadataSectionPublicationSummary;
struct Objc3RuntimeRegistrationDescriptorFrontendClosureSummary;
struct Objc3RuntimeTranslationUnitRegistrationManifestSummary;

namespace objc3::artifacts::frontend {

void WriteRuntimeRealizationLoweringReflectionArtifactSurface(
    std::ostream &manifest,
    const std::string &runtime_state_publication_emit_prefix,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
        &runtime_registration_descriptor_frontend_closure);

void WriteRuntimeDispatchTableReflectionRecordLoweringSurface(
    std::ostream &manifest,
    const std::string &runtime_state_publication_emit_prefix,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
        &runtime_registration_descriptor_frontend_closure,
    const Objc3RuntimeMetadataSectionPublicationSummary
        &runtime_metadata_section_publication,
    const Objc3RuntimeDispatchTableReflectionRecordLoweringFields
        &runtime_dispatch_table_reflection_record_lowering_fields);

}  // namespace objc3::artifacts::frontend
