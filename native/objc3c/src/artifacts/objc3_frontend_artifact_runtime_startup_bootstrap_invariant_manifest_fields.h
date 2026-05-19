#pragma once

#include <iosfwd>

#include "runtime/metadata/selector_metadata_bootstrap_surfaces.h"

namespace objc3::artifacts::frontend {

void WriteRuntimeStartupBootstrapInvariantManifestFields(
    std::ostream &manifest,
    const Objc3RuntimeStartupBootstrapInvariantSummary
        &runtime_startup_bootstrap_invariants);

}  // namespace objc3::artifacts::frontend
