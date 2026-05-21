#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OBJC3_RUNTIME_REFLECTION_ABI_VERSION 4u

typedef enum objc3_runtime_reflection_status_code {
  OBJC3_RUNTIME_REFLECTION_STATUS_OK = 0,
  OBJC3_RUNTIME_REFLECTION_STATUS_NOT_FOUND = 1,
  OBJC3_RUNTIME_REFLECTION_STATUS_INVALID_OUTPUT = -1,
  OBJC3_RUNTIME_REFLECTION_STATUS_INVALID_QUERY = -2,
  OBJC3_RUNTIME_REFLECTION_STATUS_UNAVAILABLE = -3,
  OBJC3_RUNTIME_REFLECTION_STATUS_MALFORMED_METADATA = -4,
} objc3_runtime_reflection_status_code;

typedef enum objc3_runtime_reflection_method_family {
  OBJC3_RUNTIME_REFLECTION_METHOD_FAMILY_INVALID = 0,
  OBJC3_RUNTIME_REFLECTION_METHOD_FAMILY_INSTANCE = 1,
  OBJC3_RUNTIME_REFLECTION_METHOD_FAMILY_CLASS = 2,
} objc3_runtime_reflection_method_family;

typedef enum objc3_runtime_reflection_surface_kind {
  OBJC3_RUNTIME_REFLECTION_SURFACE_INVALID = 0,
  OBJC3_RUNTIME_REFLECTION_SURFACE_STATE = 1,
  OBJC3_RUNTIME_REFLECTION_SURFACE_CLASS = 2,
  OBJC3_RUNTIME_REFLECTION_SURFACE_PROPERTY = 3,
  OBJC3_RUNTIME_REFLECTION_SURFACE_METHOD = 4,
  OBJC3_RUNTIME_REFLECTION_SURFACE_PROTOCOL = 5,
  OBJC3_RUNTIME_REFLECTION_SURFACE_PROTOCOL_CONFORMANCE = 6,
  OBJC3_RUNTIME_REFLECTION_SURFACE_CATEGORY = 7,
  OBJC3_RUNTIME_REFLECTION_SURFACE_SELECTOR = 8,
} objc3_runtime_reflection_surface_kind;

typedef struct objc3_runtime_reflection_state_snapshot {
  uint32_t abi_version;
  uint32_t snapshot_size;
  int status;
  uint64_t realized_class_count;
  uint64_t root_class_count;
  uint64_t reflectable_property_count;
  uint64_t attached_category_count;
  uint64_t protocol_descriptor_count;
  uint64_t protocol_conformance_edge_count;
  uint64_t selector_table_entry_count;
  uint64_t metadata_backed_selector_count;
  uint64_t dynamic_selector_count;
  uint64_t method_cache_entry_count;
  uint64_t class_graph_generation;
  uint64_t category_attachment_generation;
  uint64_t protocol_declaration_generation;
  uint64_t storage_surface_generation;
  uint64_t method_surface_generation;
  const char *public_api_owner;
  const char *result_lifetime_model;
  const char *unsupported_metadata_policy;
} objc3_runtime_reflection_state_snapshot;

typedef struct objc3_runtime_reflection_class_snapshot {
  uint32_t abi_version;
  uint32_t snapshot_size;
  int status;
  int found;
  int is_root_class;
  int has_super_class;
  int implementation_backed;
  int objc_final_declared;
  int objc_sealed_declared;
  uint64_t registration_order_ordinal;
  uint64_t direct_protocol_count;
  uint64_t attached_protocol_count;
  uint64_t attached_category_count;
  uint64_t runtime_property_count;
  uint64_t runtime_instance_size_bytes;
  const char *class_name;
  const char *super_class_name;
  const char *module_name;
  const char *translation_unit_identity_key;
} objc3_runtime_reflection_class_snapshot;

typedef struct objc3_runtime_reflection_property_snapshot {
  uint32_t abi_version;
  uint32_t snapshot_size;
  int status;
  int found;
  int inherited;
  int setter_available;
  int has_runtime_getter;
  int has_runtime_setter;
  int is_readonly;
  int is_nonatomic;
  int is_strong;
  int is_assign;
  int is_weak;
  int is_copy;
  int has_custom_getter;
  int has_custom_setter;
  int layout_valid;
  uint64_t attribute_count;
  uint64_t slot_index;
  uint64_t offset_bytes;
  uint64_t size_bytes;
  uint64_t alignment_bytes;
  uint64_t padding_bytes;
  uint64_t inherited_slot_count;
  uint64_t inherited_size_bytes;
  uint64_t owner_size_bytes;
  uint64_t init_order_index;
  uint64_t destroy_order_index;
  uint64_t instance_size_bytes;
  const char *queried_class_name;
  const char *resolved_class_name;
  const char *property_name;
  const char *type_name;
  const char *getter_selector;
  const char *setter_selector;
  const char *effective_getter_selector;
  const char *effective_setter_selector;
  const char *property_attribute_profile;
  const char *property_behavior_name;
  const char *ownership_lifetime_profile;
  const char *ownership_runtime_hook_profile;
  const char *accessor_ownership_profile;
} objc3_runtime_reflection_property_snapshot;

