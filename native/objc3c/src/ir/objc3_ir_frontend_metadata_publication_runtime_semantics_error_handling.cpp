#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_error_handling.h"

#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_error_handling_error_abi_replay.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_error_handling_runtime_bridge.h"

void EmitObjc3IRErrorHandlingMetadataNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRErrorAbiReplayMetadataNodes(metadata, out);
  EmitObjc3IRErrorRuntimeBridgeMetadataNodes(out);
}
