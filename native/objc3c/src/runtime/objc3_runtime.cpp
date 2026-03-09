#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <cstdint>
#include <deque>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {

constexpr std::int64_t kDispatchModulus = 2147483629LL;

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
};

RuntimeState &State() {
  static RuntimeState state;
  return state;
}

void RefreshSelectorHandlePointersUnlocked(RuntimeState &state) {
  for (SelectorSlot &slot : state.selector_slots) {
    slot.handle.selector = slot.spelling_storage.c_str();
  }
}

const objc3_runtime_selector_handle *LookupSelectorUnlocked(const char *selector) {
  // M255-D002 selector-table anchor: metadata-backed selector pools now
  // materialize the canonical runtime selector table, while direct lookup of
  // non-emitted selectors remains a dynamic fallback until M255-D003 lands
  // cache and slow-path lookup on top of the same public API.
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
  // deterministic formula while lane-D freezes the runtime-owned boundary. Real
  // method-cache and metadata-backed slow-path lookup arrive in later M255
  // lane-D issues without changing the canonical dispatch entrypoint.
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
  state.staged_registration_table = nullptr;
  state.walked_image_count = 0;
  ClearImageWalkSnapshotUnlocked(state);
}

bool TryWalkRegistrationTableUnlocked(
    RuntimeState &state,
    const objc3_runtime_registration_table *registration_table,
    const objc3_runtime_image_descriptor *image,
    RegisteredImageMetadata &record) {
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
  ClearRejectedRegistrationUnlocked(state);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
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

int objc3_runtime_replay_registered_images_for_testing(void) {
  RuntimeState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
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
  {
    std::lock_guard<std::mutex> lock(state.mutex);
    (void)LookupSelectorUnlocked(selector);
  }
  // M255-C003 runtime call ABI generation anchor: canonical runtime dispatch
  // owns nil-receiver semantics for lowered instance/class/super surfaces, so
  // a zero receiver returns zero without requiring lowering-side elision.
  if (receiver == 0) {
    return 0;
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
  ClearLiveRegistrationStateUnlocked(state);
  state.last_reset_cleared_image_local_init_state_count =
      ZeroRetainedBootstrapImageLocalInitStatesUnlocked(state);
  ++state.reset_generation;
  state.last_replay_status = OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  state.last_replayed_image_count = 0;
  state.last_replayed_module_name.clear();
  state.last_replayed_translation_unit_identity_key.clear();
}
