#include "lower/contracts/lowering_phase_io.h"

#include "ast/objc3_ast_decl_surface.h"

#include <sstream>

Objc3LoweringPhaseInput Objc3BuildLoweringPhaseInput(
    const Objc3Program &program, const std::string &source_path,
    bool arc_mode_enabled, const Objc3LoweringArtifactPlan &artifacts) {
  Objc3LoweringPhaseInput input;
  input.program = &program;
  input.source_path = source_path;
  input.module_name = program.module_name;
  input.arc_mode_enabled = arc_mode_enabled;
  input.artifacts = artifacts;
  input.typed_boundary = Objc3BuildTypedSemaToLoweringBoundary(program);
  input.runtime_metadata_handoff =
      Objc3BuildRuntimeMetadataLoweringHandoff(program);
  input.backend_handoff = Objc3BuildLoweringBackendHandoff(artifacts);
  input.lower_to_ir_handoff = Objc3BuildLoweringIRHandoff(artifacts);
  input.owner_split_explicit =
      Objc3LoweringStrictOwnerModelIsReady(
          input.stage_input_owner,
          input.owner_model,
          input.strict_no_fallback,
          input.strict_no_compatibility) &&
      Objc3LoweringStrictOwnerModelIsReady(
          input.stage_output_owner,
          input.owner_model,
          input.strict_no_fallback,
          input.strict_no_compatibility) &&
      Objc3LoweringStrictOwnerModelIsReady(
          input.diagnostic_handoff_owner,
          input.owner_model,
          input.strict_no_fallback,
          input.strict_no_compatibility);
  return input;
}

bool Objc3LoweringPhaseInputIsReady(const Objc3LoweringPhaseInput &input) {
  return input.program != nullptr && !input.module_name.empty() &&
         Objc3TypedSemaToLoweringBoundaryIsReady(input.typed_boundary) &&
         Objc3LoweringBackendHandoffIsReady(input.backend_handoff) &&
         Objc3LoweringIRHandoffIsReady(input.lower_to_ir_handoff) &&
         input.owner_split_explicit &&
         (!input.artifacts.emit_ir || !input.artifacts.ir_relative_path.empty()) &&
         (!input.artifacts.emit_object ||
          !input.artifacts.object_relative_path.empty()) &&
         (!input.artifacts.emit_manifest ||
          !input.artifacts.manifest_relative_path.empty()) &&
         (!input.artifacts.emit_runtime_metadata ||
          !input.artifacts.runtime_metadata_relative_path.empty());
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
      << ";typed_boundary="
      << Objc3TypedSemaToLoweringBoundaryReplayKey(input.typed_boundary)
      << ";runtime_metadata="
      << Objc3RuntimeMetadataLoweringHandoffReplayKey(
             input.runtime_metadata_handoff)
      << ";backend_handoff="
      << Objc3LoweringBackendHandoffReplayKey(input.backend_handoff)
      << ";lower_to_ir_handoff="
      << Objc3LoweringIRHandoffReplayKey(input.lower_to_ir_handoff)
      << ";artifacts=" << Objc3LoweringArtifactPlanReplayKey(input.artifacts)
      << ";owner_split_explicit="
      << (input.owner_split_explicit ? "true" : "false")
      << ";stage_input_owner=" << input.stage_input_owner
      << ";stage_output_owner=" << input.stage_output_owner
      << ";diagnostic_handoff_owner=" << input.diagnostic_handoff_owner
      << ";owner_model=" << input.owner_model
      << ";strict_no_fallback="
      << (input.strict_no_fallback ? "true" : "false")
      << ";strict_no_compatibility="
      << (input.strict_no_compatibility ? "true" : "false");
  return out.str();
}

std::string Objc3LoweringPhaseOutputReplayKey(
    const Objc3LoweringPhaseOutput &output) {
  std::ostringstream out;
  out << "ready=" << (output.ready ? "true" : "false")
      << ";diagnostics=" << output.diagnostics.size()
      << ";artifacts=" << Objc3LoweringArtifactPlanReplayKey(output.artifacts)
      << ";lower_to_ir_handoff="
      << Objc3LoweringIRHandoffReplayKey(output.lower_to_ir_handoff)
      << ";stage_output_owner=" << output.stage_output_owner
      << ";diagnostic_handoff_owner=" << output.diagnostic_handoff_owner
      << ";owner_model=" << output.owner_model
      << ";strict_no_fallback="
      << (output.strict_no_fallback ? "true" : "false")
      << ";strict_no_compatibility="
      << (output.strict_no_compatibility ? "true" : "false");
  for (const Objc3LoweringDiagnostic &diagnostic : output.diagnostics) {
    out << ";diagnostic={"
        << Objc3LoweringDiagnosticReplayKey(diagnostic) << "}";
  }
  return out.str();
}
