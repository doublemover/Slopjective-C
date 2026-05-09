#include "libobjc3c_frontend/frontend_ir_artifact_publication.h"

#include "libobjc3c_frontend/frontend_ir_object_status.h"
#include "libobjc3c_frontend/objc3c_frontend_context_state.h"
#include "libobjc3c_frontend/objc3c_frontend_file_output.h"

namespace objc3c::frontend {

bool PublishFrontendIrArtifact(
    objc3c_frontend_context_t *context,
    objc3c_frontend_compile_result_t *result,
    const Objc3FrontendCompileProduct &product,
    const Objc3FrontendArtifactOutputPlan &artifact_plan,
    std::filesystem::path &ir_out) {
  if (!FrontendCompileIsOk(result) || !artifact_plan.wants_ir_file) {
    return true;
  }
  if (!artifact_plan.has_out_dir) {
    result->status = OBJC3C_FRONTEND_STATUS_USAGE_ERROR;
    result->process_exit_code = 2;
    result->success = 0;
    SetFrontendContextError(
        context, "emit_ir/emit_object require out_dir in compile options.");
    return false;
  }

  ir_out = BuildFrontendIrOutputPath(artifact_plan);
  std::string io_error;
  if (!WriteFrontendTextFile(ir_out, product.artifact_bundle.ir_text,
                             io_error)) {
    result->status = OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR;
    result->process_exit_code = 2;
    result->success = 0;
    SetFrontendContextError(context, io_error.c_str());
    return false;
  }

  SetFrontendContextIrPath(context, ir_out.generic_string());
  return true;
}

}  // namespace objc3c::frontend
