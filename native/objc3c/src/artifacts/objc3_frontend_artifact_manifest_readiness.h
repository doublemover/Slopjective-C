#pragma once

#include <iosfwd>

struct Objc3FrontendArtifactBundle;

namespace objc3::artifacts::frontend {

void AppendObjc3FrontendArtifactManifestParseReadiness(
    std::ostream &manifest,
    const Objc3FrontendArtifactBundle &bundle);

}  // namespace objc3::artifacts::frontend
