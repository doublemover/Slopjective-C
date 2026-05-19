#pragma once

#include <iosfwd>

struct Objc3RuntimeBootstrapApiSummary;

namespace objc3::artifacts::frontend {

void WriteRuntimeCurrentReleaseEvidenceOwnerPayloadSurface(
    std::ostream &manifest,
    const Objc3RuntimeBootstrapApiSummary &runtime_bootstrap_api);

}  // namespace objc3::artifacts::frontend
