#include "libobjc3c_frontend/objc3c_frontend_artifact_publication.h"

#include "libobjc3c_frontend/objc3c_frontend_context_state.h"

namespace objc3c::frontend {

void SetFrontendPublicationError(objc3c_frontend_context_t *context,
                                 objc3c_frontend_compile_result_t *result,
                                 const std::string &message) {
  result->status = OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR;
  result->process_exit_code = 2;
  result->success = 0;
  SetFrontendContextError(context, message.c_str());
}

void SetFrontendDiagnosticOrOkStatus(
    objc3c_frontend_compile_result_t *result,
    const Objc3FrontendCompileProduct &product) {
  if (!product.artifact_bundle.diagnostics.empty()) {
    result->status = OBJC3C_FRONTEND_STATUS_DIAGNOSTICS;
    result->process_exit_code = 1;
    result->success = 0;
    return;
  }
  result->status = OBJC3C_FRONTEND_STATUS_OK;
  result->process_exit_code = 0;
  result->success = 1;
}

}  // namespace objc3c::frontend
