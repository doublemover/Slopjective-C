#pragma once

#include <iosfwd>
#include <string>

struct Objc3RuntimeMetadataSectionPublicationSummary;
struct Objc3RuntimeTranslationUnitRegistrationManifestSummary;

namespace objc3::artifacts::frontend {

struct Objc3ExecutableAccessorLayoutLoweringSummary;

void WriteExecutableSynthesizedAccessorPropertyLoweringSurface(
    std::ostream &manifest,
    const std::string &runtime_state_publication_emit_prefix,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    const Objc3ExecutableAccessorLayoutLoweringSummary
        &executable_accessor_layout_lowering_summary,
    const Objc3RuntimeMetadataSectionPublicationSummary
        &runtime_metadata_section_publication);

}  // namespace objc3::artifacts::frontend
