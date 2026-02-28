#pragma once

#include <filesystem>
#include <string>

#include "pipeline/objc3_frontend_artifacts.h"

Objc3FrontendArtifactBundle CompileObjc3SourceForCli(const std::filesystem::path &input_path,
                                                     const std::string &source,
                                                     const Objc3FrontendOptions &options);
