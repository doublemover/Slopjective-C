#include "driver/objc3_objectivec_path.h"

#include <clang-c/Index.h>

// parity anchor: this translation unit is the concrete libclang
// consumer the future incremental backend must continue to compile and link
// correctly when the wrapper-owned LLVM_ROOT/libclang discovery contract moves
// through CMake/Ninja.

#include <filesystem>
#include <string>
#include <vector>

#include "artifacts/objc3_runtime_state_publication_paths.h"
#include "diag/objc3_diag_utils.h"
#include "io/objc3_diagnostics_artifacts.h"
#include "io/objc3_manifest_artifacts.h"
#include "io/objc3_process.h"
#include "driver/objc3_driver_status_codes.h"
#include "driver/objc3_objectivec_symbol_manifest.h"

namespace fs = std::filesystem;

int RunObjectiveCPath(const Objc3CliOptions &cli_options) {
  const std::vector<const char *> parse_args = {"-x", "objective-c", "-std=gnu11"};
  CXIndex index = clang_createIndex(0, 0);
  CXTranslationUnit tu = clang_parseTranslationUnit(index, cli_options.input.string().c_str(), parse_args.data(),
                                                    static_cast<int>(parse_args.size()), nullptr, 0,
                                                    CXTranslationUnit_None);

  bool has_errors = false;
  std::vector<std::string> diagnostics =
      CollectObjectiveCTranslationUnitDiagnostics(tu, has_errors);

  NormalizeDiagnostics(diagnostics);
  WriteDiagnosticsArtifacts(cli_options.out_dir, cli_options.emit_prefix, diagnostics);

  if (has_errors || tu == nullptr) {
    if (tu != nullptr) {
      clang_disposeTranslationUnit(tu);
    }
    clang_disposeIndex(index);
    return Objc3DriverStatusValue(
        Objc3DriverStatusCode::kDiagnosticsPresent);
  }

  WriteManifestArtifact(cli_options.out_dir,
                        cli_options.emit_prefix,
                        BuildObjectiveCSymbolManifest(cli_options.input, tu));

  const auto publication_paths =
      objc3::artifacts::frontend::BuildRuntimeStatePublicationPathsForEmitPrefix(
          cli_options.emit_prefix);
  const fs::path object_out = cli_options.out_dir / publication_paths.object_artifact;
  const int compile_status = RunObjectiveCCompile(cli_options.clang_path, cli_options.input, object_out);

  clang_disposeTranslationUnit(tu);
  clang_disposeIndex(index);
  return compile_status == 0
             ? Objc3DriverStatusValue(Objc3DriverStatusCode::kSuccess)
             : Objc3DriverStatusValue(
                   Objc3DriverStatusCode::kNativeToolchainFailure);
}
