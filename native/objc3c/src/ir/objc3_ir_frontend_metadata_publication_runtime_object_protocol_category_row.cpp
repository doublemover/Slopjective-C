#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_protocol_category_row.h"

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_protocol_category_row_helpers.h"

void EmitObjc3IRRuntimeProtocolCategoryMetadataRow(
    const Objc3IRFrontendMetadata &metadata,
    const Objc3IRRuntimeProtocolCategoryProtocolCounts &protocol_counts,
    const Objc3IRRuntimeProtocolCategoryCategoryCounts &category_counts,
    std::ostringstream &out) {
  BeginObjc3IRRuntimeProtocolCategoryMetadataRow(
      "!57", metadata.runtime_metadata_protocol_category_emission_contract_id,
      out);
  EmitObjc3IRRuntimeProtocolCategoryStringField(
      metadata.runtime_metadata_protocol_emission_payload_model, out);
  EmitObjc3IRRuntimeProtocolCategoryStringField(
      metadata.runtime_metadata_category_emission_payload_model, out);
  EmitObjc3IRRuntimeProtocolCategoryStringField(
      metadata.runtime_metadata_protocol_reference_model, out);
  EmitObjc3IRRuntimeProtocolCategoryStringField(
      metadata.runtime_metadata_category_attachment_model, out);
  EmitObjc3IRRuntimeProtocolCategoryBoolField(
      metadata.runtime_metadata_protocol_category_emission_ready, out);
  EmitObjc3IRRuntimeProtocolCategoryBoolField(
      metadata.runtime_metadata_protocol_category_emission_fail_closed, out);
  EmitObjc3IRRuntimeProtocolCategorySizeField(protocol_counts.bundle_count,
                                              out);
  EmitObjc3IRRuntimeProtocolCategorySizeField(
      protocol_counts.inherited_reference_total, out);
  EmitObjc3IRRuntimeProtocolCategorySizeField(category_counts.bundle_count,
                                              out);
  EmitObjc3IRRuntimeProtocolCategorySizeField(
      category_counts.adopted_reference_total, out);
  EmitObjc3IRRuntimeProtocolCategorySizeField(
      category_counts.attachment_reference_total, out);
  EmitObjc3IRRuntimeProtocolCategoryStringField(
      metadata.runtime_metadata_protocol_category_typed_handoff_replay_key,
      out);
  EndObjc3IRRuntimeProtocolCategoryMetadataRow(out);
}
