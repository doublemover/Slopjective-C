#pragma once

/*
 * Internal C++ owner for backend-adjacent runtime artifact publication after
 * object emission succeeds. This is not a runtime implementation surface.
 */
#include <filesystem>
#include <string>

#include "libobjc3c_frontend/objc3_cli_frontend.h"
#include "libobjc3c_frontend/objc3c_frontend_artifact_plan.h"

namespace objc3c::frontend {

bool PublishFrontendToolchainRuntimeArtifacts(
    const Objc3FrontendCompileProduct &product,
    const Objc3FrontendArtifactOutputPlan &artifact_plan,
    const std::filesystem::path &ir_out,
    const std::filesystem::path &object_out,
    const std::filesystem::path &backend_out,
    std::string &backend_output_error,
    std::string &backend_error);

}  // namespace objc3c::frontend
