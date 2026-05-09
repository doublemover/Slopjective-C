#include "libobjc3c_frontend/objc3c_frontend_artifact_plan.h"

#include "libobjc3c_frontend/objc3c_frontend_compile_contract.h"

namespace objc3c::frontend {

Objc3FrontendArtifactOutputPlan BuildFrontendArtifactOutputPlan(
    const objc3c_frontend_compile_options_t &options,
    const std::filesystem::path &input_path) {
  Objc3FrontendArtifactOutputPlan plan;
  plan.input_path = input_path;
  plan.out_dir = ResolveFrontendOutputDir(options);
  plan.emit_prefix = ResolveFrontendEmitPrefix(options, input_path);
  plan.has_out_dir = !plan.out_dir.empty();
  plan.wants_ir_file = options.emit_ir != 0 || options.emit_object != 0;
  plan.wants_emit_stage = plan.wants_ir_file;
  return plan;
}

std::filesystem::path BuildFrontendDiagnosticsOutputPath(
    const Objc3FrontendArtifactOutputPlan &plan) {
  return plan.out_dir / (plan.emit_prefix + ".diagnostics.json");
}

std::filesystem::path BuildFrontendManifestOutputPath(
    const Objc3FrontendArtifactOutputPlan &plan) {
  return plan.out_dir / (plan.emit_prefix + ".manifest.json");
}

std::filesystem::path BuildFrontendIrOutputPath(
    const Objc3FrontendArtifactOutputPlan &plan) {
  return plan.out_dir / (plan.emit_prefix + ".ll");
}

std::filesystem::path BuildFrontendObjectOutputPath(
    const Objc3FrontendArtifactOutputPlan &plan) {
  return plan.out_dir / (plan.emit_prefix + ".obj");
}

std::filesystem::path BuildFrontendObjectBackendOutputPath(
    const Objc3FrontendArtifactOutputPlan &plan) {
  return plan.out_dir / (plan.emit_prefix + ".object-backend.txt");
}

}  // namespace objc3c::frontend
