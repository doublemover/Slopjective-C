#pragma once

#include <string>

#include "artifacts/objc3_frontend_artifact_metadata_dtos.h"

namespace objc3::artifacts::frontend {

void ApplyObjc3FrontendBlockMetadata(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const std::string &block_literal_capture_lowering_replay_key,
    const Objc3BlockLiteralCaptureLoweringContract
        &block_literal_capture_lowering_contract,
    const std::string &block_source_model_completion_replay_key,
    const Objc3BlockSourceModelCompletionContract
        &block_source_model_completion_contract,
    const std::string &block_source_storage_annotation_replay_key,
    const Objc3BlockSourceStorageAnnotationContract
        &block_source_storage_annotation_contract,
    const std::string &block_abi_invoke_trampoline_lowering_replay_key,
    const Objc3BlockAbiInvokeTrampolineLoweringContract
        &block_abi_invoke_trampoline_lowering_contract,
    const std::string &block_storage_escape_lowering_replay_key,
    const Objc3BlockStorageEscapeLoweringContract
        &block_storage_escape_lowering_contract,
    const std::string &block_copy_dispose_lowering_replay_key,
    const Objc3BlockCopyDisposeLoweringContract
        &block_copy_dispose_lowering_contract,
    const std::string &block_determinism_perf_baseline_lowering_replay_key,
    const Objc3BlockDeterminismPerfBaselineLoweringContract
        &block_determinism_perf_baseline_lowering_contract);

}  // namespace objc3::artifacts::frontend
