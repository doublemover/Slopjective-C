#pragma once

#include <cstddef>
#include <string>

struct Objc3IRFrontendRuntimeSectionsMetadata {
  std::string runtime_metadata_section_abi_contract_id;
  bool runtime_metadata_section_boundary_frozen = false;
  bool runtime_metadata_section_fail_closed = false;
  bool runtime_metadata_section_object_file_inventory_frozen = false;
  bool runtime_metadata_section_symbol_policy_frozen = false;
  bool runtime_metadata_section_visibility_model_frozen = false;
  bool runtime_metadata_section_retention_policy_frozen = false;
  bool runtime_metadata_section_ready_for_scaffold = false;
  std::string runtime_metadata_section_logical_image_info_section;
  std::string runtime_metadata_section_logical_class_descriptor_section;
  std::string runtime_metadata_section_logical_protocol_descriptor_section;
  std::string runtime_metadata_section_logical_category_descriptor_section;
  std::string runtime_metadata_section_logical_property_descriptor_section;
  std::string runtime_metadata_section_logical_ivar_descriptor_section;
  std::string runtime_metadata_section_descriptor_symbol_prefix;
  std::string runtime_metadata_section_aggregate_symbol_prefix;
  std::string runtime_metadata_section_image_info_symbol;
  std::string runtime_metadata_section_descriptor_linkage;
  std::string runtime_metadata_section_aggregate_linkage;
  std::string runtime_metadata_section_visibility;
  std::string runtime_metadata_section_retention_root;
  std::string runtime_metadata_section_publication_contract_id;
  std::string runtime_metadata_section_publication_abi_contract_id;
  bool runtime_metadata_section_publication_emitted = false;
  bool runtime_metadata_section_publication_fail_closed = false;
  bool runtime_metadata_section_publication_uses_llvm_used = false;
  bool runtime_metadata_section_publication_image_info_emitted = false;
  std::size_t runtime_metadata_section_publication_class_descriptor_count = 0;
  std::size_t runtime_metadata_section_publication_protocol_descriptor_count = 0;
  std::size_t runtime_metadata_section_publication_category_descriptor_count = 0;
  std::size_t runtime_metadata_section_publication_property_descriptor_count = 0;
  std::size_t runtime_metadata_section_publication_ivar_descriptor_count = 0;
  std::size_t runtime_metadata_section_publication_total_descriptor_count = 0;
  std::size_t runtime_metadata_section_publication_total_retained_global_count =
      0;
  std::string runtime_metadata_section_publication_image_info_symbol;
  std::string runtime_metadata_section_publication_class_aggregate_symbol;
  std::string runtime_metadata_section_publication_protocol_aggregate_symbol;
  std::string runtime_metadata_section_publication_category_aggregate_symbol;
  std::string runtime_metadata_section_publication_property_aggregate_symbol;
  std::string runtime_metadata_section_publication_ivar_aggregate_symbol;
};
