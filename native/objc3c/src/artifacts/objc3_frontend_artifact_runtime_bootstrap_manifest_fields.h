#pragma once

#include <iosfwd>

struct Objc3RuntimeBootstrapApiSummary;
struct Objc3RuntimeBootstrapFailureRestartSemanticsSummary;
struct Objc3RuntimeBootstrapLoweringSummary;
struct Objc3RuntimeBootstrapSemanticsSummary;

namespace objc3::artifacts::frontend {

void WriteRuntimeBootstrapManifestFields(
    std::ostream &manifest,
    const Objc3RuntimeBootstrapFailureRestartSemanticsSummary
        &runtime_bootstrap_failure_restart_semantics,
    const Objc3RuntimeBootstrapApiSummary &runtime_bootstrap_api,
    const Objc3RuntimeBootstrapSemanticsSummary &runtime_bootstrap_semantics,
    const Objc3RuntimeBootstrapLoweringSummary &runtime_bootstrap_lowering);

}  // namespace objc3::artifacts::frontend
