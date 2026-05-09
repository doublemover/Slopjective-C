#include "artifacts/objc3_frontend_artifact_block_lowering_plan.h"

#include <utility>

namespace {

using objc3::artifacts::frontend::BuildBlockAbiInvokeTrampolineLoweringContract;
using objc3::artifacts::frontend::BuildBlockCopyDisposeLoweringContract;
using objc3::artifacts::frontend::
    BuildBlockDeterminismPerfBaselineLoweringContract;
using objc3::artifacts::frontend::BuildBlockLiteralCaptureLoweringContract;
using objc3::artifacts::frontend::BuildBlockSourceModelCompletionContract;
using objc3::artifacts::frontend::BuildBlockSourceStorageAnnotationContract;
using objc3::artifacts::frontend::BuildBlockStorageEscapeLoweringContract;
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

Objc3FrontendArtifactBlockLoweringPlan
BuildObjc3FrontendArtifactBlockLoweringPlan(
    const Objc3FrontendPipelineResult &pipeline_result) {
  Objc3FrontendArtifactBlockLoweringPlan plan;
  plan.block_literal_capture_lowering_contract =
      BuildBlockLiteralCaptureLoweringContract(
          pipeline_result.sema_parity_surface);
  if (!IsValidObjc3BlockLiteralCaptureLoweringContract(
          plan.block_literal_capture_lowering_contract)) {
    AddPostPipelineFailure(
        plan.post_pipeline_failures,
        "O3L300",
        "LLVM IR emission failed: invalid block literal capture lowering "
        "contract");
  }
  plan.block_literal_capture_lowering_replay_key =
      Objc3BlockLiteralCaptureLoweringReplayKey(
          plan.block_literal_capture_lowering_contract);
  plan.block_source_model_completion_contract =
      BuildBlockSourceModelCompletionContract(pipeline_result.program.ast);
  if (!IsValidObjc3BlockSourceModelCompletionContract(
          plan.block_source_model_completion_contract)) {
    AddPostPipelineFailure(
        plan.post_pipeline_failures,
        "O3L300",
        "LLVM IR emission failed: invalid block source model completion "
        "contract");
  }
  plan.block_source_model_completion_replay_key =
      Objc3BlockSourceModelCompletionReplayKey(
          plan.block_source_model_completion_contract);
  plan.block_source_storage_annotation_contract =
      BuildBlockSourceStorageAnnotationContract(pipeline_result.program.ast);
  if (!IsValidObjc3BlockSourceStorageAnnotationContract(
          plan.block_source_storage_annotation_contract)) {
    AddPostPipelineFailure(
        plan.post_pipeline_failures,
        "O3L300",
        "LLVM IR emission failed: invalid block source storage annotation "
        "contract");
  }
  plan.block_source_storage_annotation_replay_key =
      Objc3BlockSourceStorageAnnotationReplayKey(
          plan.block_source_storage_annotation_contract);
  plan.block_abi_invoke_trampoline_lowering_contract =
      BuildBlockAbiInvokeTrampolineLoweringContract(
          pipeline_result.sema_parity_surface);
  if (!IsValidObjc3BlockAbiInvokeTrampolineLoweringContract(
          plan.block_abi_invoke_trampoline_lowering_contract)) {
    AddPostPipelineFailure(
        plan.post_pipeline_failures,
        "O3L300",
        "LLVM IR emission failed: invalid block ABI invoke-trampoline "
        "lowering contract");
  }
  plan.block_abi_invoke_trampoline_lowering_replay_key =
      Objc3BlockAbiInvokeTrampolineLoweringReplayKey(
          plan.block_abi_invoke_trampoline_lowering_contract);
  plan.block_storage_escape_lowering_contract =
      BuildBlockStorageEscapeLoweringContract(
          pipeline_result.sema_parity_surface);
  if (!IsValidObjc3BlockStorageEscapeLoweringContract(
          plan.block_storage_escape_lowering_contract)) {
    AddPostPipelineFailure(
        plan.post_pipeline_failures,
        "O3L300",
        "LLVM IR emission failed: invalid block storage escape lowering "
        "contract");
  }
  plan.block_storage_escape_lowering_replay_key =
      Objc3BlockStorageEscapeLoweringReplayKey(
          plan.block_storage_escape_lowering_contract);
  plan.block_copy_dispose_lowering_contract =
      BuildBlockCopyDisposeLoweringContract(
          pipeline_result.sema_parity_surface);
  if (!IsValidObjc3BlockCopyDisposeLoweringContract(
          plan.block_copy_dispose_lowering_contract)) {
    AddPostPipelineFailure(
        plan.post_pipeline_failures,
        "O3L300",
        "LLVM IR emission failed: invalid block copy-dispose lowering "
        "contract");
  }
  plan.block_copy_dispose_lowering_replay_key =
      Objc3BlockCopyDisposeLoweringReplayKey(
          plan.block_copy_dispose_lowering_contract);
  plan.block_determinism_perf_baseline_lowering_contract =
      BuildBlockDeterminismPerfBaselineLoweringContract(
          pipeline_result.sema_parity_surface);
  if (!IsValidObjc3BlockDeterminismPerfBaselineLoweringContract(
          plan.block_determinism_perf_baseline_lowering_contract)) {
    AddPostPipelineFailure(
        plan.post_pipeline_failures,
        "O3L300",
        "LLVM IR emission failed: invalid block determinism/perf baseline "
        "lowering contract");
  }
  plan.block_determinism_perf_baseline_lowering_replay_key =
      Objc3BlockDeterminismPerfBaselineLoweringReplayKey(
          plan.block_determinism_perf_baseline_lowering_contract);
  return plan;
}
