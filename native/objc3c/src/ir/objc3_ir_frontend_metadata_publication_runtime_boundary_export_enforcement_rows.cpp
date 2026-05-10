#include "ir/objc3_ir_frontend_metadata_publication_runtime_boundary_export_enforcement_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_boundary_export_row_helpers.h"

void EmitObjc3IRRuntimeExportEnforcementMetadataNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  BeginObjc3IRRuntimeBoundaryExportMetadataRow(
      "!47", metadata.runtime_export_enforcement_contract_id, out);
  EmitObjc3IRRuntimeBoundaryExportBoolField(
      metadata.runtime_export_metadata_completeness_enforced, out);
  EmitObjc3IRRuntimeBoundaryExportBoolField(
      metadata
          .runtime_export_duplicate_runtime_identity_suppression_enforced,
      out);
  EmitObjc3IRRuntimeBoundaryExportBoolField(
      metadata.runtime_export_illegal_redeclaration_mix_blocking_enforced,
      out);
  EmitObjc3IRRuntimeBoundaryExportBoolField(
      metadata.runtime_export_metadata_shape_drift_blocking_enforced, out);
  EmitObjc3IRRuntimeBoundaryExportBoolField(
      metadata.runtime_export_enforcement_fail_closed, out);
  EmitObjc3IRRuntimeBoundaryExportBoolField(
      metadata.runtime_export_ready_for_runtime_export, out);
  EmitObjc3IRRuntimeBoundaryExportSizeField(
      metadata.runtime_export_duplicate_runtime_identity_sites, out);
  EmitObjc3IRRuntimeBoundaryExportSizeField(
      metadata.runtime_export_incomplete_declaration_sites, out);
  EmitObjc3IRRuntimeBoundaryExportSizeField(
      metadata.runtime_export_illegal_redeclaration_mix_sites, out);
  EmitObjc3IRRuntimeBoundaryExportSizeField(
      metadata.runtime_export_metadata_shape_drift_sites, out);
  EndObjc3IRRuntimeBoundaryExportMetadataRow(out);
}
