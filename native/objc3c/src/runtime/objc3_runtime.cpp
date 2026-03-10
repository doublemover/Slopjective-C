#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <algorithm>
#include <cstring>
#include <cstdint>
#include <deque>
#include <mutex>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {

constexpr std::int64_t kDispatchModulus = 2147483629LL;
constexpr std::uint64_t kReceiverIdentityBase = 1024;
constexpr std::uint64_t kReceiverIdentityStride = 17;

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
  std::uint64_t discovery_root_entry_count = 0;
  std::uint64_t class_descriptor_count = 0;
  std::uint64_t protocol_descriptor_count = 0;
  std::uint64_t category_descriptor_count = 0;
  std::uint64_t property_descriptor_count = 0;
  std::uint64_t ivar_descriptor_count = 0;
  std::uint64_t selector_pool_count = 0;
  std::uint64_t string_pool_count = 0;
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
  std::string selector_storage;
  std::string class_name;
  std::string owner_identity;
  std::uint64_t normalized_receiver_identity = 0;
  std::uint64_t selector_stable_id = 0;
  std::uint64_t parameter_count = 0;
  std::uint64_t category_probe_count = 0;
  std::uint64_t protocol_probe_count = 0;
  const void *implementation = nullptr;
  RuntimeBuiltinKind builtin_kind = RuntimeBuiltinKind::None;
};

struct SlowPathResolution {
  bool resolved = false;
  bool ambiguous = false;
  bool dispatch_family_is_class = false;
  std::string selector_storage;
  std::string class_name;
  std::string owner_identity;
  std::uint64_t normalized_receiver_identity = 0;
  std::uint64_t selector_stable_id = 0;
  std::uint64_t parameter_count = 0;
  std::uint64_t category_probe_count = 0;
  std::uint64_t protocol_probe_count = 0;
  const void *implementation = nullptr;
  RuntimeBuiltinKind builtin_kind = RuntimeBuiltinKind::None;
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
};

