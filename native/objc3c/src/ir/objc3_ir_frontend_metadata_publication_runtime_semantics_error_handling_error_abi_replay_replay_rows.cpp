#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_error_handling_error_abi_replay_replay_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_error_handling_error_abi_replay_row_helpers.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IRErrorResultBridgingArtifactReplayMetadataNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  BeginObjc3IRErrorAbiReplayMetadataNode(
      "!88", kObjc3ErrorHandlingResultAndBridgingArtifactReplayContractId,
      out);
  EmitObjc3IRErrorAbiReplayStringField(
      kObjc3ErrorHandlingResultAndBridgingArtifactReplaySourceModel, out);
  EmitObjc3IRErrorAbiReplayStringField(
      kObjc3ErrorHandlingResultAndBridgingArtifactReplayModel, out);
  EmitObjc3IRErrorAbiReplayStringField(
      metadata.lowering_error_handling_result_and_bridging_artifact_replay_key,
      out);
  EmitObjc3IRErrorAbiReplaySizeField(
      metadata.imported_error_handling_result_and_bridging_artifact_modules,
      out);
  EmitObjc3IRErrorAbiReplayBoolField(
      metadata.error_handling_result_and_bridging_binary_artifact_replay_ready,
      out);
  EmitObjc3IRErrorAbiReplayBoolField(
      metadata.error_handling_result_and_bridging_runtime_import_artifact_ready,
      out);
  EmitObjc3IRErrorAbiReplayBoolField(
      metadata
          .error_handling_result_and_bridging_separate_compilation_replay_ready,
      out);
  EmitObjc3IRErrorAbiReplayBoolField(
      metadata
          .deterministic_error_handling_result_and_bridging_artifact_replay_handoff,
      out);
  EmitObjc3IRErrorAbiReplayStringField(
      kObjc3ErrorHandlingResultAndBridgingArtifactReplayFailClosedModel, out);
  EndObjc3IRErrorAbiReplayMetadataNode(out);
}
