#include "ir/objc3_ir_runtime_metadata_emission.h"

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_module_identity.h"
#include "lower/metadata/runtime_metadata_layout_policy.h"
#include "lower/objc3_lowering_contract.h"

#include <iomanip>
#include <sstream>

Objc3IRRuntimeMetadataSymbols BuildObjc3IRRuntimeMetadataSymbols(
    const std::string &module_name,
    const Objc3IRFrontendMetadata &frontend_metadata) {
  std::string seed = module_name;
  seed += "|";
  seed +=
      frontend_metadata.runtime_metadata_class_metaclass_typed_handoff_replay_key;
  seed += "|";
  seed += frontend_metadata
              .runtime_metadata_protocol_category_typed_handoff_replay_key;
  seed += "|";
  seed += frontend_metadata.runtime_metadata_member_table_typed_handoff_replay_key;
  seed += "|";
  seed += frontend_metadata
              .runtime_metadata_archive_static_link_translation_unit_identity_key;

  Objc3IRRuntimeMetadataSymbols symbols;
  symbols.linker_anchor_suffix =
      LowerHex64(StableRuntimeMetadataLinkerAnchorHash(seed));
  symbols.linker_anchor_symbol =
      "objc3_runtime_metadata_link_anchor_" + symbols.linker_anchor_suffix;
  symbols.discovery_root_symbol =
      "objc3_runtime_metadata_discovery_root_" + symbols.linker_anchor_suffix;

  const std::string safe_suffix = MakeModuleIdentifierSafeSuffix(
      frontend_metadata
          .runtime_metadata_archive_static_link_translation_unit_identity_key);
  symbols.module_name_global_symbol =
      ".objc3_runtime_module_name_" + safe_suffix;
  symbols.translation_unit_identity_global_symbol =
      ".objc3_runtime_translation_unit_identity_" + safe_suffix;
  symbols.image_descriptor_symbol =
      "__objc3_runtime_image_descriptor_" + safe_suffix;
  symbols.init_stub_symbol =
      frontend_metadata.runtime_bootstrap_lowering_init_stub_symbol_prefix +
      safe_suffix;
  symbols.registration_table_symbol =
      frontend_metadata
          .runtime_bootstrap_lowering_registration_table_symbol_prefix +
      safe_suffix;
  symbols.image_local_init_state_symbol =
      frontend_metadata
          .runtime_bootstrap_lowering_image_local_init_state_symbol_prefix +
      safe_suffix;

  symbols.registration_descriptor_identifier_safe_suffix =
      MakeModuleIdentifierSafeSuffix(
          frontend_metadata.runtime_bootstrap_registration_descriptor_identifier);
  symbols.image_root_identifier_safe_suffix = MakeModuleIdentifierSafeSuffix(
      frontend_metadata.runtime_bootstrap_image_root_identifier);
  symbols.registration_descriptor_name_global_symbol =
      ".objc3_runtime_registration_descriptor_name_" +
      symbols.registration_descriptor_identifier_safe_suffix;
  symbols.image_root_name_global_symbol =
      ".objc3_runtime_image_root_name_" +
      symbols.image_root_identifier_safe_suffix;
  symbols.registration_descriptor_symbol =
      std::string(kObjc3RuntimeBootstrapRegistrationDescriptorSymbolPrefix) +
      symbols.registration_descriptor_identifier_safe_suffix;
  symbols.image_root_symbol =
      std::string(kObjc3RuntimeBootstrapImageRootSymbolPrefix) +
      symbols.image_root_identifier_safe_suffix;
  return symbols;
}

bool Objc3IRRuntimeMetadataSectionScaffoldReady(
    const Objc3IRFrontendMetadata &frontend_metadata) {
  return frontend_metadata.runtime_metadata_section_ready_for_scaffold &&
         frontend_metadata.runtime_export_ready_for_runtime_export &&
         frontend_metadata.runtime_metadata_section_publication_emitted &&
         frontend_metadata.runtime_metadata_section_publication_fail_closed &&
         frontend_metadata.runtime_metadata_section_publication_uses_llvm_used &&
         frontend_metadata.runtime_metadata_section_publication_image_info_emitted;
}