struct EmittedClassRecord {
  const char *class_name;
  const char *bundle_owner_identity;
  const char *object_owner_identity;
  const char *super_owner_identity;
  const void *super_bundle;
  const EmittedMethodListRef *method_list_ref;
  const objc3_runtime_pointer_aggregate *adopted_protocol_refs;
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

struct RealizedClassNode {
  std::string module_name;
  std::string translation_unit_identity_key;
  std::string class_name;
  std::string class_owner_identity;
  std::string metaclass_owner_identity;
  std::string super_class_owner_identity;
  std::string super_metaclass_owner_identity;
  std::uint64_t registration_order_ordinal = 0;
  std::uint64_t base_identity = 0;
  bool is_root_class = false;
  bool implementation_backed = false;
  bool runtime_attachment_ready = false;
  const RegisteredImageMetadata *image = nullptr;
  const EmittedClassBundle *bundle = nullptr;
  std::vector<const EmittedCategoryRecord *> attached_category_records;
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
  std::uint64_t metadata_backed_selector_count = 0;
  std::uint64_t dynamic_selector_count = 0;
  std::uint64_t metadata_provider_edge_count = 0;
  std::string last_materialized_selector;
  std::uint64_t last_materialized_stable_id = 0;
  std::uint64_t last_materialized_registration_order_ordinal = 0;
  std::uint64_t last_materialized_selector_pool_index = 0;
  bool last_materialized_from_metadata = false;
  std::unordered_map<MethodCacheKey, MethodCacheEntry, MethodCacheKeyHash>
      method_cache;
  std::uint64_t method_cache_hit_count = 0;
  std::uint64_t method_cache_miss_count = 0;
  std::uint64_t slow_path_lookup_count = 0;
  std::uint64_t live_dispatch_count = 0;
  std::uint64_t fallback_dispatch_count = 0;
  std::string last_dispatch_selector;
  std::uint64_t last_dispatch_selector_stable_id = 0;
  std::uint64_t last_dispatch_normalized_receiver_identity = 0;
  std::uint64_t last_category_probe_count = 0;
  std::uint64_t last_protocol_probe_count = 0;
  bool last_dispatch_used_cache = false;
  bool last_dispatch_resolved_live_method = false;
  bool last_dispatch_fell_back = false;
  std::string last_resolved_class_name;
  std::string last_resolved_owner_identity;
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
  std::string last_protocol_conformance_class_name;
  std::string last_protocol_conformance_protocol_name;
  std::string last_protocol_conformance_owner_identity;
  std::string last_protocol_conformance_attachment_owner_identity;
};

RuntimeState &State() {
  static RuntimeState state;
  return state;
}

const void *AggregateEntry(const objc3_runtime_pointer_aggregate *aggregate,
                           std::uint64_t index);
bool IsSupportedRuntimeI32ReturnType(const char *return_type_name);
std::vector<const EmittedClassBundle *> CollectPreferredClassBundlesForImage(
    const RegisteredImageMetadata &record, const std::string &class_name);

void RefreshSelectorHandlePointersUnlocked(RuntimeState &state) {
  for (SelectorSlot &slot : state.selector_slots) {
    slot.handle.selector = slot.spelling_storage.c_str();
  }
}

void ClearMethodCacheStateUnlocked(RuntimeState &state) {
  state.method_cache.clear();
  state.method_cache_hit_count = 0;
  state.method_cache_miss_count = 0;
  state.slow_path_lookup_count = 0;
  state.live_dispatch_count = 0;
  state.fallback_dispatch_count = 0;
  state.last_dispatch_selector.clear();
  state.last_dispatch_selector_stable_id = 0;
  state.last_dispatch_normalized_receiver_identity = 0;
  state.last_category_probe_count = 0;
  state.last_protocol_probe_count = 0;
  state.last_dispatch_used_cache = false;
  state.last_dispatch_resolved_live_method = false;
  state.last_dispatch_fell_back = false;
  state.last_resolved_class_name.clear();
  state.last_resolved_owner_identity.clear();
}

void ClearRealizedClassGraphUnlocked(RuntimeState &state) {
  state.realized_class_name_by_base_identity.clear();
  state.ambiguous_realized_base_identities.clear();
  state.realized_class_node_indices_by_name.clear();
  state.realized_class_nodes.clear();
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
  state.last_protocol_conformance_class_name.clear();
  state.last_protocol_conformance_protocol_name.clear();
  state.last_protocol_conformance_owner_identity.clear();
  state.last_protocol_conformance_attachment_owner_identity.clear();
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

bool SelectorMatchesMethodEntryUnlocked(const RuntimeState &state,
                                        const char *entry_selector,
                                        std::uint64_t selector_stable_id,
                                        const char *selector_spelling) {
  if (entry_selector == nullptr) {
    return false;
  }
  bool selector_matches = false;
  if (selector_stable_id != 0) {
    const auto selector_it = state.selector_index_by_name.find(entry_selector);
    if (selector_it != state.selector_index_by_name.end()) {
      selector_matches =
          state.selector_slots[selector_it->second].handle.stable_id ==
          selector_stable_id;
    }
  }
  if (!selector_matches && selector_spelling != nullptr) {
    selector_matches = std::string(entry_selector) == selector_spelling;
  }
  return selector_matches;
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
    if (entry.selector == nullptr || entry.owner_identity == nullptr ||
        entry.return_type_name == nullptr) {
      return false;
    }
    if (!SelectorMatchesMethodEntryUnlocked(state, entry.selector,
                                            selector_stable_id,
                                            selector_spelling) ||
        entry.has_body == 0 || entry.implementation == nullptr ||
        !IsSupportedRuntimeI32ReturnType(entry.return_type_name) ||
        entry.parameter_count > 4) {
      continue;
    }
    if (resolution.resolved &&
        (resolution.implementation != entry.implementation ||
         resolution.parameter_count != entry.parameter_count ||
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
    resolution.implementation = entry.implementation;
  }
  return true;
}

bool ProbeProtocolSelectorDeclarationUnlocked(
    RuntimeState &state, const EmittedProtocolRecord *start_record,
    DispatchFamily family, std::uint64_t selector_stable_id,
    const char *selector_spelling, std::uint64_t &protocol_probe_count,
    bool &declared, bool &ambiguous) {
  // M255-D004 protocol/category-aware resolution anchor: adopted/inherited
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
    }
  }
  return true;
}

bool CollectPreferredCategoryRecordsForImage(
    const RegisteredImageMetadata &record, const std::string &class_name,
    std::vector<const EmittedCategoryRecord *> &preferred_records) {
  // M256-D001 class-realization-runtime freeze anchor: category attachment is
  // resolved from emitted category records only after one concrete class name
  // has been selected. Runtime prefers implementation records over interface
  // records per category name and fails closed on conflicting attachments.
  std::unordered_map<std::string, const EmittedCategoryRecord *> grouped_records;
  grouped_records.reserve(static_cast<std::size_t>(record.category_descriptor_count));
  for (std::uint64_t index = 0; index < record.category_descriptor_count; ++index) {
    const auto *category_record = static_cast<const EmittedCategoryRecord *>(
        AggregateEntry(record.category_descriptor_root, index));
    if (category_record == nullptr || category_record->class_name == nullptr ||
        category_record->category_name == nullptr ||
        category_record->record_kind == nullptr ||
        category_record->owner_identity == nullptr) {
      return false;
    }
    if (class_name != category_record->class_name) {
      continue;
    }
    const std::string key = category_record->category_name;
    const bool is_implementation =
        std::string(category_record->record_kind) == "implementation";
    auto existing = grouped_records.find(key);
    if (existing == grouped_records.end()) {
      grouped_records.emplace(key, category_record);
      continue;
    }
    const bool existing_is_implementation =
        std::string(existing->second->record_kind) == "implementation";
    if (existing_is_implementation == is_implementation &&
        std::string(existing->second->owner_identity) !=
            std::string(category_record->owner_identity)) {
      return false;
    }
    if (!existing_is_implementation && is_implementation) {
      existing->second = category_record;
    }
  }
  preferred_records.clear();
  preferred_records.reserve(grouped_records.size());
  for (const auto &entry : grouped_records) {
    preferred_records.push_back(entry.second);
  }
  std::sort(preferred_records.begin(), preferred_records.end(),
            [](const EmittedCategoryRecord *lhs, const EmittedCategoryRecord *rhs) {
              return std::make_tuple(std::string(lhs->class_name),
                                     std::string(lhs->category_name),
                                     std::string(lhs->record_kind),
                                     std::string(lhs->owner_identity)) <
                     std::make_tuple(std::string(rhs->class_name),
                                     std::string(rhs->category_name),
                                     std::string(rhs->record_kind),
                                     std::string(rhs->owner_identity));
            });
  return true;
}

bool ProbeProtocolSelectorDeclarationsFromAggregateUnlocked(
    RuntimeState &state,
    const objc3_runtime_pointer_aggregate *protocol_refs, DispatchFamily family,
    std::uint64_t selector_stable_id, const char *selector_spelling,
    std::uint64_t &protocol_probe_count, bool &ambiguous) {
  if (protocol_refs == nullptr) {
    return true;
  }
  for (std::uint64_t index = 0; index < protocol_refs->count; ++index) {
    const auto *protocol_record = static_cast<const EmittedProtocolRecord *>(
        AggregateEntry(protocol_refs, index));
    if (protocol_record == nullptr) {
      return false;
    }
    bool declared = false;
    if (!ProbeProtocolSelectorDeclarationUnlocked(
            state, protocol_record, family, selector_stable_id,
            selector_spelling, protocol_probe_count, declared, ambiguous)) {
      return false;
    }
    if (ambiguous) {
      return true;
    }
  }
  return true;
}

bool QueryProtocolConformanceFromProtocolRecordUnlocked(
    const EmittedProtocolRecord *start_record, const char *protocol_name,
    std::unordered_set<const EmittedProtocolRecord *> &visited,
    std::uint64_t &visited_protocol_count, std::string &matched_owner_identity) {
  if (start_record == nullptr || protocol_name == nullptr || protocol_name[0] == '\0') {
    return false;
  }
  std::vector<const EmittedProtocolRecord *> stack;
  stack.push_back(start_record);
  while (!stack.empty()) {
    const EmittedProtocolRecord *record = stack.back();
    stack.pop_back();
    if (record == nullptr || !visited.insert(record).second) {
      continue;
    }
    if (record->protocol_name == nullptr || record->owner_identity == nullptr) {
      return false;
    }
    ++visited_protocol_count;
    if (std::strcmp(record->protocol_name, protocol_name) == 0) {
      matched_owner_identity = record->owner_identity;
      return true;
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
    }
  }
  return false;
}

bool QueryProtocolConformanceFromAggregateUnlocked(
    const objc3_runtime_pointer_aggregate *protocol_refs,
    const char *protocol_name,
    std::unordered_set<const EmittedProtocolRecord *> &visited,
    std::uint64_t &visited_protocol_count, std::string &matched_owner_identity) {
  if (protocol_refs == nullptr) {
    return false;
  }
  for (std::uint64_t index = 0; index < protocol_refs->count; ++index) {
    const auto *protocol_record = static_cast<const EmittedProtocolRecord *>(
        AggregateEntry(protocol_refs, index));
    if (protocol_record == nullptr) {
      return false;
    }
    if (QueryProtocolConformanceFromProtocolRecordUnlocked(
            protocol_record, protocol_name, visited, visited_protocol_count,
            matched_owner_identity)) {
      return true;
    }
  }
  return false;
}

bool AttachRealizedCategoryRecordsUnlocked(RuntimeState &state,
                                           RealizedClassNode &node) {
  // M256-D003 category-attachment-protocol-conformance anchor: realized class
  // nodes now own preferred category attachments and direct protocol edges so
  // later dispatch/query paths consume the published graph instead of
  // rediscovering category/protocol relationships on each lookup.
  node.attached_category_records.clear();
  node.runtime_attachment_ready = false;
  if (node.image == nullptr) {
    return false;
  }
  std::vector<const EmittedCategoryRecord *> category_records;
  if (!CollectPreferredCategoryRecordsForImage(*node.image, node.class_name,
                                               category_records)) {
    return false;
  }
  for (const EmittedCategoryRecord *category_record : category_records) {
    if (category_record == nullptr || category_record->class_owner_identity == nullptr ||
        category_record->category_owner_identity == nullptr ||
        category_record->owner_identity == nullptr) {
      return false;
    }
    if (node.class_owner_identity != category_record->class_owner_identity) {
      return false;
    }
    node.attached_category_records.push_back(category_record);
    state.last_attached_category_owner_identity =
        category_record->category_owner_identity;
    state.last_attached_category_name = category_record->category_name != nullptr
                                            ? category_record->category_name
                                            : "";
    if (category_record->adopted_protocol_refs != nullptr) {
      state.realized_protocol_conformance_edge_count +=
          category_record->adopted_protocol_refs->count;
    }
  }
  state.realized_attached_category_count +=
      static_cast<std::uint64_t>(node.attached_category_records.size());
  if (node.bundle != nullptr &&
      node.bundle->class_record.adopted_protocol_refs != nullptr) {
    state.realized_protocol_conformance_edge_count +=
        node.bundle->class_record.adopted_protocol_refs->count;
  }
  node.runtime_attachment_ready = true;
  return true;
}

bool TryResolveMethodFromAttachedCategoriesUnlocked(
    RuntimeState &state, const RealizedClassNode &node, DispatchFamily family,
    std::uint64_t normalized_receiver_identity, std::uint64_t selector_stable_id,
    const char *selector_spelling, SlowPathResolution &resolution,
    bool &ambiguous, std::uint64_t &category_probe_count,
    std::uint64_t &protocol_probe_count) {
  // M255-D004 protocol/category-aware resolution anchor: once class bodies miss,
  // preferred category implementation records become the next live method tier
  // and adopted/inherited protocol records provide declaration-aware negative
  // lookup evidence for unsupported selectors.
  if (node.class_name.empty() || !node.runtime_attachment_ready) {
    return false;
  }
  for (const EmittedCategoryRecord *category_record :
       node.attached_category_records) {
    ++category_probe_count;
    if (!TryResolveMethodFromMethodListRefUnlocked(
            state, SelectMethodListRef(*category_record, family),
            node.class_name.c_str(),
            family, normalized_receiver_identity, selector_stable_id,
            selector_spelling, resolution, ambiguous)) {
      return false;
    }
    if (ambiguous || resolution.resolved) {
      return true;
    }
    if (!ProbeProtocolSelectorDeclarationsFromAggregateUnlocked(
            state, category_record->adopted_protocol_refs, family,
            selector_stable_id, selector_spelling, protocol_probe_count,
            ambiguous)) {
      return false;
    }
    if (ambiguous) {
      return true;
    }
  }
  return true;
}

bool IsImplementationOwnerIdentity(const char *owner_identity) {
  if (owner_identity == nullptr) {
    return false;
  }
  static constexpr char kImplementationPrefix[] = "implementation:";
  return std::string(owner_identity).rfind(kImplementationPrefix, 0) == 0;
}

bool IsSupportedRuntimeI32ReturnType(const char *return_type_name) {
  if (return_type_name == nullptr) {
    return false;
  }
  const std::string type_name = return_type_name;
  return type_name != "void" && type_name != "bool";
}

bool TryResolveRuntimeBuiltinObjectSampleMethod(
    const std::string &class_name, DispatchFamily family,
    const char *selector_spelling, SlowPathResolution &resolution) {
  if (class_name.empty() || selector_spelling == nullptr ||
      selector_spelling[0] == '\0') {
    return false;
  }
  const std::string selector = selector_spelling;
  if (family == DispatchFamily::Class &&
      (selector == "alloc" || selector == "new")) {
    resolution.resolved = true;
    resolution.dispatch_family_is_class = true;
    resolution.class_name = class_name;
    resolution.selector_storage = selector;
    resolution.owner_identity =
        "runtime-builtin:" + class_name + "::class_method:" + selector;
    resolution.parameter_count = 0;
    resolution.implementation = nullptr;
    resolution.builtin_kind =
        selector == "alloc" ? RuntimeBuiltinKind::Alloc : RuntimeBuiltinKind::New;
    return true;
  }
  if (family == DispatchFamily::Instance && selector == "init") {
    resolution.resolved = true;
    resolution.dispatch_family_is_class = false;
    resolution.class_name = class_name;
    resolution.selector_storage = selector;
    resolution.owner_identity =
        "runtime-builtin:" + class_name + "::instance_method:init";
    resolution.parameter_count = 0;
    resolution.implementation = nullptr;
    resolution.builtin_kind = RuntimeBuiltinKind::Init;
    return true;
  }
  return false;
}

bool DecodeReceiverIdentity(int receiver,
                            std::uint64_t &base_identity,
                            DispatchFamily &family,
                            std::uint64_t &normalized_receiver_identity) {
  if (receiver <= 0) {
    return false;
  }
  const std::int64_t signed_receiver = receiver;
  if (signed_receiver < static_cast<std::int64_t>(kReceiverIdentityBase)) {
    return false;
  }
  const std::int64_t delta =
      signed_receiver - static_cast<std::int64_t>(kReceiverIdentityBase);
  const std::int64_t ordinal =
      delta / static_cast<std::int64_t>(kReceiverIdentityStride);
  const std::int64_t salt =
      delta % static_cast<std::int64_t>(kReceiverIdentityStride);
  if (ordinal < 0) {
    return false;
  }
  base_identity = static_cast<std::uint64_t>(
      static_cast<std::int64_t>(kReceiverIdentityBase) +
      ordinal * static_cast<std::int64_t>(kReceiverIdentityStride));
  switch (salt) {
    case 0:
    case 2:
      family = DispatchFamily::Class;
      normalized_receiver_identity = base_identity + 2u;
      return true;
    case 1:
      family = DispatchFamily::Instance;
      normalized_receiver_identity = base_identity + 1u;
      return true;
    default:
      return false;
  }
}

std::vector<const RegisteredImageMetadata *> OrderedRegisteredImages(
    const RuntimeState &state) {
  std::vector<const RegisteredImageMetadata *> ordered;
  ordered.reserve(state.registered_image_metadata_by_identity_key.size());
  for (const auto &entry : state.registered_image_metadata_by_identity_key) {
    ordered.push_back(&entry.second);
  }
  std::sort(
      ordered.begin(), ordered.end(),
      [](const RegisteredImageMetadata *lhs, const RegisteredImageMetadata *rhs) {
        if (lhs->registration_order_ordinal != rhs->registration_order_ordinal) {
          return lhs->registration_order_ordinal < rhs->registration_order_ordinal;
        }
        return lhs->translation_unit_identity_key < rhs->translation_unit_identity_key;
      });
  return ordered;
}

bool CollectSortedImageClassNames(const RegisteredImageMetadata &record,
                                  std::vector<std::string> &class_names) {
  std::unordered_set<std::string> seen;
  seen.reserve(static_cast<std::size_t>(record.class_descriptor_count));
  for (std::uint64_t index = 0; index < record.class_descriptor_count; ++index) {
    const auto *bundle = static_cast<const EmittedClassBundle *>(
        AggregateEntry(record.class_descriptor_root, index));
    if (bundle == nullptr || bundle->class_record.class_name == nullptr ||
        bundle->class_record.class_name[0] == '\0') {
      return false;
    }
    seen.insert(bundle->class_record.class_name);
  }
  class_names.assign(seen.begin(), seen.end());
  std::sort(class_names.begin(), class_names.end());
  return true;
}

void RebuildRealizedClassGraphUnlocked(RuntimeState &state) {
  // M256-D002 metaclass-graph-root-class anchor: runtime now republishes a
  // realized class/metaclass graph keyed by stable receiver base identities,
  // preserving root classes as explicit graph nodes rather than rediscovering
  // the class family from emitted bundles on every dispatch.
  ClearRealizedClassGraphUnlocked(state);

  struct ReceiverBinding {
    std::string class_name;
    bool ambiguous = false;
  };

  std::unordered_map<std::size_t, ReceiverBinding> bindings_by_ordinal;
  std::vector<const RegisteredImageMetadata *> ordered_images =
      OrderedRegisteredImages(state);
  for (const RegisteredImageMetadata *record : ordered_images) {
    std::vector<std::string> class_names;
    if (!CollectSortedImageClassNames(*record, class_names)) {
      continue;
    }
    for (std::size_t ordinal = 0; ordinal < class_names.size(); ++ordinal) {
      ReceiverBinding &binding = bindings_by_ordinal[ordinal];
      if (binding.class_name.empty() && !binding.ambiguous) {
        binding.class_name = class_names[ordinal];
        continue;
      }
      if (binding.class_name != class_names[ordinal]) {
        binding.class_name.clear();
        binding.ambiguous = true;
      }
    }
  }
  for (const auto &[ordinal, binding] : bindings_by_ordinal) {
    const std::uint64_t base_identity = BuildReceiverBaseIdentity(ordinal);
    if (binding.ambiguous) {
      state.ambiguous_realized_base_identities.insert(base_identity);
      continue;
    }
    if (!binding.class_name.empty()) {
      state.realized_class_name_by_base_identity.emplace(base_identity,
                                                         binding.class_name);
    }
  }
  state.receiver_class_binding_count =
      static_cast<std::uint64_t>(state.realized_class_name_by_base_identity.size());

  std::unordered_map<const EmittedClassBundle *, std::size_t> node_index_by_bundle;
  for (const RegisteredImageMetadata *record : ordered_images) {
    std::vector<std::string> class_names;
    if (!CollectSortedImageClassNames(*record, class_names)) {
      continue;
    }
    std::unordered_map<std::string, std::size_t> ordinal_by_class_name;
    ordinal_by_class_name.reserve(class_names.size());
    for (std::size_t ordinal = 0; ordinal < class_names.size(); ++ordinal) {
      ordinal_by_class_name.emplace(class_names[ordinal], ordinal);
    }
    for (const std::string &class_name : class_names) {
      const auto ordinal_it = ordinal_by_class_name.find(class_name);
      if (ordinal_it == ordinal_by_class_name.end()) {
        continue;
      }
      const auto bundles = CollectPreferredClassBundlesForImage(*record, class_name);
      for (const EmittedClassBundle *bundle : bundles) {
        if (bundle == nullptr) {
          continue;
        }
        RealizedClassNode node;
        node.module_name = record->module_name;
        node.translation_unit_identity_key = record->translation_unit_identity_key;
        node.class_name = class_name;
        node.class_owner_identity =
            bundle->class_record.object_owner_identity != nullptr
                ? bundle->class_record.object_owner_identity
                : "";
        node.metaclass_owner_identity =
            bundle->metaclass_record.object_owner_identity != nullptr
                ? bundle->metaclass_record.object_owner_identity
                : "";
        node.super_class_owner_identity =
            bundle->class_record.super_owner_identity != nullptr
                ? bundle->class_record.super_owner_identity
                : "";
        node.super_metaclass_owner_identity =
            bundle->metaclass_record.super_owner_identity != nullptr
                ? bundle->metaclass_record.super_owner_identity
                : "";
        node.registration_order_ordinal = record->registration_order_ordinal;
        node.base_identity = BuildReceiverBaseIdentity(ordinal_it->second);
        node.is_root_class = bundle->class_record.super_bundle == nullptr;
        node.implementation_backed =
            (bundle->class_record.method_list_ref != nullptr &&
             IsImplementationOwnerIdentity(
                 bundle->class_record.method_list_ref->owner_identity)) ||
            (bundle->metaclass_record.method_list_ref != nullptr &&
             IsImplementationOwnerIdentity(
                 bundle->metaclass_record.method_list_ref->owner_identity));
        node.image = record;
        node.bundle = bundle;
        const std::size_t node_index = state.realized_class_nodes.size();
        state.realized_class_nodes.push_back(std::move(node));
        node_index_by_bundle.emplace(bundle, node_index);
        state.realized_class_node_indices_by_name[class_name].push_back(node_index);
      }
    }
  }

  for (std::size_t index = 0; index < state.realized_class_nodes.size(); ++index) {
    RealizedClassNode &node = state.realized_class_nodes[index];
    if (node.bundle != nullptr && node.bundle->class_record.super_bundle != nullptr) {
      const auto super_it = node_index_by_bundle.find(
          static_cast<const EmittedClassBundle *>(node.bundle->class_record.super_bundle));
      if (super_it != node_index_by_bundle.end()) {
        node.super_node_index = super_it->second;
        node.has_super_node = true;
        ++state.realized_metaclass_edge_count;
      }
    }
    if (node.is_root_class) {
      ++state.realized_root_class_count;
    }
    (void)AttachRealizedCategoryRecordsUnlocked(state, node);
  }

  if (!state.realized_class_nodes.empty()) {
    const RealizedClassNode &last_node = state.realized_class_nodes.back();
    state.last_realized_class_name = last_node.class_name;
    state.last_realized_class_owner_identity = last_node.class_owner_identity;
    state.last_realized_metaclass_owner_identity =
        last_node.metaclass_owner_identity;
  }
}

bool ResolveReceiverClassNameUnlocked(const RuntimeState &state,
                                      std::uint64_t base_identity,
                                      std::string &class_name,
                                      bool &ambiguous) {
  ambiguous = false;
  if (base_identity < kReceiverIdentityBase ||
      ((base_identity - kReceiverIdentityBase) % kReceiverIdentityStride) != 0) {
    return false;
  }
  if (state.ambiguous_realized_base_identities.find(base_identity) !=
      state.ambiguous_realized_base_identities.end()) {
    ambiguous = true;
    return false;
  }
  const auto found =
      state.realized_class_name_by_base_identity.find(base_identity);
  if (found == state.realized_class_name_by_base_identity.end()) {
    return false;
  }
  class_name = found->second;
  return true;
}

std::vector<const EmittedClassBundle *> CollectPreferredClassBundlesForImage(
    const RegisteredImageMetadata &record, const std::string &class_name) {
  std::vector<const EmittedClassBundle *> implementation_bundles;
  std::vector<const EmittedClassBundle *> fallback_bundles;
  implementation_bundles.reserve(static_cast<std::size_t>(record.class_descriptor_count));
  fallback_bundles.reserve(static_cast<std::size_t>(record.class_descriptor_count));
  for (std::uint64_t index = 0; index < record.class_descriptor_count; ++index) {
    const auto *bundle = static_cast<const EmittedClassBundle *>(
        AggregateEntry(record.class_descriptor_root, index));
    if (bundle == nullptr || bundle->class_record.class_name == nullptr ||
        class_name != bundle->class_record.class_name) {
      continue;
    }
    const bool implementation_backed =
        bundle->class_record.method_list_ref != nullptr &&
        IsImplementationOwnerIdentity(bundle->class_record.method_list_ref->owner_identity);
    if (implementation_backed) {
      implementation_bundles.push_back(bundle);
    } else {
      fallback_bundles.push_back(bundle);
    }
  }
  return implementation_bundles.empty() ? fallback_bundles
                                        : implementation_bundles;
}

bool TryResolveMethodFromRealizedClassChainUnlocked(
    RuntimeState &state, const RealizedClassNode *start_node,
    DispatchFamily family, std::uint64_t normalized_receiver_identity,
    std::uint64_t selector_stable_id, const char *selector_spelling,
    SlowPathResolution &resolution, bool &ambiguous,
    std::uint64_t &category_probe_count,
    std::uint64_t &protocol_probe_count) {
  // M256-D001 class-realization-runtime freeze anchor: runtime walks the
  // emitted class/metaclass chain directly, consults attached categories at
  // each realized class node, and preserves protocol checks as
  // declaration-aware negative evidence only.
  std::unordered_set<const RealizedClassNode *> visited;
  const RealizedClassNode *node = start_node;
  while (node != nullptr && visited.insert(node).second) {
    if (node->bundle == nullptr || node->image == nullptr) {
      return false;
    }
    const EmittedClassBundle *bundle = node->bundle;
    const EmittedClassRecord &record =
        family == DispatchFamily::Class ? bundle->metaclass_record
                                        : bundle->class_record;
    if (!TryResolveMethodFromMethodListRefUnlocked(
            state, record.method_list_ref, record.class_name, family,
            normalized_receiver_identity, selector_stable_id, selector_spelling,
            resolution, ambiguous)) {
      return false;
    }
    if (ambiguous || resolution.resolved) {
      return true;
    }
    if (!TryResolveMethodFromAttachedCategoriesUnlocked(
            state, *node, family,
            normalized_receiver_identity, selector_stable_id, selector_spelling,
            resolution, ambiguous, category_probe_count,
            protocol_probe_count)) {
      return false;
    }
    if (ambiguous || resolution.resolved) {
      return true;
    }
    if (!ProbeProtocolSelectorDeclarationsFromAggregateUnlocked(
            state, record.adopted_protocol_refs, family, selector_stable_id,
            selector_spelling, protocol_probe_count, ambiguous)) {
      return false;
    }
    if (ambiguous) {
      return true;
    }
    node = node->has_super_node ? &state.realized_class_nodes[node->super_node_index]
                                : nullptr;
  }
  return true;
}

SlowPathResolution ResolveMethodSlowPathUnlocked(
    RuntimeState &state, std::uint64_t base_identity,
    std::uint64_t normalized_receiver_identity, DispatchFamily family,
    std::uint64_t selector_stable_id, const char *selector_spelling) {
  // M255-D003: live lookup walks the emitted class/metaclass graph that came
  // through startup registration, resolves one deterministic class family for
  // the receiver identity, and then records the outcome in the method cache.
  SlowPathResolution resolution;
  resolution.selector_storage = selector_spelling != nullptr ? selector_spelling : "";
  resolution.normalized_receiver_identity = normalized_receiver_identity;
  resolution.selector_stable_id = selector_stable_id;
  std::uint64_t category_probe_count = 0;
  std::uint64_t protocol_probe_count = 0;

  bool receiver_ambiguous = false;
  std::string resolved_class_name;
  if (!ResolveReceiverClassNameUnlocked(state, base_identity, resolved_class_name,
                                        receiver_ambiguous)) {
    resolution.ambiguous = receiver_ambiguous;
    return resolution;
  }

  const auto realized_nodes_it =
      state.realized_class_node_indices_by_name.find(resolved_class_name);
  if (realized_nodes_it == state.realized_class_node_indices_by_name.end()) {
    return resolution;
  }

  for (const std::size_t node_index : realized_nodes_it->second) {
    if (node_index >= state.realized_class_nodes.size()) {
      continue;
    }
    const RealizedClassNode &node = state.realized_class_nodes[node_index];
    SlowPathResolution image_resolution;
    image_resolution.selector_storage =
        selector_spelling != nullptr ? selector_spelling : "";
    image_resolution.normalized_receiver_identity = normalized_receiver_identity;
    image_resolution.selector_stable_id = selector_stable_id;
    std::uint64_t image_category_probe_count = 0;
    std::uint64_t image_protocol_probe_count = 0;
    bool method_ambiguous = false;
    if (!TryResolveMethodFromRealizedClassChainUnlocked(
            state, &node, family, normalized_receiver_identity,
            selector_stable_id, selector_spelling, image_resolution,
            method_ambiguous, image_category_probe_count,
            image_protocol_probe_count)) {
      return SlowPathResolution{};
    }
    if (method_ambiguous) {
      resolution = SlowPathResolution{};
      resolution.ambiguous = true;
      resolution.selector_storage =
          selector_spelling != nullptr ? selector_spelling : "";
      resolution.normalized_receiver_identity = normalized_receiver_identity;
      resolution.selector_stable_id = selector_stable_id;
      resolution.category_probe_count =
          category_probe_count + image_category_probe_count;
      resolution.protocol_probe_count =
          protocol_probe_count + image_protocol_probe_count;
      return resolution;
    }
    category_probe_count += image_category_probe_count;
    protocol_probe_count += image_protocol_probe_count;
    if (image_resolution.resolved) {
      if (resolution.resolved &&
          (resolution.implementation != image_resolution.implementation ||
           resolution.parameter_count != image_resolution.parameter_count ||
           resolution.owner_identity != image_resolution.owner_identity ||
           resolution.class_name != image_resolution.class_name)) {
        resolution = SlowPathResolution{};
        resolution.ambiguous = true;
        resolution.selector_storage =
            selector_spelling != nullptr ? selector_spelling : "";
        resolution.normalized_receiver_identity = normalized_receiver_identity;
        resolution.selector_stable_id = selector_stable_id;
        resolution.category_probe_count = category_probe_count;
        resolution.protocol_probe_count = protocol_probe_count;
        return resolution;
      }
      if (!resolution.resolved) {
        resolution = image_resolution;
      }
    }
  }
  resolution.category_probe_count = category_probe_count;
  resolution.protocol_probe_count = protocol_probe_count;
  if (!resolution.resolved && !resolution.ambiguous &&
      TryResolveRuntimeBuiltinObjectSampleMethod(resolved_class_name, family,
                                                selector_spelling, resolution)) {
    resolution.normalized_receiver_identity = normalized_receiver_identity;
    resolution.selector_stable_id = selector_stable_id;
    resolution.category_probe_count = category_probe_count;
    resolution.protocol_probe_count = protocol_probe_count;
  }
  return resolution;
}

int InvokeResolvedMethod(const void *implementation, std::uint64_t parameter_count,
                         int a0, int a1, int a2, int a3) {
  switch (parameter_count) {
    case 0:
      return reinterpret_cast<int (*)()>(const_cast<void *>(implementation))();
    case 1:
      return reinterpret_cast<int (*)(int)>(const_cast<void *>(implementation))(a0);
    case 2:
      return reinterpret_cast<int (*)(int, int)>(
          const_cast<void *>(implementation))(a0, a1);
    case 3:
      return reinterpret_cast<int (*)(int, int, int)>(
          const_cast<void *>(implementation))(a0, a1, a2);
    case 4:
      return reinterpret_cast<int (*)(int, int, int, int)>(
          const_cast<void *>(implementation))(a0, a1, a2, a3);
    default:
      return 0;
  }
}

int InvokeRuntimeBuiltinMethod(RuntimeBuiltinKind builtin_kind, int receiver,
                               std::uint64_t base_identity) {
  switch (builtin_kind) {
    case RuntimeBuiltinKind::Alloc:
    case RuntimeBuiltinKind::New:
      return static_cast<int>(base_identity + 1u);
    case RuntimeBuiltinKind::Init:
      return receiver;
    case RuntimeBuiltinKind::None:
      return 0;
  }
  return 0;
}

const objc3_runtime_selector_handle *LookupSelectorUnlocked(const char *selector) {
  // M255-D002 selector-table anchor: metadata-backed selector pools now
  // materialize the canonical runtime selector table, while direct lookup of
  // non-emitted selectors remains a dynamic fallback. M255-D003 layers
  // method-cache and class/metaclass slow-path dispatch on top of the same
  // preserved public lookup/dispatch surface.
  if (selector == nullptr) {
    return nullptr;
  }

  RuntimeState &state = State();
  const auto found = state.selector_index_by_name.find(selector);
  if (found != state.selector_index_by_name.end()) {
    return &state.selector_slots[found->second].handle;
  }

  SelectorSlot slot;
  slot.spelling_storage = selector;
  state.selector_slots.push_back(std::move(slot));
  RefreshSelectorHandlePointersUnlocked(state);
  SelectorSlot &stored = state.selector_slots.back();
  stored.handle.stable_id = static_cast<std::uint64_t>(state.selector_slots.size());
  const std::size_t index = state.selector_slots.size() - 1u;
  state.selector_index_by_name.emplace(stored.spelling_storage, index);
  ++state.dynamic_selector_count;
  state.last_materialized_selector = stored.spelling_storage;
  state.last_materialized_stable_id = stored.handle.stable_id;
  state.last_materialized_registration_order_ordinal = 0;
  state.last_materialized_selector_pool_index = 0;
  state.last_materialized_from_metadata = false;
  return &stored.handle;
}

bool MaterializeSelectorLookupEntryUnlocked(RuntimeState &state,
                                            const char *selector,
                                            std::uint64_t registration_order_ordinal,
                                            std::uint64_t selector_pool_index) {
  if (selector == nullptr || selector[0] == '\0') {
    return false;
  }

  const auto found = state.selector_index_by_name.find(selector);
  if (found == state.selector_index_by_name.end()) {
    SelectorSlot slot;
    slot.spelling_storage = selector;
    slot.metadata_backed = true;
    slot.metadata_provider_count = 1;
    slot.first_registration_order_ordinal = registration_order_ordinal;
    slot.last_registration_order_ordinal = registration_order_ordinal;
    slot.first_selector_pool_index = selector_pool_index;
    slot.last_selector_pool_index = selector_pool_index;
    state.selector_slots.push_back(std::move(slot));
    RefreshSelectorHandlePointersUnlocked(state);
    SelectorSlot &stored = state.selector_slots.back();
    stored.handle.stable_id =
        static_cast<std::uint64_t>(state.selector_slots.size());
    state.selector_index_by_name.emplace(stored.spelling_storage,
                                         state.selector_slots.size() - 1u);
    ++state.metadata_backed_selector_count;
    ++state.metadata_provider_edge_count;
    state.last_materialized_selector = stored.spelling_storage;
    state.last_materialized_stable_id = stored.handle.stable_id;
    state.last_materialized_registration_order_ordinal =
        registration_order_ordinal;
    state.last_materialized_selector_pool_index = selector_pool_index;
    state.last_materialized_from_metadata = true;
    return true;
  }

  SelectorSlot &stored = state.selector_slots[found->second];
  if (!stored.metadata_backed) {
    stored.metadata_backed = true;
    stored.first_registration_order_ordinal = registration_order_ordinal;
    stored.first_selector_pool_index = selector_pool_index;
    ++state.metadata_backed_selector_count;
    if (state.dynamic_selector_count > 0) {
      --state.dynamic_selector_count;
    }
  }
  ++stored.metadata_provider_count;
  stored.last_registration_order_ordinal = registration_order_ordinal;
  stored.last_selector_pool_index = selector_pool_index;
  ++state.metadata_provider_edge_count;
  state.last_materialized_selector = stored.spelling_storage;
  state.last_materialized_stable_id = stored.handle.stable_id;
  state.last_materialized_registration_order_ordinal =
      registration_order_ordinal;
  state.last_materialized_selector_pool_index = selector_pool_index;
  state.last_materialized_from_metadata = true;
  return true;
}

std::int64_t ComputeSelectorScore(const char *selector) {
  if (selector == nullptr) {
    return 0;
  }

  std::int64_t selector_score = 0;
  std::int64_t index = 1;
  const unsigned char *cursor =
      reinterpret_cast<const unsigned char *>(selector);
  while (*cursor != 0U) {
    selector_score =
        (selector_score + (static_cast<std::int64_t>(*cursor) * index)) %
        kDispatchModulus;
    ++cursor;
    ++index;
  }
  return selector_score;
}

int ComputeDispatchResult(int receiver, const char *selector, int a0, int a1,
                          int a2, int a3) {
  // M255-D001 lookup-dispatch-runtime anchor: live dispatch still preserves the
  // deterministic formula as the fail-closed fallback. M255-D003 now routes
  // supported class/metaclass resolutions through emitted method bodies first
  // and returns to this arithmetic path only for unresolved or unsupported
  // runtime lookups without changing the canonical dispatch entrypoint.
  std::int64_t value = 41;
  value += static_cast<std::int64_t>(receiver) * 97;
  value += static_cast<std::int64_t>(a0) * 7;
  value += static_cast<std::int64_t>(a1) * 11;
  value += static_cast<std::int64_t>(a2) * 13;
  value += static_cast<std::int64_t>(a3) * 17;
  value += ComputeSelectorScore(selector) * 19;
  value %= kDispatchModulus;
  if (value < 0) {
    value += kDispatchModulus;
  }
  return static_cast<int>(value);
}

std::uint64_t DescriptorTotal(
    const objc3_runtime_image_descriptor *image) {
  return image->class_descriptor_count + image->protocol_descriptor_count +
         image->category_descriptor_count + image->property_descriptor_count +
         image->ivar_descriptor_count;
}

std::uint64_t AggregateCount(const objc3_runtime_pointer_aggregate *aggregate) {
  return aggregate == nullptr ? 0 : aggregate->count;
}

const void *AggregateEntry(const objc3_runtime_pointer_aggregate *aggregate,
                           std::uint64_t index) {
  if (aggregate == nullptr || index >= aggregate->count) {
    return nullptr;
  }
  return aggregate->entries[index];
}

bool AggregateContainsPointer(const objc3_runtime_pointer_aggregate *aggregate,
                              const void *target) {
  if (aggregate == nullptr || target == nullptr) {
    return false;
  }
  for (std::uint64_t index = 0; index < aggregate->count; ++index) {
    if (aggregate->entries[index] == target) {
      return true;
    }
  }
  return false;
}

bool ImageDescriptorsMatch(const objc3_runtime_image_descriptor *lhs,
                           const objc3_runtime_image_descriptor *rhs) {
  if (lhs == nullptr || rhs == nullptr) {
    return false;
  }

  const std::string lhs_module =
      lhs->module_name != nullptr ? lhs->module_name : "";
  const std::string rhs_module =
      rhs->module_name != nullptr ? rhs->module_name : "";
  const std::string lhs_identity =
      lhs->translation_unit_identity_key != nullptr
          ? lhs->translation_unit_identity_key
          : "";
  const std::string rhs_identity =
      rhs->translation_unit_identity_key != nullptr
          ? rhs->translation_unit_identity_key
          : "";
  return lhs_module == rhs_module &&
         lhs_identity == rhs_identity &&
         lhs->registration_order_ordinal == rhs->registration_order_ordinal &&
         lhs->class_descriptor_count == rhs->class_descriptor_count &&
         lhs->protocol_descriptor_count == rhs->protocol_descriptor_count &&
         lhs->category_descriptor_count == rhs->category_descriptor_count &&
         lhs->property_descriptor_count == rhs->property_descriptor_count &&
         lhs->ivar_descriptor_count == rhs->ivar_descriptor_count;
}

const char *StableCString(const std::string &text) {
  return text.empty() ? nullptr : text.c_str();
}

void MarkRejectedRegistrationUnlocked(
    RuntimeState &state, const objc3_runtime_image_descriptor *image,
    int status) {
  state.last_registration_status = status;
  state.last_rejected_module_name =
      image != nullptr && image->module_name != nullptr ? image->module_name : "";
  state.last_rejected_translation_unit_identity_key =
      image != nullptr && image->translation_unit_identity_key != nullptr
          ? image->translation_unit_identity_key
          : "";
  state.last_rejected_registration_order_ordinal =
      image != nullptr ? image->registration_order_ordinal : 0;
}

void ClearRejectedRegistrationUnlocked(RuntimeState &state) {
  state.last_rejected_module_name.clear();
  state.last_rejected_translation_unit_identity_key.clear();
  state.last_rejected_registration_order_ordinal = 0;
}

void ClearImageWalkSnapshotUnlocked(RuntimeState &state) {
  state.last_discovery_root_entry_count = 0;
  state.last_walked_class_descriptor_count = 0;
  state.last_walked_protocol_descriptor_count = 0;
  state.last_walked_category_descriptor_count = 0;
  state.last_walked_property_descriptor_count = 0;
  state.last_walked_ivar_descriptor_count = 0;
  state.last_walked_selector_pool_count = 0;
  state.last_walked_string_pool_count = 0;
  state.last_linker_anchor_matches_discovery_root = false;
  state.last_registration_used_staged_table = false;
  state.last_walked_module_name.clear();
  state.last_walked_translation_unit_identity_key.clear();
}

void ApplyImageWalkRecordUnlocked(RuntimeState &state,
                                  const RegisteredImageMetadata &record) {
  state.walked_image_count = static_cast<std::uint64_t>(
      state.registered_image_metadata_by_identity_key.size());
  state.last_discovery_root_entry_count = record.discovery_root_entry_count;
  state.last_walked_class_descriptor_count = record.class_descriptor_count;
  state.last_walked_protocol_descriptor_count = record.protocol_descriptor_count;
  state.last_walked_category_descriptor_count = record.category_descriptor_count;
  state.last_walked_property_descriptor_count = record.property_descriptor_count;
  state.last_walked_ivar_descriptor_count = record.ivar_descriptor_count;
  state.last_walked_selector_pool_count = record.selector_pool_count;
  state.last_walked_string_pool_count = record.string_pool_count;
  state.last_linker_anchor_matches_discovery_root =
      record.linker_anchor_matches_discovery_root;
  state.last_registration_used_staged_table =
      record.used_staged_registration_table;
  state.last_walked_module_name = record.module_name;
  state.last_walked_translation_unit_identity_key =
      record.translation_unit_identity_key;
}

void RetainBootstrapRecordUnlocked(RuntimeState &state,
                                   const RegisteredImageMetadata &record) {
  const auto found = state.retained_bootstrap_metadata_by_identity_key.find(
      record.translation_unit_identity_key);
  if (found == state.retained_bootstrap_metadata_by_identity_key.end()) {
    state.retained_bootstrap_identity_order.push_back(
        record.translation_unit_identity_key);
  }
  state.retained_bootstrap_metadata_by_identity_key
      [record.translation_unit_identity_key] = record;
}

std::uint64_t ZeroRetainedBootstrapImageLocalInitStatesUnlocked(
    RuntimeState &state) {
  std::uint64_t cleared_count = 0;
  for (const std::string &identity_key : state.retained_bootstrap_identity_order) {
    const auto found =
        state.retained_bootstrap_metadata_by_identity_key.find(identity_key);
    if (found == state.retained_bootstrap_metadata_by_identity_key.end()) {
      continue;
    }
    const RegisteredImageMetadata &record = found->second;
    if (record.registration_table == nullptr ||
        record.registration_table->image_local_init_state == nullptr) {
      continue;
    }
    *record.registration_table->image_local_init_state = 0;
    ++cleared_count;
  }
  return cleared_count;
}

void ClearLiveRegistrationStateUnlocked(RuntimeState &state) {
  state.registered_image_count = 0;
  state.registered_descriptor_total = 0;
  state.next_expected_registration_order_ordinal = 1;
  state.last_successful_registration_order_ordinal = 0;
  state.last_registration_status = OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  state.last_registered_module_name.clear();
  state.last_registered_translation_unit_identity_key.clear();
  state.last_rejected_module_name.clear();
  state.last_rejected_translation_unit_identity_key.clear();
  state.last_rejected_registration_order_ordinal = 0;
  state.registration_order_by_identity_key.clear();
  state.registered_image_metadata_by_identity_key.clear();
  state.selector_index_by_name.clear();
  state.selector_slots.clear();
  state.metadata_backed_selector_count = 0;
  state.dynamic_selector_count = 0;
  state.metadata_provider_edge_count = 0;
  state.last_materialized_selector.clear();
  state.last_materialized_stable_id = 0;
  state.last_materialized_registration_order_ordinal = 0;
  state.last_materialized_selector_pool_index = 0;
  state.last_materialized_from_metadata = false;
  ClearMethodCacheStateUnlocked(state);
  ClearRealizedClassGraphUnlocked(state);
  state.staged_registration_table = nullptr;
  state.walked_image_count = 0;
  ClearImageWalkSnapshotUnlocked(state);
}

bool TryWalkRegistrationTableUnlocked(
    RuntimeState &state,
    const objc3_runtime_registration_table *registration_table,
    const objc3_runtime_image_descriptor *image,
    RegisteredImageMetadata &record) {
  // M263-D001 runtime-bootstrap-table-consumption anchor: staged registration
  // tables must match the image descriptor exactly, discovery-root membership
  // must close over every descriptor family before image state is published,
  // and the walked image snapshot is derived from this validated table rather
  // than from sidecar manifests or replay-only bookkeeping.
  if (registration_table == nullptr ||
      registration_table->abi_version != 1 ||
      registration_table->pointer_field_count != 11 ||
      registration_table->image_descriptor == nullptr ||
      !ImageDescriptorsMatch(registration_table->image_descriptor, image) ||
      registration_table->discovery_root == nullptr ||
      registration_table->linker_anchor == nullptr ||
      registration_table->class_descriptor_root == nullptr ||
      registration_table->protocol_descriptor_root == nullptr ||
      registration_table->category_descriptor_root == nullptr ||
      registration_table->property_descriptor_root == nullptr ||
      registration_table->ivar_descriptor_root == nullptr ||
      registration_table->image_local_init_state == nullptr) {
    return false;
  }

  const void *const linker_anchor_target =
      *reinterpret_cast<const void *const *>(registration_table->linker_anchor);
  const std::uint64_t discovery_root_entry_count =
      AggregateCount(registration_table->discovery_root);
  if (discovery_root_entry_count < 6) {
    return false;
  }

  const std::uint64_t class_descriptor_count =
      AggregateCount(registration_table->class_descriptor_root);
  const std::uint64_t protocol_descriptor_count =
      AggregateCount(registration_table->protocol_descriptor_root);
  const std::uint64_t category_descriptor_count =
      AggregateCount(registration_table->category_descriptor_root);
  const std::uint64_t property_descriptor_count =
      AggregateCount(registration_table->property_descriptor_root);
  const std::uint64_t ivar_descriptor_count =
      AggregateCount(registration_table->ivar_descriptor_root);
  const std::uint64_t selector_pool_count =
      AggregateCount(registration_table->selector_pool_root);
  const std::uint64_t string_pool_count =
      AggregateCount(registration_table->string_pool_root);
  const bool linker_anchor_matches_discovery_root =
      linker_anchor_target == registration_table->discovery_root;

  if (class_descriptor_count != image->class_descriptor_count ||
      protocol_descriptor_count != image->protocol_descriptor_count ||
      category_descriptor_count != image->category_descriptor_count ||
      property_descriptor_count != image->property_descriptor_count ||
      ivar_descriptor_count != image->ivar_descriptor_count ||
      !linker_anchor_matches_discovery_root ||
      !AggregateContainsPointer(registration_table->discovery_root,
                                registration_table->class_descriptor_root) ||
      !AggregateContainsPointer(registration_table->discovery_root,
                                registration_table->protocol_descriptor_root) ||
      !AggregateContainsPointer(registration_table->discovery_root,
                                registration_table->category_descriptor_root) ||
      !AggregateContainsPointer(registration_table->discovery_root,
                                registration_table->property_descriptor_root) ||
      !AggregateContainsPointer(registration_table->discovery_root,
                                registration_table->ivar_descriptor_root)) {
    return false;
  }

  if (registration_table->selector_pool_root != nullptr &&
      !AggregateContainsPointer(registration_table->discovery_root,
                                registration_table->selector_pool_root)) {
    return false;
  }
  if (registration_table->string_pool_root != nullptr &&
      !AggregateContainsPointer(registration_table->discovery_root,
                                registration_table->string_pool_root)) {
    return false;
  }

  std::unordered_set<std::string> selector_pool_spelling_set;
  selector_pool_spelling_set.reserve(
      static_cast<std::size_t>(selector_pool_count));
  std::vector<std::string> selector_pool_spellings;
  selector_pool_spellings.reserve(static_cast<std::size_t>(selector_pool_count));
  for (std::uint64_t index = 0; index < selector_pool_count; ++index) {
    const char *selector = reinterpret_cast<const char *>(
        AggregateEntry(registration_table->selector_pool_root, index));
    if (selector == nullptr || selector[0] == '\0') {
      return false;
    }
    const auto inserted = selector_pool_spelling_set.emplace(selector);
    if (!inserted.second) {
      return false;
    }
    selector_pool_spellings.emplace_back(selector);
  }
  for (std::uint64_t index = 0; index < string_pool_count; ++index) {
    const char *value = reinterpret_cast<const char *>(
        AggregateEntry(registration_table->string_pool_root, index));
    if (value == nullptr) {
      return false;
    }
  }

  record.module_name = image->module_name;
  record.translation_unit_identity_key = image->translation_unit_identity_key;
  record.registration_order_ordinal = image->registration_order_ordinal;
  record.registration_table = registration_table;
  record.discovery_root = registration_table->discovery_root;
  record.class_descriptor_root = registration_table->class_descriptor_root;
  record.protocol_descriptor_root = registration_table->protocol_descriptor_root;
  record.category_descriptor_root = registration_table->category_descriptor_root;
  record.property_descriptor_root = registration_table->property_descriptor_root;
  record.ivar_descriptor_root = registration_table->ivar_descriptor_root;
  record.selector_pool_root = registration_table->selector_pool_root;
  record.string_pool_root = registration_table->string_pool_root;
  record.discovery_root_entry_count = discovery_root_entry_count;
  record.class_descriptor_count = class_descriptor_count;
  record.protocol_descriptor_count = protocol_descriptor_count;
  record.category_descriptor_count = category_descriptor_count;
  record.property_descriptor_count = property_descriptor_count;
  record.ivar_descriptor_count = ivar_descriptor_count;
  record.selector_pool_count = selector_pool_count;
  record.string_pool_count = string_pool_count;
  record.linker_anchor_matches_discovery_root =
      linker_anchor_matches_discovery_root;
  record.used_staged_registration_table = true;

  for (std::size_t index = 0; index < selector_pool_spellings.size(); ++index) {
    if (!MaterializeSelectorLookupEntryUnlocked(
            state, selector_pool_spellings[index].c_str(),
            image->registration_order_ordinal,
            static_cast<std::uint64_t>(index + 1u))) {
      return false;
    }
  }
  return true;
}

int RegisterImageUnlocked(
    RuntimeState &state, const objc3_runtime_image_descriptor *image,
    const objc3_runtime_registration_table *staged_registration_table,
    bool retain_bootstrap_record, bool mark_image_local_init_state) {
  // M263-D001 runtime-bootstrap-table-consumption anchor: duplicate identity
  // rejection and out-of-order rejection happen before live counters advance,
  // while successful staged-table consumption is the only path allowed to
  // publish bootstrap-visible image-walk state.
  if (image == nullptr || image->module_name == nullptr ||
      image->module_name[0] == '\0' ||
      image->translation_unit_identity_key == nullptr ||
      image->translation_unit_identity_key[0] == '\0' ||
      image->registration_order_ordinal == 0) {
    MarkRejectedRegistrationUnlocked(
        state, image, OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR);
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  if (state.registration_order_by_identity_key.find(
          image->translation_unit_identity_key) !=
      state.registration_order_by_identity_key.end()) {
    MarkRejectedRegistrationUnlocked(
        state, image,
        OBJC3_RUNTIME_REGISTRATION_STATUS_DUPLICATE_TRANSLATION_UNIT_IDENTITY_KEY);
    return OBJC3_RUNTIME_REGISTRATION_STATUS_DUPLICATE_TRANSLATION_UNIT_IDENTITY_KEY;
  }

  if (image->registration_order_ordinal !=
      state.next_expected_registration_order_ordinal) {
    MarkRejectedRegistrationUnlocked(
        state, image,
        OBJC3_RUNTIME_REGISTRATION_STATUS_OUT_OF_ORDER_REGISTRATION);
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OUT_OF_ORDER_REGISTRATION;
  }

  std::uint64_t descriptor_total = DescriptorTotal(image);
  if (staged_registration_table != nullptr) {
    RegisteredImageMetadata record;
    if (!TryWalkRegistrationTableUnlocked(state, staged_registration_table, image,
                                          record)) {
      ClearImageWalkSnapshotUnlocked(state);
      MarkRejectedRegistrationUnlocked(
          state, image, OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR);
      return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
    }
    descriptor_total = record.class_descriptor_count +
                       record.protocol_descriptor_count +
                       record.category_descriptor_count +
                       record.property_descriptor_count +
                       record.ivar_descriptor_count;
    if (retain_bootstrap_record) {
      RetainBootstrapRecordUnlocked(state, record);
    }
    state.registered_image_metadata_by_identity_key
        [record.translation_unit_identity_key] = record;
    ApplyImageWalkRecordUnlocked(
        state,
        state.registered_image_metadata_by_identity_key
            .at(record.translation_unit_identity_key));
    if (mark_image_local_init_state &&
        staged_registration_table->image_local_init_state != nullptr) {
      *staged_registration_table->image_local_init_state = 1;
    }
  } else {
    ClearImageWalkSnapshotUnlocked(state);
  }

  ++state.registered_image_count;
  state.registered_descriptor_total += descriptor_total;
  state.next_expected_registration_order_ordinal =
      image->registration_order_ordinal + 1;
  state.last_successful_registration_order_ordinal =
      image->registration_order_ordinal;
  state.last_registration_status = OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  state.last_registered_module_name = image->module_name;
  state.last_registered_translation_unit_identity_key =
      image->translation_unit_identity_key;
  state.registration_order_by_identity_key.emplace(
      image->translation_unit_identity_key, image->registration_order_ordinal);
  // M256-D002 metaclass-graph-root-class anchor: successful registration now
  // republishes a runtime-owned realized class/metaclass graph and root-class
  // baseline before dispatch can consume the image.
  RebuildRealizedClassGraphUnlocked(state);
  ClearRejectedRegistrationUnlocked(state);
  ClearMethodCacheStateUnlocked(state);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

bool ProtocolExistsByNameUnlocked(const RuntimeState &state,
                                  const char *protocol_name) {
  if (protocol_name == nullptr || protocol_name[0] == '\0') {
    return false;
  }
  for (const RegisteredImageMetadata *record : OrderedRegisteredImages(state)) {
    for (std::uint64_t index = 0; index < record->protocol_descriptor_count;
         ++index) {
      const auto *protocol_record = static_cast<const EmittedProtocolRecord *>(
          AggregateEntry(record->protocol_descriptor_root, index));
      if (protocol_record == nullptr || protocol_record->protocol_name == nullptr) {
        continue;
      }
      if (std::strcmp(protocol_record->protocol_name, protocol_name) == 0) {
        return true;
      }
    }
  }
  return false;
}

bool QueryRealizedClassProtocolConformanceUnlocked(
    RuntimeState &state, const RealizedClassNode *start_node,
    const char *protocol_name, std::uint64_t &visited_protocol_count,
    std::string &matched_protocol_owner_identity,
    std::string &matched_attachment_owner_identity) {
  // M256-D003 category-attachment-protocol-conformance anchor: runtime-facing
  // protocol conformance queries walk realized class nodes, attached category
  // protocol refs, and inherited protocol closures without widening the public
  // dispatch ABI.
  if (start_node == nullptr || protocol_name == nullptr || protocol_name[0] == '\0') {
    return false;
  }
  std::unordered_set<const EmittedProtocolRecord *> visited_protocols;
  std::unordered_set<const RealizedClassNode *> visited_nodes;
  const RealizedClassNode *node = start_node;
  while (node != nullptr && visited_nodes.insert(node).second) {
    if (node->bundle == nullptr) {
      return false;
    }
    matched_attachment_owner_identity.clear();
    const EmittedClassRecord &record = node->bundle->class_record;
    if (QueryProtocolConformanceFromAggregateUnlocked(
            record.adopted_protocol_refs, protocol_name, visited_protocols,
            visited_protocol_count, matched_protocol_owner_identity)) {
      return true;
    }
    for (const EmittedCategoryRecord *category_record :
         node->attached_category_records) {
      if (QueryProtocolConformanceFromAggregateUnlocked(
              category_record->adopted_protocol_refs, protocol_name,
              visited_protocols, visited_protocol_count,
              matched_protocol_owner_identity)) {
        matched_attachment_owner_identity =
            category_record->category_owner_identity != nullptr
                ? category_record->category_owner_identity
                : "";
        return true;
      }
    }
    node = node->has_super_node ? &state.realized_class_nodes[node->super_node_index]
                                : nullptr;
  }
  return false;
}

}  // namespace

extern "C" void objc3_runtime_stage_registration_table_for_bootstrap(
    const objc3_runtime_registration_table *registration_table) {
  RuntimeState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  state.staged_registration_table = registration_table;
}

// M254-D001 runtime-bootstrap-api anchor: registration, selector lookup,
// dispatch, snapshot, and reset remain the frozen bootstrap runtime boundary.
// D002/D003 may extend image walk and reset behavior, but they must preserve
// this surface and its fail-closed status/result contract.
int objc3_runtime_register_image(const objc3_runtime_image_descriptor *image) {
  RuntimeState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  const objc3_runtime_registration_table *const staged_registration_table =
      state.staged_registration_table;
  // M263-D001 runtime-bootstrap-table-consumption anchor: staging is one-shot
  // and is consumed by the next public registration call only.
  state.staged_registration_table = nullptr;
  return RegisterImageUnlocked(state, image, staged_registration_table, true,
                               false);
}

int objc3_runtime_copy_image_walk_state_for_testing(
    objc3_runtime_image_walk_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  RuntimeState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  // M263-D002 live-registration-discovery-replay anchor: discovery-root/image
  // walk state stays runtime-owned, is republished after deterministic replay,
  // and remains the canonical proof surface for the last discovered metadata
  // families rather than a manifest-only summary.
  snapshot->walked_image_count = state.walked_image_count;
  snapshot->last_discovery_root_entry_count =
      state.last_discovery_root_entry_count;
  snapshot->last_walked_class_descriptor_count =
      state.last_walked_class_descriptor_count;
  snapshot->last_walked_protocol_descriptor_count =
      state.last_walked_protocol_descriptor_count;
  snapshot->last_walked_category_descriptor_count =
      state.last_walked_category_descriptor_count;
  snapshot->last_walked_property_descriptor_count =
      state.last_walked_property_descriptor_count;
  snapshot->last_walked_ivar_descriptor_count =
      state.last_walked_ivar_descriptor_count;
  snapshot->last_walked_selector_pool_count =
      state.last_walked_selector_pool_count;
  snapshot->last_walked_string_pool_count =
      state.last_walked_string_pool_count;
  snapshot->last_linker_anchor_matches_discovery_root =
      state.last_linker_anchor_matches_discovery_root ? 1 : 0;
  snapshot->last_registration_used_staged_table =
      state.last_registration_used_staged_table ? 1 : 0;
  snapshot->last_walked_module_name =
      StableCString(state.last_walked_module_name);
  snapshot->last_walked_translation_unit_identity_key =
      StableCString(state.last_walked_translation_unit_identity_key);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

int objc3_runtime_copy_reset_replay_state_for_testing(
    objc3_runtime_reset_replay_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  RuntimeState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  // M263-D002 live-registration-discovery-replay anchor: reset/replay evidence
  // is published directly from runtime state so probes can verify retained
  // bootstrap catalog ownership, reset clears, and replayed identity/generation
  // without trusting sidecar manifests.
  snapshot->retained_bootstrap_image_count =
      static_cast<std::uint64_t>(state.retained_bootstrap_identity_order.size());
  snapshot->last_reset_cleared_image_local_init_state_count =
      state.last_reset_cleared_image_local_init_state_count;
  snapshot->last_replayed_image_count = state.last_replayed_image_count;
  snapshot->reset_generation = state.reset_generation;
  snapshot->replay_generation = state.replay_generation;
  snapshot->last_replay_status = state.last_replay_status;
  snapshot->last_replayed_module_name =
      StableCString(state.last_replayed_module_name);
  snapshot->last_replayed_translation_unit_identity_key =
      StableCString(state.last_replayed_translation_unit_identity_key);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

int objc3_runtime_copy_selector_lookup_table_state_for_testing(
    objc3_runtime_selector_lookup_table_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  RuntimeState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  snapshot->selector_table_entry_count =
      static_cast<std::uint64_t>(state.selector_slots.size());
  snapshot->metadata_backed_selector_count = state.metadata_backed_selector_count;
  snapshot->dynamic_selector_count = state.dynamic_selector_count;
  snapshot->metadata_provider_edge_count = state.metadata_provider_edge_count;
  snapshot->last_materialized_selector =
      StableCString(state.last_materialized_selector);
  snapshot->last_materialized_stable_id = state.last_materialized_stable_id;
  snapshot->last_materialized_registration_order_ordinal =
      state.last_materialized_registration_order_ordinal;
  snapshot->last_materialized_selector_pool_index =
      state.last_materialized_selector_pool_index;
  snapshot->last_materialized_from_metadata =
      state.last_materialized_from_metadata ? 1 : 0;
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

int objc3_runtime_copy_selector_lookup_entry_for_testing(
    const char *selector,
    objc3_runtime_selector_lookup_entry_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  snapshot->found = 0;
  snapshot->metadata_backed = 0;
  snapshot->stable_id = 0;
  snapshot->metadata_provider_count = 0;
  snapshot->first_registration_order_ordinal = 0;
  snapshot->last_registration_order_ordinal = 0;
  snapshot->first_selector_pool_index = 0;
  snapshot->last_selector_pool_index = 0;
  snapshot->canonical_selector = nullptr;

  if (selector == nullptr || selector[0] == '\0') {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  }

  RuntimeState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  const auto found = state.selector_index_by_name.find(selector);
  if (found == state.selector_index_by_name.end()) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  }

  const SelectorSlot &slot = state.selector_slots[found->second];
  snapshot->found = 1;
  snapshot->metadata_backed = slot.metadata_backed ? 1 : 0;
  snapshot->stable_id = slot.handle.stable_id;
  snapshot->metadata_provider_count = slot.metadata_provider_count;
  snapshot->first_registration_order_ordinal =
      slot.first_registration_order_ordinal;
  snapshot->last_registration_order_ordinal =
      slot.last_registration_order_ordinal;
  snapshot->first_selector_pool_index = slot.first_selector_pool_index;
  snapshot->last_selector_pool_index = slot.last_selector_pool_index;
  snapshot->canonical_selector = slot.handle.selector;
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

int objc3_runtime_copy_method_cache_state_for_testing(
    objc3_runtime_method_cache_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  RuntimeState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  snapshot->cache_entry_count =
      static_cast<std::uint64_t>(state.method_cache.size());
  snapshot->cache_hit_count = state.method_cache_hit_count;
  snapshot->cache_miss_count = state.method_cache_miss_count;
  snapshot->slow_path_lookup_count = state.slow_path_lookup_count;
  snapshot->live_dispatch_count = state.live_dispatch_count;
  snapshot->fallback_dispatch_count = state.fallback_dispatch_count;
  snapshot->last_selector_stable_id = state.last_dispatch_selector_stable_id;
  snapshot->last_normalized_receiver_identity =
      state.last_dispatch_normalized_receiver_identity;
  snapshot->last_category_probe_count = state.last_category_probe_count;
  snapshot->last_protocol_probe_count = state.last_protocol_probe_count;
  snapshot->last_dispatch_used_cache = state.last_dispatch_used_cache ? 1 : 0;
  snapshot->last_dispatch_resolved_live_method =
      state.last_dispatch_resolved_live_method ? 1 : 0;
  snapshot->last_dispatch_fell_back = state.last_dispatch_fell_back ? 1 : 0;
  snapshot->last_selector = StableCString(state.last_dispatch_selector);
  snapshot->last_resolved_class_name =
      StableCString(state.last_resolved_class_name);
  snapshot->last_resolved_owner_identity =
      StableCString(state.last_resolved_owner_identity);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

int objc3_runtime_copy_method_cache_entry_for_testing(
    int receiver, const char *selector,
    objc3_runtime_method_cache_entry_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  snapshot->found = 0;
  snapshot->resolved = 0;
  snapshot->dispatch_family_is_class = 0;
  snapshot->normalized_receiver_identity = 0;
  snapshot->selector_stable_id = 0;
  snapshot->parameter_count = 0;
  snapshot->category_probe_count = 0;
  snapshot->protocol_probe_count = 0;
  snapshot->selector = nullptr;
  snapshot->resolved_class_name = nullptr;
  snapshot->resolved_owner_identity = nullptr;

  RuntimeState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  std::uint64_t base_identity = 0;
  std::uint64_t normalized_receiver_identity = 0;
  DispatchFamily family = DispatchFamily::Invalid;
  if (!DecodeReceiverIdentity(receiver, base_identity, family,
                              normalized_receiver_identity)) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  }
  if (selector == nullptr || selector[0] == '\0') {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  }
  const auto selector_it = state.selector_index_by_name.find(selector);
  if (selector_it == state.selector_index_by_name.end()) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  }
  const MethodCacheKey key{normalized_receiver_identity,
                           state.selector_slots[selector_it->second]
                               .handle.stable_id};
  const auto cache_it = state.method_cache.find(key);
  if (cache_it == state.method_cache.end()) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  }
  const MethodCacheEntry &entry = cache_it->second;
  snapshot->found = 1;
  snapshot->resolved = entry.resolved ? 1 : 0;
  snapshot->dispatch_family_is_class =
      entry.dispatch_family_is_class ? 1 : 0;
  snapshot->normalized_receiver_identity = entry.normalized_receiver_identity;
  snapshot->selector_stable_id = entry.selector_stable_id;
  snapshot->parameter_count = entry.parameter_count;
  snapshot->category_probe_count = entry.category_probe_count;
  snapshot->protocol_probe_count = entry.protocol_probe_count;
  snapshot->selector = StableCString(entry.selector_storage);
  snapshot->resolved_class_name = StableCString(entry.class_name);
  snapshot->resolved_owner_identity = StableCString(entry.owner_identity);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

int objc3_runtime_copy_realized_class_graph_state_for_testing(
    objc3_runtime_realized_class_graph_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  RuntimeState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  snapshot->realized_class_count =
      static_cast<std::uint64_t>(state.realized_class_nodes.size());
  snapshot->root_class_count = state.realized_root_class_count;
  snapshot->metaclass_edge_count = state.realized_metaclass_edge_count;
  snapshot->receiver_class_binding_count = state.receiver_class_binding_count;
  snapshot->attached_category_count = state.realized_attached_category_count;
  snapshot->protocol_conformance_edge_count =
      state.realized_protocol_conformance_edge_count;
  snapshot->last_realized_class_name =
      StableCString(state.last_realized_class_name);
  snapshot->last_realized_class_owner_identity =
      StableCString(state.last_realized_class_owner_identity);
  snapshot->last_realized_metaclass_owner_identity =
      StableCString(state.last_realized_metaclass_owner_identity);
  snapshot->last_attached_category_owner_identity =
      StableCString(state.last_attached_category_owner_identity);
  snapshot->last_attached_category_name =
      StableCString(state.last_attached_category_name);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

int objc3_runtime_copy_realized_class_entry_for_testing(
    const char *class_name, objc3_runtime_realized_class_entry_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  snapshot->found = 0;
  snapshot->base_identity = 0;
  snapshot->registration_order_ordinal = 0;
  snapshot->is_root_class = 0;
  snapshot->implementation_backed = 0;
  snapshot->attached_category_count = 0;
  snapshot->direct_protocol_count = 0;
  snapshot->attached_protocol_count = 0;
  snapshot->module_name = nullptr;
  snapshot->translation_unit_identity_key = nullptr;
  snapshot->class_name = nullptr;
  snapshot->class_owner_identity = nullptr;
  snapshot->metaclass_owner_identity = nullptr;
  snapshot->super_class_owner_identity = nullptr;
  snapshot->super_metaclass_owner_identity = nullptr;
  snapshot->last_attached_category_owner_identity = nullptr;
  snapshot->last_attached_category_name = nullptr;

  if (class_name == nullptr || class_name[0] == '\0') {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  }

  RuntimeState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  const auto found = state.realized_class_node_indices_by_name.find(class_name);
  if (found == state.realized_class_node_indices_by_name.end() ||
      found->second.empty()) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  }
  const std::size_t node_index = found->second.front();
  if (node_index >= state.realized_class_nodes.size()) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  }
  const RealizedClassNode &node = state.realized_class_nodes[node_index];
  snapshot->found = 1;
  snapshot->base_identity = node.base_identity;
  snapshot->registration_order_ordinal = node.registration_order_ordinal;
  snapshot->is_root_class = node.is_root_class ? 1 : 0;
  snapshot->implementation_backed = node.implementation_backed ? 1 : 0;
  snapshot->attached_category_count =
      static_cast<std::uint64_t>(node.attached_category_records.size());
  snapshot->direct_protocol_count =
      (node.bundle != nullptr &&
       node.bundle->class_record.adopted_protocol_refs != nullptr)
          ? node.bundle->class_record.adopted_protocol_refs->count
          : 0;
  snapshot->module_name = StableCString(node.module_name);
  snapshot->translation_unit_identity_key =
      StableCString(node.translation_unit_identity_key);
  snapshot->class_name = StableCString(node.class_name);
  snapshot->class_owner_identity = StableCString(node.class_owner_identity);
  snapshot->metaclass_owner_identity = StableCString(node.metaclass_owner_identity);
  snapshot->super_class_owner_identity =
      StableCString(node.super_class_owner_identity);
  snapshot->super_metaclass_owner_identity =
      StableCString(node.super_metaclass_owner_identity);
  std::uint64_t attached_protocol_count = 0;
  for (const EmittedCategoryRecord *category_record : node.attached_category_records) {
    if (category_record != nullptr && category_record->adopted_protocol_refs != nullptr) {
      attached_protocol_count += category_record->adopted_protocol_refs->count;
    }
  }
  snapshot->attached_protocol_count = attached_protocol_count;
  if (!node.attached_category_records.empty()) {
    const EmittedCategoryRecord *last_category =
        node.attached_category_records.back();
    snapshot->last_attached_category_owner_identity =
        last_category->category_owner_identity != nullptr
            ? last_category->category_owner_identity
            : nullptr;
    snapshot->last_attached_category_name =
        last_category->category_name != nullptr ? last_category->category_name
                                                : nullptr;
  }
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

int objc3_runtime_copy_protocol_conformance_query_for_testing(
    const char *class_name, const char *protocol_name,
    objc3_runtime_protocol_conformance_query_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  snapshot->class_found = 0;
  snapshot->protocol_found = 0;
  snapshot->conforms = 0;
  snapshot->visited_protocol_count = 0;
  snapshot->attached_category_count = 0;
  snapshot->class_name = nullptr;
  snapshot->protocol_name = nullptr;
  snapshot->matched_protocol_owner_identity = nullptr;
  snapshot->matched_attachment_owner_identity = nullptr;

  if (class_name == nullptr || class_name[0] == '\0' ||
      protocol_name == nullptr || protocol_name[0] == '\0') {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  }

  RuntimeState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  state.last_protocol_conformance_class_name = class_name;
  state.last_protocol_conformance_protocol_name = protocol_name;
  state.last_protocol_conformance_owner_identity.clear();
  state.last_protocol_conformance_attachment_owner_identity.clear();
  snapshot->class_name = StableCString(state.last_protocol_conformance_class_name);
  snapshot->protocol_name =
      StableCString(state.last_protocol_conformance_protocol_name);
  snapshot->protocol_found = ProtocolExistsByNameUnlocked(state, protocol_name) ? 1 : 0;

  const auto found = state.realized_class_node_indices_by_name.find(class_name);
  if (found == state.realized_class_node_indices_by_name.end() ||
      found->second.empty()) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  }
  const std::size_t node_index = found->second.front();
  if (node_index >= state.realized_class_nodes.size()) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  }

  const RealizedClassNode &node = state.realized_class_nodes[node_index];
  snapshot->class_found = 1;
  snapshot->attached_category_count =
      static_cast<std::uint64_t>(node.attached_category_records.size());

  std::string matched_protocol_owner_identity;
  std::string matched_attachment_owner_identity;
  if (QueryRealizedClassProtocolConformanceUnlocked(
          state, &node, protocol_name, snapshot->visited_protocol_count,
          matched_protocol_owner_identity, matched_attachment_owner_identity)) {
    snapshot->conforms = 1;
    state.last_protocol_conformance_owner_identity = matched_protocol_owner_identity;
    state.last_protocol_conformance_attachment_owner_identity =
        matched_attachment_owner_identity;
    snapshot->matched_protocol_owner_identity =
        StableCString(state.last_protocol_conformance_owner_identity);
    snapshot->matched_attachment_owner_identity =
        StableCString(state.last_protocol_conformance_attachment_owner_identity);
  }
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

int objc3_runtime_replay_registered_images_for_testing(void) {
  RuntimeState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  // M263-D002 live-registration-discovery-replay anchor: replay consumes the
  // retained bootstrap catalog only when live registration state is empty,
  // republishes discovery/image-walk state through the same staged-table path,
  // and records last-replayed identity plus replay-generation evidence.
  // M263-D003 live-restart-hardening anchor: replay fails closed when live
  // runtime state is still populated, repeated restart cycles must remain
  // deterministic, and replay-generation evidence must advance monotonically.
  state.last_replay_status = OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  state.last_replayed_image_count = 0;
  state.last_replayed_module_name.clear();
  state.last_replayed_translation_unit_identity_key.clear();

  if (state.registered_image_count != 0 ||
      !state.registration_order_by_identity_key.empty() ||
      state.next_expected_registration_order_ordinal != 1 ||
      state.staged_registration_table != nullptr) {
    state.last_replay_status = OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
    return state.last_replay_status;
  }

  for (const std::string &identity_key : state.retained_bootstrap_identity_order) {
    const auto found =
        state.retained_bootstrap_metadata_by_identity_key.find(identity_key);
    if (found == state.retained_bootstrap_metadata_by_identity_key.end() ||
        found->second.registration_table == nullptr ||
        found->second.registration_table->image_descriptor == nullptr) {
      ClearLiveRegistrationStateUnlocked(state);
      state.last_reset_cleared_image_local_init_state_count =
          ZeroRetainedBootstrapImageLocalInitStatesUnlocked(state);
      state.last_replay_status = OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
      return state.last_replay_status;
    }

    const RegisteredImageMetadata &record = found->second;
    const int status = RegisterImageUnlocked(state,
                                             record.registration_table->image_descriptor,
                                             record.registration_table, false,
                                             true);
    if (status != OBJC3_RUNTIME_REGISTRATION_STATUS_OK) {
      ClearLiveRegistrationStateUnlocked(state);
      state.last_reset_cleared_image_local_init_state_count =
          ZeroRetainedBootstrapImageLocalInitStatesUnlocked(state);
      state.last_replay_status = status;
      state.last_replayed_image_count = 0;
      state.last_replayed_module_name.clear();
      state.last_replayed_translation_unit_identity_key.clear();
      return status;
    }

    ++state.last_replayed_image_count;
    state.last_replayed_module_name = record.module_name;
    state.last_replayed_translation_unit_identity_key =
        record.translation_unit_identity_key;
  }

  ++state.replay_generation;
  state.last_replay_status = OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

const objc3_runtime_selector_handle *objc3_runtime_lookup_selector(
    const char *selector) {
  RuntimeState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  return LookupSelectorUnlocked(selector);
}

int objc3_runtime_dispatch_i32(int receiver, const char *selector, int a0,
                               int a1, int a2, int a3) {
  RuntimeState &state = State();
  const void *resolved_implementation = nullptr;
  std::uint64_t resolved_parameter_count = 0;
  RuntimeBuiltinKind resolved_builtin_kind = RuntimeBuiltinKind::None;
  bool resolved_live_method = false;
  std::uint64_t receiver_base_identity = 0;
  {
    std::lock_guard<std::mutex> lock(state.mutex);
    const objc3_runtime_selector_handle *selector_handle =
        LookupSelectorUnlocked(selector);
    state.last_dispatch_selector = selector != nullptr ? selector : "";
    state.last_dispatch_selector_stable_id =
        selector_handle != nullptr ? selector_handle->stable_id : 0;
    state.last_dispatch_normalized_receiver_identity = 0;
    state.last_category_probe_count = 0;
    state.last_protocol_probe_count = 0;
    state.last_dispatch_used_cache = false;
    state.last_dispatch_resolved_live_method = false;
    state.last_dispatch_fell_back = false;
    state.last_resolved_class_name.clear();
    state.last_resolved_owner_identity.clear();

    if (receiver != 0 && selector_handle != nullptr) {
      std::uint64_t base_identity = 0;
      std::uint64_t normalized_receiver_identity = 0;
      DispatchFamily family = DispatchFamily::Invalid;
      if (DecodeReceiverIdentity(receiver, base_identity, family,
                                 normalized_receiver_identity)) {
        receiver_base_identity = base_identity;
        state.last_dispatch_normalized_receiver_identity =
            normalized_receiver_identity;
        const MethodCacheKey cache_key{normalized_receiver_identity,
                                       selector_handle->stable_id};
        const auto cache_it = state.method_cache.find(cache_key);
        if (cache_it != state.method_cache.end()) {
          const MethodCacheEntry &entry = cache_it->second;
          ++state.method_cache_hit_count;
          state.last_dispatch_used_cache = true;
          state.last_dispatch_resolved_live_method = entry.resolved;
          state.last_dispatch_fell_back = !entry.resolved;
          state.last_category_probe_count = entry.category_probe_count;
          state.last_protocol_probe_count = entry.protocol_probe_count;
          state.last_resolved_class_name = entry.class_name;
          state.last_resolved_owner_identity = entry.owner_identity;
          if (entry.resolved) {
            resolved_live_method = true;
            resolved_builtin_kind = entry.builtin_kind;
            resolved_implementation = entry.implementation;
            resolved_parameter_count = entry.parameter_count;
            ++state.live_dispatch_count;
          } else {
            ++state.fallback_dispatch_count;
          }
        } else {
          ++state.method_cache_miss_count;
          ++state.slow_path_lookup_count;
          SlowPathResolution resolution = ResolveMethodSlowPathUnlocked(
              state, base_identity, normalized_receiver_identity, family,
              selector_handle->stable_id, selector_handle->selector);
          MethodCacheEntry cache_entry;
          cache_entry.resolved = resolution.resolved;
          cache_entry.dispatch_family_is_class =
              resolution.dispatch_family_is_class;
          cache_entry.selector_storage = resolution.selector_storage;
          cache_entry.class_name = resolution.class_name;
          cache_entry.owner_identity = resolution.owner_identity;
          cache_entry.normalized_receiver_identity =
              normalized_receiver_identity;
          cache_entry.selector_stable_id = selector_handle->stable_id;
          cache_entry.parameter_count = resolution.parameter_count;
          cache_entry.category_probe_count = resolution.category_probe_count;
          cache_entry.protocol_probe_count = resolution.protocol_probe_count;
          cache_entry.implementation = resolution.implementation;
          cache_entry.builtin_kind = resolution.builtin_kind;
          state.method_cache.emplace(cache_key, std::move(cache_entry));
          state.last_dispatch_resolved_live_method = resolution.resolved;
          state.last_dispatch_fell_back = !resolution.resolved;
          state.last_category_probe_count = resolution.category_probe_count;
          state.last_protocol_probe_count = resolution.protocol_probe_count;
          state.last_resolved_class_name = resolution.class_name;
          state.last_resolved_owner_identity = resolution.owner_identity;
          if (resolution.resolved) {
            resolved_live_method = true;
            resolved_builtin_kind = resolution.builtin_kind;
            resolved_implementation = resolution.implementation;
            resolved_parameter_count = resolution.parameter_count;
            ++state.live_dispatch_count;
          } else {
            ++state.fallback_dispatch_count;
          }
        }
      } else {
        ++state.fallback_dispatch_count;
        state.last_dispatch_fell_back = true;
      }
    }
  }
  // M255-C003 runtime call ABI generation anchor: canonical runtime dispatch
  // owns nil-receiver semantics for lowered instance/class/super surfaces, so
  // a zero receiver returns zero without requiring lowering-side elision.
  if (receiver == 0) {
    return 0;
  }
  if (resolved_live_method && resolved_implementation != nullptr) {
    return InvokeResolvedMethod(resolved_implementation, resolved_parameter_count,
                                a0, a1, a2, a3);
  }
  if (resolved_live_method && resolved_builtin_kind != RuntimeBuiltinKind::None) {
    return InvokeRuntimeBuiltinMethod(resolved_builtin_kind, receiver,
                                      receiver_base_identity);
  }
  return ComputeDispatchResult(receiver, selector, a0, a1, a2, a3);
}

int objc3_runtime_copy_registration_state_for_testing(
    objc3_runtime_registration_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  RuntimeState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  snapshot->registered_image_count = state.registered_image_count;
  snapshot->registered_descriptor_total = state.registered_descriptor_total;
  snapshot->next_expected_registration_order_ordinal =
      state.next_expected_registration_order_ordinal;
  snapshot->last_successful_registration_order_ordinal =
      state.last_successful_registration_order_ordinal;
  snapshot->last_registration_status = state.last_registration_status;
  snapshot->last_registered_module_name =
      StableCString(state.last_registered_module_name);
  snapshot->last_registered_translation_unit_identity_key =
      StableCString(state.last_registered_translation_unit_identity_key);
  snapshot->last_rejected_module_name =
      StableCString(state.last_rejected_module_name);
  snapshot->last_rejected_translation_unit_identity_key =
      StableCString(state.last_rejected_translation_unit_identity_key);
  snapshot->last_rejected_registration_order_ordinal =
      state.last_rejected_registration_order_ordinal;
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

extern "C" int objc3_msgsend_i32(int receiver, const char *selector, int a0,
                                 int a1, int a2, int a3) {
  // M255-C004 compatibility bridge: live lowering no longer emits this symbol.
  // It stays exported as a formula-parity alias and compatibility/test surface.
  return objc3_runtime_dispatch_i32(receiver, selector, a0, a1, a2, a3);
}

void objc3_runtime_reset_for_testing(void) {
  RuntimeState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  // M263-D003 live-restart-hardening anchor: teardown clears only live runtime
  // state, preserves the retained bootstrap catalog for restart, zeroes image-
  // local init cells, and advances reset-generation evidence for repeated
  // restart-cycle probes.
  ClearLiveRegistrationStateUnlocked(state);
  state.last_reset_cleared_image_local_init_state_count =
      ZeroRetainedBootstrapImageLocalInitStatesUnlocked(state);
  ++state.reset_generation;
  state.last_replay_status = OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  state.last_replayed_image_count = 0;
  state.last_replayed_module_name.clear();
  state.last_replayed_translation_unit_identity_key.clear();
}
