#include "libobjc3c_frontend/frontend_ir_object_status.h"

#include <cstdint>

#include "libobjc3c_frontend/objc3c_frontend_context_state.h"

namespace objc3c::frontend {

void SetFrontendEmitError(objc3c_frontend_context_t *context,
                          objc3c_frontend_compile_result_t *result,
                          int process_exit_code,
                          const std::string &message,
                          std::vector<std::string> &emit_diagnostics) {
  result->status = OBJC3C_FRONTEND_STATUS_EMIT_ERROR;
  result->process_exit_code = process_exit_code;
  result->success = 0;
  emit_diagnostics.push_back(message);
  SetFrontendContextError(context, message.c_str());
}

void SetFrontendEmitUsageError(objc3c_frontend_context_t *context,
                               objc3c_frontend_compile_result_t *result,
                               const std::string &message,
                               const std::string &diagnostic,
                               std::vector<std::string> &emit_diagnostics) {
  result->status = OBJC3C_FRONTEND_STATUS_USAGE_ERROR;
  result->process_exit_code = 2;
  result->success = 0;
  SetFrontendContextError(context, message.c_str());
  emit_diagnostics.push_back(diagnostic);
}

bool FrontendCompileIsOk(const objc3c_frontend_compile_result_t *result) {
  return result != nullptr && result->status == OBJC3C_FRONTEND_STATUS_OK;
}

bool WantsClangObjectBackend(const objc3c_frontend_compile_options_t &options) {
  return options.ir_object_backend ==
         static_cast<uint8_t>(OBJC3C_FRONTEND_IR_OBJECT_BACKEND_CLANG);
}

bool WantsLlvmDirectObjectBackend(
    const objc3c_frontend_compile_options_t &options) {
  return options.ir_object_backend ==
         static_cast<uint8_t>(OBJC3C_FRONTEND_IR_OBJECT_BACKEND_LLVM_DIRECT);
}

std::string BuildBackendFailureDiagnostic(bool wants_clang_backend,
                                          int compile_status,
                                          const std::string &backend_output_error,
                                          const std::string &backend_error) {
  if (!backend_output_error.empty()) {
    return "error:1:1: LLVM object emission failed: " + backend_output_error +
           " [O3E002]";
  }
  if (!backend_error.empty()) {
    return "error:1:1: LLVM object emission failed: " + backend_error +
           " [O3E002]";
  }
  if (wants_clang_backend) {
    return "error:1:1: LLVM object emission failed: clang exited with status " +
           std::to_string(compile_status) + " [O3E002]";
  }
  return "error:1:1: LLVM object emission failed: llc exited with status " +
         std::to_string(compile_status) + " [O3E002]";
}

}  // namespace objc3c::frontend
