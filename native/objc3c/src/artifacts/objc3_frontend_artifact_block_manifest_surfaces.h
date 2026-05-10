#pragma once

#include <iosfwd>
#include <string>

struct Objc3BlockAbiInvokeTrampolineLoweringContract;
struct Objc3BlockCopyDisposeLoweringContract;
struct Objc3BlockDeterminismPerfBaselineLoweringContract;
struct Objc3BlockLiteralCaptureLoweringContract;
struct Objc3BlockSourceModelCompletionContract;
struct Objc3BlockSourceStorageAnnotationContract;
struct Objc3BlockStorageEscapeLoweringContract;

namespace objc3::artifacts::frontend {

void WriteBlockManifestSurfaces(
    std::ostream &manifest,
    const Objc3BlockLiteralCaptureLoweringContract
        &block_literal_capture_lowering_contract,
    const std::string &block_literal_capture_lowering_replay_key,
    const Objc3BlockSourceModelCompletionContract
        &block_source_model_completion_contract,
    const std::string &block_source_model_completion_replay_key,
    const Objc3BlockSourceStorageAnnotationContract
        &block_source_storage_annotation_contract,
    const std::string &block_source_storage_annotation_replay_key,
    const Objc3BlockAbiInvokeTrampolineLoweringContract
        &block_abi_invoke_trampoline_lowering_contract,
    const std::string &block_abi_invoke_trampoline_lowering_replay_key,
    const Objc3BlockStorageEscapeLoweringContract
        &block_storage_escape_lowering_contract,
    const std::string &block_storage_escape_lowering_replay_key,
    const Objc3BlockCopyDisposeLoweringContract
        &block_copy_dispose_lowering_contract,
    const std::string &block_copy_dispose_lowering_replay_key,
    const Objc3BlockDeterminismPerfBaselineLoweringContract
        &block_determinism_perf_baseline_lowering_contract,
    const std::string &block_determinism_perf_baseline_lowering_replay_key);

}  // namespace objc3::artifacts::frontend
