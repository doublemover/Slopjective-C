#pragma once

#include <cstddef>
#include <string>
#include <vector>

struct Objc3ExecutableMetadataInterfaceGraphNode {
  std::string class_name;
  std::string owner_identity;
  std::string class_owner_identity;
  std::string metaclass_owner_identity;
  std::string super_class_owner_identity;
  std::string super_metaclass_owner_identity;
  std::string instance_method_owner_identity;
  std::string class_method_owner_identity;
  bool has_super = false;
  bool declaration_complete = false;
  std::size_t property_count = 0;
  std::size_t method_count = 0;
  std::size_t class_method_count = 0;
  std::size_t instance_method_count = 0;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3ExecutableMetadataImplementationGraphNode {
  std::string class_name;
  std::string owner_identity;
  std::string interface_owner_identity;
  std::string class_owner_identity;
  std::string metaclass_owner_identity;
  std::string super_class_owner_identity;
  std::string super_metaclass_owner_identity;
  std::string instance_method_owner_identity;
  std::string class_method_owner_identity;
  bool has_matching_interface = false;
  bool has_super = false;
  bool declaration_complete = false;
  std::size_t property_count = 0;
  std::size_t method_count = 0;
  std::size_t class_method_count = 0;
  std::size_t instance_method_count = 0;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3ExecutableMetadataClassGraphNode {
  std::string class_name;
  std::string owner_identity;
  std::string interface_owner_identity;
  std::string implementation_owner_identity;
  std::string metaclass_owner_identity;
  std::string super_class_owner_identity;
  std::string super_metaclass_owner_identity;
  std::vector<std::string> adopted_protocol_owner_identities_lexicographic;
  std::string instance_method_owner_identity;
  std::string class_method_owner_identity;
  bool has_interface = false;
  bool has_implementation = false;
  bool has_super = false;
  bool objc_final_declared = false;
  bool objc_sealed_declared = false;
  bool realization_identity_complete = false;
  std::size_t interface_property_count = 0;
  std::size_t implementation_property_count = 0;
  std::size_t interface_method_count = 0;
  std::size_t implementation_method_count = 0;
  std::size_t interface_class_method_count = 0;
  std::size_t implementation_class_method_count = 0;
  std::size_t interface_instance_method_count = 0;
  std::size_t implementation_instance_method_count = 0;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3ExecutableMetadataMetaclassGraphNode {
  std::string class_name;
  std::string owner_identity;
  std::string class_owner_identity;
  std::string interface_owner_identity;
  std::string implementation_owner_identity;
  std::string super_metaclass_owner_identity;
  bool derived_from_interface = false;
  bool has_implementation = false;
  bool has_super = false;
  std::size_t interface_class_method_count = 0;
  std::size_t implementation_class_method_count = 0;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3ExecutableMetadataProtocolGraphNode {
  std::string protocol_name;
  std::string owner_identity;
  std::vector<std::string> inherited_protocol_owner_identities_lexicographic;
  std::size_t property_count = 0;
  std::size_t method_count = 0;
  bool is_forward_declaration = false;
  bool declaration_complete = false;
  bool inherited_protocol_identity_complete = false;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3ExecutableMetadataCategoryGraphNode {
  std::string class_name;
  std::string category_name;
  std::string owner_identity;
  std::string interface_owner_identity;
  std::string implementation_owner_identity;
  std::string class_owner_identity;
  std::vector<std::string> adopted_protocol_owner_identities_lexicographic;
  bool has_interface = false;
  bool has_implementation = false;
  bool declaration_complete = false;
  bool attachment_identity_complete = false;
  bool conformance_identity_complete = false;
  std::size_t interface_property_count = 0;
  std::size_t implementation_property_count = 0;
  std::size_t interface_method_count = 0;
  std::size_t implementation_method_count = 0;
  std::size_t interface_class_method_count = 0;
  std::size_t implementation_class_method_count = 0;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3ExecutableMetadataPropertyGraphNode {
  std::string owner_kind;
  std::string owner_name;
  std::string owner_identity;
  std::string declaration_owner_identity;
  std::string export_owner_identity;
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
  std::string ownership_lifetime_profile;
  std::string ownership_runtime_hook_profile;
  std::string effective_getter_selector;
  bool effective_setter_available = false;
  std::string effective_setter_selector;
  std::string accessor_ownership_profile;
  bool synthesizes_executable_accessors = false;
  std::string getter_storage_runtime_helper_symbol;
  std::string setter_storage_runtime_helper_symbol;
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
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3ExecutableMetadataMethodGraphNode {
  std::string owner_kind;
  std::string owner_name;
  std::string owner_identity;
  std::string declaration_owner_identity;
  std::string export_owner_identity;
  std::string selector;
  bool is_class_method = false;
  bool has_body = false;
  bool effective_direct_dispatch = false;
  bool objc_final_declared = false;
  std::size_t parameter_count = 0;
  std::string return_type_name;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3ExecutableMetadataIvarGraphNode {
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
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3ExecutableMetadataGraphEdge {
  std::string edge_kind;
  std::string source_owner_identity;
  std::string target_owner_identity;
  unsigned line = 1;
  unsigned column = 1;
};
