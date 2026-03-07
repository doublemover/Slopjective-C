#include "driver/objc3_objc3_path.h"

#include <exception>
#include <filesystem>
#include <iostream>
#include <string>

#include "driver/objc3_frontend_options.h"
#include "io/objc3_diagnostics_artifacts.h"
#include "io/objc3_file_io.h"
#include "io/objc3_manifest_artifacts.h"
#include "io/objc3_process.h"
#include "io/objc3_toolchain_runtime_ga_operations_core_feature_surface.h"
#include "io/objc3_toolchain_runtime_ga_operations_scaffold.h"
#include "libobjc3c_frontend/objc3_cli_frontend.h"

namespace fs = std::filesystem;

int RunObjc3LanguagePath(const Objc3CliOptions &cli_options) {
  try {
    const std::string source = ReadText(cli_options.input);
    const Objc3FrontendOptions frontend_options = BuildObjc3FrontendOptions(cli_options);
    Objc3FrontendArtifactBundle artifacts = CompileObjc3SourceForCli(cli_options.input, source, frontend_options);
    WriteDiagnosticsArtifacts(cli_options.out_dir,
                              cli_options.emit_prefix,
                              artifacts.stage_diagnostics,
                              artifacts.post_pipeline_diagnostics);
    if (!artifacts.manifest_json.empty()) {
      // M251-C001 freeze: manifest emission remains the authoritative published
      // metadata-section ABI surface until later object-section emission lands.
      // M251-C002 scaffold: manifest emission mirrors the live runtime-metadata
      // section scaffold inventory so tooling can diff JSON against emitted
      // LLVM IR/object evidence without treating the manifest as the only source.
      // M251-C003 object inspection harness: manifest emission also publishes
      // the llvm-readobj/llvm-objdump matrix so object inspection remains tied
      // to canonical emitted artifacts instead of ad hoc operator commands.
      // M251-D001 freeze: manifest emission also publishes the reserved native
      // runtime support-library surface (target/header/entrypoints/link mode)
      // so D002/D003 must preserve one canonical runtime-library contract while
      // the deterministic test shim remains non-canonical evidence only.
      // M251-D002 core feature: manifest emission also publishes the live
      // native runtime-library skeleton/build contract so the real in-tree
      // archive/header/source/probe surface stays synchronized with emitted IR
      // evidence while driver link wiring remains deferred to M251-D003.
      WriteManifestArtifact(cli_options.out_dir, cli_options.emit_prefix, artifacts.manifest_json);
    }
    if (!artifacts.diagnostics.empty()) {
      return 1;
    }

    // M251-A003 expands the handoff so manifest projection survives fail-closed
    // later lowering/object gates; native runtime linking remains a later
    // milestone.

    const fs::path ir_out = cli_options.out_dir / (cli_options.emit_prefix + ".ll");
    WriteText(ir_out, artifacts.ir_text);

    const fs::path object_out = cli_options.out_dir / (cli_options.emit_prefix + ".obj");
    const bool clang_backend_selected = cli_options.ir_object_backend == Objc3IrObjectBackend::kClang;
    const bool llvm_direct_backend_selected = cli_options.ir_object_backend == Objc3IrObjectBackend::kLLVMDirect;
#if defined(OBJC3C_ENABLE_LLVM_DIRECT_OBJECT_EMISSION)
    const bool llvm_direct_backend_enabled = true;
#else
    const bool llvm_direct_backend_enabled = false;
#endif
    const Objc3ToolchainRuntimeGaOperationsScaffold toolchain_runtime_ga_operations_scaffold =
        BuildObjc3ToolchainRuntimeGaOperationsScaffold(
            clang_backend_selected,
            llvm_direct_backend_selected,
            cli_options.clang_path,
            cli_options.llc_path,
            llvm_direct_backend_enabled,
            ir_out,
            object_out);
    std::string toolchain_runtime_scaffold_reason;
    if (!IsObjc3ToolchainRuntimeGaOperationsScaffoldReady(
            toolchain_runtime_ga_operations_scaffold,
            toolchain_runtime_scaffold_reason)) {
      std::cerr << "toolchain/runtime readiness scaffold fail-closed: "
                << toolchain_runtime_scaffold_reason << "\n";
      return 3;
    }

    int compile_status = 0;
    const fs::path backend_out = cli_options.out_dir / (cli_options.emit_prefix + ".object-backend.txt");
    const std::string backend_text =
        cli_options.ir_object_backend == Objc3IrObjectBackend::kClang ? "clang\n" : "llvm-direct\n";
    if (cli_options.ir_object_backend == Objc3IrObjectBackend::kClang) {
      compile_status = RunIRCompile(cli_options.clang_path, ir_out, object_out);
    } else {
      std::string backend_error;
      compile_status = RunIRCompileLLVMDirect(cli_options.llc_path, ir_out, object_out, backend_error);
      if (!backend_error.empty()) {
        std::cerr << backend_error << "\n";
      }
    }

    bool backend_output_recorded = false;
    std::string backend_output_payload;
    if (compile_status == 0) {
      WriteText(backend_out, backend_text);
      backend_output_recorded = true;
      backend_output_payload = backend_text;
    }

    const Objc3ToolchainRuntimeGaOperationsCoreFeatureSurface toolchain_runtime_core_feature_surface =
        BuildObjc3ToolchainRuntimeGaOperationsCoreFeatureSurface(
            toolchain_runtime_ga_operations_scaffold,
            compile_status,
            backend_output_recorded,
            backend_out,
            backend_output_payload);
    std::string toolchain_runtime_core_feature_reason;
    if (!IsObjc3ToolchainRuntimeGaOperationsCoreFeatureSurfaceReady(
            toolchain_runtime_core_feature_surface,
            toolchain_runtime_core_feature_reason)) {
      std::cerr << "toolchain/runtime core feature fail-closed: "
                << toolchain_runtime_core_feature_reason << "\n";
      return 3;
    }

    return 0;
  } catch (const std::exception &io_error) {
    std::cerr << "artifact io failure: " << io_error.what() << "\n";
    return 3;
  }
}
