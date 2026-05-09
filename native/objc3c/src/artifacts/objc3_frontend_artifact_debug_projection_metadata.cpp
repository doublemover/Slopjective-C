#include "artifacts/objc3_frontend_artifact_debug_projection_metadata.h"

namespace objc3::artifacts::frontend {

void ApplyObjc3FrontendDebugProjectionMetadata(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const Objc3ExecutableMetadataDebugProjectionSummary
        &executable_metadata_debug_projection) {
  ir_frontend_metadata.executable_metadata_debug_projection_contract_id =
      executable_metadata_debug_projection.contract_id;
  ir_frontend_metadata
      .executable_metadata_debug_projection_typed_handoff_contract_id =
      executable_metadata_debug_projection.typed_lowering_handoff_contract_id;
  ir_frontend_metadata
      .executable_metadata_debug_projection_source_graph_contract_id =
      executable_metadata_debug_projection.source_graph_contract_id;
  ir_frontend_metadata.executable_metadata_debug_projection_named_metadata_name =
      executable_metadata_debug_projection.named_metadata_name;
  ir_frontend_metadata
      .executable_metadata_debug_projection_manifest_surface_path =
      executable_metadata_debug_projection.manifest_surface_path;
  ir_frontend_metadata
      .executable_metadata_debug_projection_typed_handoff_surface_path =
      executable_metadata_debug_projection.typed_handoff_surface_path;
  ir_frontend_metadata
      .executable_metadata_debug_projection_source_graph_surface_path =
      executable_metadata_debug_projection.source_graph_surface_path;
  ir_frontend_metadata.executable_metadata_debug_projection_matrix_published =
      executable_metadata_debug_projection.matrix_published;
  ir_frontend_metadata.executable_metadata_debug_projection_fail_closed =
      executable_metadata_debug_projection.fail_closed;
  ir_frontend_metadata
      .executable_metadata_debug_projection_manifest_debug_surface_published =
      executable_metadata_debug_projection.manifest_debug_surface_published;
  ir_frontend_metadata
      .executable_metadata_debug_projection_ir_named_metadata_published =
      executable_metadata_debug_projection.ir_named_metadata_published;
  ir_frontend_metadata
      .executable_metadata_debug_projection_replay_anchor_deterministic =
      executable_metadata_debug_projection.replay_anchor_deterministic;
  ir_frontend_metadata
      .executable_metadata_debug_projection_active_typed_handoff_ready =
      executable_metadata_debug_projection.active_typed_handoff_ready;
  ir_frontend_metadata.executable_metadata_debug_projection_matrix_row_count =
      executable_metadata_debug_projection.matrix_row_count;
  ir_frontend_metadata.executable_metadata_debug_projection_replay_key =
      executable_metadata_debug_projection.replay_key;
  ir_frontend_metadata
      .executable_metadata_debug_projection_active_typed_handoff_replay_key =
      executable_metadata_debug_projection.active_typed_handoff_replay_key;
  ir_frontend_metadata.executable_metadata_debug_projection_row0_descriptor =
      BuildExecutableMetadataDebugProjectionRowDescriptor(
          executable_metadata_debug_projection.rows[0]);
  ir_frontend_metadata.executable_metadata_debug_projection_row1_descriptor =
      BuildExecutableMetadataDebugProjectionRowDescriptor(
          executable_metadata_debug_projection.rows[1]);
  ir_frontend_metadata.executable_metadata_debug_projection_row2_descriptor =
      BuildExecutableMetadataDebugProjectionRowDescriptor(
          executable_metadata_debug_projection.rows[2]);
}

}  // namespace objc3::artifacts::frontend
