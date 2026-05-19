#pragma once

#include <iosfwd>

struct Objc3RuntimeBootstrapApiSummary;
struct Objc3RuntimeBootstrapSemanticsSummary;

namespace objc3::artifacts::frontend {

void WriteRuntimeInstallationAbiSurface(
    std::ostream &manifest,
    const Objc3RuntimeBootstrapApiSummary &runtime_bootstrap_api);

void WriteRuntimeLoaderLifecycleSurface(
    std::ostream &manifest,
    const Objc3RuntimeBootstrapSemanticsSummary &runtime_bootstrap_semantics);

}  // namespace objc3::artifacts::frontend
