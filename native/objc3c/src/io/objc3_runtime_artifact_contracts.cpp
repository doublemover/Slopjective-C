#include "io/objc3_runtime_artifact_contracts.h"

#include "io/objc3_process_internal.h"
#include "runtime/metadata/runtime_ownership_contracts.h"

namespace objc3c::io {

namespace {

Objc3RuntimeRegistrationSymbolOwnerRecord BuildRuntimeRegistrationSymbolOwnerRecord(
    const std::string &constructor_root_symbol,
    const std::string &constructor_init_stub_symbol_prefix,
    const std::string &bootstrap_registration_table_symbol_prefix,
    const std::string &bootstrap_image_local_init_state_symbol_prefix,
    const std::string &translation_unit_identity_model,
    const Objc3RuntimeMetadataLinkerRetentionArtifacts
        &linker_retention_artifacts) {
  const std::string safe_identity_suffix =
      objc3c::support::MakeIdentifierSafeSuffix(
          linker_retention_artifacts.translation_unit_identity_key,
          "translation_unit");
  return Objc3RuntimeRegistrationSymbolOwnerRecord{
      constructor_root_symbol,
      constructor_init_stub_symbol_prefix + safe_identity_suffix,
      bootstrap_registration_table_symbol_prefix + safe_identity_suffix,
      bootstrap_image_local_init_state_symbol_prefix + safe_identity_suffix,
      translation_unit_identity_model,
      linker_retention_artifacts.translation_unit_identity_key};
}

}  // namespace

Objc3RuntimeRegistrationSymbolOwnerRecord
BuildRuntimeTranslationUnitRegistrationSymbolOwnerRecord(
    const Objc3RuntimeTranslationUnitRegistrationManifestArtifactInputs &inputs,
    const Objc3RuntimeMetadataLinkerRetentionArtifacts
        &linker_retention_artifacts) {
  return BuildRuntimeRegistrationSymbolOwnerRecord(
      inputs.constructor_root_symbol, inputs.constructor_init_stub_symbol_prefix,
      inputs.bootstrap_registration_table_symbol_prefix,
      inputs.bootstrap_image_local_init_state_symbol_prefix,
      inputs.translation_unit_identity_model, linker_retention_artifacts);
}

Objc3RuntimeRegistrationSymbolOwnerRecord
BuildRuntimeRegistrationDescriptorSymbolOwnerRecord(
    const Objc3RuntimeRegistrationDescriptorArtifactInputs &inputs,
    const Objc3RuntimeMetadataLinkerRetentionArtifacts
        &linker_retention_artifacts) {
  return BuildRuntimeRegistrationSymbolOwnerRecord(
      inputs.constructor_root_symbol, inputs.constructor_init_stub_symbol_prefix,
      inputs.bootstrap_registration_table_symbol_prefix,
      inputs.bootstrap_image_local_init_state_symbol_prefix,
      inputs.translation_unit_identity_model, linker_retention_artifacts);
}

bool ValidateRuntimeRegistrationDescriptorArtifactInputs(
    const Objc3RuntimeRegistrationDescriptorArtifactInputs &inputs,
    const Objc3RuntimeMetadataLinkerRetentionArtifacts &linker_retention_artifacts,
    std::string &error) {
  if (inputs.contract_id.empty() ||
      inputs.registration_manifest_contract_id.empty() ||
      inputs.source_surface_contract_id.empty() ||
      inputs.bootstrap_lowering_contract_id.empty() ||
      inputs.payload_model.empty() ||
      inputs.artifact_relative_path.empty() || inputs.authority_model.empty() ||
      inputs.translation_unit_identity_model.empty() ||
      inputs.payload_ownership_model.empty() ||
      inputs.runtime_support_library_archive_relative_path.empty() ||
      inputs.registration_entrypoint_symbol.empty() ||
      inputs.registration_descriptor_pragma_name.empty() ||
      inputs.image_root_pragma_name.empty() || inputs.module_identity_source.empty() ||
      inputs.registration_descriptor_identifier.empty() ||
      inputs.registration_descriptor_identity_source.empty() ||
      inputs.image_root_identifier.empty() ||
      inputs.image_root_identity_source.empty() ||
      inputs.bootstrap_visible_metadata_ownership_model.empty() ||
      inputs.constructor_root_symbol.empty() ||
      inputs.constructor_init_stub_symbol_prefix.empty() ||
      inputs.bootstrap_registration_table_symbol_prefix.empty() ||
      inputs.bootstrap_image_local_init_state_symbol_prefix.empty() ||
      inputs.bootstrap_registration_table_layout_model.empty() ||
      inputs.bootstrap_image_local_initialization_model.empty() ||
      inputs.bootstrap_constructor_root_emission_state.empty() ||
      inputs.bootstrap_init_stub_emission_state.empty() ||
      inputs.bootstrap_registration_table_emission_state.empty() ||
      inputs.bootstrap_registration_table_abi_version == 0 ||
      inputs.bootstrap_registration_table_pointer_field_count == 0 ||
      inputs.total_descriptor_count !=
          inputs.class_descriptor_count + inputs.protocol_descriptor_count +
              inputs.category_descriptor_count +
              inputs.property_descriptor_count + inputs.ivar_descriptor_count ||
      inputs.translation_unit_registration_order_ordinal == 0 ||
      inputs.object_artifact_relative_path.empty() ||
      inputs.backend_artifact_relative_path.empty() ||
      !objc3c::runtime::RuntimeOwnerSplitContractIsReady() ||
      linker_retention_artifacts.translation_unit_identity_key.empty() ||
      linker_retention_artifacts.translation_unit_identity_model.empty() ||
      linker_retention_artifacts.object_format.empty() ||
      linker_retention_artifacts.linker_anchor_symbol.empty() ||
      linker_retention_artifacts.discovery_root_symbol.empty()) {
    error =
        "runtime registration descriptor artifact inputs incomplete for " +
        inputs.artifact_relative_path;
    return false;
  }
  if (linker_retention_artifacts.translation_unit_identity_model !=
      inputs.translation_unit_identity_model) {
    error =
        "runtime registration descriptor identity model drifted from linker-retention artifacts";
    return false;
  }
  return true;
}

bool ValidateMetaprogrammingMacroHostProcessCacheArtifactInputs(
    const Objc3MetaprogrammingMacroHostProcessCacheArtifactInputs &inputs,
    const std::filesystem::path &source_input_path,
    std::string &error) {
  if (inputs.contract_id.empty() || inputs.source_contract_id.empty() ||
      inputs.surface_path.empty() || inputs.artifact_relative_path.empty() ||
      inputs.host_executable_relative_path.empty() ||
      inputs.cache_root_relative_path.empty() || inputs.host_model.empty() ||
      inputs.toolchain_model.empty() || inputs.cache_model.empty() ||
      inputs.fail_closed_model.empty() || inputs.replay_key.empty()) {
    error = "metaprogramming macro host process/cache artifact inputs are incomplete";
    return false;
  }
  if (!std::filesystem::exists(source_input_path)) {
    error = "metaprogramming macro host process/cache source input not found: " +
            source_input_path.generic_string();
    return false;
  }
  return true;
}

}  // namespace objc3c::io
