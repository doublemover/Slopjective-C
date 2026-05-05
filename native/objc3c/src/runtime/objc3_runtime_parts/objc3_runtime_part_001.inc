#if defined(_WIN32) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif
#if defined(_WIN32) && !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif
#if defined(_WIN32) && !defined(NOMINMAX)
#define NOMINMAX
#endif

#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <deque>
#include <memory>
#include <mutex>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#if defined(_WIN32)
#include <windows.h>
#endif

namespace {

constexpr std::uint64_t kReceiverIdentityBase = 1024;
constexpr std::uint64_t kReceiverIdentityStride = 17;
constexpr const char *kObjc3DefaultMetaprogrammingHostCacheRoot =
    "tmp/artifacts/objc3c-native/cache/metaprogramming";
[[maybe_unused]] constexpr const char *kObjc3ConformancePublicationContractId =
    "objc3c.driver.conformance.report.publication.v1";
[[maybe_unused]] constexpr const char *kObjc3ConformanceClaimOperationsContractId =
    "objc3c.toolchain.conformance.claim.operations.v1";
[[maybe_unused]] constexpr const char *kObjc3ConformanceClaimSelectionModel =
    "canonical-language-profile-drives-current-publication-surface";
[[maybe_unused]] constexpr const char *kObjc3ConformanceClaimFailureModel =
    "strictness-and-strict-concurrency-remain-fail-closed-and-unclaimed";
[[maybe_unused]] constexpr const char
    *kObjc3AdvancedFeatureOpsContractId =
        "objc3c.advanced.feature.ci.runbook.dashboard.contract.v1";
[[maybe_unused]] constexpr const char
    *kObjc3AdvancedFeatureReportingContractId =
        "objc3c.tooling.feature.aware.conformance.report.emission.v1";
[[maybe_unused]] constexpr const char
    *kObjc3AdvancedFeatureReleaseEvidenceContractId =
        "objc3c.tooling.corpus.sharding.release.evidence.packaging.v1";
[[maybe_unused]] constexpr const char
    *kObjc3AdvancedFeatureEvidenceGateScriptPath =
    "scripts/check_release_evidence.py";

const char *MetaprogrammingHostCacheRootForTesting() {
#if defined(_WIN32)
  char buffer[32768] = {};
  const DWORD length = GetEnvironmentVariableA(
      "OBJC3C_METAPROGRAMMING_CACHE_ROOT", buffer,
      static_cast<DWORD>(sizeof(buffer)));
  if (length == 0 || length >= sizeof(buffer)) {
    return kObjc3DefaultMetaprogrammingHostCacheRoot;
  }
  static std::string cached_override;
  cached_override.assign(buffer, static_cast<std::size_t>(length));
  return cached_override.c_str();
#else
  const char *override_value = std::getenv("OBJC3C_METAPROGRAMMING_CACHE_ROOT");
  if (override_value == nullptr || override_value[0] == '\0') {
    return kObjc3DefaultMetaprogrammingHostCacheRoot;
  }
  static std::string cached_override;
  cached_override = override_value;
  return cached_override.c_str();
#endif
}
[[maybe_unused]] constexpr const char
    *kObjc3AdvancedFeatureEvidenceRunbookPath =
        "spec/conformance/release_evidence_gate_maintenance.md";
[[maybe_unused]] constexpr const char
    *kObjc3AdvancedFeatureDashboardSchemaPath =
        "schemas/objc3-conformance-dashboard-status-v1.schema.json";
[[maybe_unused]] constexpr const char
    *kObjc3AdvancedFeatureReleaseEvidenceOperationContractId =
        "objc3c.tooling.release.evidence.toolchain.operations.v1";
[[maybe_unused]] constexpr const char
    *kObjc3AdvancedFeatureDashboardStatusPublicationContractId =
        "objc3c.tooling.dashboard.status.publication.v1";

struct SelectorSlot {
  std::string spelling_storage;
  objc3_runtime_selector_handle handle{};
  bool metadata_backed = false;
  std::uint64_t metadata_provider_count = 0;
  std::uint64_t first_registration_order_ordinal = 0;
  std::uint64_t last_registration_order_ordinal = 0;
  std::uint64_t first_selector_pool_index = 0;
  std::uint64_t last_selector_pool_index = 0;
};

struct KeyPathSlot {
  std::uint64_t stable_id = 0;
  std::string root_name_storage;
  std::string component_path_storage;
  std::string profile_storage;
  std::string generic_metadata_replay_key_storage;
  bool root_is_self = false;
  bool ambiguous = false;
  std::uint64_t component_count = 0;
  std::uint64_t metadata_provider_count = 0;
  std::uint64_t first_registration_order_ordinal = 0;
  std::uint64_t last_registration_order_ordinal = 0;
};

struct RegisteredImageMetadata {
  std::string module_name;
  std::string translation_unit_identity_key;
  std::uint64_t registration_order_ordinal = 0;
  const objc3_runtime_registration_table *registration_table = nullptr;
  const objc3_runtime_pointer_aggregate *discovery_root = nullptr;
  const objc3_runtime_pointer_aggregate *class_descriptor_root = nullptr;
  const objc3_runtime_pointer_aggregate *protocol_descriptor_root = nullptr;
  const objc3_runtime_pointer_aggregate *category_descriptor_root = nullptr;
  const objc3_runtime_pointer_aggregate *property_descriptor_root = nullptr;
  const objc3_runtime_pointer_aggregate *ivar_descriptor_root = nullptr;
  const objc3_runtime_pointer_aggregate *selector_pool_root = nullptr;
  const objc3_runtime_pointer_aggregate *string_pool_root = nullptr;
  const objc3_runtime_pointer_aggregate *keypath_descriptor_root = nullptr;
  std::uint64_t discovery_root_entry_count = 0;
  std::uint64_t class_descriptor_count = 0;
  std::uint64_t protocol_descriptor_count = 0;
  std::uint64_t category_descriptor_count = 0;
  std::uint64_t property_descriptor_count = 0;
  std::uint64_t ivar_descriptor_count = 0;
  std::uint64_t selector_pool_count = 0;
  std::uint64_t string_pool_count = 0;
  std::uint64_t keypath_descriptor_count = 0;
  bool linker_anchor_matches_discovery_root = false;
  bool used_staged_registration_table = false;
};

enum class DispatchFamily {
  Invalid = 0,
  Instance = 1,
  Class = 2,
};

enum class RuntimeBuiltinKind {
  None = 0,
  Alloc = 1,
  Init = 2,
  New = 3,
  PropertyGetter = 4,
  PropertySetter = 5,
};

enum class RuntimeMethodReturnKind {
  Unsupported = 0,
  I32Like = 1,
  Bool = 2,
  Void = 3,
};

struct RealizedPropertyAccessor;

struct PropertyLookupCacheKey {
  std::uint64_t start_base_identity = 0;
  std::string property_name;

  bool operator==(const PropertyLookupCacheKey &other) const {
    return start_base_identity == other.start_base_identity &&
           property_name == other.property_name;
  }
};

struct PropertyLookupCacheKeyHash {
  std::size_t operator()(const PropertyLookupCacheKey &key) const {
    return std::hash<std::uint64_t>{}(key.start_base_identity) ^
           (std::hash<std::string>{}(key.property_name) << 1u);
  }
};

struct PropertyLookupCacheEntry {
  bool found = false;
  bool inherited = false;
  std::size_t resolved_node_index = 0;
  std::size_t accessor_index = 0;
};

struct MethodCacheKey {
  std::uint64_t normalized_receiver_identity = 0;
  std::uint64_t selector_stable_id = 0;

