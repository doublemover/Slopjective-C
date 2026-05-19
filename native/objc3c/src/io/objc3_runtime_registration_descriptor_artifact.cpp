#include "io/objc3_runtime_artifact_contracts.h"
#include "io/objc3_runtime_registration_descriptor_document.h"

bool TryBuildObjc3RuntimeRegistrationDescriptorArtifact(
    const Objc3RuntimeRegistrationDescriptorArtifactInputs &inputs,
    const Objc3RuntimeMetadataLinkerRetentionArtifacts
        &linker_retention_artifacts,
    std::string &descriptor_json,
    std::string &error) {
  descriptor_json.clear();
  error.clear();
  if (!objc3c::io::ValidateRuntimeRegistrationDescriptorArtifactInputs(
          inputs, linker_retention_artifacts, error)) {
    return false;
  }

  const Objc3RuntimeRegistrationSymbolOwnerRecord symbol_owner_record =
      objc3c::io::BuildRuntimeRegistrationDescriptorSymbolOwnerRecord(
          inputs, linker_retention_artifacts);

  descriptor_json = BuildObjc3RuntimeRegistrationDescriptorArtifactDocumentJson(
      inputs, linker_retention_artifacts, symbol_owner_record);
  return true;
}
