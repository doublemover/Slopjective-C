#include "runtime/dispatch/method_fast_path_seed.h"

#include "runtime/dispatch/dispatch_family.h"
#include "runtime/dispatch/dispatch_snapshot_contracts.h"
#include "runtime/dispatch/method_cache.h"
#include "runtime/dispatch/runtime_method_return.h"
#include "runtime/dispatch/runtime_resolution_records.h"
#include "runtime/metadata/runtime_emitted_records.h"
#include "runtime/metadata/runtime_realized_records.h"
#include "runtime/state/runtime_cache_invalidation.h"
#include "runtime/state/runtime_state_records.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <utility>

namespace objc3c::runtime {

namespace {

const EmittedMethodListEntry *MethodListEntries(
    const EmittedMethodListHeader *header) {
  return header == nullptr
             ? nullptr
             : reinterpret_cast<const EmittedMethodListEntry *>(header + 1);
}

const EmittedMethodListRef *SelectCategoryMethodListRef(
    const EmittedCategoryRecord &record, DispatchFamily family) {
  return family == DispatchFamily::Class ? record.class_method_list_ref
                                         : record.instance_method_list_ref;
}

bool HasSelectorInMethodListRefUnlocked(
    const EmittedMethodListRef *method_list_ref, const char *selector) {
  if (method_list_ref == nullptr || method_list_ref->count == 0 ||
      method_list_ref->method_list == nullptr || selector == nullptr ||
      selector[0] == '\0') {
    return false;
  }
  const auto *header =
      static_cast<const EmittedMethodListHeader *>(method_list_ref->method_list);
  if (header == nullptr || header->count != method_list_ref->count) {
    return false;
  }
  const EmittedMethodListEntry *entries = MethodListEntries(header);
  for (std::uint64_t index = 0; index < header->count; ++index) {
    const EmittedMethodListEntry &entry = entries[index];
    if (entry.selector != nullptr && std::strcmp(entry.selector, selector) == 0) {
      return true;
    }
  }
  return false;
}

bool HasAttachedCategorySelectorConflictUnlocked(const RealizedClassNode &node,
                                                 DispatchFamily family,
                                                 const char *selector) {
  for (const EmittedCategoryRecord *category_record :
       node.attached_category_records) {
    if (category_record == nullptr) {
      continue;
    }
    const EmittedMethodListRef *method_list_ref =
        SelectCategoryMethodListRef(*category_record, family);
    if (HasSelectorInMethodListRefUnlocked(method_list_ref, selector)) {
      return true;
    }
  }
  return false;
}

std::string BuildDispatchIntentFastPathReason(
    const RealizedClassNode &node, const EmittedClassRecord &record,
    const EmittedMethodListEntry &entry) {
  if (entry.effective_direct_dispatch) {
    return "direct";
  }
  if (entry.objc_final_declared) {
    return "method-final";
  }
  if (record.objc_final_declared || node.objc_final_declared) {
    return "class-final";
  }
  if (record.objc_sealed_declared || node.objc_sealed_declared) {
    return "class-sealed";
  }
  return {};
}

std::size_t EstimateSeedableMethodEntriesForMethodList(
    const EmittedMethodListRef *method_list_ref) {
  if (method_list_ref == nullptr || method_list_ref->count == 0 ||
      method_list_ref->method_list == nullptr) {
    return 0;
  }
  const auto *header =
      static_cast<const EmittedMethodListHeader *>(method_list_ref->method_list);
  if (header == nullptr || header->count != method_list_ref->count) {
    return 0;
  }
  const EmittedMethodListEntry *entries = MethodListEntries(header);
  std::size_t count = 0;
  for (std::uint64_t index = 0; index < header->count; ++index) {
    const EmittedMethodListEntry &entry = entries[index];
    if (entry.selector == nullptr || entry.owner_identity == nullptr ||
        entry.return_type_name == nullptr || entry.has_body == 0 ||
        entry.implementation == nullptr || entry.parameter_count > 4) {
      continue;
    }
    if (ClassifyRuntimeReturnType(entry.return_type_name) ==
        RuntimeMethodReturnKind::Unsupported) {
      continue;
    }
    ++count;
  }
  return count;
}

void ReserveMethodCacheForFastPathSeedUnlocked(RuntimeState &state) {
  std::size_t reserve_count = state.method_cache.size();
  for (const RealizedClassNode &node : state.realized_class_nodes) {
    if (node.bundle == nullptr) {
      continue;
    }
    reserve_count += EstimateSeedableMethodEntriesForMethodList(
        node.bundle->class_record.method_list_ref);
    reserve_count += EstimateSeedableMethodEntriesForMethodList(
        node.bundle->metaclass_record.method_list_ref);
  }
  state.method_cache.reserve(reserve_count);
}

void SeedDispatchIntentFastPathCacheForMethodListUnlocked(
    RuntimeState &state, const RealizedClassNode &node,
    const EmittedClassRecord &record, DispatchFamily family,
    std::uint64_t normalized_receiver_identity) {
  const EmittedMethodListRef *method_list_ref = record.method_list_ref;
  if (method_list_ref == nullptr || method_list_ref->count == 0 ||
      method_list_ref->method_list == nullptr) {
    return;
  }
  const auto *header =
      static_cast<const EmittedMethodListHeader *>(method_list_ref->method_list);
  if (header == nullptr || header->count != method_list_ref->count) {
    return;
  }
  const EmittedMethodListEntry *entries = MethodListEntries(header);
  for (std::uint64_t index = 0; index < header->count; ++index) {
    const EmittedMethodListEntry &entry = entries[index];
    if (entry.selector == nullptr || entry.owner_identity == nullptr ||
        entry.return_type_name == nullptr || entry.has_body == 0 ||
        entry.implementation == nullptr || entry.parameter_count > 4) {
      continue;
    }
    const RuntimeMethodReturnKind return_kind =
        ClassifyRuntimeReturnType(entry.return_type_name);
    if (return_kind == RuntimeMethodReturnKind::Unsupported) {
      continue;
    }
    const std::string fast_path_reason =
        BuildDispatchIntentFastPathReason(node, record, entry);
    if (fast_path_reason.empty()) {
      continue;
    }
    if (HasAttachedCategorySelectorConflictUnlocked(node, family,
                                                    entry.selector)) {
      continue;
    }
    const auto selector_it = state.selector_index_by_name.find(entry.selector);
    if (selector_it == state.selector_index_by_name.end()) {
      continue;
    }
    const std::uint64_t selector_stable_id =
        state.selector_slots[selector_it->second].handle.stable_id;
    if (selector_stable_id == 0) {
      continue;
    }
    const MethodCacheKey cache_key{node.base_identity,
                                   normalized_receiver_identity,
                                   selector_stable_id};
    if (state.method_cache.find(cache_key) != state.method_cache.end()) {
      continue;
    }
    MethodCacheEntry cache_entry;
    cache_entry.cache_abi_version = OBJC3_RUNTIME_METHOD_CACHE_ABI_VERSION;
    cache_entry.resolved = true;
    cache_entry.dispatch_family_is_class = family == DispatchFamily::Class;
    cache_entry.fast_path_seeded = true;
    cache_entry.effective_direct_dispatch = entry.effective_direct_dispatch;
    cache_entry.objc_final_declared =
        entry.objc_final_declared || record.objc_final_declared ||
        node.objc_final_declared;
    cache_entry.objc_sealed_declared =
        record.objc_sealed_declared || node.objc_sealed_declared;
    cache_entry.selector_storage = entry.selector;
    cache_entry.fast_path_reason = fast_path_reason;
    cache_entry.class_name = node.class_name;
    cache_entry.owner_identity = entry.owner_identity;
    cache_entry.lookup_start_base_identity = node.base_identity;
    cache_entry.normalized_receiver_identity = normalized_receiver_identity;
    cache_entry.selector_stable_id = selector_stable_id;
    cache_entry.cache_entry_generation =
        state.next_method_cache_entry_generation++;
    cache_entry.parameter_count = entry.parameter_count;
    cache_entry.return_kind = return_kind;
    cache_entry.miss_status = OBJC3_RUNTIME_DISPATCH_STATUS_OK;
    cache_entry.cache_registered_image_count = state.registered_image_count;
    cache_entry.cache_last_successful_registration_order_ordinal =
        state.last_successful_registration_order_ordinal;
    cache_entry.cache_reset_generation = state.reset_generation;
    cache_entry.cache_replay_generation = state.replay_generation;
    cache_entry.cache_realized_class_node_count =
        static_cast<std::uint64_t>(state.realized_class_nodes.size());
    StampMethodCacheMutationGenerationsUnlocked(cache_entry, state);
    cache_entry.implementation = entry.implementation;
    if (state.method_cache.emplace(cache_key, std::move(cache_entry)).second) {
      ++state.fast_path_seed_count;
    }
  }
}

}  // namespace

void SeedDispatchIntentFastPathCacheUnlocked(RuntimeState &state) {
  // live-dispatch-fast-path anchor: registration-time class-graph rebuild
  // pre-seeds deterministic cache entries for safe implementation-backed
  // direct/final/sealed methods.
  ReserveMethodCacheForFastPathSeedUnlocked(state);
  for (const RealizedClassNode &node : state.realized_class_nodes) {
    if (node.bundle == nullptr) {
      continue;
    }
    SeedDispatchIntentFastPathCacheForMethodListUnlocked(
        state, node, node.bundle->class_record, DispatchFamily::Instance,
        node.base_identity + 1u);
    SeedDispatchIntentFastPathCacheForMethodListUnlocked(
        state, node, node.bundle->metaclass_record, DispatchFamily::Class,
        node.base_identity + 2u);
  }
}

}  // namespace objc3c::runtime
