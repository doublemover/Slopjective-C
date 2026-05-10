#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_error_handling_error_abi_replay.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_error_handling_error_abi_replay_error_abi_rows.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_error_handling_error_abi_replay_replay_rows.h"

void EmitObjc3IRErrorAbiReplayMetadataNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRErrorThrowsAbiPropagationReplayMetadataNode(metadata, out);
  EmitObjc3IRErrorResultBridgingArtifactReplayMetadataNode(metadata, out);
}
