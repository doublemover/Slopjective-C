#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

struct Objc3IRFrontendRuntimeBootstrapMetadata {
  std::string runtime_metadata_archive_static_link_discovery_contract_id;
  std::string runtime_metadata_archive_static_link_anchor_seed_model;
  std::string
      runtime_metadata_archive_static_link_translation_unit_identity_model;
  std::string runtime_metadata_archive_static_link_merge_model;
  std::string runtime_metadata_archive_static_link_response_artifact_suffix;
  std::string runtime_metadata_archive_static_link_discovery_artifact_suffix;
  bool runtime_metadata_archive_static_link_discovery_ready = false;
  std::string runtime_metadata_archive_static_link_translation_unit_identity_key;
  std::string runtime_bootstrap_lowering_contract_id;
  std::string runtime_bootstrap_lowering_boundary_model;
  std::string runtime_bootstrap_lowering_constructor_root_symbol;
  std::string runtime_bootstrap_lowering_init_stub_symbol_prefix;
  std::string runtime_bootstrap_lowering_registration_table_symbol_prefix;
  std::string runtime_bootstrap_lowering_image_local_init_state_symbol_prefix;
  std::string runtime_bootstrap_lowering_registration_entrypoint_symbol;
  std::string runtime_bootstrap_lowering_global_ctor_list_model;
  std::string runtime_bootstrap_lowering_registration_table_layout_model;
  std::string runtime_bootstrap_lowering_image_local_initialization_model;
  std::string
      runtime_bootstrap_registration_descriptor_image_root_lowering_contract_id;
  std::string runtime_bootstrap_registration_descriptor_identifier;
  std::string runtime_bootstrap_image_root_identifier;
  std::uint64_t runtime_bootstrap_registration_order_ordinal = 1;
  std::size_t runtime_bootstrap_lowering_registration_table_abi_version = 0;
  std::size_t runtime_bootstrap_lowering_registration_table_pointer_field_count =
      0;
  std::string runtime_bootstrap_lowering_constructor_root_emission_state;
  std::string runtime_bootstrap_lowering_init_stub_emission_state;
  std::string runtime_bootstrap_lowering_registration_table_emission_state;
  bool runtime_bootstrap_lowering_bootstrap_ir_materialization_landed = false;
  bool runtime_bootstrap_lowering_image_local_initialization_landed = false;
  bool runtime_bootstrap_lowering_ready = false;
  bool runtime_bootstrap_lowering_fail_closed = false;
};
