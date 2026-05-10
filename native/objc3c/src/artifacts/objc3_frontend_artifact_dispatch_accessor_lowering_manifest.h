#pragma once

#include <iosfwd>
#include <string>

#include "artifacts/objc3_frontend_artifact_dispatch_accessor_manifest_contracts.h"

struct Objc3RuntimeMetadataSectionPublicationSummary;
struct Objc3RuntimeTranslationUnitRegistrationManifestSummary;

namespace objc3::artifacts::frontend {

struct Objc3AccessorStorageLoweringMetadataSummary;

void WriteDispatchAndSynthesizedAccessorLoweringSurface(
    std::ostream &manifest,
    const std::string &runtime_state_publication_emit_prefix,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    const Objc3DispatchAndSynthesizedAccessorLoweringFields
        &dispatch_and_synthesized_accessor_lowering_fields,
    const Objc3AccessorStorageLoweringMetadataSummary
        &accessor_storage_lowering_metadata_summary,
    const Objc3RuntimeMetadataSectionPublicationSummary
        &runtime_metadata_section_publication);

}  // namespace objc3::artifacts::frontend
