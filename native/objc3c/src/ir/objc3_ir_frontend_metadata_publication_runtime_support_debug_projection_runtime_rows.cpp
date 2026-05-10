#include "ir/objc3_ir_frontend_metadata_publication_runtime_support_debug_projection_runtime_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_support_debug_projection_row_helpers.h"

void BeginObjc3IRExecutableDebugProjectionMetadataNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  BeginObjc3IRRuntimeSupportDebugProjectionMetadataRow(
      "!54", metadata.executable_metadata_debug_projection_contract_id, out);
  EmitObjc3IRRuntimeSupportDebugProjectionStringField(
      metadata.executable_metadata_debug_projection_typed_handoff_contract_id,
      out);
  EmitObjc3IRRuntimeSupportDebugProjectionStringField(
      metadata.executable_metadata_debug_projection_source_graph_contract_id,
      out);
}

void EndObjc3IRExecutableDebugProjectionMetadataNode(
    std::ostringstream &out) {
  EndObjc3IRRuntimeSupportDebugProjectionMetadataRow(out);
}