  bool operator==(const MethodCacheKey &other) const {
    return normalized_receiver_identity == other.normalized_receiver_identity &&
           selector_stable_id == other.selector_stable_id;
  }
};

struct MethodCacheKeyHash {
  std::size_t operator()(const MethodCacheKey &key) const {
    return std::hash<std::uint64_t>{}(key.normalized_receiver_identity) ^
           (std::hash<std::uint64_t>{}(key.selector_stable_id) << 1u);
  }
};

struct MethodCacheEntry {
  bool resolved = false;
  bool dispatch_family_is_class = false;
  bool fast_path_seeded = false;
  bool effective_direct_dispatch = false;
  bool objc_final_declared = false;
  bool objc_sealed_declared = false;
  std::string selector_storage;
  std::string fast_path_reason;
  std::string class_name;
  std::string owner_identity;
  std::uint64_t normalized_receiver_identity = 0;
  std::uint64_t selector_stable_id = 0;
  std::uint64_t parameter_count = 0;
  RuntimeMethodReturnKind return_kind = RuntimeMethodReturnKind::Unsupported;
  std::uint64_t category_probe_count = 0;
  std::uint64_t protocol_probe_count = 0;
  const void *implementation = nullptr;
  RuntimeBuiltinKind builtin_kind = RuntimeBuiltinKind::None;
  const RealizedPropertyAccessor *runtime_property_accessor = nullptr;
};

struct SlowPathResolution {
  bool resolved = false;
  bool ambiguous = false;
  bool dispatch_family_is_class = false;
  bool effective_direct_dispatch = false;
  bool objc_final_declared = false;
  bool objc_sealed_declared = false;
  std::string selector_storage;
  std::string fast_path_reason;
  std::string class_name;
  std::string owner_identity;
  std::uint64_t normalized_receiver_identity = 0;
  std::uint64_t selector_stable_id = 0;
  std::uint64_t parameter_count = 0;
  RuntimeMethodReturnKind return_kind = RuntimeMethodReturnKind::Unsupported;
  std::uint64_t category_probe_count = 0;
  std::uint64_t protocol_probe_count = 0;
  const void *implementation = nullptr;
  RuntimeBuiltinKind builtin_kind = RuntimeBuiltinKind::None;
  const RealizedPropertyAccessor *runtime_property_accessor = nullptr;
};

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
  bool root_is_self;
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

struct RealizedPropertyAccessor {
  const EmittedPropertyDescriptor *property_descriptor = nullptr;
  const EmittedIvarDescriptor *ivar_descriptor = nullptr;
  RuntimeMethodReturnKind getter_return_kind = RuntimeMethodReturnKind::Unsupported;
  std::string getter_owner_identity;
  std::string setter_owner_identity;
};

struct RuntimeWeakSlotRef {
  int owner_receiver = 0;
  std::size_t offset = 0;
  std::size_t size = 0;
};

struct RuntimeInstanceRecord {
  std::uint64_t receiver_identity = 0;
  std::uint64_t base_identity = 0;
  std::string class_name;
  std::size_t instance_size_bytes = 0;
  std::vector<unsigned char> storage_bytes;
  std::uint64_t retain_count = 1;
};

using RuntimeBlockInvokeFn = int (*)(void *, int, int, int, int);
using RuntimeBlockCopyHelperFn = void (*)(void *);
using RuntimeBlockDisposeHelperFn = void (*)(void *);

// block-runtime API/object-layout freeze anchor: the current block
// handle/object layout remains private runtime state. The compiler/runtime
// contract is one opaque copied storage blob plus a private invoke pointer; no
// public block-object memory layout is promised yet.
// block-runtime allocation/copy-dispose/invoke anchor: runtime block
// records now preserve aligned copied storage plus optional helper pointers so
// promotion/invoke/final-dispose behavior for pointer-capture blocks executes
// inside the private runtime model.
struct RuntimeBlockRecord {
  int block_handle = 0;
  bool has_pointer_capture_storage = false;
  std::size_t storage_size_bytes = 0;
  std::vector<std::uint64_t> storage_words;
  std::vector<std::unique_ptr<int>> promoted_capture_cells;
  RuntimeBlockInvokeFn invoke = nullptr;
  RuntimeBlockCopyHelperFn copy_helper = nullptr;
  RuntimeBlockDisposeHelperFn dispose_helper = nullptr;
  std::uint64_t retain_count = 1;
};

constexpr std::size_t kRuntimeBlockPointerCaptureHeaderSlotCount = 3u;

unsigned char *RuntimeBlockStorageBytes(RuntimeBlockRecord &record) {
  return reinterpret_cast<unsigned char *>(record.storage_words.data());
}

const unsigned char *RuntimeBlockStorageBytes(const RuntimeBlockRecord &record) {
  return reinterpret_cast<const unsigned char *>(record.storage_words.data());
}

std::size_t RuntimeBlockPointerCaptureSlotCount(const RuntimeBlockRecord &record) {
  if (!record.has_pointer_capture_storage) {
    return 0u;
  }
  const std::size_t header_size =
      sizeof(void *) * kRuntimeBlockPointerCaptureHeaderSlotCount;
  if (record.storage_size_bytes < header_size) {
    return 0u;
  }
  const std::size_t payload_size = record.storage_size_bytes - header_size;
  return payload_size / sizeof(void *);
}

bool PromotePointerCaptureCellsIntoRuntimeOwnedStorage(RuntimeBlockRecord &record) {
  // byref-forwarding/heap-promotion/ownership-interop anchor:
  // pointer-capture promotion now rewrites capture slots onto runtime-owned
  // heap cells so escaping blocks no longer borrow stack-cell addresses after
  // promotion.
  if (!record.has_pointer_capture_storage) {
    return true;
  }
  const std::size_t header_size =
      sizeof(void *) * kRuntimeBlockPointerCaptureHeaderSlotCount;
  if (record.storage_size_bytes < header_size) {
    return false;
  }
  const std::size_t payload_size = record.storage_size_bytes - header_size;
  if (payload_size % sizeof(void *) != 0u) {
    return false;
  }
  const std::size_t capture_slot_count =
      RuntimeBlockPointerCaptureSlotCount(record);
  unsigned char *const storage_bytes = RuntimeBlockStorageBytes(record);
  record.promoted_capture_cells.clear();
  record.promoted_capture_cells.reserve(capture_slot_count);
  for (std::size_t slot_index = 0; slot_index < capture_slot_count; ++slot_index) {
    void *capture_cell = nullptr;
    std::memcpy(&capture_cell,
                storage_bytes + header_size + slot_index * sizeof(void *),
                sizeof(capture_cell));
    if (capture_cell == nullptr) {
      record.promoted_capture_cells.push_back(nullptr);
      continue;
    }
    auto promoted_cell = std::make_unique<int>(0);
    std::memcpy(promoted_cell.get(), capture_cell, sizeof(int));
    void *promoted_cell_ptr = promoted_cell.get();
    std::memcpy(storage_bytes + header_size + slot_index * sizeof(void *),
                &promoted_cell_ptr, sizeof(promoted_cell_ptr));
    record.promoted_capture_cells.push_back(std::move(promoted_cell));
  }
  return true;
}

struct RealizedClassNode {
  std::string module_name;
  std::string translation_unit_identity_key;
  std::string class_name;
  std::string bundle_owner_identity;
  std::string interface_owner_identity;
  std::string class_owner_identity;
  std::string metaclass_owner_identity;
  std::string super_class_owner_identity;
  std::string super_metaclass_owner_identity;
  std::uint64_t registration_order_ordinal = 0;
  std::uint64_t base_identity = 0;
  bool is_root_class = false;
  bool implementation_backed = false;
  bool objc_final_declared = false;
  bool objc_sealed_declared = false;
  bool runtime_attachment_ready = false;
  bool runtime_layout_ready = false;
  std::size_t runtime_instance_size_bytes = 0;
  const RegisteredImageMetadata *image = nullptr;
  const EmittedClassBundle *bundle = nullptr;
  std::vector<const EmittedCategoryRecord *> attached_category_records;
  std::vector<RealizedPropertyAccessor> runtime_property_accessors;
  std::size_t super_node_index = 0;
  bool has_super_node = false;
};

struct RuntimeState {
  std::mutex mutex;
  std::uint64_t registered_image_count = 0;
  std::uint64_t registered_descriptor_total = 0;
  std::uint64_t next_expected_registration_order_ordinal = 1;
  std::uint64_t last_successful_registration_order_ordinal = 0;
  int last_registration_status = OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  std::string last_registered_module_name;
  std::string last_registered_translation_unit_identity_key;
  std::string last_rejected_module_name;
  std::string last_rejected_translation_unit_identity_key;
  std::uint64_t last_rejected_registration_order_ordinal = 0;
  std::unordered_map<std::string, std::uint64_t>
      registration_order_by_identity_key;
  std::unordered_map<std::string, RegisteredImageMetadata>
      registered_image_metadata_by_identity_key;
  std::vector<std::string> retained_bootstrap_identity_order;
  std::unordered_map<std::string, RegisteredImageMetadata>
      retained_bootstrap_metadata_by_identity_key;
  std::unordered_map<std::string, std::size_t> selector_index_by_name;
  std::deque<SelectorSlot> selector_slots;
  std::unordered_map<std::uint64_t, KeyPathSlot> keypath_slots;
  std::unordered_map<PropertyLookupCacheKey, PropertyLookupCacheEntry,
                     PropertyLookupCacheKeyHash>
      property_lookup_cache;
  std::uint64_t metadata_backed_selector_count = 0;
  std::uint64_t dynamic_selector_count = 0;
  std::uint64_t metadata_provider_edge_count = 0;
  std::uint64_t image_backed_keypath_count = 0;
  std::uint64_t ambiguous_keypath_handle_count = 0;
  std::string last_materialized_selector;
  std::uint64_t last_materialized_stable_id = 0;
  std::uint64_t last_materialized_registration_order_ordinal = 0;
  std::uint64_t last_materialized_selector_pool_index = 0;
  bool last_materialized_from_metadata = false;
  std::uint64_t last_materialized_keypath_handle = 0;
  std::uint64_t last_materialized_keypath_registration_order_ordinal = 0;
  std::string last_materialized_keypath_profile;
  std::uint64_t last_queried_keypath_handle = 0;
  bool last_keypath_query_found = false;
  bool last_keypath_query_ambiguous = false;
  std::string last_resolved_keypath_profile;
  std::unordered_map<MethodCacheKey, MethodCacheEntry, MethodCacheKeyHash>
      method_cache;
  std::uint64_t method_cache_hit_count = 0;
  std::uint64_t method_cache_miss_count = 0;
  std::uint64_t slow_path_lookup_count = 0;
  std::uint64_t live_dispatch_count = 0;
  std::uint64_t strict_dispatch_error_count = 0;
  std::uint64_t fast_path_seed_count = 0;
  std::uint64_t fast_path_hit_count = 0;
  std::string last_dispatch_selector;
  std::uint64_t last_dispatch_selector_stable_id = 0;
  std::uint64_t last_dispatch_normalized_receiver_identity = 0;
  std::uint64_t last_category_probe_count = 0;
  std::uint64_t last_protocol_probe_count = 0;
  bool last_dispatch_used_cache = false;
  bool last_dispatch_used_fast_path = false;
  bool last_dispatch_resolved_live_method = false;
  bool last_dispatch_strict_error = false;
  bool last_dispatch_effective_direct_dispatch = false;
  bool last_dispatch_used_builtin = false;
  std::string last_fast_path_reason;
  std::string last_dispatch_path;
  std::string last_dispatch_implementation_kind;
  std::string last_dispatch_property_name;
  std::string last_resolved_class_name;
  std::string last_resolved_owner_identity;
  std::uint64_t last_dispatch_parameter_count = 0;
  std::uint64_t last_dispatch_property_base_identity = 0;
  std::uint64_t last_dispatch_property_slot_index = 0;
  const objc3_runtime_registration_table *staged_registration_table = nullptr;
  std::uint64_t walked_image_count = 0;
  std::uint64_t last_discovery_root_entry_count = 0;
  std::uint64_t last_walked_class_descriptor_count = 0;
  std::uint64_t last_walked_protocol_descriptor_count = 0;
  std::uint64_t last_walked_category_descriptor_count = 0;
  std::uint64_t last_walked_property_descriptor_count = 0;
  std::uint64_t last_walked_ivar_descriptor_count = 0;
  std::uint64_t last_walked_selector_pool_count = 0;
  std::uint64_t last_walked_string_pool_count = 0;
  std::uint64_t last_walked_keypath_descriptor_count = 0;
  bool last_linker_anchor_matches_discovery_root = false;
  bool last_registration_used_staged_table = false;
  std::string last_walked_module_name;
  std::string last_walked_translation_unit_identity_key;
  std::uint64_t last_reset_cleared_image_local_init_state_count = 0;
  std::uint64_t last_replayed_image_count = 0;
  std::uint64_t reset_generation = 0;
  std::uint64_t replay_generation = 0;
  int last_replay_status = OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  std::string last_replayed_module_name;
  std::string last_replayed_translation_unit_identity_key;
  std::unordered_map<std::uint64_t, std::string>
      realized_class_name_by_base_identity;
  std::unordered_set<std::uint64_t> ambiguous_realized_base_identities;
  std::unordered_map<std::string, std::vector<std::size_t>>
      realized_class_node_indices_by_name;
  std::vector<RealizedClassNode> realized_class_nodes;
  std::uint64_t realized_root_class_count = 0;
  std::uint64_t realized_metaclass_edge_count = 0;
  std::uint64_t receiver_class_binding_count = 0;
  std::uint64_t realized_attached_category_count = 0;
  std::uint64_t realized_protocol_conformance_edge_count = 0;
  std::string last_realized_class_name;
  std::string last_realized_class_owner_identity;
  std::string last_realized_metaclass_owner_identity;
  std::string last_attached_category_owner_identity;
  std::string last_attached_category_name;
  std::string last_queried_class_name;
  std::string last_resolved_class_query_name;
  std::string last_resolved_class_query_owner_identity;
  bool last_class_query_found = false;
  std::string last_protocol_conformance_class_name;
  std::string last_protocol_conformance_protocol_name;
  std::string last_protocol_conformance_owner_identity;
  std::string last_protocol_conformance_attachment_owner_identity;
  bool last_protocol_query_class_found = false;
  bool last_protocol_query_protocol_found = false;
  bool last_protocol_query_conforms = false;
  std::unordered_map<int, RuntimeInstanceRecord> runtime_instances_by_receiver;
  std::unordered_map<int, RuntimeBlockRecord> runtime_blocks_by_handle;
  std::unordered_map<int, std::vector<RuntimeWeakSlotRef>>
      weak_slot_refs_by_target_receiver;
  int next_runtime_instance_receiver = 0x100000;
  int next_runtime_block_handle = 0x200000;
  std::uint64_t live_runtime_instance_count = 0;
  std::uint64_t last_allocated_runtime_instance_receiver = 0;
  std::uint64_t last_allocated_runtime_instance_base_identity = 0;
  std::uint64_t last_allocated_runtime_instance_size_bytes = 0;
  std::string last_allocated_runtime_instance_class_name;
  std::string last_queried_property_class_name;
  std::string last_queried_property_name;
  std::string last_reflected_property_class_name;
  std::string last_reflected_property_owner_identity;
  bool last_property_query_found = false;
  bool last_property_query_inherited = false;
  bool last_property_query_used_cache = false;
  std::uint64_t property_lookup_cache_hit_count = 0;
  std::uint64_t property_lookup_cache_miss_count = 0;
};

RuntimeState &State() {
  static RuntimeState state;
  return state;
}

const objc3_runtime_selector_handle *LookupSelectorUnlocked(const char *selector);

struct RuntimeDispatchFrame {
  int receiver = 0;
  std::uint64_t base_identity = 0;
  const RealizedPropertyAccessor *runtime_property_accessor = nullptr;
  std::vector<int> autorelease_values;
};

struct RuntimeAutoreleasePoolFrame {
  std::vector<int> values;
};

thread_local std::vector<RuntimeDispatchFrame> g_runtime_dispatch_frames;
thread_local RuntimeDispatchFrame g_runtime_testing_dispatch_frame;
thread_local bool g_runtime_has_testing_dispatch_frame = false;
thread_local std::vector<RuntimeAutoreleasePoolFrame>
    g_runtime_autoreleasepool_frames;
thread_local std::uint64_t g_runtime_autoreleasepool_max_depth = 0;
thread_local std::uint64_t g_runtime_autoreleasepool_drained_value_count = 0;
thread_local std::uint64_t g_runtime_arc_debug_retain_call_count = 0;
thread_local std::uint64_t g_runtime_arc_debug_release_call_count = 0;
thread_local std::uint64_t g_runtime_arc_debug_autorelease_call_count = 0;
thread_local std::uint64_t g_runtime_arc_debug_autoreleasepool_push_count = 0;
thread_local std::uint64_t g_runtime_arc_debug_autoreleasepool_pop_count = 0;
thread_local std::uint64_t g_runtime_arc_debug_current_property_read_count = 0;
thread_local std::uint64_t g_runtime_arc_debug_current_property_write_count = 0;
thread_local std::uint64_t g_runtime_arc_debug_current_property_exchange_count = 0;
thread_local std::uint64_t g_runtime_arc_debug_weak_current_property_load_count = 0;
thread_local std::uint64_t g_runtime_arc_debug_weak_current_property_store_count = 0;
thread_local int g_runtime_last_autoreleased_value = 0;
thread_local int g_runtime_last_drained_autorelease_value = 0;
thread_local int g_runtime_arc_debug_last_retain_value = 0;
thread_local int g_runtime_arc_debug_last_release_value = 0;
thread_local int g_runtime_arc_debug_last_autorelease_value = 0;
thread_local int g_runtime_arc_debug_last_property_read_value = 0;
thread_local int g_runtime_arc_debug_last_property_written_value = 0;
thread_local int g_runtime_arc_debug_last_property_exchange_previous_value = 0;
thread_local int g_runtime_arc_debug_last_property_exchange_new_value = 0;
thread_local int g_runtime_arc_debug_last_weak_loaded_value = 0;
thread_local int g_runtime_arc_debug_last_weak_stored_value = 0;
thread_local int g_runtime_arc_debug_last_property_receiver = 0;
thread_local std::string g_runtime_arc_debug_last_property_name;
thread_local std::string g_runtime_arc_debug_last_property_owner_identity;
thread_local std::uint64_t g_runtime_block_promote_call_count = 0;
thread_local std::uint64_t g_runtime_block_invoke_call_count = 0;
thread_local int g_runtime_last_promoted_block_handle = 0;
thread_local int g_runtime_last_promote_has_pointer_capture_storage = 0;
thread_local int g_runtime_last_invoked_block_handle = 0;
thread_local int g_runtime_last_block_invoke_result = 0;
thread_local std::uint64_t g_runtime_error_bridge_store_call_count = 0;
thread_local std::uint64_t g_runtime_error_bridge_load_call_count = 0;
thread_local std::uint64_t g_runtime_error_bridge_status_bridge_call_count = 0;
thread_local std::uint64_t g_runtime_error_bridge_nserror_bridge_call_count = 0;
thread_local std::uint64_t g_runtime_error_bridge_catch_match_call_count = 0;
thread_local int g_runtime_error_bridge_last_stored_error_value = 0;
thread_local int g_runtime_error_bridge_last_loaded_error_value = 0;
thread_local int g_runtime_error_bridge_last_status_bridge_status_value = 0;
thread_local int g_runtime_error_bridge_last_status_bridge_error_value = 0;
thread_local int g_runtime_error_bridge_last_nserror_bridge_error_value = 0;
thread_local int g_runtime_error_bridge_last_catch_match_error_value = 0;
thread_local int g_runtime_error_bridge_last_catch_match_kind = 0;
thread_local int g_runtime_error_bridge_last_catch_match_is_catch_all = 0;
thread_local int g_runtime_error_bridge_last_catch_match_result = 0;
thread_local std::string g_runtime_error_bridge_last_catch_kind_name;
thread_local std::uint64_t g_runtime_async_continuation_allocation_call_count = 0;
thread_local std::uint64_t g_runtime_async_continuation_handoff_call_count = 0;
thread_local std::uint64_t g_runtime_async_continuation_resume_call_count = 0;
thread_local int g_runtime_async_continuation_next_handle = 1;
thread_local int g_runtime_async_continuation_last_allocated_handle = 0;
thread_local int g_runtime_async_continuation_last_allocated_resume_entry_tag = 0;
thread_local int g_runtime_async_continuation_last_allocated_executor_tag = 0;
thread_local int g_runtime_async_continuation_last_handoff_handle = 0;
thread_local int g_runtime_async_continuation_last_handoff_executor_tag = 0;
thread_local int g_runtime_async_continuation_last_resume_handle = 0;
thread_local int g_runtime_async_continuation_last_resume_result_value = 0;
thread_local int g_runtime_async_continuation_last_resume_return_value = 0;
thread_local std::unordered_set<int> g_runtime_async_live_continuation_handles;
thread_local std::uint64_t g_runtime_task_spawn_call_count = 0;
thread_local std::uint64_t g_runtime_task_scope_call_count = 0;
thread_local std::uint64_t g_runtime_task_add_task_call_count = 0;
thread_local std::uint64_t g_runtime_task_wait_next_call_count = 0;
thread_local std::uint64_t g_runtime_task_cancel_all_call_count = 0;
thread_local std::uint64_t g_runtime_task_cancellation_poll_call_count = 0;
thread_local std::uint64_t g_runtime_task_on_cancel_call_count = 0;
thread_local std::uint64_t g_runtime_task_executor_hop_call_count = 0;
thread_local std::uint64_t g_runtime_actor_isolation_thunk_call_count = 0;
thread_local std::uint64_t g_runtime_actor_nonisolated_entry_call_count = 0;
thread_local std::uint64_t g_runtime_actor_hop_to_executor_call_count = 0;
thread_local std::uint64_t g_runtime_actor_replay_proof_call_count = 0;
thread_local std::uint64_t g_runtime_actor_race_guard_call_count = 0;
thread_local std::uint64_t g_runtime_actor_bind_executor_call_count = 0;
thread_local std::uint64_t g_runtime_actor_mailbox_enqueue_call_count = 0;
thread_local std::uint64_t g_runtime_actor_mailbox_drain_call_count = 0;
thread_local int g_runtime_task_last_spawn_kind = 0;
thread_local int g_runtime_task_last_spawn_executor_tag = 0;
thread_local int g_runtime_task_last_scope_executor_tag = 0;
thread_local int g_runtime_task_last_add_task_executor_tag = 0;
thread_local int g_runtime_task_last_wait_next_executor_tag = 0;
thread_local int g_runtime_task_last_cancel_all_executor_tag = 0;
thread_local int g_runtime_task_last_cancellation_poll_executor_tag = 0;
thread_local int g_runtime_task_last_on_cancel_executor_tag = 0;
thread_local int g_runtime_task_last_executor_hop_executor_tag = 0;
thread_local int g_runtime_task_last_executor_hop_value = 0;
thread_local int g_runtime_task_last_wait_next_result = 0;
thread_local int g_runtime_task_last_cancel_all_result = 0;
thread_local int g_runtime_task_last_cancellation_poll_result = 0;
thread_local int g_runtime_actor_last_isolation_executor_tag = 0;
thread_local int g_runtime_actor_last_nonisolated_value = 0;
thread_local int g_runtime_actor_last_nonisolated_executor_tag = 0;
thread_local int g_runtime_actor_last_hop_value = 0;
thread_local int g_runtime_actor_last_hop_executor_tag = 0;
thread_local int g_runtime_actor_last_hop_result = 0;
thread_local int g_runtime_actor_last_replay_proof_executor_tag = 0;
thread_local int g_runtime_actor_last_race_guard_executor_tag = 0;
thread_local int g_runtime_actor_last_bound_actor_handle = 0;
thread_local int g_runtime_actor_last_bound_executor_tag = 0;
thread_local int g_runtime_actor_last_mailbox_actor_handle = 0;
thread_local int g_runtime_actor_last_mailbox_enqueued_value = 0;
thread_local int g_runtime_actor_last_mailbox_executor_tag = 0;
thread_local int g_runtime_actor_last_mailbox_depth = 0;
thread_local int g_runtime_actor_last_mailbox_drained_value = 0;
thread_local std::unordered_map<int, std::deque<int>> g_runtime_actor_mailboxes;

const void *AggregateEntry(const objc3_runtime_pointer_aggregate *aggregate,
                           std::uint64_t index);
RuntimeMethodReturnKind ClassifyRuntimeReturnType(const char *return_type_name);
std::vector<const EmittedClassBundle *> CollectPreferredClassBundlesForImage(
    const RegisteredImageMetadata &record, const std::string &class_name);

void ClearMethodCacheStateUnlocked(RuntimeState &state) {
  state.method_cache.clear();
  state.method_cache_hit_count = 0;
  state.method_cache_miss_count = 0;
  state.slow_path_lookup_count = 0;
  state.live_dispatch_count = 0;
  state.strict_dispatch_error_count = 0;
  state.fast_path_seed_count = 0;
  state.fast_path_hit_count = 0;
  state.last_dispatch_selector.clear();
  state.last_dispatch_selector_stable_id = 0;
  state.last_dispatch_normalized_receiver_identity = 0;
  state.last_category_probe_count = 0;
  state.last_protocol_probe_count = 0;
  state.last_dispatch_used_cache = false;
  state.last_dispatch_used_fast_path = false;
  state.last_dispatch_resolved_live_method = false;
  state.last_dispatch_strict_error = false;
  state.last_dispatch_effective_direct_dispatch = false;
  state.last_dispatch_used_builtin = false;
  state.last_fast_path_reason.clear();
  state.last_dispatch_path.clear();
  state.last_dispatch_implementation_kind.clear();
  state.last_dispatch_property_name.clear();
  state.last_resolved_class_name.clear();
  state.last_resolved_owner_identity.clear();
  state.last_dispatch_parameter_count = 0;
  state.last_dispatch_property_base_identity = 0;
  state.last_dispatch_property_slot_index = 0;
}

void ClearRealizedClassGraphUnlocked(RuntimeState &state) {
  state.realized_class_name_by_base_identity.clear();
  state.ambiguous_realized_base_identities.clear();
  state.realized_class_node_indices_by_name.clear();
  state.realized_class_nodes.clear();
  state.property_lookup_cache.clear();
  state.property_lookup_cache_hit_count = 0;
  state.property_lookup_cache_miss_count = 0;
  state.realized_root_class_count = 0;
  state.realized_metaclass_edge_count = 0;
  state.receiver_class_binding_count = 0;
  state.realized_attached_category_count = 0;
  state.realized_protocol_conformance_edge_count = 0;
  state.last_realized_class_name.clear();
  state.last_realized_class_owner_identity.clear();
  state.last_realized_metaclass_owner_identity.clear();
  state.last_attached_category_owner_identity.clear();
  state.last_attached_category_name.clear();
  state.last_queried_class_name.clear();
  state.last_resolved_class_query_name.clear();
  state.last_resolved_class_query_owner_identity.clear();
  state.last_class_query_found = false;
  state.last_protocol_conformance_class_name.clear();
  state.last_protocol_conformance_protocol_name.clear();
  state.last_protocol_conformance_owner_identity.clear();
  state.last_protocol_conformance_attachment_owner_identity.clear();
  state.last_protocol_query_class_found = false;
  state.last_protocol_query_protocol_found = false;
  state.last_protocol_query_conforms = false;
  state.last_queried_property_class_name.clear();
  state.last_queried_property_name.clear();
  state.last_reflected_property_class_name.clear();
  state.last_reflected_property_owner_identity.clear();
  state.last_property_query_found = false;
  state.last_property_query_inherited = false;
  state.last_property_query_used_cache = false;
  state.last_property_query_used_cache = false;
}

void ClearRuntimeInstanceStateUnlocked(RuntimeState &state) {
  state.runtime_instances_by_receiver.clear();
  state.runtime_blocks_by_handle.clear();
  state.weak_slot_refs_by_target_receiver.clear();
  state.next_runtime_instance_receiver = 0x100000;
  state.next_runtime_block_handle = 0x200000;
  state.live_runtime_instance_count = 0;
  state.last_allocated_runtime_instance_receiver = 0;
  state.last_allocated_runtime_instance_base_identity = 0;
  state.last_allocated_runtime_instance_size_bytes = 0;
  state.last_allocated_runtime_instance_class_name.clear();
}

std::size_t AlignTo(std::size_t value, std::size_t alignment) {
  const std::size_t effective_alignment = std::max<std::size_t>(alignment, 1u);
  const std::size_t remainder = value % effective_alignment;
  return remainder == 0u ? value : value + (effective_alignment - remainder);
}

RuntimeDispatchFrame *CurrentRuntimeDispatchFrame() {
  if (!g_runtime_dispatch_frames.empty()) {
    return &g_runtime_dispatch_frames.back();
  }
  return g_runtime_has_testing_dispatch_frame ? &g_runtime_testing_dispatch_frame
                                              : nullptr;
}

void PushRuntimeDispatchFrame(int receiver, std::uint64_t base_identity,
                              const RealizedPropertyAccessor *accessor) {
  RuntimeDispatchFrame frame;
  frame.receiver = receiver;
  frame.base_identity = base_identity;
  frame.runtime_property_accessor = accessor;
  g_runtime_dispatch_frames.push_back(std::move(frame));
}

std::vector<int> PopRuntimeDispatchFrameAutoreleaseValues() {
  if (g_runtime_dispatch_frames.empty()) {
    return {};
  }
  RuntimeDispatchFrame frame = std::move(g_runtime_dispatch_frames.back());
  g_runtime_dispatch_frames.pop_back();
  return std::move(frame.autorelease_values);
}

void ResetRuntimeAutoreleasepoolStateForTesting() {
  g_runtime_dispatch_frames.clear();
  g_runtime_testing_dispatch_frame = RuntimeDispatchFrame{};
  g_runtime_has_testing_dispatch_frame = false;
  g_runtime_autoreleasepool_frames.clear();
  g_runtime_autoreleasepool_max_depth = 0;
  g_runtime_autoreleasepool_drained_value_count = 0;
  g_runtime_last_autoreleased_value = 0;
  g_runtime_last_drained_autorelease_value = 0;
  g_runtime_arc_debug_retain_call_count = 0;
  g_runtime_arc_debug_release_call_count = 0;
  g_runtime_arc_debug_autorelease_call_count = 0;
  g_runtime_arc_debug_autoreleasepool_push_count = 0;
  g_runtime_arc_debug_autoreleasepool_pop_count = 0;
  g_runtime_arc_debug_current_property_read_count = 0;
  g_runtime_arc_debug_current_property_write_count = 0;
  g_runtime_arc_debug_current_property_exchange_count = 0;
  g_runtime_arc_debug_weak_current_property_load_count = 0;
  g_runtime_arc_debug_weak_current_property_store_count = 0;
  g_runtime_arc_debug_last_retain_value = 0;
  g_runtime_arc_debug_last_release_value = 0;
  g_runtime_arc_debug_last_autorelease_value = 0;
  g_runtime_arc_debug_last_property_read_value = 0;
  g_runtime_arc_debug_last_property_written_value = 0;
  g_runtime_arc_debug_last_property_exchange_previous_value = 0;
  g_runtime_arc_debug_last_property_exchange_new_value = 0;
  g_runtime_arc_debug_last_weak_loaded_value = 0;
  g_runtime_arc_debug_last_weak_stored_value = 0;
  g_runtime_arc_debug_last_property_receiver = 0;
  g_runtime_arc_debug_last_property_name.clear();
  g_runtime_arc_debug_last_property_owner_identity.clear();
  g_runtime_block_promote_call_count = 0;
  g_runtime_block_invoke_call_count = 0;
  g_runtime_last_promoted_block_handle = 0;
  g_runtime_last_promote_has_pointer_capture_storage = 0;
  g_runtime_last_invoked_block_handle = 0;
  g_runtime_last_block_invoke_result = 0;
  g_runtime_error_bridge_store_call_count = 0;
  g_runtime_error_bridge_load_call_count = 0;
  g_runtime_error_bridge_status_bridge_call_count = 0;
  g_runtime_error_bridge_nserror_bridge_call_count = 0;
  g_runtime_error_bridge_catch_match_call_count = 0;
  g_runtime_error_bridge_last_stored_error_value = 0;
  g_runtime_error_bridge_last_loaded_error_value = 0;
  g_runtime_error_bridge_last_status_bridge_status_value = 0;
  g_runtime_error_bridge_last_status_bridge_error_value = 0;
  g_runtime_error_bridge_last_nserror_bridge_error_value = 0;
  g_runtime_error_bridge_last_catch_match_error_value = 0;
  g_runtime_error_bridge_last_catch_match_kind = 0;
  g_runtime_error_bridge_last_catch_match_is_catch_all = 0;
  g_runtime_error_bridge_last_catch_match_result = 0;
  g_runtime_error_bridge_last_catch_kind_name.clear();
  g_runtime_async_continuation_allocation_call_count = 0;
  g_runtime_async_continuation_handoff_call_count = 0;
  g_runtime_async_continuation_resume_call_count = 0;
  g_runtime_async_continuation_next_handle = 1;
  g_runtime_async_continuation_last_allocated_handle = 0;
  g_runtime_async_continuation_last_allocated_resume_entry_tag = 0;
  g_runtime_async_continuation_last_allocated_executor_tag = 0;
  g_runtime_async_continuation_last_handoff_handle = 0;
  g_runtime_async_continuation_last_handoff_executor_tag = 0;
  g_runtime_async_continuation_last_resume_handle = 0;
  g_runtime_async_continuation_last_resume_result_value = 0;
  g_runtime_async_continuation_last_resume_return_value = 0;
  g_runtime_async_live_continuation_handles.clear();
  g_runtime_task_spawn_call_count = 0;
  g_runtime_task_scope_call_count = 0;
  g_runtime_task_add_task_call_count = 0;
  g_runtime_task_wait_next_call_count = 0;
  g_runtime_task_cancel_all_call_count = 0;
  g_runtime_task_cancellation_poll_call_count = 0;
  g_runtime_task_on_cancel_call_count = 0;
  g_runtime_task_executor_hop_call_count = 0;
  g_runtime_actor_isolation_thunk_call_count = 0;
  g_runtime_actor_nonisolated_entry_call_count = 0;
  g_runtime_actor_hop_to_executor_call_count = 0;
  g_runtime_actor_replay_proof_call_count = 0;
  g_runtime_actor_race_guard_call_count = 0;
  g_runtime_actor_bind_executor_call_count = 0;
  g_runtime_actor_mailbox_enqueue_call_count = 0;
  g_runtime_actor_mailbox_drain_call_count = 0;
  g_runtime_task_last_spawn_kind = 0;
  g_runtime_task_last_spawn_executor_tag = 0;
  g_runtime_task_last_scope_executor_tag = 0;
  g_runtime_task_last_add_task_executor_tag = 0;
  g_runtime_task_last_wait_next_executor_tag = 0;
  g_runtime_task_last_cancel_all_executor_tag = 0;
  g_runtime_task_last_cancellation_poll_executor_tag = 0;
  g_runtime_task_last_on_cancel_executor_tag = 0;
  g_runtime_task_last_executor_hop_executor_tag = 0;
  g_runtime_task_last_executor_hop_value = 0;
  g_runtime_task_last_wait_next_result = 0;
  g_runtime_task_last_cancel_all_result = 0;
  g_runtime_task_last_cancellation_poll_result = 0;
  g_runtime_actor_last_isolation_executor_tag = 0;
  g_runtime_actor_last_nonisolated_value = 0;
  g_runtime_actor_last_nonisolated_executor_tag = 0;
  g_runtime_actor_last_hop_value = 0;
  g_runtime_actor_last_hop_executor_tag = 0;
  g_runtime_actor_last_hop_result = 0;
  g_runtime_actor_last_replay_proof_executor_tag = 0;
  g_runtime_actor_last_race_guard_executor_tag = 0;
  g_runtime_actor_last_bound_actor_handle = 0;
  g_runtime_actor_last_bound_executor_tag = 0;
  g_runtime_actor_last_mailbox_actor_handle = 0;
  g_runtime_actor_last_mailbox_enqueued_value = 0;
  g_runtime_actor_last_mailbox_executor_tag = 0;
  g_runtime_actor_last_mailbox_depth = 0;
  g_runtime_actor_last_mailbox_drained_value = 0;
  g_runtime_actor_mailboxes.clear();
}

void RecordArcDebugPropertyContext(const RuntimeDispatchFrame *frame) {
  g_runtime_arc_debug_last_property_receiver =
      frame != nullptr ? frame->receiver : 0;
  g_runtime_arc_debug_last_property_name.clear();
  g_runtime_arc_debug_last_property_owner_identity.clear();
  if (frame == nullptr || frame->runtime_property_accessor == nullptr ||
      frame->runtime_property_accessor->property_descriptor == nullptr) {
    return;
  }
  const EmittedPropertyDescriptor &descriptor =
      *frame->runtime_property_accessor->property_descriptor;
  if (descriptor.property_name != nullptr) {
    g_runtime_arc_debug_last_property_name = descriptor.property_name;
  }
  if (descriptor.declaration_owner_identity != nullptr) {
    g_runtime_arc_debug_last_property_owner_identity =
        descriptor.declaration_owner_identity;
  } else if (descriptor.export_owner_identity != nullptr) {
    g_runtime_arc_debug_last_property_owner_identity =
        descriptor.export_owner_identity;
  } else if (descriptor.owner_identity != nullptr) {
    g_runtime_arc_debug_last_property_owner_identity = descriptor.owner_identity;
  }
}

const char *RuntimeErrorCatchKindName(int catch_kind) {
  switch (catch_kind) {
  case 1:
    return "nserror";
  case 2:
    return "id<error>";
  default:
    return "unknown";
  }
}

bool RuntimeErrorCatchKindMatches(int catch_kind) {
  switch (catch_kind) {
  case 1:
  case 2:
    return true;
  default:
    return false;
  }
}

void PushRuntimeAutoreleasePoolFrame() {
  g_runtime_autoreleasepool_frames.push_back({});
  ++g_runtime_arc_debug_autoreleasepool_push_count;
  g_runtime_autoreleasepool_max_depth = std::max<std::uint64_t>(
      g_runtime_autoreleasepool_max_depth,
      static_cast<std::uint64_t>(g_runtime_autoreleasepool_frames.size()));
}

std::vector<int> PopRuntimeAutoreleasePoolFrameValues() {
  if (g_runtime_autoreleasepool_frames.empty()) {
    return {};
  }
  RuntimeAutoreleasePoolFrame frame =
      std::move(g_runtime_autoreleasepool_frames.back());
  g_runtime_autoreleasepool_frames.pop_back();
  return std::move(frame.values);
}

std::uint64_t CountQueuedAutoreleaseValues() {
  std::uint64_t total = 0;
  for (const RuntimeAutoreleasePoolFrame &frame : g_runtime_autoreleasepool_frames) {
    total += static_cast<std::uint64_t>(frame.values.size());
  }
  return total;
}

void EnqueueAutoreleaseValue(int value) {
  if (value == 0) {
    return;
  }
  g_runtime_last_autoreleased_value = value;
  if (!g_runtime_autoreleasepool_frames.empty()) {
    g_runtime_autoreleasepool_frames.back().values.push_back(value);
    return;
  }
  if (g_runtime_dispatch_frames.size() >= 2u) {
    g_runtime_dispatch_frames[g_runtime_dispatch_frames.size() - 2u]
        .autorelease_values.push_back(value);
    return;
  }
  if (RuntimeDispatchFrame *frame = CurrentRuntimeDispatchFrame()) {
    frame->autorelease_values.push_back(value);
  }
}

bool IsRuntimeManagedReceiverValueUnlocked(const RuntimeState &state, int value) {
  return value != 0 &&
         state.runtime_instances_by_receiver.find(value) !=
             state.runtime_instances_by_receiver.end();
}

void RemoveWeakSlotRefUnlocked(RuntimeState &state, int target_receiver,
                               int owner_receiver, std::size_t offset,
                               std::size_t size) {
  if (target_receiver == 0) {
    return;
  }
  const auto found = state.weak_slot_refs_by_target_receiver.find(target_receiver);
  if (found == state.weak_slot_refs_by_target_receiver.end()) {
    return;
  }
  auto &refs = found->second;
  refs.erase(
      std::remove_if(
          refs.begin(), refs.end(),
          [&](const RuntimeWeakSlotRef &ref) {
            return ref.owner_receiver == owner_receiver && ref.offset == offset &&
                   ref.size == size;
          }),
      refs.end());
  if (refs.empty()) {
    state.weak_slot_refs_by_target_receiver.erase(found);
  }
}

void RegisterWeakSlotRefUnlocked(RuntimeState &state, int target_receiver,
                                 int owner_receiver, std::size_t offset,
                                 std::size_t size) {
  if (target_receiver == 0) {
    return;
  }
  std::vector<RuntimeWeakSlotRef> &refs =
      state.weak_slot_refs_by_target_receiver[target_receiver];
  const auto duplicate =
      std::find_if(refs.begin(), refs.end(), [&](const RuntimeWeakSlotRef &ref) {
        return ref.owner_receiver == owner_receiver && ref.offset == offset &&
               ref.size == size;
      });
  if (duplicate == refs.end()) {
    refs.push_back(RuntimeWeakSlotRef{owner_receiver, offset, size});
  }
}

void RemoveWeakSlotRefsOwnedByReceiverUnlocked(RuntimeState &state,
                                               int owner_receiver) {
  for (auto it = state.weak_slot_refs_by_target_receiver.begin();
       it != state.weak_slot_refs_by_target_receiver.end();) {
    auto &refs = it->second;
    refs.erase(std::remove_if(refs.begin(), refs.end(),
                              [&](const RuntimeWeakSlotRef &ref) {
                                return ref.owner_receiver == owner_receiver;
                              }),
               refs.end());
    if (refs.empty()) {
      it = state.weak_slot_refs_by_target_receiver.erase(it);
    } else {
      ++it;
    }
  }
}

void ZeroWeakSlotRefsForTargetUnlocked(RuntimeState &state, int target_receiver) {
  const auto found = state.weak_slot_refs_by_target_receiver.find(target_receiver);
  if (found == state.weak_slot_refs_by_target_receiver.end()) {
    return;
  }
  for (const RuntimeWeakSlotRef &ref : found->second) {
    const auto owner_it = state.runtime_instances_by_receiver.find(ref.owner_receiver);
    if (owner_it == state.runtime_instances_by_receiver.end()) {
      continue;
    }
    RuntimeInstanceRecord &owner = owner_it->second;
    if (ref.size == 0u || ref.offset + ref.size > owner.storage_bytes.size()) {
      continue;
    }
    std::fill(owner.storage_bytes.begin() + static_cast<std::ptrdiff_t>(ref.offset),
              owner.storage_bytes.begin() +
                  static_cast<std::ptrdiff_t>(ref.offset + ref.size),
              0);
  }
  state.weak_slot_refs_by_target_receiver.erase(found);
}

std::string BuildSynthesizedInstanceMethodOwnerIdentity(
    const char *declaration_owner_identity, const char *selector) {
  const std::string owner =
      declaration_owner_identity != nullptr ? declaration_owner_identity : "";
  const std::string selector_text = selector != nullptr ? selector : "";
  return owner + "::instance_method:" + selector_text;
}

std::uint64_t BuildReceiverBaseIdentity(std::size_t ordinal) {
  return kReceiverIdentityBase +
         static_cast<std::uint64_t>(ordinal) * kReceiverIdentityStride;
}

const EmittedMethodListEntry *MethodListEntries(
    const EmittedMethodListHeader *header) {
  return header == nullptr
             ? nullptr
             : reinterpret_cast<const EmittedMethodListEntry *>(header + 1);
}

const EmittedMethodListRef *SelectMethodListRef(const EmittedProtocolRecord &record,
                                                DispatchFamily family) {
  return family == DispatchFamily::Class ? record.class_method_list_ref
                                         : record.instance_method_list_ref;
}

const EmittedMethodListRef *SelectMethodListRef(const EmittedCategoryRecord &record,
                                                DispatchFamily family) {
  return family == DispatchFamily::Class ? record.class_method_list_ref
                                         : record.instance_method_list_ref;
}

bool SelectorMatchesMethodEntryUnlocked(RuntimeState &state,
                                        const char *entry_selector,
                                        std::uint64_t selector_stable_id,
                                        const char *selector_spelling) {
  if (entry_selector == nullptr) {
    return false;
  }
  if (selector_stable_id == 0 && selector_spelling != nullptr &&
      selector_spelling[0] != '\0') {
    const objc3_runtime_selector_handle *lookup_handle =
        LookupSelectorUnlocked(selector_spelling);
    selector_stable_id = lookup_handle != nullptr ? lookup_handle->stable_id : 0;
  }
  if (selector_stable_id == 0) {
    return false;
  }
  const objc3_runtime_selector_handle *entry_handle =
      LookupSelectorUnlocked(entry_selector);
  return entry_handle != nullptr &&
         entry_handle->stable_id == selector_stable_id;
}

bool TryResolveMethodFromMethodListRefUnlocked(
    RuntimeState &state, const EmittedMethodListRef *method_list_ref,
    const char *resolved_class_name, DispatchFamily family,
    std::uint64_t normalized_receiver_identity, std::uint64_t selector_stable_id,
    const char *selector_spelling, SlowPathResolution &resolution,
    bool &ambiguous) {
  if (method_list_ref == nullptr || method_list_ref->count == 0 ||
      method_list_ref->method_list == nullptr) {
    return true;
  }
  const auto *header =
      static_cast<const EmittedMethodListHeader *>(method_list_ref->method_list);
  if (header == nullptr || header->count != method_list_ref->count) {
    return false;
  }
  const EmittedMethodListEntry *entries = MethodListEntries(header);
  for (std::uint64_t index = 0; index < header->count; ++index) {
    const EmittedMethodListEntry &entry = entries[index];
    const RuntimeMethodReturnKind entry_return_kind =
        ClassifyRuntimeReturnType(entry.return_type_name);
    if (entry.selector == nullptr || entry.owner_identity == nullptr ||
        entry.return_type_name == nullptr) {
      return false;
    }
    if (!SelectorMatchesMethodEntryUnlocked(state, entry.selector,
                                            selector_stable_id,
                                            selector_spelling) ||
        entry.has_body == 0 || entry.implementation == nullptr ||
        entry_return_kind == RuntimeMethodReturnKind::Unsupported ||
        entry.parameter_count > 4) {
      continue;
    }
    if (resolution.resolved &&
        (resolution.implementation != entry.implementation ||
         resolution.parameter_count != entry.parameter_count ||
         resolution.return_kind != entry_return_kind ||
         resolution.owner_identity != entry.owner_identity ||
         resolution.class_name !=
             (resolved_class_name != nullptr ? resolved_class_name : ""))) {
      ambiguous = true;
      return true;
    }
    resolution.resolved = true;
    resolution.dispatch_family_is_class = family == DispatchFamily::Class;
    resolution.selector_storage = selector_spelling != nullptr ? selector_spelling : "";
    resolution.class_name =
        resolved_class_name != nullptr ? resolved_class_name : "";
    resolution.owner_identity = entry.owner_identity;
    resolution.normalized_receiver_identity = normalized_receiver_identity;
    resolution.selector_stable_id = selector_stable_id;
    resolution.parameter_count = entry.parameter_count;
    resolution.return_kind = entry_return_kind;
    resolution.implementation = entry.implementation;
    resolution.effective_direct_dispatch = entry.effective_direct_dispatch;
    resolution.objc_final_declared = entry.objc_final_declared;
  }
  return true;
}

bool ProbeProtocolSelectorDeclarationUnlocked(
    RuntimeState &state, const EmittedProtocolRecord *start_record,
    DispatchFamily family, std::uint64_t selector_stable_id,
    const char *selector_spelling, std::uint64_t &protocol_probe_count,
    bool &declared, bool &ambiguous) {
  // protocol/category-aware resolution anchor: adopted/inherited
  // protocol method lists now provide declaration-aware negative resolution
  // while remaining non-callable, so unresolved selectors can fall closed to
  // compatibility dispatch with explicit protocol probe evidence.
  (void)ambiguous;
  if (start_record == nullptr) {
    return true;
  }
  std::vector<const EmittedProtocolRecord *> stack;
  stack.push_back(start_record);
  std::unordered_set<const EmittedProtocolRecord *> visited;
  while (!stack.empty()) {
    const EmittedProtocolRecord *record = stack.back();
    stack.pop_back();
    if (record == nullptr || !visited.insert(record).second) {
      continue;
    }
    if (record->protocol_name == nullptr || record->owner_identity == nullptr) {
      return false;
    }
    ++protocol_probe_count;
    const EmittedMethodListRef *method_list_ref = SelectMethodListRef(*record, family);
    if (method_list_ref != nullptr && method_list_ref->count != 0 &&
        method_list_ref->method_list != nullptr) {
      const auto *header =
          static_cast<const EmittedMethodListHeader *>(method_list_ref->method_list);
      if (header == nullptr || header->count != method_list_ref->count) {
        return false;
      }
      const EmittedMethodListEntry *entries = MethodListEntries(header);
      for (std::uint64_t index = 0; index < header->count; ++index) {
        const EmittedMethodListEntry &entry = entries[index];
        if (entry.selector == nullptr || entry.owner_identity == nullptr ||
            entry.return_type_name == nullptr) {
          return false;
        }
        if (!SelectorMatchesMethodEntryUnlocked(state, entry.selector,
                                                selector_stable_id,
                                                selector_spelling)) {
          continue;
        }
        declared = true;
      }
    }
    const objc3_runtime_pointer_aggregate *inherited_refs =
        record->inherited_protocol_refs;
    if (inherited_refs == nullptr) {
      continue;
    }
    for (std::uint64_t index = 0; index < inherited_refs->count; ++index) {
      const auto *inherited_record = static_cast<const EmittedProtocolRecord *>(
          AggregateEntry(inherited_refs, index));
      if (inherited_record == nullptr) {
        return false;
      }
      stack.push_back(inherited_record);
