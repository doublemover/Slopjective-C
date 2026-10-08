#pragma once

#include <cstddef>
#include <string>
#include <vector>

struct Objc3IRRuntimeMetadataClassMetaclassBundle {
  std::string class_name;
  std::string owner_identity;
  std::string class_owner_identity;
  std::string metaclass_owner_identity;
  std::string super_bundle_owner_identity;
  std::string super_class_owner_identity;
  std::string super_metaclass_owner_identity;
  std::vector<std::string> adopted_protocol_owner_identities_lexicographic;
  std::string instance_method_owner_identity;
  std::string class_method_owner_identity;
  bool has_super = false;
  bool objc_final_declared = false;
  bool objc_sealed_declared = false;
  std::size_t instance_method_count = 0;
  std::size_t class_method_count = 0;
};

struct Objc3IRRuntimeMetadataProtocolBundle {
  std::string protocol_name;
  std::string owner_identity;
  std::vector<std::string> inherited_protocol_owner_identities_lexicographic;
  std::size_t property_count = 0;
  std::size_t method_count = 0;
  bool is_forward_declaration = false;
};

struct Objc3IRRuntimeMetadataCategoryBundle {
  std::string record_kind;
  std::string class_name;
  std::string category_name;
  std::string owner_identity;
  std::string category_owner_identity;
  std::string class_owner_identity;
  std::vector<std::string> adopted_protocol_owner_identities_lexicographic;
  std::size_t property_count = 0;
  std::size_t instance_method_count = 0;
  std::size_t class_method_count = 0;
};

struct Objc3IRRuntimeMetadataMethodEntry {
  std::string owner_identity;
  std::string selector;
  std::string return_type_name;
  std::size_t parameter_count = 0;
  bool has_body = false;
  bool effective_direct_dispatch = false;
  bool objc_final_declared = false;
  bool throws_error_out_abi_ready = false;
};

struct Objc3IRRuntimeMetadataMethodListBundle {
  std::string owner_kind;
  std::string owner_name;
  std::string owner_family_kind;
  std::string declaration_owner_identity;
  std::string export_owner_identity;
  std::string list_kind;
  std::vector<Objc3IRRuntimeMetadataMethodEntry> entries_lexicographic;
};

struct Objc3IRRuntimeMetadataPropertyBundle {
  std::string owner_kind;
  std::string owner_name;
  std::string owner_identity;
  std::string declaration_owner_identity;
  std::string export_owner_identity;
  bool synthesizes_executable_accessors = false;
  std::string property_name;
  std::string type_name;
  bool has_getter = false;
  std::string getter_selector;
  bool has_setter = false;
  std::string setter_selector;
  std::string ivar_binding_symbol;
  std::string executable_synthesized_binding_kind;
  std::string executable_synthesized_binding_symbol;
  std::string property_attribute_profile;
  bool property_behavior_declared = false;
  std::string property_behavior_name;
  std::string ownership_lifetime_profile;
  std::string ownership_runtime_hook_profile;
  std::string effective_getter_selector;
  bool effective_setter_available = false;
  std::string effective_setter_selector;
  std::string accessor_ownership_profile;
  std::string executable_ivar_layout_symbol;
  std::size_t executable_ivar_layout_slot_index = 0;
  std::size_t executable_ivar_layout_size_bytes = 0;
  std::size_t executable_ivar_layout_alignment_bytes = 0;
  std::size_t executable_ivar_layout_offset_bytes = 0;
  std::size_t executable_ivar_layout_padding_bytes = 0;
  std::size_t executable_ivar_layout_inherited_slot_count = 0;
  std::size_t executable_ivar_layout_inherited_size_bytes = 0;
  std::size_t executable_ivar_layout_owner_size_bytes = 0;
  std::size_t executable_ivar_init_order_index = 0;
  std::size_t executable_ivar_destroy_order_index = 0;
  bool executable_ivar_layout_valid = false;
  std::string executable_ivar_layout_replay_key;
};

struct Objc3IRRuntimeMetadataIvarBundle {
  std::string owner_kind;
  std::string owner_name;
  std::string owner_identity;
  std::string declaration_owner_identity;
  std::string export_owner_identity;
  std::string property_owner_identity;
  std::string property_name;
  std::string ivar_binding_symbol;
  std::string executable_synthesized_binding_kind;
  std::string executable_synthesized_binding_symbol;
  std::string executable_ivar_layout_symbol;
  std::size_t executable_ivar_layout_slot_index = 0;
  std::size_t executable_ivar_layout_size_bytes = 0;
  std::size_t executable_ivar_layout_alignment_bytes = 0;
  std::size_t executable_ivar_layout_offset_bytes = 0;
  std::size_t executable_ivar_layout_padding_bytes = 0;
  std::size_t executable_ivar_layout_inherited_slot_count = 0;
  std::size_t executable_ivar_layout_inherited_size_bytes = 0;
  std::size_t executable_ivar_layout_owner_size_bytes = 0;
  std::size_t executable_ivar_init_order_index = 0;
  std::size_t executable_ivar_destroy_order_index = 0;
  bool executable_ivar_layout_valid = false;
  std::string executable_ivar_layout_replay_key;
};

struct Objc3IRMetaprogrammingDerivedMethodBundle {
  std::string implementation_name;
  std::string declaration_owner_identity;
  std::string export_owner_identity;
  std::string derive_name;
  std::string selector;
  std::string emitted_symbol;
  std::size_t parameter_count = 0;
  unsigned line = 1;
  unsigned column = 1;
};

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

struct Objc3IRFrontendRuntimeSourceClosureMetadata {
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
};
