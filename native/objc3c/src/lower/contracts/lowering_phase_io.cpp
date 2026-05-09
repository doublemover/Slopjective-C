#include "lower/contracts/lowering_phase_io.h"

#include "ast/objc3_ast_decl_surface.h"

#include <sstream>

namespace {

std::string BuildRelativeArtifactPath(const std::string &emit_prefix,
                                      const std::string &suffix) {
  return emit_prefix + suffix;
}

}  // namespace

Objc3LoweringArtifactPlan Objc3BuildLoweringArtifactPlan(
    const std::string &output_directory, const std::string &emit_prefix,
    bool emit_ir, bool emit_object, bool emit_manifest,
    bool emit_runtime_metadata) {
  Objc3LoweringArtifactPlan plan;
  plan.output_directory = output_directory;
  plan.emit_prefix = emit_prefix.empty() ? "module" : emit_prefix;
  plan.emit_ir = emit_ir;
  plan.emit_object = emit_object;
  plan.emit_manifest = emit_manifest;
  plan.emit_runtime_metadata = emit_runtime_metadata;
  plan.ir_relative_path = BuildRelativeArtifactPath(plan.emit_prefix, ".ll");
  plan.object_relative_path = BuildRelativeArtifactPath(plan.emit_prefix, ".obj");
  plan.manifest_relative_path =
      BuildRelativeArtifactPath(plan.emit_prefix, ".manifest.json");
  plan.runtime_metadata_relative_path =
      BuildRelativeArtifactPath(plan.emit_prefix, ".runtime-metadata.bin");
  return plan;
}

Objc3LoweringPhaseInput Objc3BuildLoweringPhaseInput(
    const Objc3Program &program, const std::string &source_path,
    bool arc_mode_enabled, const Objc3LoweringArtifactPlan &artifacts) {
  Objc3LoweringPhaseInput input;
  input.program = &program;
  input.source_path = source_path;
  input.module_name = program.module_name;
  input.arc_mode_enabled = arc_mode_enabled;
  input.artifacts = artifacts;
  return input;
}

bool Objc3LoweringPhaseInputIsReady(const Objc3LoweringPhaseInput &input) {
  return input.program != nullptr && !input.module_name.empty() &&
         (!input.artifacts.emit_ir || !input.artifacts.ir_relative_path.empty()) &&
         (!input.artifacts.emit_object ||
          !input.artifacts.object_relative_path.empty()) &&
         (!input.artifacts.emit_manifest ||
          !input.artifacts.manifest_relative_path.empty()) &&
         (!input.artifacts.emit_runtime_metadata ||
          !input.artifacts.runtime_metadata_relative_path.empty());
}

std::string Objc3LoweringArtifactPlanReplayKey(
    const Objc3LoweringArtifactPlan &plan) {
  std::ostringstream out;
  out << "out=" << plan.output_directory << ";prefix=" << plan.emit_prefix
      << ";ir=" << (plan.emit_ir ? "true" : "false")
      << ";object=" << (plan.emit_object ? "true" : "false")
      << ";manifest=" << (plan.emit_manifest ? "true" : "false")
      << ";runtime_metadata="
      << (plan.emit_runtime_metadata ? "true" : "false")
      << ";ir_path=" << plan.ir_relative_path
      << ";object_path=" << plan.object_relative_path
      << ";manifest_path=" << plan.manifest_relative_path
      << ";runtime_metadata_path=" << plan.runtime_metadata_relative_path;
  return out.str();
}

std::string Objc3LoweringPhaseInputReplayKey(
    const Objc3LoweringPhaseInput &input) {
  std::ostringstream out;
  out << "ready=" << (Objc3LoweringPhaseInputIsReady(input) ? "true" : "false")
      << ";source=" << input.source_path << ";module=" << input.module_name
      << ";arc=" << (input.arc_mode_enabled ? "true" : "false")
      << ";program="
      << (input.program == nullptr ? "(none)"
                                   : Objc3ProgramLoweringReplayKey(*input.program))
      << ";artifacts=" << Objc3LoweringArtifactPlanReplayKey(input.artifacts);
  return out.str();
}

std::string Objc3LoweringPhaseOutputReplayKey(
    const Objc3LoweringPhaseOutput &output) {
  std::ostringstream out;
  out << "ready=" << (output.ready ? "true" : "false")
      << ";diagnostics=" << output.diagnostics.size()
      << ";artifacts=" << Objc3LoweringArtifactPlanReplayKey(output.artifacts);
  for (const Objc3LoweringDiagnostic &diagnostic : output.diagnostics) {
    out << ";diagnostic={"
        << Objc3LoweringDiagnosticReplayKey(diagnostic) << "}";
  }
  return out.str();
}
