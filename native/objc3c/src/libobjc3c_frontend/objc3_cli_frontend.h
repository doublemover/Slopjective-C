#pragma once

#include <filesystem>
#include <string>

#include "pipeline/objc3_frontend_artifacts.h"
#include "pipeline/objc3_frontend_pipeline.h"

struct Objc3FrontendCompileProduct {
  Objc3FrontendPipelineResult pipeline_result;
  Objc3FrontendArtifactBundle artifact_bundle;
};

Objc3FrontendCompileProduct CompileObjc3SourceWithPipeline(const std::filesystem::path &input_path,
                                                           const std::string &source,
                                                           const Objc3FrontendOptions &options);

Objc3FrontendArtifactBundle CompileObjc3SourceForCli(const std::filesystem::path &input_path,
                                                     const std::string &source,
                                                     const Objc3FrontendOptions &options);
