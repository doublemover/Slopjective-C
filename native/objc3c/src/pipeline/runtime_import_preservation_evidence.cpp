#include "pipeline/runtime_import_preservation_owners.h"

#include "pipeline/runtime_import_type_system_preservation.h"

namespace objc3c::pipeline::runtime_import_preservation {

bool PopulatePreservationEvidence(const RuntimeImportJsonValue::Object &root,
                                  Objc3ImportedRuntimeModuleSurface &surface,
                                  std::string &error) {
  if (!PopulateFrontendClosureSummary(root,
                                      surface.frontend_closure_summary,
                                      error)) {
    return false;
  }
  if (!PopulateImportedTypeSystemOptionalKeypathSurface(root, surface, error)) {
    return false;
  }
  if (!PopulateImportedTypeSystemGenericContractPreservation(root, surface,
                                                            error)) {
    return false;
  }
  if (!PopulateImportedTypeSystemNullabilityContractPreservation(root, surface,
                                                                error)) {
    return false;
  }
  if (!PopulateImportedTypeSystemProtocolContractPreservation(root, surface,
                                                             error)) {
    return false;
  }
  if (!PopulateImportedRuntimeLanguageEvidence(root, surface, error)) {
    return false;
  }
  if (!PopulateImportedInteropEvidence(root, surface, error)) {
    return false;
  }
  if (!PopulateImportedMetaprogrammingEvidence(root, surface, error)) {
    return false;
  }
  if (!PopulateImportedDispatchEvidence(root, surface, error)) {
    return false;
  }
  return PopulateImportedRuntimeArtifactEvidence(root, surface, error);
}

}  // namespace objc3c::pipeline::runtime_import_preservation
