#pragma once

#include <iosfwd>

struct Objc3FrontendArtifactRuntimeMetadataPlan;

namespace objc3::artifacts::frontend {

void AppendObjc3FrontendArtifactExecutableMetadataRuntimeIngestManifestFields(
    std::ostream &manifest,
    const Objc3FrontendArtifactRuntimeMetadataPlan &runtime_metadata_plan);

}  // namespace objc3::artifacts::frontend
