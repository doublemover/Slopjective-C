#pragma once

#include <iosfwd>
#include <string>

struct Objc3RuntimeBootstrapSemanticsSummary;
struct Objc3RuntimeTranslationUnitRegistrationManifestSummary;

namespace objc3::artifacts::frontend {

void WriteRuntimeStatePublicationSurface(
    std::ostream &manifest,
    const std::string &runtime_state_publication_emit_prefix,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    const Objc3RuntimeBootstrapSemanticsSummary &runtime_bootstrap_semantics);

}  // namespace objc3::artifacts::frontend
