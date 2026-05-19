#pragma once

/*
 * Internal C++ owner for compile-result initialization, lowering contract
 * normalization, pipeline invocation, and baseline artifact publication.
 */
#include <filesystem>
#include <string>
#include <vector>

#include "libobjc3c_frontend/objc3_cli_frontend.h"
#include "libobjc3c_frontend/objc3c_frontend_artifact_plan.h"
#include "libobjc3c_frontend/objc3c_frontend_context.h"
#include "libobjc3c_frontend/objc3c_frontend_options.h"
#include "libobjc3c_frontend/objc3c_frontend_result.h"

namespace objc3c::frontend {

struct Objc3FrontendCompileRun {
  Objc3FrontendCompileProduct product;
  Objc3FrontendArtifactOutputPlan artifact_plan;
  bool sema_attempted = false;
  bool lower_attempted = false;
  std::vector<std::string> emit_diagnostics;
};

bool PrepareFrontendCompileRun(
    objc3c_frontend_context_t *context,
    objc3c_frontend_compile_result_t *result,
    const std::filesystem::path &input_path,
    const std::string &source_text,
    const objc3c_frontend_compile_options_t &options,
    Objc3FrontendCompileRun &run);

}  // namespace objc3c::frontend
