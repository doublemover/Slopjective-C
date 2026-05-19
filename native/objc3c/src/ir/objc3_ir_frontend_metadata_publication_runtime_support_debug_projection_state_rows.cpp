#include "ir/objc3_ir_frontend_metadata_publication_runtime_support_debug_projection_state_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_support_debug_projection_row_helpers.h"

void EmitObjc3IRExecutableDebugProjectionStateFields(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRRuntimeSupportDebugProjectionBoolField(
      metadata.executable_metadata_debug_projection_matrix_published, out);
  EmitObjc3IRRuntimeSupportDebugProjectionBoolField(
      metadata.executable_metadata_debug_projection_fail_closed, out);
  EmitObjc3IRRuntimeSupportDebugProjectionBoolField(
      metadata
          .executable_metadata_debug_projection_manifest_debug_surface_published,
      out);
  EmitObjc3IRRuntimeSupportDebugProjectionBoolField(
      metadata.executable_metadata_debug_projection_ir_named_metadata_published,
      out);
  EmitObjc3IRRuntimeSupportDebugProjectionBoolField(
      metadata.executable_metadata_debug_projection_replay_anchor_deterministic,
      out);
  EmitObjc3IRRuntimeSupportDebugProjectionBoolField(
      metadata.executable_metadata_debug_projection_active_typed_handoff_ready,
      out);
  EmitObjc3IRRuntimeSupportDebugProjectionSizeField(
      metadata.executable_metadata_debug_projection_matrix_row_count, out);
}
