#include "io/objc3_process_internal.h"
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

  const std::string safe_identity_suffix =
      objc3c::support::MakeIdentifierSafeSuffix(
          linker_retention_artifacts.translation_unit_identity_key,
          "translation_unit");
  const std::string constructor_init_stub_symbol =
      inputs.constructor_init_stub_symbol_prefix + safe_identity_suffix;
  const std::string bootstrap_registration_table_symbol =
      inputs.bootstrap_registration_table_symbol_prefix + safe_identity_suffix;
  const std::string bootstrap_image_local_init_state_symbol =
      inputs.bootstrap_image_local_init_state_symbol_prefix +
      safe_identity_suffix;

  descriptor_json = BuildObjc3RuntimeRegistrationDescriptorArtifactDocumentJson(
      inputs, linker_retention_artifacts, constructor_init_stub_symbol,
      bootstrap_registration_table_symbol,
      bootstrap_image_local_init_state_symbol);
  return true;
}
