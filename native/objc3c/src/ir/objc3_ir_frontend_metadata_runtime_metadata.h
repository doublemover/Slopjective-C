#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "ir/objc3_ir_frontend_metadata_runtime_bootstrap.h"
#include "ir/objc3_ir_frontend_metadata_runtime_bundles.h"

struct Objc3IRFrontendRuntimeMetadata
    : Objc3IRFrontendRuntimeBootstrapMetadata {
  std::string runtime_metadata_source_ownership_contract_id;
  std::string runtime_metadata_source_schema;
  std::string runtime_metadata_ivar_source_model;
  std::size_t runtime_metadata_class_record_count = 0;
  std::size_t runtime_metadata_protocol_record_count = 0;
  std::size_t runtime_metadata_category_interface_record_count = 0;
  std::size_t runtime_metadata_category_implementation_record_count = 0;
  std::size_t runtime_metadata_property_record_count = 0;
  std::size_t runtime_metadata_method_record_count = 0;
  std::size_t runtime_metadata_ivar_record_count = 0;
  bool frontend_owns_runtime_metadata_source_records = false;
  bool runtime_metadata_source_records_ready_for_lowering = false;
  bool native_runtime_library_present = false;
  bool runtime_metadata_source_boundary_fail_closed = false;
  bool runtime_link_test_only = true;
  bool deterministic_runtime_metadata_source_schema = false;
  std::string runtime_export_legality_contract_id;
  bool runtime_export_semantic_boundary_frozen = false;
  bool runtime_export_metadata_export_enforcement_ready = false;
  bool runtime_export_fail_closed = false;
  bool runtime_export_duplicate_runtime_identity_enforcement_pending = true;
  bool runtime_export_incomplete_declaration_export_blocking_pending = true;
  bool runtime_export_illegal_redeclaration_mix_export_blocking_pending = true;
  std::size_t runtime_export_class_record_count = 0;
  std::size_t runtime_export_protocol_record_count = 0;
  std::size_t runtime_export_category_record_count = 0;
  std::size_t runtime_export_property_record_count = 0;
  std::size_t runtime_export_method_record_count = 0;
  std::size_t runtime_export_ivar_record_count = 0;
  std::size_t runtime_export_invalid_protocol_composition_sites = 0;
  std::size_t runtime_export_property_attribute_invalid_entries = 0;
  std::size_t runtime_export_property_attribute_contract_violations = 0;
  std::size_t runtime_export_invalid_type_annotation_sites = 0;
  std::size_t runtime_export_property_ivar_binding_missing = 0;
  std::size_t runtime_export_property_ivar_binding_conflicts = 0;
  std::size_t runtime_export_implementation_resolution_misses = 0;
  std::size_t runtime_export_method_resolution_misses = 0;
  bool runtime_export_boundary_ready = false;
  std::string runtime_export_enforcement_contract_id;
  bool runtime_export_metadata_completeness_enforced = false;
  bool runtime_export_duplicate_runtime_identity_suppression_enforced = false;
  bool runtime_export_illegal_redeclaration_mix_blocking_enforced = false;
  bool runtime_export_metadata_shape_drift_blocking_enforced = false;
  bool runtime_export_enforcement_fail_closed = false;
  bool runtime_export_ready_for_runtime_export = false;
  std::size_t runtime_export_duplicate_runtime_identity_sites = 0;
  std::size_t runtime_export_incomplete_declaration_sites = 0;
  std::size_t runtime_export_illegal_redeclaration_mix_sites = 0;
  std::size_t runtime_export_metadata_shape_drift_sites = 0;
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
  std::size_t runtime_metadata_section_publication_total_retained_global_count = 0;
  std::string runtime_metadata_section_publication_image_info_symbol;
  std::string runtime_metadata_section_publication_class_aggregate_symbol;
  std::string runtime_metadata_section_publication_protocol_aggregate_symbol;
  std::string runtime_metadata_section_publication_category_aggregate_symbol;
  std::string runtime_metadata_section_publication_property_aggregate_symbol;
  std::string runtime_metadata_section_publication_ivar_aggregate_symbol;
  std::string runtime_metadata_class_metaclass_emission_contract_id;
  std::string runtime_metadata_class_metaclass_payload_model;
  std::string runtime_metadata_class_metaclass_name_model;
  std::string runtime_metadata_class_metaclass_super_link_model;
  std::string runtime_metadata_class_metaclass_method_list_reference_model;
  std::string executable_class_metaclass_source_closure_contract_id;
  std::string executable_class_metaclass_parent_identity_model;
  std::string executable_class_metaclass_method_owner_identity_model;
  std::string executable_class_metaclass_object_identity_model;
  bool runtime_metadata_class_metaclass_emission_ready = false;
  bool runtime_metadata_class_metaclass_emission_fail_closed = false;
  bool executable_class_metaclass_source_closure_ready = false;
  std::size_t executable_class_metaclass_declaration_node_count = 0;
  std::size_t executable_class_metaclass_parent_identity_edge_count = 0;
  std::size_t executable_class_metaclass_method_owner_identity_edge_count = 0;
  std::size_t executable_class_metaclass_object_identity_edge_count = 0;
  std::string runtime_metadata_class_metaclass_typed_handoff_replay_key;
  std::vector<Objc3IRRuntimeMetadataClassMetaclassBundle>
      runtime_metadata_class_metaclass_bundles_lexicographic;
  std::string executable_protocol_category_source_closure_contract_id;
  std::string executable_protocol_inheritance_identity_model;
  std::string executable_category_attachment_identity_model;
  std::string executable_protocol_category_conformance_identity_model;
  std::string runtime_metadata_protocol_category_emission_contract_id;
  std::string runtime_metadata_protocol_emission_payload_model;
  std::string runtime_metadata_category_emission_payload_model;
  std::string runtime_metadata_protocol_reference_model;
  std::string runtime_metadata_category_attachment_model;
  bool executable_protocol_category_source_closure_ready = false;
  bool runtime_metadata_protocol_category_emission_ready = false;
  bool runtime_metadata_protocol_category_emission_fail_closed = false;
  std::size_t executable_protocol_category_protocol_node_count = 0;
  std::size_t executable_protocol_category_category_node_count = 0;
  std::size_t executable_protocol_inheritance_identity_edge_count = 0;
  std::size_t executable_category_attachment_identity_edge_count = 0;
  std::size_t executable_protocol_category_conformance_identity_edge_count = 0;
  std::string runtime_metadata_protocol_category_typed_handoff_replay_key;
  std::vector<Objc3IRRuntimeMetadataProtocolBundle>
      runtime_metadata_protocol_bundles_lexicographic;
  std::vector<Objc3IRRuntimeMetadataCategoryBundle>
      runtime_metadata_category_bundles_lexicographic;
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
