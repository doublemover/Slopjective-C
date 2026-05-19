#include "ir/objc3_ir_frontend_metadata_publication_runtime_boundary_export_legality_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_boundary_export_row_helpers.h"

void EmitObjc3IRRuntimeExportBoundaryLegalityFields(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRRuntimeBoundaryExportBoolField(
      metadata.runtime_export_duplicate_runtime_identity_enforcement_pending,
      out);
  EmitObjc3IRRuntimeBoundaryExportBoolField(
      metadata.runtime_export_incomplete_declaration_export_blocking_pending,
      out);
  EmitObjc3IRRuntimeBoundaryExportBoolField(
      metadata
          .runtime_export_illegal_redeclaration_mix_export_blocking_pending,
      out);
  EmitObjc3IRRuntimeBoundaryExportSizeField(
      metadata.runtime_export_class_record_count, out);
  EmitObjc3IRRuntimeBoundaryExportSizeField(
      metadata.runtime_export_protocol_record_count, out);
  EmitObjc3IRRuntimeBoundaryExportSizeField(
      metadata.runtime_export_category_record_count, out);
  EmitObjc3IRRuntimeBoundaryExportSizeField(
      metadata.runtime_export_property_record_count, out);
  EmitObjc3IRRuntimeBoundaryExportSizeField(
      metadata.runtime_export_method_record_count, out);
  EmitObjc3IRRuntimeBoundaryExportSizeField(
      metadata.runtime_export_ivar_record_count, out);
  EmitObjc3IRRuntimeBoundaryExportSizeField(
      metadata.runtime_export_invalid_protocol_composition_sites, out);
  EmitObjc3IRRuntimeBoundaryExportSizeField(
      metadata.runtime_export_property_attribute_invalid_entries, out);
  EmitObjc3IRRuntimeBoundaryExportSizeField(
      metadata.runtime_export_property_attribute_contract_violations, out);
  EmitObjc3IRRuntimeBoundaryExportSizeField(
      metadata.runtime_export_invalid_type_annotation_sites, out);
  EmitObjc3IRRuntimeBoundaryExportSizeField(
      metadata.runtime_export_property_ivar_binding_missing, out);
  EmitObjc3IRRuntimeBoundaryExportSizeField(
      metadata.runtime_export_property_ivar_binding_conflicts, out);
  EmitObjc3IRRuntimeBoundaryExportSizeField(
      metadata.runtime_export_implementation_resolution_misses, out);
  EmitObjc3IRRuntimeBoundaryExportSizeField(
      metadata.runtime_export_method_resolution_misses, out);
}
