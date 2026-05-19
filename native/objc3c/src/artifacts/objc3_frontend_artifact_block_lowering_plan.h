#pragma once

#include <string>
#include <vector>

#include "artifacts/objc3_frontend_artifact_diagnostics.h"
#include "lower/contracts/block_abi_lowering_contract_records.h"
#include "lower/contracts/block_source_closure_contracts.h"
#include "pipeline/objc3_frontend_types.h"

struct Objc3FrontendArtifactBlockLoweringPlan {
  Objc3BlockLiteralCaptureLoweringContract
      block_literal_capture_lowering_contract;
  std::string block_literal_capture_lowering_replay_key;
  Objc3BlockSourceModelCompletionContract
      block_source_model_completion_contract;
  std::string block_source_model_completion_replay_key;
  Objc3BlockSourceStorageAnnotationContract
      block_source_storage_annotation_contract;
  std::string block_source_storage_annotation_replay_key;
  Objc3BlockAbiInvokeTrampolineLoweringContract
      block_abi_invoke_trampoline_lowering_contract;
  std::string block_abi_invoke_trampoline_lowering_replay_key;
  Objc3BlockStorageEscapeLoweringContract
      block_storage_escape_lowering_contract;
  std::string block_storage_escape_lowering_replay_key;
  Objc3BlockCopyDisposeLoweringContract block_copy_dispose_lowering_contract;
  std::string block_copy_dispose_lowering_replay_key;
  Objc3BlockDeterminismPerfBaselineLoweringContract
      block_determinism_perf_baseline_lowering_contract;
  std::string block_determinism_perf_baseline_lowering_replay_key;
  std::vector<
      objc3::artifacts::frontend::Objc3FrontendArtifactPostPipelineFailure>
      post_pipeline_failures;
};

Objc3FrontendArtifactBlockLoweringPlan
BuildObjc3FrontendArtifactBlockLoweringPlan(
    const Objc3FrontendPipelineResult &pipeline_result);
