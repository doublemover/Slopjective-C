#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_retention_archive_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_retention_row_helpers.h"

void EmitObjc3IRRuntimeObjectArchiveStaticLinkDiscoveryMetadataNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  BeginObjc3IRRuntimeObjectRetentionMetadataRow(
      "!63",
      metadata.runtime_metadata_archive_static_link_discovery_contract_id, out);
  EmitObjc3IRRuntimeObjectRetentionStringField(
      metadata.runtime_metadata_archive_static_link_anchor_seed_model, out);
  EmitObjc3IRRuntimeObjectRetentionStringField(
      metadata
          .runtime_metadata_archive_static_link_translation_unit_identity_model,
      out);
  EmitObjc3IRRuntimeObjectRetentionStringField(
      metadata.runtime_metadata_archive_static_link_merge_model, out);
  EmitObjc3IRRuntimeObjectRetentionBoolField(
      metadata.runtime_metadata_archive_static_link_discovery_ready, out);
  EmitObjc3IRRuntimeObjectRetentionStringField(
      metadata.runtime_metadata_archive_static_link_response_artifact_suffix,
      out);
  EmitObjc3IRRuntimeObjectRetentionStringField(
      metadata.runtime_metadata_archive_static_link_discovery_artifact_suffix,
      out);
  EmitObjc3IRRuntimeObjectRetentionStringField(
      metadata.runtime_metadata_archive_static_link_translation_unit_identity_key,
      out);
  EndObjc3IRRuntimeObjectRetentionMetadataRow(out);
}
