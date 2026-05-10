#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_error_handling_error_abi_replay_error_abi_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_error_handling_error_abi_replay_row_helpers.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IRErrorThrowsAbiPropagationReplayMetadataNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  BeginObjc3IRErrorAbiReplayMetadataNode(
      "!87", kObjc3ErrorHandlingThrowsAbiPropagationLoweringContractId, out);
  EmitObjc3IRErrorAbiReplayStringField(
      kObjc3ErrorHandlingThrowsAbiPropagationLoweringSourceModel, out);
  EmitObjc3IRErrorAbiReplayStringField(
      kObjc3ErrorHandlingThrowsAbiPropagationLoweringAbiModel, out);
  EmitObjc3IRErrorAbiReplayStringField(
      metadata.lowering_throws_propagation_replay_key, out);
  EmitObjc3IRErrorAbiReplayStringField(
      metadata.lowering_result_like_replay_key, out);
  EmitObjc3IRErrorAbiReplayStringField(
      metadata.lowering_ns_error_bridging_replay_key, out);
  EmitObjc3IRErrorAbiReplayStringField(
      metadata.lowering_unwind_cleanup_replay_key, out);
  EmitObjc3IRErrorAbiReplayBoolField(
      metadata.deterministic_throws_propagation_lowering_handoff, out);
  EmitObjc3IRErrorAbiReplayBoolField(
      metadata.deterministic_result_like_lowering_handoff, out);
  EmitObjc3IRErrorAbiReplayBoolField(
      metadata.deterministic_ns_error_bridging_lowering_handoff, out);
  EmitObjc3IRErrorAbiReplayBoolField(
      metadata.deterministic_unwind_cleanup_lowering_handoff, out);
  EmitObjc3IRErrorAbiReplayStringField(
      kObjc3ErrorHandlingThrowsAbiPropagationLoweringFailClosedModel, out);
  EndObjc3IRErrorAbiReplayMetadataNode(out);
}
