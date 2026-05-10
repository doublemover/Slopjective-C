#include "ir/objc3_ir_frontend_metadata_publication_runtime_support_debug_projection.h"

#include <sstream>

#include "ir/objc3_ir_c_string.h"
#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRExecutableDebugProjectionMetadataNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!54 = !{!\""
      << EscapeCStringLiteral(
             metadata.executable_metadata_debug_projection_contract_id)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.executable_metadata_debug_projection_typed_handoff_contract_id)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.executable_metadata_debug_projection_source_graph_contract_id)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.executable_metadata_debug_projection_named_metadata_name)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.executable_metadata_debug_projection_manifest_surface_path)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata
                 .executable_metadata_debug_projection_typed_handoff_surface_path)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.executable_metadata_debug_projection_source_graph_surface_path)
      << "\", i1 "
      << (metadata.executable_metadata_debug_projection_matrix_published ? 1 : 0)
      << ", i1 "
      << (metadata.executable_metadata_debug_projection_fail_closed ? 1 : 0)
      << ", i1 "
      << (metadata
                  .executable_metadata_debug_projection_manifest_debug_surface_published
              ? 1
              : 0)
      << ", i1 "
      << (metadata
                  .executable_metadata_debug_projection_ir_named_metadata_published
              ? 1
              : 0)
      << ", i1 "
      << (metadata
                  .executable_metadata_debug_projection_replay_anchor_deterministic
              ? 1
              : 0)
      << ", i1 "
      << (metadata
                  .executable_metadata_debug_projection_active_typed_handoff_ready
              ? 1
              : 0)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.executable_metadata_debug_projection_matrix_row_count)
      << ", !\""
      << EscapeCStringLiteral(
             metadata.executable_metadata_debug_projection_replay_key)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata
                 .executable_metadata_debug_projection_active_typed_handoff_replay_key)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.executable_metadata_debug_projection_row0_descriptor)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.executable_metadata_debug_projection_row1_descriptor)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.executable_metadata_debug_projection_row2_descriptor)
      << "\"}\n";
}
