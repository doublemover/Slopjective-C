#include "ir/objc3_ir_frontend_metadata_publication_runtime_support_debug_projection_debug_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_support_debug_projection_row_helpers.h"

void EmitObjc3IRExecutableDebugProjectionSurfaceFields(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRRuntimeSupportDebugProjectionStringField(
      metadata.executable_metadata_debug_projection_named_metadata_name, out);
  EmitObjc3IRRuntimeSupportDebugProjectionStringField(
      metadata.executable_metadata_debug_projection_manifest_surface_path, out);
  EmitObjc3IRRuntimeSupportDebugProjectionStringField(
      metadata.executable_metadata_debug_projection_typed_handoff_surface_path,
      out);
  EmitObjc3IRRuntimeSupportDebugProjectionStringField(
      metadata.executable_metadata_debug_projection_source_graph_surface_path,
      out);
}

void EmitObjc3IRExecutableDebugProjectionReplayFields(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRRuntimeSupportDebugProjectionStringField(
      metadata.executable_metadata_debug_projection_replay_key, out);
  EmitObjc3IRRuntimeSupportDebugProjectionStringField(
      metadata
          .executable_metadata_debug_projection_active_typed_handoff_replay_key,
      out);
  EmitObjc3IRRuntimeSupportDebugProjectionStringField(
      metadata.executable_metadata_debug_projection_row0_descriptor, out);
  EmitObjc3IRRuntimeSupportDebugProjectionStringField(
      metadata.executable_metadata_debug_projection_row1_descriptor, out);
  EmitObjc3IRRuntimeSupportDebugProjectionStringField(
      metadata.executable_metadata_debug_projection_row2_descriptor, out);
}
