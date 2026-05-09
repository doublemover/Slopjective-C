#include "io/objc3_diagnostics_artifacts.h"

#include <string>
#include <vector>

#include "io/objc3_diagnostics_artifact_document.h"
#include "io/objc3_file_io.h"

namespace {

void WriteDiagnosticsTextArtifact(const std::filesystem::path &out_dir,
                                  const std::string &emit_prefix,
                                  const std::vector<std::string> &diagnostics) {
  WriteText(out_dir / (emit_prefix + ".diagnostics.txt"),
            BuildDiagnosticsTextArtifact(diagnostics));
}

void WriteDiagnosticsJsonArtifact(const std::filesystem::path &out_dir,
                                  const std::string &emit_prefix,
                                  const std::vector<std::string> &diagnostics) {
  WriteText(out_dir / (emit_prefix + ".diagnostics.json"),
            BuildDiagnosticsJsonArtifact(diagnostics));
}

}  // namespace

void WriteDiagnosticsArtifacts(const std::filesystem::path &out_dir,
                               const std::string &emit_prefix,
                               const Objc3FrontendDiagnosticsBus &stage_diagnostics,
                               const std::vector<std::string> &post_pipeline_diagnostics) {
  const std::vector<std::string> diagnostics = FlattenStageDiagnostics(stage_diagnostics, post_pipeline_diagnostics);
  WriteDiagnosticsTextArtifact(out_dir, emit_prefix, diagnostics);
  WriteDiagnosticsJsonArtifact(out_dir, emit_prefix, diagnostics);
}

void WriteDiagnosticsArtifacts(const std::filesystem::path &out_dir,
                               const std::string &emit_prefix,
                               const std::vector<std::string> &diagnostics) {
  WriteDiagnosticsTextArtifact(out_dir, emit_prefix, diagnostics);
  WriteDiagnosticsJsonArtifact(out_dir, emit_prefix, diagnostics);
}
