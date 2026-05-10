#include "ir/objc3_ir_frontend_metadata_publication_runtime_boundary_export_runtime_boundary_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_boundary_export_legality_rows.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_boundary_export_row_helpers.h"

void EmitObjc3IRRuntimeExportBoundaryMetadataNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  BeginObjc3IRRuntimeBoundaryExportMetadataRow(
      "!46", metadata.runtime_export_legality_contract_id, out);
  EmitObjc3IRRuntimeBoundaryExportBoolField(
      metadata.runtime_export_semantic_boundary_frozen, out);
  EmitObjc3IRRuntimeBoundaryExportBoolField(
      metadata.runtime_export_metadata_export_enforcement_ready, out);
  EmitObjc3IRRuntimeBoundaryExportBoolField(
      metadata.runtime_export_fail_closed, out);
  EmitObjc3IRRuntimeExportBoundaryLegalityFields(metadata, out);
  EmitObjc3IRRuntimeBoundaryExportBoolField(
      metadata.runtime_export_boundary_ready, out);
  EndObjc3IRRuntimeBoundaryExportMetadataRow(out);
}
