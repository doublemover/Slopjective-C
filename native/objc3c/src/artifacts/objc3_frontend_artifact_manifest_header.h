#pragma once

#include <filesystem>
#include <iosfwd>

#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend {

void AppendObjc3FrontendArtifactManifestHeader(
    std::ostream &manifest,
    const std::filesystem::path &input_path,
    const Objc3Program &program,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options);

}  // namespace objc3::artifacts::frontend
