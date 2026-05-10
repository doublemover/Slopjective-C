#include "pipeline/runtime_import_preservation_owners.h"

#include <string>

#include "pipeline/runtime_import_preservation_runtime_artifact_evidence_owners.h"

namespace objc3c::pipeline::runtime_import_preservation {

bool PopulateImportedRuntimeArtifactEvidence(
    const RuntimeImportJsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  if (!PopulateImportedRuntimeBlockOwnershipArtifactPreservation(root, surface,
                                                                error)) {
    return false;
  }
  return PopulateImportedRuntimeStorageReflectionArtifactPreservation(root, surface,
                                                                     error);
}

}  // namespace objc3c::pipeline::runtime_import_preservation
