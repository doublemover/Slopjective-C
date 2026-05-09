#include "io/objc3_runtime_artifact_document_renderers.h"

#include <sstream>

#include "io/objc3_process_internal.h"

std::string BuildObjc3RuntimeMetadataLinkerRetentionDiscoveryJson(
    const Objc3RuntimeMetadataLinkerRetentionArtifacts &artifacts) {
  std::ostringstream discovery;
  JsonObjectWriter discovery_document(discovery);
  discovery_document.StringField("contract_id",
                                 kObjc3RuntimeLinkerRetentionContractId);
  discovery_document.StringField("object_format", artifacts.object_format);
  discovery_document.StringField("object_artifact",
                                 artifacts.object_artifact_relative_path);
  discovery_document.StringField("linker_anchor_symbol",
                                 artifacts.linker_anchor_symbol);
  discovery_document.StringField("discovery_root_symbol",
                                 artifacts.discovery_root_symbol);
  discovery_document.StringField("linker_anchor_logical_section",
                                 artifacts.linker_anchor_logical_section);
  discovery_document.StringField("discovery_root_logical_section",
                                 artifacts.discovery_root_logical_section);
  discovery_document.StringField("linker_anchor_emitted_section",
                                 artifacts.linker_anchor_emitted_section);
  discovery_document.StringField("discovery_root_emitted_section",
                                 artifacts.discovery_root_emitted_section);
  discovery_document.StringField("linker_response_artifact_suffix",
                                 artifacts.linker_response_artifact_suffix);
  discovery_document.StringField("discovery_artifact_suffix",
                                 artifacts.discovery_artifact_suffix);
  discovery_document.StringField("translation_unit_identity_model",
                                 artifacts.translation_unit_identity_model);
  discovery_document.StringField("translation_unit_identity_key",
                                 artifacts.translation_unit_identity_key);
  discovery_document.StringArrayField("driver_linker_flags",
                                      {artifacts.driver_linker_flag});
  return FinishJsonObject(discovery_document, discovery);
}

std::string BuildObjc3RuntimeRegistrationDescriptorArtifactDocumentJson(
    const Objc3RuntimeRegistrationDescriptorArtifactInputs &inputs,
    const Objc3RuntimeMetadataLinkerRetentionArtifacts &linker_retention_artifacts,
    const std::string &constructor_init_stub_symbol,
    const std::string &bootstrap_registration_table_symbol,
    const std::string &bootstrap_image_local_init_state_symbol) {
  std::ostringstream out;
  JsonObjectWriter descriptor(out);
  descriptor.StringField("contract_id", inputs.contract_id);
  descriptor.StringField("registration_manifest_contract_id",
                         inputs.registration_manifest_contract_id);
  descriptor.StringField("source_surface_contract_id",
                         inputs.source_surface_contract_id);
  descriptor.StringField("bootstrap_lowering_contract_id",
                         inputs.bootstrap_lowering_contract_id);
  descriptor.StringField("payload_model", inputs.payload_model);
  descriptor.StringField("artifact", inputs.artifact_relative_path);
  descriptor.StringField("authority_model", inputs.authority_model);
  descriptor.StringField("translation_unit_identity_model",
                         inputs.translation_unit_identity_model);
  descriptor.StringField(
      "translation_unit_identity_key",
      linker_retention_artifacts.translation_unit_identity_key);
  descriptor.StringField("payload_ownership_model",
                         inputs.payload_ownership_model);
  descriptor.StringField("runtime_support_library_archive_relative_path",
                         inputs.runtime_support_library_archive_relative_path);
  descriptor.StringField("registration_entrypoint_symbol",
                         inputs.registration_entrypoint_symbol);
  descriptor.StringField("registration_descriptor_pragma_name",
                         inputs.registration_descriptor_pragma_name);
  descriptor.StringField("image_root_pragma_name",
                         inputs.image_root_pragma_name);
  descriptor.StringField("module_identity_source",
                         inputs.module_identity_source);
  descriptor.StringField("registration_descriptor_identifier",
                         inputs.registration_descriptor_identifier);
  descriptor.StringField("registration_descriptor_identity_source",
                         inputs.registration_descriptor_identity_source);
  descriptor.StringField("image_root_identifier", inputs.image_root_identifier);
  descriptor.StringField("image_root_identity_source",
                         inputs.image_root_identity_source);
  descriptor.StringField("bootstrap_visible_metadata_ownership_model",
                         inputs.bootstrap_visible_metadata_ownership_model);
  descriptor.StringField("constructor_root_symbol",
                         inputs.constructor_root_symbol);
  descriptor.StringField("constructor_init_stub_symbol",
                         constructor_init_stub_symbol);
  descriptor.StringField("bootstrap_registration_table_symbol",
                         bootstrap_registration_table_symbol);
  descriptor.StringField("bootstrap_image_local_init_state_symbol",
                         bootstrap_image_local_init_state_symbol);
  descriptor.StringField("bootstrap_registration_table_layout_model",
                         inputs.bootstrap_registration_table_layout_model);
  descriptor.StringField("bootstrap_image_local_initialization_model",
                         inputs.bootstrap_image_local_initialization_model);
  descriptor.StringField("bootstrap_constructor_root_emission_state",
                         inputs.bootstrap_constructor_root_emission_state);
  descriptor.StringField("bootstrap_init_stub_emission_state",
                         inputs.bootstrap_init_stub_emission_state);
  descriptor.StringField("bootstrap_registration_table_emission_state",
                         inputs.bootstrap_registration_table_emission_state);
  descriptor.UnsignedField("bootstrap_registration_table_abi_version",
                           inputs.bootstrap_registration_table_abi_version);
  descriptor.UnsignedField(
      "bootstrap_registration_table_pointer_field_count",
      inputs.bootstrap_registration_table_pointer_field_count);
  descriptor.SizeField("class_descriptor_count",
                       inputs.class_descriptor_count);
  descriptor.SizeField("protocol_descriptor_count",
                       inputs.protocol_descriptor_count);
  descriptor.SizeField("category_descriptor_count",
                       inputs.category_descriptor_count);
  descriptor.SizeField("property_descriptor_count",
                       inputs.property_descriptor_count);
  descriptor.SizeField("ivar_descriptor_count", inputs.ivar_descriptor_count);
  descriptor.SizeField("total_descriptor_count", inputs.total_descriptor_count);
  descriptor.UnsignedField("translation_unit_registration_order_ordinal",
                           inputs.translation_unit_registration_order_ordinal);
  descriptor.StringField("object_artifact", inputs.object_artifact_relative_path);
  descriptor.StringField("backend_artifact",
                         inputs.backend_artifact_relative_path);
  descriptor.StringField("object_format", linker_retention_artifacts.object_format);
  descriptor.StringField("linker_anchor_symbol",
                         linker_retention_artifacts.linker_anchor_symbol);
  descriptor.StringField("discovery_root_symbol",
                         linker_retention_artifacts.discovery_root_symbol);
  descriptor.BoolField("ready_for_descriptor_artifact_emission", true);
  descriptor.BoolField("ready_for_registration_descriptor_lowering", true);
  descriptor.BoolField("ready_for_loader_table_lowering", true);
  return FinishJsonObject(descriptor, out);
}

