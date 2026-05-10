#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "ir/objc3_ir_frontend_metadata_runtime_bundles.h"

struct Objc3IRFrontendRuntimeMemberStorageMetadata {
  std::string runtime_metadata_member_table_emission_contract_id;
  std::string runtime_metadata_method_list_emission_payload_model;
  std::string runtime_metadata_method_list_grouping_model;
  std::string runtime_metadata_property_descriptor_emission_payload_model;
  std::string runtime_metadata_ivar_descriptor_emission_payload_model;
  bool runtime_metadata_member_table_emission_ready = false;
  bool runtime_metadata_member_table_emission_fail_closed = false;
  std::string runtime_metadata_member_table_typed_handoff_replay_key;
  std::size_t executable_property_attribute_profile_entries = 0;
  std::size_t executable_accessor_ownership_profile_entries = 0;
  std::size_t executable_synthesized_binding_entries = 0;
  std::size_t executable_ivar_layout_entries = 0;
  std::string executable_property_ivar_source_model_replay_key;
  std::string executable_ivar_layout_emission_contract_id;
  std::string executable_ivar_layout_descriptor_model;
  std::string executable_ivar_offset_global_model;
  std::string executable_ivar_layout_table_model;
  bool executable_ivar_layout_emission_ready = false;
  bool executable_ivar_layout_emission_fail_closed = false;
  std::size_t executable_ivar_offset_global_entries = 0;
  std::size_t executable_ivar_layout_table_entries = 0;
  std::size_t executable_ivar_layout_owner_entries = 0;
  std::string executable_ivar_layout_emission_replay_key;
  std::vector<Objc3IRRuntimeMetadataMethodListBundle>
      runtime_metadata_method_list_bundles_lexicographic;
  std::vector<Objc3IRRuntimeMetadataPropertyBundle>
      runtime_metadata_property_bundles_lexicographic;
  std::vector<Objc3IRRuntimeMetadataIvarBundle>
      runtime_metadata_ivar_bundles_lexicographic;
};
