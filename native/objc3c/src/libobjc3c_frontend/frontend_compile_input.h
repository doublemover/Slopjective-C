#pragma once

/*
 * Internal C++ owner for public compile entrypoint argument checks and source
 * materialization. The exported C entrypoints keep only ABI dispatch.
 */
#include <filesystem>
#include <string>

#include "libobjc3c_frontend/objc3c_frontend_context.h"
#include "libobjc3c_frontend/objc3c_frontend_options.h"
#include "libobjc3c_frontend/objc3c_frontend_result.h"

namespace objc3c::frontend {

enum class FrontendCompileEntrypointKind {
  kFile,
  kSource,
};

struct FrontendCompileInput {
  std::filesystem::path input_path;
  std::string source_text;
};

objc3c_frontend_status_t ValidateFrontendCompileEntrypointArguments(
    FrontendCompileEntrypointKind kind,
    objc3c_frontend_context_t *context,
    const objc3c_frontend_compile_options_t *options,
    objc3c_frontend_compile_result_t *result,
    bool &accepted);

objc3c_frontend_status_t ValidateFrontendCompileEntrypointOptions(
    FrontendCompileEntrypointKind kind,
    objc3c_frontend_context_t *context,
    const objc3c_frontend_compile_options_t &options,
    objc3c_frontend_compile_result_t *result,
    bool &accepted);

bool LoadFrontendCompileFileInput(
    objc3c_frontend_context_t *context,
    const objc3c_frontend_compile_options_t &options,
    objc3c_frontend_compile_result_t *result,
    FrontendCompileInput &input);

FrontendCompileInput BuildFrontendCompileSourceInput(
    const objc3c_frontend_compile_options_t &options);

}  // namespace objc3c::frontend