bool Objc3IRRuntimeBootstrapLoweringReady(
    const Objc3IRFrontendMetadata &frontend_metadata) {
  return Objc3IRRuntimeMetadataSectionScaffoldReady(frontend_metadata) &&
         frontend_metadata.runtime_bootstrap_lowering_ready &&
         frontend_metadata.runtime_bootstrap_lowering_fail_closed &&
         !frontend_metadata.runtime_bootstrap_lowering_contract_id.empty() &&
         !frontend_metadata.runtime_bootstrap_lowering_constructor_root_symbol
              .empty() &&
         !frontend_metadata.runtime_bootstrap_lowering_init_stub_symbol_prefix
              .empty() &&
         !frontend_metadata
              .runtime_bootstrap_lowering_registration_table_symbol_prefix
              .empty() &&
         !frontend_metadata
              .runtime_bootstrap_lowering_image_local_init_state_symbol_prefix
              .empty() &&
         !frontend_metadata.runtime_bootstrap_lowering_registration_entrypoint_symbol
              .empty() &&
         !frontend_metadata.runtime_bootstrap_lowering_global_ctor_list_model
              .empty() &&
         !frontend_metadata.runtime_bootstrap_lowering_registration_table_layout_model
              .empty() &&
         !frontend_metadata
              .runtime_bootstrap_lowering_image_local_initialization_model
              .empty() &&
         frontend_metadata
                 .runtime_bootstrap_lowering_registration_table_abi_version >
             0 &&
         frontend_metadata
                 .runtime_bootstrap_lowering_registration_table_pointer_field_count >
             0 &&
         frontend_metadata
             .runtime_bootstrap_lowering_bootstrap_ir_materialization_landed &&
         frontend_metadata
             .runtime_bootstrap_lowering_image_local_initialization_landed &&
         !frontend_metadata
              .runtime_metadata_archive_static_link_translation_unit_identity_key
              .empty();
}

bool Objc3IRRuntimeBootstrapRegistrationDescriptorImageRootLoweringReady(
    const Objc3IRFrontendMetadata &frontend_metadata) {
  return Objc3IRRuntimeBootstrapLoweringReady(frontend_metadata) &&
         !frontend_metadata
              .runtime_bootstrap_registration_descriptor_image_root_lowering_contract_id
              .empty() &&
         !frontend_metadata.runtime_bootstrap_registration_descriptor_identifier
              .empty() &&
         !frontend_metadata.runtime_bootstrap_image_root_identifier.empty();
}

