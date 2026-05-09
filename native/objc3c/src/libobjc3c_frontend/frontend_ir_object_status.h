#pragma once

#include <string>
#include <vector>

#include "libobjc3c_frontend/objc3c_frontend_context.h"
#include "libobjc3c_frontend/objc3c_frontend_options.h"
#include "libobjc3c_frontend/objc3c_frontend_result.h"

namespace objc3c::frontend {

void SetFrontendEmitError(objc3c_frontend_context_t *context,
                          objc3c_frontend_compile_result_t *result,
                          int process_exit_code,
                          const std::string &message,
                          std::vector<std::string> &emit_diagnostics);

void SetFrontendEmitUsageError(objc3c_frontend_context_t *context,
                               objc3c_frontend_compile_result_t *result,
                               const std::string &message,
                               const std::string &diagnostic,
                               std::vector<std::string> &emit_diagnostics);

bool FrontendCompileIsOk(const objc3c_frontend_compile_result_t *result);

bool WantsClangObjectBackend(const objc3c_frontend_compile_options_t &options);

bool WantsLlvmDirectObjectBackend(
    const objc3c_frontend_compile_options_t &options);

std::string BuildBackendFailureDiagnostic(bool wants_clang_backend,
                                          int compile_status,
                                          const std::string &backend_output_error,
                                          const std::string &backend_error);

}  // namespace objc3c::frontend
