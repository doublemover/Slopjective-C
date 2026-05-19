#include "driver/objc3_driver_object_backend.h"

#include "driver/objc3_driver_diagnostic_output.h"
#include "driver/objc3_driver_status_codes.h"
#include "io/objc3_file_io.h"
#include "io/objc3_process.h"

namespace fs = std::filesystem;

Objc3DriverObjectBackendResult EmitObjc3DriverObjectBackend(
    const Objc3CliOptions &cli_options,
    const std::string &ir_text) {
  Objc3DriverObjectBackendResult result;
  result.ir_out = cli_options.out_dir / (cli_options.emit_prefix + ".ll");
  result.object_out = cli_options.out_dir / (cli_options.emit_prefix + ".obj");
  result.backend_out =
      cli_options.out_dir / (cli_options.emit_prefix + ".object-backend.txt");

  WriteText(result.ir_out, ir_text);
  const bool clang_backend_selected =
      cli_options.ir_object_backend == Objc3IrObjectBackend::kClang;
  const bool llvm_direct_backend_selected =
      cli_options.ir_object_backend == Objc3IrObjectBackend::kLLVMDirect;
#if defined(OBJC3C_ENABLE_LLVM_DIRECT_OBJECT_EMISSION)
  const bool llvm_direct_backend_enabled = true;
#else
  const bool llvm_direct_backend_enabled = false;
#endif
  result.scaffold = BuildObjc3ToolchainRuntimeGaOperationsScaffold(
      clang_backend_selected,
      llvm_direct_backend_selected,
      cli_options.clang_path,
      cli_options.llc_path,
      llvm_direct_backend_enabled,
      result.ir_out,
      result.object_out);
  std::string scaffold_reason;
  if (!IsObjc3ToolchainRuntimeGaOperationsScaffoldReady(result.scaffold,
                                                        scaffold_reason)) {
    EmitObjc3DriverError(
        "toolchain/runtime readiness contract fail-closed: " +
        scaffold_reason);
    result.status_code = Objc3DriverStatusValue(
        Objc3DriverStatusCode::kNativeToolchainFailure);
    result.compile_status = Objc3DriverStatusValue(
        Objc3DriverStatusCode::kNativeToolchainFailure);
    return result;
  }

  const std::string backend_text =
      cli_options.ir_object_backend == Objc3IrObjectBackend::kClang
          ? "clang\n"
          : "llvm-direct\n";
  if (cli_options.ir_object_backend == Objc3IrObjectBackend::kClang) {
    result.compile_status =
        RunIRCompile(cli_options.clang_path, result.ir_out, result.object_out);
  } else {
    std::string backend_error;
    result.compile_status = RunIRCompileLLVMDirect(
        cli_options.llc_path, result.ir_out, result.object_out, backend_error);
    if (!backend_error.empty()) {
      EmitObjc3DriverError(backend_error);
    }
  }
  if (result.compile_status == 0) {
    WriteText(result.backend_out, backend_text);
    result.backend_output_recorded = true;
    result.backend_output_payload = backend_text;
  }
  return result;
}