bool BuildObjc3IRRuntimeMetadataLayoutPolicy(
    const Objc3IRFrontendMetadata &frontend_metadata,
    Objc3RuntimeMetadataLayoutPolicy &policy, std::string &error) {
  Objc3RuntimeMetadataLayoutPolicyInput input;
  input.abi_contract_id =
      frontend_metadata.runtime_metadata_section_abi_contract_id;
  input.scaffold_contract_id =
      frontend_metadata.runtime_metadata_section_publication_contract_id;
  input.section_boundary_ready =
      frontend_metadata.runtime_metadata_section_ready_for_scaffold;
  input.runtime_export_ready =
      frontend_metadata.runtime_export_ready_for_runtime_export;
  input.scaffold_emitted =
      frontend_metadata.runtime_metadata_section_publication_emitted;
  input.scaffold_fail_closed =
      frontend_metadata.runtime_metadata_section_publication_fail_closed;
  input.uses_llvm_used =
      frontend_metadata.runtime_metadata_section_publication_uses_llvm_used;
  input.image_info_emitted =
      frontend_metadata.runtime_metadata_section_publication_image_info_emitted;
  input.image_info_symbol =
      frontend_metadata.runtime_metadata_section_publication_image_info_symbol;
  input.image_info_section =
      frontend_metadata.runtime_metadata_section_logical_image_info_section;
  input.descriptor_symbol_prefix =
      frontend_metadata.runtime_metadata_section_descriptor_symbol_prefix;
  input.descriptor_linkage =
      frontend_metadata.runtime_metadata_section_descriptor_linkage;
  input.aggregate_linkage =
      frontend_metadata.runtime_metadata_section_aggregate_linkage;
  input.metadata_visibility =
      frontend_metadata.runtime_metadata_section_visibility;
  input.retention_root = frontend_metadata.runtime_metadata_section_retention_root;
  input.total_retained_global_count =
      frontend_metadata
          .runtime_metadata_section_publication_total_retained_global_count;
  input.families = {{
      {kObjc3RuntimeMetadataLayoutPolicyClassFamily,
       frontend_metadata.runtime_metadata_section_logical_class_descriptor_section,
       frontend_metadata.runtime_metadata_section_publication_class_aggregate_symbol,
       frontend_metadata
           .runtime_metadata_section_publication_class_descriptor_count},
      {kObjc3RuntimeMetadataLayoutPolicyProtocolFamily,
       frontend_metadata
           .runtime_metadata_section_logical_protocol_descriptor_section,
       frontend_metadata
           .runtime_metadata_section_publication_protocol_aggregate_symbol,
       frontend_metadata
           .runtime_metadata_section_publication_protocol_descriptor_count},
      {kObjc3RuntimeMetadataLayoutPolicyCategoryFamily,
       frontend_metadata
           .runtime_metadata_section_logical_category_descriptor_section,
       frontend_metadata
           .runtime_metadata_section_publication_category_aggregate_symbol,
       frontend_metadata
           .runtime_metadata_section_publication_category_descriptor_count},
      {kObjc3RuntimeMetadataLayoutPolicyPropertyFamily,
       frontend_metadata
           .runtime_metadata_section_logical_property_descriptor_section,
       frontend_metadata
           .runtime_metadata_section_publication_property_aggregate_symbol,
       frontend_metadata
           .runtime_metadata_section_publication_property_descriptor_count},
      {kObjc3RuntimeMetadataLayoutPolicyIvarFamily,
       frontend_metadata.runtime_metadata_section_logical_ivar_descriptor_section,
       frontend_metadata.runtime_metadata_section_publication_ivar_aggregate_symbol,
       frontend_metadata.runtime_metadata_section_publication_ivar_descriptor_count},
  }};
  return TryBuildObjc3RuntimeMetadataLayoutPolicy(input, policy, error);
}

std::string FormatObjc3IRRuntimeMetadataDescriptorOrdinal(
    std::size_t ordinal) {
  std::ostringstream formatted;
  formatted << std::setw(4) << std::setfill('0') << ordinal;
  return formatted.str();
}

std::string BuildObjc3IRRuntimeMetadataDescriptorSymbol(
    const std::string &descriptor_symbol_prefix, const std::string &kind,
    std::size_t ordinal) {
  return "@" + descriptor_symbol_prefix + kind + "_" +
         FormatObjc3IRRuntimeMetadataDescriptorOrdinal(ordinal);
}

std::string BuildObjc3IRRuntimeMetadataAuxiliarySymbol(
    const std::string &descriptor_symbol_prefix, const std::string &kind,
    const std::string &suffix, std::size_t ordinal) {
  return "@" + descriptor_symbol_prefix + kind + "_" + suffix + "_" +
         FormatObjc3IRRuntimeMetadataDescriptorOrdinal(ordinal);
}

const char *Objc3IRRuntimeBootstrapImageDescriptorType() {
  return "{ ptr, ptr, i64, i64, i64, i64, i64, i64 }";
}

const char *Objc3IRRuntimeBootstrapRegistrationTableType() {
  return "{ i64, i64, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr, ptr }";
}
