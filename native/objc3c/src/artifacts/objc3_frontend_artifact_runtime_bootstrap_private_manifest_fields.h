#pragma once

#include <iosfwd>
#include <string>

#include "runtime/metadata/runtime_metadata_bootstrap.h"
#include "runtime/metadata/selector_metadata_bootstrap_legality_surfaces.h"
#include "runtime/metadata/selector_metadata_bootstrap_surfaces.h"
#include "runtime/metadata/selector_metadata_registration_descriptor_surfaces.h"

namespace objc3::artifacts::frontend {

void WriteRuntimeBootstrapPrivateManifestFields(
    std::ostream &manifest,
    const Objc3RuntimeBootstrapApiSummary &runtime_bootstrap_api,
    const Objc3RuntimeBootstrapLoweringSummary &runtime_bootstrap_lowering,
    const Objc3RuntimeBootstrapFailureRestartSemanticsSummary
        &runtime_bootstrap_failure_restart_semantics,
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
        &runtime_registration_descriptor_frontend_closure,
    const std::string &translation_unit_identity_key);

}  // namespace objc3::artifacts::frontend
