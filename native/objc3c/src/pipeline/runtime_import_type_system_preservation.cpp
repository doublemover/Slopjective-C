#include "pipeline/runtime_import_type_system_preservation.h"

#include "pipeline/runtime_import_preservation_owners.h"

namespace objc3c::pipeline {

bool PopulateImportedTypeSystemOptionalKeypathSurface(
    const RuntimeImportJsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  return runtime_import_preservation::
      PopulateImportedTypeSystemOptionalKeypathSurfaceEvidence(root, surface,
                                                              error);
}

bool PopulateImportedTypeSystemGenericContractPreservation(
    const RuntimeImportJsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  return runtime_import_preservation::
      PopulateImportedTypeSystemGenericContractEvidence(root, surface, error);
}

bool PopulateImportedTypeSystemNullabilityContractPreservation(
    const RuntimeImportJsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  return runtime_import_preservation::
      PopulateImportedTypeSystemNullabilityContractEvidence(root, surface,
                                                           error);
}

bool PopulateImportedTypeSystemProtocolContractPreservation(
    const RuntimeImportJsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  return runtime_import_preservation::
      PopulateImportedTypeSystemProtocolContractEvidence(root, surface, error);
}

}  // namespace objc3c::pipeline
