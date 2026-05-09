#include "artifacts/objc3_frontend_artifact_interop_lowering_plan.h"

#include <utility>

namespace {

using objc3::artifacts::evidence::
    BuildErrorHandlingResultAndBridgingArtifactReplayEvidence;
using objc3::artifacts::frontend::
    BuildInteropFfiMetadataInterfacePreservationContract;
using objc3::artifacts::frontend::
    BuildInteropForeignCallLifetimeLoweringContract;
using objc3::artifacts::frontend::
    BuildInteropForeignSurfaceInterfacePreservationSummary;
using objc3::artifacts::frontend::BuildInteropHeaderModuleBridgeGenerationSummary;
using objc3::artifacts::frontend::BuildInteropInteropLoweringContract;
using objc3::artifacts::frontend::Objc3FrontendArtifactPostPipelineFailure;

void AddPostPipelineFailure(
    std::vector<Objc3FrontendArtifactPostPipelineFailure> &failures,
    const char *code,
    std::string message) {
  Objc3FrontendArtifactPostPipelineFailure failure;
  failure.present = true;
  failure.code = code == nullptr ? "" : code;
  failure.message = std::move(message);
  failures.push_back(std::move(failure));
}

}  // namespace

Objc3FrontendArtifactInteropLoweringPlan
BuildObjc3FrontendArtifactInteropLoweringPlan(
    const Objc3Program &program,
    const Objc3FrontendInteropForeignImportSourceClosureSummary
        &interop_foreign_import_source_closure_summary,
    const Objc3FrontendInteropCppSwiftInteropAnnotationSourceCompletionSummary
        &interop_cpp_swift_interop_annotation_source_completion_summary,
    const Objc3InteropInteropSemanticModelSummary
        &interop_interop_semantic_model_summary,
    const Objc3InteropInteropRuntimeParitySummary
        &interop_interop_runtime_parity_summary,
    const Objc3InteropCppInteropInteractionSummary
        &interop_cpp_interop_interaction_summary,
    const Objc3InteropSwiftInteropIsolationSummary
        &interop_swift_interop_isolation_summary,
    const std::string
        &error_handling_throws_abi_propagation_lowering_replay_key,
    const std::string &throws_propagation_lowering_replay_key,
    const std::string &result_like_lowering_replay_key,
    const std::string &ns_error_bridging_lowering_replay_key,
    const std::string &unwind_cleanup_lowering_replay_key,
    bool deterministic_error_handling_throws_abi_propagation_lowering,
    bool runtime_import_artifact_ready,
    const std::vector<Objc3ImportedRuntimeModuleSurface>
        &imported_runtime_module_surfaces) {
  Objc3FrontendArtifactInteropLoweringPlan plan;
  plan.error_handling_result_and_bridging_artifact_replay_summary =
      BuildErrorHandlingResultAndBridgingArtifactReplayEvidence(
          error_handling_throws_abi_propagation_lowering_replay_key,
          throws_propagation_lowering_replay_key,
          result_like_lowering_replay_key,
          ns_error_bridging_lowering_replay_key,
          unwind_cleanup_lowering_replay_key,
          deterministic_error_handling_throws_abi_propagation_lowering,
          runtime_import_artifact_ready,
          imported_runtime_module_surfaces);
  plan.interop_foreign_surface_interface_preservation_summary =
      BuildInteropForeignSurfaceInterfacePreservationSummary(
          program,
          interop_foreign_import_source_closure_summary,
          interop_cpp_swift_interop_annotation_source_completion_summary,
          runtime_import_artifact_ready,
          imported_runtime_module_surfaces);
  plan.interop_interop_lowering_contract = BuildInteropInteropLoweringContract(
      interop_interop_semantic_model_summary,
      interop_interop_runtime_parity_summary,
      interop_cpp_interop_interaction_summary,
      interop_swift_interop_isolation_summary,
      plan.interop_foreign_surface_interface_preservation_summary);
  if (!IsValidObjc3InteropInteropLoweringContract(
          plan.interop_interop_lowering_contract)) {
    AddPostPipelineFailure(
        plan.post_pipeline_failures,
        "O3L300",
        "LLVM IR emission failed: invalid Part 11 interop lowering contract");
  }
  plan.interop_interop_lowering_replay_key =
      Objc3InteropInteropLoweringReplayKey(plan.interop_interop_lowering_contract);
  plan.interop_foreign_call_lifetime_lowering_contract =
      BuildInteropForeignCallLifetimeLoweringContract(
          program,
          plan.interop_interop_lowering_contract,
          interop_cpp_interop_interaction_summary,
          plan.interop_foreign_surface_interface_preservation_summary);
  if (!IsValidObjc3InteropForeignCallLifetimeLoweringContract(
          plan.interop_foreign_call_lifetime_lowering_contract)) {
    AddPostPipelineFailure(
        plan.post_pipeline_failures,
        "O3L300",
        "LLVM IR emission failed: invalid Part 11 foreign call and lifetime "
        "lowering contract");
  }
  plan.interop_foreign_call_lifetime_lowering_replay_key =
      Objc3InteropForeignCallLifetimeLoweringReplayKey(
          plan.interop_foreign_call_lifetime_lowering_contract);
  plan.interop_ffi_metadata_interface_preservation_contract =
      BuildInteropFfiMetadataInterfacePreservationContract(
          plan.interop_foreign_call_lifetime_lowering_contract,
          plan.interop_foreign_call_lifetime_lowering_replay_key,
          plan.interop_foreign_surface_interface_preservation_summary,
          imported_runtime_module_surfaces,
          runtime_import_artifact_ready,
          plan.interop_ffi_metadata_interface_preservation_replay_key);
  if (!IsValidObjc3InteropFfiMetadataInterfacePreservationContract(
          plan.interop_ffi_metadata_interface_preservation_contract)) {
    AddPostPipelineFailure(
        plan.post_pipeline_failures,
        "O3L300",
        "LLVM IR emission failed: invalid Part 11 ffi metadata/interface "
        "preservation contract");
  }
  plan.interop_header_module_bridge_generation_summary =
      BuildInteropHeaderModuleBridgeGenerationSummary(
          program,
          plan.interop_foreign_surface_interface_preservation_summary,
          plan.interop_ffi_metadata_interface_preservation_contract,
          plan.interop_ffi_metadata_interface_preservation_replay_key,
          imported_runtime_module_surfaces);
  return plan;
}
