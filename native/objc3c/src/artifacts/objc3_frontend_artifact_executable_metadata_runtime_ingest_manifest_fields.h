#pragma once

#include <iosfwd>

#include "artifacts/objc3_frontend_artifact_runtime_metadata_plan.h"

namespace objc3::artifacts::frontend {

void AppendObjc3FrontendArtifactExecutableMetadataRuntimeIngestManifestFields(
    std::ostream &manifest,
    const Objc3FrontendArtifactRuntimeMetadataPlan &runtime_metadata_plan);

}  // namespace objc3::artifacts::frontend
