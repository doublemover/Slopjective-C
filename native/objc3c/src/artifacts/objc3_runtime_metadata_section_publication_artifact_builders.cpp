#include "artifacts/objc3_runtime_metadata_section_publication_artifact_builders.h"

namespace objc3::artifacts::frontend::runtime_metadata_section_publication {

Objc3RuntimeMetadataSectionAbiFreezeSummary
RuntimeMetadataSectionPublicationArtifactBuilder::BuildAbiFreeze(
    const Objc3RuntimeMetadataSourceOwnershipBoundary
        &runtime_metadata_source_ownership,
    const Objc3RuntimeExportLegalityBoundary &runtime_export_legality,
    const Objc3RuntimeExportEnforcementSummary &runtime_export_enforcement) {
  Objc3RuntimeMetadataSectionAbiFreezeSummary summary;
  summary.boundary_frozen = true;
  summary.fail_closed = true;
  summary.object_file_section_inventory_frozen = true;
  summary.symbol_policy_frozen = true;
  summary.visibility_model_frozen = true;
  summary.retention_policy_frozen = true;
  summary.runtime_metadata_source_boundary_ready =
      IsReadyObjc3RuntimeMetadataSourceOwnershipBoundary(
          runtime_metadata_source_ownership);
  summary.runtime_export_legality_boundary_ready =
      IsReadyObjc3RuntimeExportLegalityBoundary(runtime_export_legality);
  summary.runtime_export_enforcement_ready =
      IsReadyObjc3RuntimeExportEnforcementSummary(runtime_export_enforcement);
  summary.ready_for_section_scaffold =
      summary.runtime_metadata_source_boundary_ready &&
      summary.runtime_export_legality_boundary_ready &&
      summary.runtime_export_enforcement_ready;
  if (!summary.ready_for_section_scaffold) {
    summary.failure_reason =
        "runtime metadata section ABI freeze prerequisites are not ready";
  }
  return summary;
}

Objc3RuntimeMetadataSectionPublicationSummary
RuntimeMetadataSectionPublicationArtifactBuilder::BuildPublication(
    const Objc3RuntimeMetadataSectionAbiFreezeSummary
        &runtime_metadata_section_abi,
    const Objc3RuntimeExportLegalityBoundary &runtime_export_legality,
    const Objc3RuntimeExportEnforcementSummary &runtime_export_enforcement) {
  Objc3RuntimeMetadataSectionPublicationSummary summary;
  summary.fail_closed = true;
  if (!IsReadyObjc3RuntimeMetadataSectionAbiFreezeSummary(
          runtime_metadata_section_abi) ||
      !IsReadyObjc3RuntimeExportEnforcementSummary(
          runtime_export_enforcement)) {
    summary.failure_reason =
        "runtime metadata section publication prerequisites are not ready";
    return summary;
  }

  summary.publication_emitted = true;
  summary.uses_llvm_used = true;
  summary.image_info_emitted = true;
  summary.class_descriptor_count = runtime_export_legality.class_record_count;
  summary.protocol_descriptor_count =
      runtime_export_legality.protocol_record_count;
  summary.category_descriptor_count =
      runtime_export_legality.category_record_count;
  summary.property_descriptor_count =
      runtime_export_legality.property_record_count;
  summary.ivar_descriptor_count = runtime_export_legality.ivar_record_count;
  summary.total_descriptor_count =
      summary.class_descriptor_count + summary.protocol_descriptor_count +
      summary.category_descriptor_count + summary.property_descriptor_count +
      summary.ivar_descriptor_count;
  summary.total_retained_global_count = summary.total_descriptor_count + 6u;
  return summary;
}

Objc3RuntimeMetadataObjectInspectionHarnessSummary
RuntimeMetadataSectionPublicationArtifactBuilder::BuildObjectInspectionHarness(
    const Objc3RuntimeMetadataSectionAbiFreezeSummary
        &runtime_metadata_section_abi,
    const Objc3RuntimeMetadataSectionPublicationSummary
        &runtime_metadata_section_publication) {
  Objc3RuntimeMetadataObjectInspectionHarnessSummary summary;
  summary.fail_closed = true;
  if (!IsReadyObjc3RuntimeMetadataSectionAbiFreezeSummary(
          runtime_metadata_section_abi) ||
      !IsReadyObjc3RuntimeMetadataSectionPublicationSummary(
          runtime_metadata_section_publication)) {
    summary.failure_reason =
        "runtime metadata object inspection harness prerequisites are not ready";
    return summary;
  }

  summary.matrix_published = true;
  summary.uses_llvm_readobj = true;
  summary.uses_llvm_objdump = true;
  summary.matrix_row_count = 2u;
  return summary;
}

}  // namespace objc3::artifacts::frontend::runtime_metadata_section_publication
