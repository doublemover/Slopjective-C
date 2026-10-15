#pragma once

#include <iosfwd>
#include <string>

struct Objc3RuntimeBootstrapSemanticsSummary;
struct Objc3RuntimeTranslationUnitRegistrationManifestSummary;

namespace objc3::artifacts::frontend {

struct RuntimeStatePublicationPaths;

void WriteRuntimeStatePublicationSurface(
    std::ostream &manifest,
    const RuntimeStatePublicationPaths &runtime_state_publication_paths,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    const Objc3RuntimeBootstrapSemanticsSummary &runtime_bootstrap_semantics);

}  // namespace objc3::artifacts::frontend
