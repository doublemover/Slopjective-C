#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "pipeline/objc3_frontend_types.h"

struct Objc3FrontendArtifactBundle {
  std::vector<std::string> diagnostics;
  std::string manifest_json;
  std::string ir_text;
};

Objc3FrontendArtifactBundle BuildObjc3FrontendArtifacts(const std::filesystem::path &input_path,
                                                        const Objc3FrontendPipelineResult &pipeline_result,
                                                        const Objc3FrontendOptions &options);
