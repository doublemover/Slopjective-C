#pragma once

#include <iosfwd>

#include "runtime/metadata/runtime_metadata_bootstrap.h"
#include "runtime/metadata/selector_metadata_bootstrap_legality_surfaces.h"
#include "runtime/metadata/selector_metadata_bootstrap_surfaces.h"

namespace objc3::artifacts::frontend {

void WriteRuntimeBootstrapManifestFields(
    std::ostream &manifest,
    const Objc3RuntimeBootstrapFailureRestartSemanticsSummary
        &runtime_bootstrap_failure_restart_semantics,
    const Objc3RuntimeBootstrapApiSummary &runtime_bootstrap_api,
    const Objc3RuntimeBootstrapSemanticsSummary &runtime_bootstrap_semantics,
    const Objc3RuntimeBootstrapLoweringSummary &runtime_bootstrap_lowering);

}  // namespace objc3::artifacts::frontend
