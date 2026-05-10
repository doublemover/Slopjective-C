#include "ir/objc3_ir_frontend_metadata_publication_runtime_support_debug_projection.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata_publication_runtime_support_debug_projection_debug_rows.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_support_debug_projection_runtime_rows.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_support_debug_projection_state_rows.h"

void EmitObjc3IRExecutableDebugProjectionMetadataNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  BeginObjc3IRExecutableDebugProjectionMetadataNode(metadata, out);
  EmitObjc3IRExecutableDebugProjectionSurfaceFields(metadata, out);
  EmitObjc3IRExecutableDebugProjectionStateFields(metadata, out);
  EmitObjc3IRExecutableDebugProjectionReplayFields(metadata, out);
  EndObjc3IRExecutableDebugProjectionMetadataNode(out);
}