std::string BuildObjc3MetaprogrammingMacroHostProcessCacheArtifactDocumentJson(
    const Objc3MetaprogrammingMacroHostProcessCacheArtifactInputs &inputs,
    const std::filesystem::path &source_input_path,
    const std::string &cache_key,
    const std::filesystem::path &cache_entry,
    const std::filesystem::path &cache_summary_path,
    const std::filesystem::path &cache_runtime_import_path,
    const std::filesystem::path &cache_manifest_path,
    bool launch_attempted,
    bool cache_hit,
    int host_process_exit_code) {
  std::ostringstream out;
  JsonObjectWriter artifact(out);
  artifact.StringField("contract_id", inputs.contract_id);
  artifact.StringField("source_contract_id", inputs.source_contract_id);
  artifact.StringField("surface_path", inputs.surface_path);
  artifact.StringField("artifact", inputs.artifact_relative_path);
  artifact.StringField("host_executable_relative_path",
                       inputs.host_executable_relative_path);
  artifact.StringField("cache_root_relative_path",
                       inputs.cache_root_relative_path);
  artifact.StringField("cache_key", cache_key);
  artifact.StringField("cache_entry_relative_path",
                       cache_entry.generic_string());
  artifact.StringField("cache_summary_relative_path",
                       cache_summary_path.generic_string());
  artifact.StringField("cache_runtime_import_surface_relative_path",
                       cache_runtime_import_path.generic_string());
  artifact.StringField("cache_manifest_relative_path",
                       cache_manifest_path.generic_string());
  artifact.StringField("host_model", inputs.host_model);
  artifact.StringField("toolchain_model", inputs.toolchain_model);
  artifact.StringField("cache_model", inputs.cache_model);
  artifact.StringField("fail_closed_model", inputs.fail_closed_model);
  artifact.StringField("source_input_path", source_input_path.generic_string());
  artifact.BoolField("cache_ready", true);
  artifact.BoolField("launch_attempted", launch_attempted);
  artifact.BoolField("cache_hit", cache_hit);
  artifact.BoolField("cache_summary_present",
                     std::filesystem::exists(cache_summary_path));
  artifact.BoolField("cache_runtime_import_surface_present",
                     std::filesystem::exists(cache_runtime_import_path));
  artifact.BoolField("cache_manifest_present",
                     std::filesystem::exists(cache_manifest_path));
  artifact.StringField("cache_materialization_state",
                       launch_attempted ? "materialized" : "cache-hit");
  artifact.IntField("host_process_exit_code", host_process_exit_code);
  artifact.BoolField("deterministic", inputs.deterministic);
  artifact.StringField("replay_key", inputs.replay_key);
  return FinishJsonObject(artifact, out);
}
