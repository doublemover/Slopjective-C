#include "io/objc3_runtime_registration_manifest_document.h"
#include "io/objc3_runtime_artifact_contracts.h"

bool TryBuildObjc3RuntimeTranslationUnitRegistrationManifestArtifact(
    const Objc3RuntimeTranslationUnitRegistrationManifestArtifactInputs &inputs,
    const Objc3RuntimeMetadataLinkerRetentionArtifacts
        &linker_retention_artifacts,
    std::size_t runtime_metadata_binary_byte_count,
    std::string &manifest_json,
    std::string &error) {
  manifest_json.clear();
  error.clear();
  if (!objc3c::io::ValidateRuntimeTranslationUnitRegistrationManifestArtifactInputs(
          inputs, linker_retention_artifacts, error)) {
    return false;
  }

  const Objc3RuntimeRegistrationSymbolOwnerRecord symbol_owner_record =
      objc3c::io::BuildRuntimeTranslationUnitRegistrationSymbolOwnerRecord(
          inputs, linker_retention_artifacts);

  manifest_json = BuildObjc3RuntimeTranslationUnitRegistrationManifestDocumentJson(
      inputs, linker_retention_artifacts, symbol_owner_record,
      runtime_metadata_binary_byte_count);
  return true;
}
