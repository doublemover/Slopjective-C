#include "libobjc3c_frontend/frontend_ir_object_emission.h"

#include "io/objc3_process.h"
#include "io/objc3_toolchain_runtime_ga_operations_core_feature_surface.h"
#include "io/objc3_toolchain_runtime_ga_operations_scaffold.h"
#include "libobjc3c_frontend/frontend_ir_artifact_publication.h"
#include "libobjc3c_frontend/frontend_ir_object_status.h"
#include "libobjc3c_frontend/frontend_toolchain_runtime_artifacts.h"
#include "libobjc3c_frontend/objc3c_frontend_compile_contract.h"
#include "libobjc3c_frontend/objc3c_frontend_context_state.h"
#include "libobjc3c_frontend/objc3c_frontend_file_output.h"

namespace objc3c::frontend {

namespace {

void PublishFrontendObjectArtifact(
    objc3c_frontend_context_t *context,
    objc3c_frontend_compile_result_t *result,
    const Objc3FrontendCompileProduct &product,
    const Objc3FrontendArtifactOutputPlan &artifact_plan,
    const objc3c_frontend_compile_options_t &options,
    const std::filesystem::path &ir_out,
    std::vector<std::string> &emit_diagnostics) {
  if (!FrontendCompileIsOk(result) || options.emit_object == 0) {
    return;
  }

  const bool wants_clang_backend = WantsClangObjectBackend(options);
  const bool wants_llvm_direct_backend = WantsLlvmDirectObjectBackend(options);
  if (!wants_clang_backend && !wants_llvm_direct_backend) {
    SetFrontendEmitUsageError(
        context, result,
        "emit_object requires a valid ir_object_backend (clang|llvm-direct).",
        "error:1:1: emit_object requires valid ir_object_backend (clang|llvm-direct) [O3E001]",
        emit_diagnostics);
    return;
  }
  if (wants_clang_backend && IsMissingFrontendBorrowedPath(options.clang_path)) {
    SetFrontendEmitUsageError(
        context, result, "emit_object requires clang_path in compile options.",
        "error:1:1: emit_object requires clang_path in compile options [O3E001]",
        emit_diagnostics);
    return;
  }
  if (wants_llvm_direct_backend &&
      IsMissingFrontendBorrowedPath(options.llc_path)) {
    SetFrontendEmitUsageError(
        context, result,
        "emit_object requires llc_path in compile options for llvm-direct backend.",
        "error:1:1: emit_object requires llc_path in compile options for llvm-direct backend [O3E001]",
        emit_diagnostics);
    return;
  }

  const std::filesystem::path object_out =
      BuildFrontendObjectOutputPath(artifact_plan);
  const std::filesystem::path backend_out =
      BuildFrontendObjectBackendOutputPath(artifact_plan);
  const std::string backend_text =
      wants_clang_backend ? "clang\n" : "llvm-direct\n";
  int compile_status = 0;
#if defined(OBJC3C_ENABLE_LLVM_DIRECT_OBJECT_EMISSION)
  const bool llvm_direct_backend_enabled = true;
#else
  const bool llvm_direct_backend_enabled = false;
#endif
  const std::filesystem::path clang_path =
      OptionalBorrowedFrontendFilesystemPath(options.clang_path);
  const std::filesystem::path llc_path =
      OptionalBorrowedFrontendFilesystemPath(options.llc_path);
  const Objc3ToolchainRuntimeGaOperationsScaffold
      toolchain_runtime_ga_operations_scaffold =
          BuildObjc3ToolchainRuntimeGaOperationsScaffold(
              wants_clang_backend, wants_llvm_direct_backend, clang_path,
              llc_path, llvm_direct_backend_enabled, ir_out, object_out);
  std::string toolchain_runtime_scaffold_reason;
  if (!IsObjc3ToolchainRuntimeGaOperationsScaffoldReady(
          toolchain_runtime_ga_operations_scaffold,
          toolchain_runtime_scaffold_reason)) {
    SetFrontendEmitError(
        context, result, 125,
        "error:1:1: LLVM object emission failed: toolchain/runtime readiness contract fail-closed: " +
            toolchain_runtime_scaffold_reason + " [O3E002]",
        emit_diagnostics);
    return;
  }

  bool backend_output_recorded = false;
  std::string backend_output_payload;
  std::string backend_output_error;
  std::string backend_error;
  if (wants_clang_backend) {
    compile_status = RunIRCompile(clang_path, ir_out, object_out);
  } else {
    compile_status =
        RunIRCompileLLVMDirect(llc_path, ir_out, object_out, backend_error);
  }

  if (compile_status == 0) {
    if (!WriteFrontendTextFile(backend_out, backend_text,
                               backend_output_error)) {
      compile_status = 125;
    } else {
      backend_output_recorded = true;
      backend_output_payload = backend_text;
      if (!PublishFrontendToolchainRuntimeArtifacts(
              product, artifact_plan, ir_out, object_out, backend_out,
              backend_output_error, backend_error)) {
        compile_status = 125;
      }
    }
  }

  const Objc3ToolchainRuntimeGaOperationsCoreFeatureSurface
      toolchain_runtime_core_feature_surface =
          BuildObjc3ToolchainRuntimeGaOperationsCoreFeatureSurface(
              toolchain_runtime_ga_operations_scaffold, compile_status,
              backend_output_recorded, backend_out, backend_output_payload);
  std::string toolchain_runtime_core_feature_reason;
  const bool toolchain_runtime_core_feature_ready =
      IsObjc3ToolchainRuntimeGaOperationsCoreFeatureSurfaceReady(
          toolchain_runtime_core_feature_surface,
          toolchain_runtime_core_feature_reason);
  if (compile_status != 0) {
    SetFrontendEmitError(
        context, result, compile_status,
        BuildBackendFailureDiagnostic(wants_clang_backend, compile_status,
                                      backend_output_error, backend_error),
        emit_diagnostics);
  } else if (!toolchain_runtime_core_feature_ready) {
    SetFrontendEmitError(
        context, result, 125,
        "error:1:1: LLVM object emission failed: toolchain/runtime core feature fail-closed: " +
            toolchain_runtime_core_feature_reason + " [O3E002]",
        emit_diagnostics);
  } else {
    SetFrontendContextObjectPath(context, object_out.generic_string());
  }
}

}  // namespace

void PublishFrontendIrAndObjectArtifacts(
    objc3c_frontend_context_t *context,
    objc3c_frontend_compile_result_t *result,
    const Objc3FrontendCompileProduct &product,
    const Objc3FrontendArtifactOutputPlan &artifact_plan,
    const objc3c_frontend_compile_options_t &options,
    std::vector<std::string> &emit_diagnostics) {
  std::filesystem::path ir_out;
  if (!PublishFrontendIrArtifact(context, result, product, artifact_plan,
                                 ir_out)) {
    return;
  }
  PublishFrontendObjectArtifact(context, result, product, artifact_plan, options,
                                ir_out, emit_diagnostics);
}

}  // namespace objc3c::frontend
