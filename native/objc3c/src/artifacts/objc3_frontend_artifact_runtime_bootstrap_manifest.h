#pragma once

#include <iosfwd>
#include <string>

#include "runtime/metadata/runtime_metadata_bootstrap.h"
#include "runtime/metadata/selector_metadata_bootstrap_legality_surfaces.h"
#include "runtime/metadata/selector_metadata_bootstrap_surfaces.h"
#include "runtime/metadata/selector_metadata_registration_descriptor_surfaces.h"
#include "runtime/metadata/selector_metadata_registration_manifest.h"

namespace objc3::artifacts::frontend {

void WriteRuntimeBootstrapRegistrationSourceSurface(
    std::ostream &manifest,
    const std::string &runtime_state_publication_emit_prefix,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
        &runtime_registration_descriptor_frontend_closure,
    const Objc3RuntimeRegistrationDescriptorImageRootSourceSurfaceSummary
        &runtime_registration_descriptor_image_root_source_surface,
    const Objc3RuntimeBootstrapLoweringSummary &runtime_bootstrap_lowering,
    const Objc3RuntimeBootstrapLegalitySemanticsSummary
        &runtime_bootstrap_legality_semantics);

void WriteRuntimeBootstrapLoweringRegistrationArtifactSurface(
    std::ostream &manifest,
    const std::string &runtime_state_publication_emit_prefix,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
        &runtime_registration_descriptor_frontend_closure,
    const Objc3RuntimeBootstrapLoweringSummary &runtime_bootstrap_lowering,
    const Objc3RuntimeBootstrapSemanticsSummary &runtime_bootstrap_semantics);

void WriteRuntimeMultiImageStartupOrderingSourceSurface(
    std::ostream &manifest,
    const std::string &runtime_state_publication_emit_prefix,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
        &runtime_registration_descriptor_frontend_closure,
    const Objc3RuntimeBootstrapLegalitySemanticsSummary
        &runtime_bootstrap_legality_semantics,
    const Objc3RuntimeBootstrapFailureRestartSemanticsSummary
        &runtime_bootstrap_failure_restart_semantics,
    const Objc3RuntimeBootstrapApiSummary &runtime_bootstrap_api,
    const Objc3RuntimeBootstrapSemanticsSummary &runtime_bootstrap_semantics);

}  // namespace objc3::artifacts::frontend
