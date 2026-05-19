#include "libobjc3c_frontend/objc3c_frontend.h"

#include <mutex>

#include "libobjc3c_frontend/frontend_compile_input.h"
#include "libobjc3c_frontend/frontend_compile_pipeline.h"
#include "libobjc3c_frontend/frontend_conformance_artifacts.h"
#include "libobjc3c_frontend/frontend_ir_object_emission.h"
#include "libobjc3c_frontend/frontend_stage_finalization.h"
#include "libobjc3c_frontend/objc3c_frontend_context_state.h"

namespace {

objc3c_frontend_status_t CompileObjc3FrontendInput(
    objc3c_frontend_context_t *context,
    const objc3c::frontend::FrontendCompileInput &input,
    const objc3c_frontend_compile_options_t &options,
    objc3c_frontend_compile_result_t *result) {
  objc3c::frontend::Objc3FrontendCompileRun run;
  if (!objc3c::frontend::PrepareFrontendCompileRun(
          context, result, input.input_path, input.source_text, options, run)) {
    return objc3c::frontend::ReturnWithFrontendContextResultPayload(context,
                                                                    result);
  }

  objc3c::frontend::PublishFrontendConformanceReportArtifacts(
      context, result, run.product, run.artifact_plan);
  objc3c::frontend::PublishFrontendIrAndObjectArtifacts(
      context, result, run.product, run.artifact_plan, options,
      run.emit_diagnostics);

  return objc3c::frontend::FinalizeFrontendCompileResult(
      context, result, run.product, run.artifact_plan, run.sema_attempted,
      run.lower_attempted, run.emit_diagnostics);
}

objc3c_frontend_status_t CompileFrontendFileInput(
    objc3c_frontend_context_t *context,
    const objc3c_frontend_compile_options_t &options,
    objc3c_frontend_compile_result_t *result) {
  objc3c::frontend::FrontendCompileInput input;
  if (!objc3c::frontend::LoadFrontendCompileFileInput(context, options, result,
                                                      input)) {
    return result->status;
  }
  return CompileObjc3FrontendInput(context, input, options, result);
}

objc3c_frontend_status_t CompileFrontendSourceInput(
    objc3c_frontend_context_t *context,
    const objc3c_frontend_compile_options_t &options,
    objc3c_frontend_compile_result_t *result) {
  const objc3c::frontend::FrontendCompileInput input =
      objc3c::frontend::BuildFrontendCompileSourceInput(options);
  return CompileObjc3FrontendInput(context, input, options, result);
}

objc3c_frontend_status_t CompileFrontendEntrypoint(
    objc3c::frontend::FrontendCompileEntrypointKind kind,
    objc3c_frontend_context_t *context,
    const objc3c_frontend_compile_options_t *options,
    objc3c_frontend_compile_result_t *result) {
  bool accepted = false;
  objc3c_frontend_status_t status =
      objc3c::frontend::ValidateFrontendCompileEntrypointArguments(
          kind, context, options, result, accepted);
  if (!accepted) {
    return status;
  }

  std::lock_guard<std::mutex> lock(
      objc3c::frontend::FrontendContextMutex(context));
  status = objc3c::frontend::ValidateFrontendCompileEntrypointOptions(
      kind, context, *options, result, accepted);
  if (!accepted) {
    return status;
  }

  switch (kind) {
    case objc3c::frontend::FrontendCompileEntrypointKind::kFile:
      return CompileFrontendFileInput(context, *options, result);
    case objc3c::frontend::FrontendCompileEntrypointKind::kSource:
      return CompileFrontendSourceInput(context, *options, result);
  }

  return objc3c::frontend::SetFrontendUsageError(
      context, result, "unknown frontend compile entrypoint.");
}

}  // namespace

extern "C" OBJC3C_FRONTEND_API objc3c_frontend_status_t
objc3c_frontend_compile_file(
    objc3c_frontend_context_t *context,
    const objc3c_frontend_compile_options_t *options,
    objc3c_frontend_compile_result_t *result) {
  return CompileFrontendEntrypoint(
      objc3c::frontend::FrontendCompileEntrypointKind::kFile, context, options,
      result);
}

extern "C" OBJC3C_FRONTEND_API objc3c_frontend_status_t
objc3c_frontend_compile_source(
    objc3c_frontend_context_t *context,
    const objc3c_frontend_compile_options_t *options,
    objc3c_frontend_compile_result_t *result) {
  return CompileFrontendEntrypoint(
      objc3c::frontend::FrontendCompileEntrypointKind::kSource, context,
      options, result);
}
