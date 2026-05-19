#include "pipeline/runtime_import_manifest_preservation.h"

namespace objc3c::pipeline {

bool ValidateImportedRuntimeRegistrationManifestDescriptorInventory(
    const Objc3ImportedRuntimeModulePackagingPeerArtifacts &artifacts,
    std::string &error) {
  if (artifacts.total_descriptor_count !=
      artifacts.class_descriptor_count + artifacts.protocol_descriptor_count +
          artifacts.category_descriptor_count +
          artifacts.property_descriptor_count + artifacts.ivar_descriptor_count) {
    error =
        "runtime registration manifest descriptor counts are internally inconsistent";
    return false;
  }
  return true;
}

}  // namespace objc3c::pipeline
