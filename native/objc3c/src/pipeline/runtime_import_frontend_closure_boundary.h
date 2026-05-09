#pragma once

#include <cstddef>
#include <string>

#include "pipeline/objc3_frontend_types.h"
#include "support/objc3_runtime_metadata_record_set.h"

namespace objc3c::pipeline {

inline void PreserveImportedRuntimeMetadataSourceRecordInventory(
    Objc3RuntimeAwareImportModuleFrontendClosureSummary &summary,
    const Objc3RuntimeMetadataSourceRecordSet &records) {
  summary.runtime_metadata_source_records_ready = true;
  summary.class_record_count = records.classes_lexicographic.size();
  summary.protocol_record_count = records.protocols_lexicographic.size();
  summary.category_record_count = records.categories_lexicographic.size();
  summary.property_record_count = records.properties_lexicographic.size();
  summary.method_record_count = records.methods_lexicographic.size();
  summary.ivar_record_count = records.ivars_lexicographic.size();
}

inline bool ValidateImportedRuntimeFrontendClosureInventory(
    const Objc3RuntimeAwareImportModuleFrontendClosureSummary &summary,
    const Objc3RuntimeMetadataSourceRecordSet &records,
    std::size_t metadata_reference_count,
    std::string &error) {
  const std::size_t declaration_count =
      objc3c::support::CountRuntimeMetadataSourceRecordSetDeclarations(records);
  if (summary.runtime_owned_declaration_count != declaration_count) {
    error =
        "runtime-owned declaration count does not match imported record inventory";
    return false;
  }
  if (summary.metadata_reference_count != metadata_reference_count) {
    error =
        "metadata reference count does not match imported reference inventory";
    return false;
  }
  return true;
}

inline bool ValidateImportedRuntimeFrontendClosureHardCutoverBoundary(
    const Objc3RuntimeAwareImportModuleFrontendClosureSummary &summary,
    std::string &error) {
  if (summary.contract_id !=
      kObjc3RuntimeAwareImportModuleFrontendClosureContractId) {
    error = "unexpected import-surface contract id";
    return false;
  }
  if (summary.source_surface_contract_id !=
      kObjc3RuntimeAwareImportModuleSurfaceContractId) {
    error = "unexpected import-surface source contract id";
    return false;
  }
  if (summary.frontend_surface_path !=
      kObjc3RuntimeAwareImportModuleFrontendClosureSurfacePath) {
    error = "unexpected import-surface frontend surface path";
    return false;
  }
  if (summary.payload_model !=
      kObjc3RuntimeAwareImportModuleFrontendClosurePayloadModel) {
    error = "unexpected import-surface payload model";
    return false;
  }
  if (summary.artifact_relative_path !=
      kObjc3RuntimeAwareImportModuleFrontendClosureArtifactRelativePath) {
    error = "unexpected import-surface artifact name";
    return false;
  }
  if (summary.authority_model !=
      kObjc3RuntimeAwareImportModuleFrontendAuthorityModel) {
    error = "unexpected import-surface authority model";
    return false;
  }
  if (summary.payload_ownership_model !=
      kObjc3RuntimeAwareImportModuleFrontendPayloadOwnershipModel) {
    error = "unexpected import-surface ownership model";
    return false;
  }
  if (!summary.ready_for_frontend_module_consumption) {
    error =
        "import-surface artifact is not ready for frontend module consumption";
    return false;
  }
  if (!IsReadyObjc3RuntimeAwareImportModuleFrontendClosureSummary(summary)) {
    error = "import-surface closure summary is incomplete";
    return false;
  }
  return true;
}

}  // namespace objc3c::pipeline