typedef struct objc3_runtime_reflection_method_snapshot {
  uint32_t abi_version;
  uint32_t snapshot_size;
  int status;
  int found;
  int family;
  int inherited;
  int declared_in_category;
  int has_body;
  int effective_direct_dispatch;
  int objc_final_declared;
  uint64_t selector_stable_id;
  uint64_t parameter_count;
  uint64_t category_probe_count;
  uint64_t protocol_probe_count;
  const char *queried_class_name;
  const char *resolved_class_name;
  const char *selector;
  const char *return_type_name;
  const char *return_kind;
  const char *category_name;
} objc3_runtime_reflection_method_snapshot;

typedef struct objc3_runtime_reflection_protocol_snapshot {
  uint32_t abi_version;
  uint32_t snapshot_size;
  int status;
  int found;
  uint64_t inherited_protocol_count;
  uint64_t property_count;
  uint64_t method_count;
  uint64_t instance_method_count;
  uint64_t class_method_count;
  const char *protocol_name;
} objc3_runtime_reflection_protocol_snapshot;

typedef struct objc3_runtime_reflection_protocol_conformance_snapshot {
  uint32_t abi_version;
  uint32_t snapshot_size;
  int status;
  int class_found;
  int protocol_found;
  int conforms;
  int matched_from_category;
  int matched_from_superclass;
  int matched_via_inherited_protocol;
  uint64_t visited_protocol_count;
  uint64_t attached_category_count;
  uint64_t matched_protocol_depth;
  const char *class_name;
  const char *protocol_name;
  const char *matched_class_name;
  const char *failure_reason;
} objc3_runtime_reflection_protocol_conformance_snapshot;

typedef struct objc3_runtime_reflection_category_snapshot {
  uint32_t abi_version;
  uint32_t snapshot_size;
  int status;
  int found;
  uint64_t adopted_protocol_count;
  uint64_t property_count;
  uint64_t instance_method_count;
  uint64_t class_method_count;
  const char *class_name;
  const char *category_name;
  const char *record_kind;
} objc3_runtime_reflection_category_snapshot;

typedef struct objc3_runtime_reflection_selector_snapshot {
  uint32_t abi_version;
  uint32_t snapshot_size;
  int status;
  int found;
  int metadata_backed;
  uint64_t stable_id;
  uint64_t metadata_provider_count;
  uint64_t first_registration_order_ordinal;
  uint64_t last_registration_order_ordinal;
  const char *canonical_selector;
} objc3_runtime_reflection_selector_snapshot;

typedef struct objc3_runtime_reflection_surface_snapshot {
  uint32_t abi_version;
  uint32_t snapshot_size;
  int status;
  int surface_kind;
  int issue_ref;
  int supported;
  int realized_state_backed;
  int bounded_public_abi;
  int fail_closed;
  int creates_dynamic_runtime_state;
  int exposes_private_testing_snapshot;
  int unsupported_metadata_status;
  int malformed_metadata_status;
  const char *support_claim;
  const char *surface_name;
  const char *entrypoint_name;
  const char *snapshot_type_name;
  const char *runtime_anchor;
  const char *query_boundary;
  const char *unsupported_policy;
} objc3_runtime_reflection_surface_snapshot;

uint32_t objc3_runtime_reflection_api_abi_version(void);
uint64_t objc3_runtime_reflection_surface_count(void);
int objc3_runtime_copy_reflection_surface(
    uint64_t index, objc3_runtime_reflection_surface_snapshot *snapshot);
int objc3_runtime_copy_reflection_surface_by_kind(
    int surface_kind, objc3_runtime_reflection_surface_snapshot *snapshot);
int objc3_runtime_copy_reflection_state(
    objc3_runtime_reflection_state_snapshot *snapshot);
int objc3_runtime_copy_reflection_class(
    const char *class_name, objc3_runtime_reflection_class_snapshot *snapshot);
int objc3_runtime_copy_reflection_class_at(
    uint64_t index, objc3_runtime_reflection_class_snapshot *snapshot);
int objc3_runtime_copy_reflection_property(
    const char *class_name, const char *property_name,
    objc3_runtime_reflection_property_snapshot *snapshot);
int objc3_runtime_copy_reflection_property_at(
    const char *class_name, uint64_t index,
    objc3_runtime_reflection_property_snapshot *snapshot);
int objc3_runtime_copy_reflection_method(
    const char *class_name, const char *selector, int family,
    objc3_runtime_reflection_method_snapshot *snapshot);
int objc3_runtime_copy_reflection_method_at(
    const char *class_name, uint64_t index, int family,
    objc3_runtime_reflection_method_snapshot *snapshot);
int objc3_runtime_copy_reflection_protocol(
    const char *protocol_name,
    objc3_runtime_reflection_protocol_snapshot *snapshot);
int objc3_runtime_copy_reflection_protocol_at(
    uint64_t index, objc3_runtime_reflection_protocol_snapshot *snapshot);
int objc3_runtime_copy_reflection_protocol_conformance(
    const char *class_name, const char *protocol_name,
    objc3_runtime_reflection_protocol_conformance_snapshot *snapshot);
int objc3_runtime_copy_reflection_category(
    const char *class_name, const char *category_name,
    objc3_runtime_reflection_category_snapshot *snapshot);
int objc3_runtime_copy_reflection_category_at(
    const char *class_name, uint64_t index,
    objc3_runtime_reflection_category_snapshot *snapshot);
int objc3_runtime_copy_reflection_selector(
    const char *selector, objc3_runtime_reflection_selector_snapshot *snapshot);
int objc3_runtime_copy_reflection_selector_at(
    uint64_t index, objc3_runtime_reflection_selector_snapshot *snapshot);

#ifdef __cplusplus
}
#endif
