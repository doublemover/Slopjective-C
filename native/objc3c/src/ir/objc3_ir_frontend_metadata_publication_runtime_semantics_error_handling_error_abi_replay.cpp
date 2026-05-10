#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_error_handling_error_abi_replay.h"

#include <sstream>

#include "ir/objc3_ir_c_string.h"
#include "ir/objc3_ir_frontend_metadata.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IRErrorAbiReplayMetadataNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!87 = !{!\""
      << EscapeCStringLiteral(kObjc3ErrorHandlingThrowsAbiPropagationLoweringContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3ErrorHandlingThrowsAbiPropagationLoweringSourceModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3ErrorHandlingThrowsAbiPropagationLoweringAbiModel)
      << "\", !\""
      << EscapeCStringLiteral(metadata.lowering_throws_propagation_replay_key)
      << "\", !\""
      << EscapeCStringLiteral(metadata.lowering_result_like_replay_key)
      << "\", !\""
      << EscapeCStringLiteral(metadata.lowering_ns_error_bridging_replay_key)
      << "\", !\""
      << EscapeCStringLiteral(metadata.lowering_unwind_cleanup_replay_key)
      << "\", i1 "
      << (metadata.deterministic_throws_propagation_lowering_handoff ? 1 : 0)
      << ", i1 "
      << (metadata.deterministic_result_like_lowering_handoff ? 1 : 0)
      << ", i1 "
      << (metadata.deterministic_ns_error_bridging_lowering_handoff ? 1 : 0)
      << ", i1 "
      << (metadata.deterministic_unwind_cleanup_lowering_handoff ? 1 : 0)
      << ", !\""
      << EscapeCStringLiteral(
             kObjc3ErrorHandlingThrowsAbiPropagationLoweringFailClosedModel)
      << "\"}\n";
  out << "!88 = !{!\""
      << EscapeCStringLiteral(
             kObjc3ErrorHandlingResultAndBridgingArtifactReplayContractId)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ErrorHandlingResultAndBridgingArtifactReplaySourceModel)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ErrorHandlingResultAndBridgingArtifactReplayModel)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.lowering_error_handling_result_and_bridging_artifact_replay_key)
      << "\", i64 "
      << static_cast<unsigned long long>(
             metadata.imported_error_handling_result_and_bridging_artifact_modules)
      << ", i1 "
      << (metadata.error_handling_result_and_bridging_binary_artifact_replay_ready
              ? 1
              : 0)
      << ", i1 "
      << (metadata
                  .error_handling_result_and_bridging_runtime_import_artifact_ready
              ? 1
              : 0)
      << ", i1 "
      << (metadata
                  .error_handling_result_and_bridging_separate_compilation_replay_ready
              ? 1
              : 0)
      << ", i1 "
      << (metadata
                  .deterministic_error_handling_result_and_bridging_artifact_replay_handoff
              ? 1
              : 0)
      << ", !\""
      << EscapeCStringLiteral(
             kObjc3ErrorHandlingResultAndBridgingArtifactReplayFailClosedModel)
      << "\"}\n";
}
