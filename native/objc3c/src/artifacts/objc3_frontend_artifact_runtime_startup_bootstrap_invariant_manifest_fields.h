#pragma once

#include <iosfwd>

struct Objc3RuntimeStartupBootstrapInvariantSummary;

namespace objc3::artifacts::frontend {

void WriteRuntimeStartupBootstrapInvariantManifestFields(
    std::ostream &manifest,
    const Objc3RuntimeStartupBootstrapInvariantSummary
        &runtime_startup_bootstrap_invariants);

}  // namespace objc3::artifacts::frontend
