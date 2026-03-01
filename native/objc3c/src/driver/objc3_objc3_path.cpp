#include "driver/objc3_objc3_path.h"

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "driver/objc3_frontend_options.h"
#include "io/objc3_diagnostics_artifacts.h"
#include "io/objc3_file_io.h"
#include "io/objc3_manifest_artifacts.h"
#include "io/objc3_process.h"
#include "libobjc3c_frontend/objc3_cli_frontend.h"

namespace fs = std::filesystem;

int RunObjc3LanguagePath(const Objc3CliOptions &cli_options) {
  const std::string source = ReadText(cli_options.input);
  const Objc3FrontendOptions frontend_options = BuildObjc3FrontendOptions(cli_options);
  Objc3FrontendArtifactBundle artifacts = CompileObjc3SourceForCli(cli_options.input, source, frontend_options);
  std::vector<std::string> diagnostics;
  diagnostics.reserve(artifacts.stage_diagnostics.size() + artifacts.post_pipeline_diagnostics.size());
  diagnostics.insert(diagnostics.end(),
                     artifacts.stage_diagnostics.lexer.begin(),
                     artifacts.stage_diagnostics.lexer.end());
  diagnostics.insert(diagnostics.end(),
                     artifacts.stage_diagnostics.parser.begin(),
                     artifacts.stage_diagnostics.parser.end());
  diagnostics.insert(diagnostics.end(),
                     artifacts.stage_diagnostics.semantic.begin(),
                     artifacts.stage_diagnostics.semantic.end());
  diagnostics.insert(diagnostics.end(),
                     artifacts.post_pipeline_diagnostics.begin(),
                     artifacts.post_pipeline_diagnostics.end());
  WriteDiagnosticsArtifacts(cli_options.out_dir, cli_options.emit_prefix, diagnostics);
  if (!artifacts.diagnostics.empty()) {
    return 1;
  }

  WriteManifestArtifact(cli_options.out_dir, cli_options.emit_prefix, artifacts.manifest_json);

  const fs::path ir_out = cli_options.out_dir / (cli_options.emit_prefix + ".ll");
  WriteText(ir_out, artifacts.ir_text);

  const fs::path object_out = cli_options.out_dir / (cli_options.emit_prefix + ".obj");
  int compile_status = 0;
  if (cli_options.ir_object_backend == Objc3IrObjectBackend::kClang) {
    compile_status = RunIRCompile(cli_options.clang_path, ir_out, object_out);
  } else {
    std::string backend_error;
    compile_status = RunIRCompileLLVMDirect(cli_options.llc_path, ir_out, object_out, backend_error);
    if (!backend_error.empty()) {
      std::cerr << backend_error << "\n";
    }
  }

  if (compile_status == 0) {
    const fs::path backend_out = cli_options.out_dir / (cli_options.emit_prefix + ".object-backend.txt");
    const std::string backend_text =
        cli_options.ir_object_backend == Objc3IrObjectBackend::kClang ? "clang\n" : "llvm-direct\n";
    WriteText(backend_out, backend_text);
  }

  return compile_status == 0 ? 0 : 3;
}
