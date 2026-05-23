#pragma once

#include "runtime/state/runtime_bootstrap_contracts.h"

#include <cstdint>

namespace objc3c::runtime {

struct EmittedMethodListRef {
  std::uint64_t count;
  const char *owner_identity;
  const void *method_list;
};

struct EmittedMethodListHeader {
  std::uint64_t count;
  const char *declaration_owner_identity;
  const char *export_owner_identity;
};

struct EmittedMethodListEntry {
  const char *selector;
  const char *owner_identity;
  const char *return_type_name;
  std::uint64_t parameter_count;
  const void *implementation;
  std::uint64_t has_body;
  bool effective_direct_dispatch;
  bool objc_final_declared;
};

struct EmittedKeyPathDescriptor {
  std::uint64_t stable_id;
  const char *root_name;
  const char *component_path;
  const char *profile;
  const char *generic_metadata_replay_key;
  const char *component_owner_identity_path;
  const char *component_member_identity_path;
  const char *component_type_identity_path;
  const char *source_span_id;
  const char *root_type_identity;
  const char *value_type_identity;
  const char *object_model_owner_identity;
  const char *object_model_member_identity;
  const char *debug_source_map_key;
  const char *diagnostic_anchor_key;
  std::uint32_t source_line;
  std::uint32_t source_column;
  bool root_is_self;
  bool fallback_interpretation_allowed;
};

struct EmittedClassRecord {
  const char *class_name;
  const char *bundle_owner_identity;
  const char *object_owner_identity;
  const char *super_owner_identity;
  const void *super_bundle;
  const EmittedMethodListRef *method_list_ref;
  const objc3_runtime_pointer_aggregate *adopted_protocol_refs;
  bool objc_final_declared;
  bool objc_sealed_declared;
};

struct EmittedClassBundle {
  EmittedClassRecord class_record;
  EmittedClassRecord metaclass_record;
};

struct EmittedProtocolRecord {
  const char *protocol_name;
  const char *owner_identity;
  const objc3_runtime_pointer_aggregate *inherited_protocol_refs;
  const EmittedMethodListRef *instance_method_list_ref;
  const EmittedMethodListRef *class_method_list_ref;
  std::uint64_t property_count;
  std::uint64_t method_count;
  std::uint64_t instance_method_count;
  std::uint64_t class_method_count;
  bool is_forward_declaration;
};

struct EmittedCategoryRecord {
  const char *class_name;
  const char *category_name;
  const char *record_kind;
  const char *owner_identity;
  const char *class_owner_identity;
  const char *category_owner_identity;
  const objc3_runtime_pointer_aggregate *attachments;
  const objc3_runtime_pointer_aggregate *adopted_protocol_refs;
  const EmittedMethodListRef *instance_method_list_ref;
  const EmittedMethodListRef *class_method_list_ref;
  std::uint64_t property_count;
  std::uint64_t instance_method_count;
  std::uint64_t class_method_count;
};

struct EmittedPropertyDescriptor {
  const char *property_name;
  const char *type_name;
  const char *owner_identity;
  const char *declaration_owner_identity;
  const char *export_owner_identity;
  const char *getter_selector;
  const char *setter_selector;
  const char *effective_getter_selector;
  const char *effective_setter_selector;
  const char *ivar_binding_symbol;
  const char *synthesized_binding_symbol;
  const char *ivar_layout_symbol;
  const char *property_attribute_profile;
  const char *ownership_lifetime_profile;
  const char *ownership_runtime_hook_profile;
  const char *accessor_ownership_profile;
  const void *getter_implementation;
  const void *setter_implementation;
  const char *ivar_layout_replay_key;
  std::uint64_t ivar_layout_slot_index;
  std::uint64_t ivar_layout_size_bytes;
  std::uint64_t ivar_layout_alignment_bytes;
  std::uint64_t ivar_layout_offset_bytes;
  std::uint64_t ivar_layout_padding_bytes;
  std::uint64_t ivar_layout_inherited_slot_count;
  std::uint64_t ivar_layout_inherited_size_bytes;
  std::uint64_t ivar_layout_owner_size_bytes;
  std::uint64_t ivar_init_order_index;
  std::uint64_t ivar_destroy_order_index;
  bool ivar_layout_valid;
  bool has_getter;
  bool has_setter;
  bool effective_setter_available;
};

struct EmittedIvarLayoutRecord {
  const char *layout_symbol;
  const char *layout_replay_key;
  std::uint64_t slot_index;
  std::uint64_t offset_bytes;
  std::uint64_t size_bytes;
  std::uint64_t alignment_bytes;
  std::uint64_t padding_bytes;
  std::uint64_t inherited_slot_count;
  std::uint64_t inherited_size_bytes;
  std::uint64_t owner_size_bytes;
  std::uint64_t init_order_index;
  std::uint64_t destroy_order_index;
  bool layout_valid;
};

struct EmittedIvarDescriptor {
  const char *owner_identity;
  const char *declaration_owner_identity;
  const char *export_owner_identity;
  const char *property_owner_identity;
  const char *property_name;
  const char *ivar_binding_symbol;
  const EmittedIvarLayoutRecord *layout_record;
  const std::uint64_t *offset_global;
  const char *layout_replay_key;
  std::uint64_t slot_index;
  std::uint64_t offset_bytes;
  std::uint64_t size_bytes;
  std::uint64_t alignment_bytes;
  std::uint64_t padding_bytes;
  std::uint64_t inherited_slot_count;
  std::uint64_t inherited_size_bytes;
  std::uint64_t owner_size_bytes;
  std::uint64_t init_order_index;
  std::uint64_t destroy_order_index;
  bool layout_valid;
};

}  // namespace objc3c::runtime
