#include "runtime/classes/class_graph.h"

#include "runtime/classes/category_attachment.h"
#include "runtime/classes/metaclass_graph.h"
#include "runtime/classes/protocol_conformance.h"
#include "runtime/dispatch/dispatch_family.h"
#include "runtime/images/image_descriptor.h"
#include "runtime/images/multi_image_ordering.h"
#include "runtime/metadata/runtime_emitted_records.h"
#include "runtime/metadata/runtime_registration_records.h"
#include "runtime/metadata/runtime_realized_records.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "runtime/state/runtime_state_clear.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/state/runtime_state_store.h"
#include "runtime/strings/borrowed_string.h"
#include "runtime/storage/property_layout_realization.h"

#include <algorithm>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <mutex>

namespace objc3c::runtime {

namespace {

constexpr std::uint64_t kReceiverIdentityBase = 1024;
constexpr std::uint64_t kReceiverIdentityStride = 17;

}  // namespace

std::uint64_t RuntimeReceiverIdentityBase() {
  return kReceiverIdentityBase;
}

std::uint64_t RuntimeReceiverIdentityStride() {
  return kReceiverIdentityStride;
}

std::uint64_t BuildReceiverBaseIdentity(std::size_t ordinal) {
  return kReceiverIdentityBase +
         static_cast<std::uint64_t>(ordinal) * kReceiverIdentityStride;
}

bool IsRuntimeReceiverBaseIdentity(std::uint64_t base_identity) {
  return base_identity >= kReceiverIdentityBase &&
         ((base_identity - kReceiverIdentityBase) %
          kReceiverIdentityStride) == 0;
}

bool DecodeReceiverIdentity(const RuntimeState &state, int receiver,
                            std::uint64_t &base_identity,
                            DispatchFamily &family,
                            std::uint64_t &normalized_receiver_identity) {
  if (receiver <= 0) {
    return false;
  }
  const auto runtime_instance_it =
      state.runtime_instances_by_receiver.find(receiver);
  if (runtime_instance_it != state.runtime_instances_by_receiver.end()) {
    base_identity = runtime_instance_it->second.base_identity;
    family = DispatchFamily::Instance;
    normalized_receiver_identity = base_identity + 1u;
    return true;
  }
  const std::int64_t signed_receiver = receiver;
  if (signed_receiver < static_cast<std::int64_t>(RuntimeReceiverIdentityBase())) {
    return false;
  }
  const std::int64_t delta =
      signed_receiver - static_cast<std::int64_t>(RuntimeReceiverIdentityBase());
  const std::int64_t ordinal =
      delta / static_cast<std::int64_t>(RuntimeReceiverIdentityStride());
  const std::int64_t salt =
      delta % static_cast<std::int64_t>(RuntimeReceiverIdentityStride());
  if (ordinal < 0) {
    return false;
  }
  base_identity = static_cast<std::uint64_t>(
      static_cast<std::int64_t>(RuntimeReceiverIdentityBase()) +
      ordinal * static_cast<std::int64_t>(RuntimeReceiverIdentityStride()));
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

const RealizedClassNode *FindRealizedClassNodeByBaseIdentityUnlocked(
    const RuntimeState &state, std::uint64_t base_identity) {
  const auto class_name_it =
      state.realized_class_name_by_base_identity.find(base_identity);
  if (class_name_it == state.realized_class_name_by_base_identity.end()) {
    return nullptr;
  }
  const auto node_indexes_it =
      state.realized_class_node_indices_by_name.find(class_name_it->second);
  if (node_indexes_it == state.realized_class_node_indices_by_name.end()) {
    return nullptr;
  }
  for (const std::size_t node_index : node_indexes_it->second) {
    if (node_index >= state.realized_class_nodes.size()) {
      continue;
    }
    const RealizedClassNode &node = state.realized_class_nodes[node_index];
    if (node.base_identity == base_identity) {
      return &node;
    }
  }
  return nullptr;
}

namespace {

const void *ClassGraphAggregateEntry(
    const objc3_runtime_pointer_aggregate *aggregate, std::uint64_t index) {
  return RuntimeAggregateEntry(aggregate, index);
}

bool IsImplementationOwnerIdentity(const char *owner_identity) {
  if (owner_identity == nullptr) {
    return false;
  }
  static constexpr char kImplementationPrefix[] = "implementation:";
  return std::string(owner_identity).rfind(kImplementationPrefix, 0) == 0;
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
      [](const RegisteredImageMetadata *lhs,
         const RegisteredImageMetadata *rhs) {
        return CompareRuntimeImageRegistrationOrder(
                   lhs->registration_order_ordinal,
                   lhs->translation_unit_identity_key.c_str(),
                   rhs->registration_order_ordinal,
                   rhs->translation_unit_identity_key.c_str()) < 0;
      });
  return ordered;
}

bool CollectSortedImageClassNames(const RegisteredImageMetadata &record,
                                  std::vector<std::string> &class_names) {
  std::unordered_set<std::string> seen;
  seen.reserve(static_cast<std::size_t>(record.class_descriptor_count));
  for (std::uint64_t index = 0; index < record.class_descriptor_count; ++index) {
    const auto *bundle = static_cast<const EmittedClassBundle *>(
        ClassGraphAggregateEntry(record.class_descriptor_root, index));
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

std::vector<const EmittedClassBundle *> CollectPreferredClassBundlesForImage(
    const RegisteredImageMetadata &record, const std::string &class_name) {
  std::vector<const EmittedClassBundle *> implementation_bundles;
  std::vector<const EmittedClassBundle *> candidate_bundles;
  implementation_bundles.reserve(
      static_cast<std::size_t>(record.class_descriptor_count));
  candidate_bundles.reserve(
      static_cast<std::size_t>(record.class_descriptor_count));
  for (std::uint64_t index = 0; index < record.class_descriptor_count; ++index) {
    const auto *bundle = static_cast<const EmittedClassBundle *>(
        ClassGraphAggregateEntry(record.class_descriptor_root, index));
    if (bundle == nullptr || bundle->class_record.class_name == nullptr ||
        class_name != bundle->class_record.class_name) {
      continue;
    }
    const bool implementation_backed =
        bundle->class_record.method_list_ref != nullptr &&
        IsImplementationOwnerIdentity(
            bundle->class_record.method_list_ref->owner_identity);
    if (implementation_backed) {
      implementation_bundles.push_back(bundle);
    } else {
      candidate_bundles.push_back(bundle);
    }
  }
  return implementation_bundles.empty() ? candidate_bundles
                                        : implementation_bundles;
}

}  // namespace

void RebuildRealizedClassGraphUnlocked(RuntimeState &state) {
  // metaclass-graph-root-class anchor: runtime now republishes a
  // realized class/metaclass graph keyed by stable receiver base identities,
  // preserving root classes as explicit graph nodes rather than rediscovering
  // the class family from emitted bundles on every dispatch.
  ClearRealizedClassGraphUnlocked(state);

  std::vector<const RegisteredImageMetadata *> ordered_images =
      OrderedRegisteredImages(state);
  std::size_t estimated_realized_node_count = 0;
  std::unordered_set<std::string> global_class_name_set;
  for (const RegisteredImageMetadata *record : ordered_images) {
    if (record == nullptr) {
      continue;
    }
    estimated_realized_node_count +=
        static_cast<std::size_t>(record->class_descriptor_count);
  }
  for (const RegisteredImageMetadata *record : ordered_images) {
    std::vector<std::string> class_names;
    if (!CollectSortedImageClassNames(*record, class_names)) {
      continue;
    }
    global_class_name_set.reserve(global_class_name_set.size() +
                                  class_names.size());
    for (const std::string &class_name : class_names) {
      global_class_name_set.insert(class_name);
    }
  }

  std::vector<std::string> global_class_names(global_class_name_set.begin(),
                                             global_class_name_set.end());
  std::sort(global_class_names.begin(), global_class_names.end());
  std::unordered_map<std::string, std::size_t> global_ordinal_by_class_name;
  global_ordinal_by_class_name.reserve(global_class_names.size());
  state.realized_class_name_by_base_identity.reserve(global_class_names.size());
  for (std::size_t ordinal = 0; ordinal < global_class_names.size(); ++ordinal) {
    const std::string &class_name = global_class_names[ordinal];
    global_ordinal_by_class_name.emplace(class_name, ordinal);
    const std::uint64_t base_identity = BuildReceiverBaseIdentity(ordinal);
    state.realized_class_name_by_base_identity.emplace(base_identity,
                                                       class_name);
  }
  state.receiver_class_binding_count =
      static_cast<std::uint64_t>(
          state.realized_class_name_by_base_identity.size());

  std::unordered_map<const EmittedClassBundle *, std::size_t>
      node_index_by_bundle;
  node_index_by_bundle.reserve(estimated_realized_node_count);
  state.realized_class_nodes.reserve(estimated_realized_node_count);
  state.realized_class_node_indices_by_name.reserve(global_class_names.size());
  for (const RegisteredImageMetadata *record : ordered_images) {
    std::vector<std::string> class_names;
    if (!CollectSortedImageClassNames(*record, class_names)) {
      continue;
    }
    for (const std::string &class_name : class_names) {
      const auto ordinal_it = global_ordinal_by_class_name.find(class_name);
      if (ordinal_it == global_ordinal_by_class_name.end()) {
        continue;
      }
      const auto bundles =
          CollectPreferredClassBundlesForImage(*record, class_name);
      for (const EmittedClassBundle *bundle : bundles) {
        if (bundle == nullptr) {
          continue;
        }
        RealizedClassNode node;
        node.module_name = record->module_name;
        node.translation_unit_identity_key =
            record->translation_unit_identity_key;
        node.class_name = class_name;
        node.bundle_owner_identity =
            bundle->class_record.bundle_owner_identity != nullptr
                ? bundle->class_record.bundle_owner_identity
                : "";
        node.interface_owner_identity = node.bundle_owner_identity;
        for (std::uint64_t candidate_index = 0;
             candidate_index < record->class_descriptor_count;
             ++candidate_index) {
          const auto *candidate = static_cast<const EmittedClassBundle *>(
              ClassGraphAggregateEntry(record->class_descriptor_root,
                                       candidate_index));
          if (candidate == nullptr ||
              candidate->class_record.class_name == nullptr ||
              candidate->class_record.bundle_owner_identity == nullptr) {
            continue;
          }
          if (class_name != candidate->class_record.class_name) {
            continue;
          }
          if (std::string(candidate->class_record.bundle_owner_identity).rfind(
                  "interface:", 0) == 0) {
            node.interface_owner_identity =
                candidate->class_record.bundle_owner_identity;
            break;
          }
        }
        node.class_owner_identity =
            bundle->class_record.object_owner_identity != nullptr
                ? bundle->class_record.object_owner_identity
                : "";
        node.metaclass_owner_identity =
            bundle->metaclass_record.object_owner_identity != nullptr
                ? bundle->metaclass_record.object_owner_identity
                : "";
        if (!RuntimeMetaclassEdgeIsMaterializable(
                node.class_owner_identity.c_str(),
                node.metaclass_owner_identity.c_str())) {
          continue;
        }
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
        node.objc_final_declared = bundle->class_record.objc_final_declared;
        node.objc_sealed_declared = bundle->class_record.objc_sealed_declared;
        node.image = record;
        node.bundle = bundle;
        const std::size_t node_index = state.realized_class_nodes.size();
        state.realized_class_nodes.push_back(std::move(node));
        node_index_by_bundle.emplace(bundle, node_index);
        state.realized_class_node_indices_by_name[class_name].push_back(
            node_index);
      }
    }
  }

  for (std::size_t index = 0; index < state.realized_class_nodes.size();
       ++index) {
    RealizedClassNode &node = state.realized_class_nodes[index];
    if (node.bundle != nullptr &&
        node.bundle->class_record.super_bundle != nullptr) {
      const auto super_it = node_index_by_bundle.find(
          static_cast<const EmittedClassBundle *>(
              node.bundle->class_record.super_bundle));
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
    (void)AttachRealizedPropertyLayoutRecordsUnlocked(state, node);
  }

  if (!state.realized_class_nodes.empty()) {
    const RealizedClassNode &last_node = state.realized_class_nodes.back();
    state.last_realized_class_name = last_node.class_name;
    state.last_realized_class_owner_identity = last_node.class_owner_identity;
    state.last_realized_metaclass_owner_identity =
        last_node.metaclass_owner_identity;
  }
}

}  // namespace objc3c::runtime

extern "C" int objc3_runtime_copy_realized_class_graph_state_for_testing(
    objc3_runtime_realized_class_graph_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  objc3c::runtime::RuntimeState &state = objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  snapshot->realized_class_count =
      static_cast<std::uint64_t>(state.realized_class_nodes.size());
  snapshot->root_class_count = state.realized_root_class_count;
  snapshot->metaclass_edge_count = state.realized_metaclass_edge_count;
  snapshot->receiver_class_binding_count = state.receiver_class_binding_count;
  snapshot->attached_category_count = state.realized_attached_category_count;
  snapshot->protocol_conformance_edge_count =
      state.realized_protocol_conformance_edge_count;
  snapshot->live_instance_count = state.live_runtime_instance_count;
  snapshot->last_allocated_receiver_identity =
      state.last_allocated_runtime_instance_receiver;
  snapshot->last_allocated_base_identity =
      state.last_allocated_runtime_instance_base_identity;
  snapshot->last_allocated_instance_size_bytes =
      state.last_allocated_runtime_instance_size_bytes;
  snapshot->last_realized_class_name =
      objc3c::runtime::BorrowRuntimeCString(state.last_realized_class_name);
  snapshot->last_realized_class_owner_identity =
      objc3c::runtime::BorrowRuntimeCString(
          state.last_realized_class_owner_identity);
  snapshot->last_realized_metaclass_owner_identity =
      objc3c::runtime::BorrowRuntimeCString(
          state.last_realized_metaclass_owner_identity);
  snapshot->last_attached_category_owner_identity =
      objc3c::runtime::BorrowRuntimeCString(
          state.last_attached_category_owner_identity);
  snapshot->last_attached_category_name =
      objc3c::runtime::BorrowRuntimeCString(
          state.last_attached_category_name);
  snapshot->last_allocated_class_name =
      objc3c::runtime::BorrowRuntimeCString(
          state.last_allocated_runtime_instance_class_name);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

extern "C" int objc3_runtime_copy_realized_class_entry_for_testing(
    const char *class_name,
    objc3_runtime_realized_class_entry_snapshot *snapshot) {
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
  snapshot->runtime_property_accessor_count = 0;
  snapshot->runtime_instance_size_bytes = 0;
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

  objc3c::runtime::RuntimeState &state = objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  state.last_queried_class_name = class_name;
  state.last_resolved_class_query_name.clear();
  state.last_resolved_class_query_owner_identity.clear();
  state.last_class_query_found = false;
  const auto found = state.realized_class_node_indices_by_name.find(class_name);
  if (found == state.realized_class_node_indices_by_name.end() ||
      found->second.empty()) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  }
  const std::size_t node_index = found->second.front();
  if (node_index >= state.realized_class_nodes.size()) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  }
  const objc3c::runtime::RealizedClassNode &node =
      state.realized_class_nodes[node_index];
  state.last_class_query_found = true;
  state.last_resolved_class_query_name = node.class_name;
  state.last_resolved_class_query_owner_identity = node.class_owner_identity;
  snapshot->found = 1;
  snapshot->base_identity = node.base_identity;
  snapshot->registration_order_ordinal = node.registration_order_ordinal;
  snapshot->is_root_class = node.is_root_class ? 1 : 0;
  snapshot->implementation_backed = node.implementation_backed ? 1 : 0;
  snapshot->attached_category_count =
      static_cast<std::uint64_t>(node.attached_category_records.size());
  snapshot->runtime_property_accessor_count =
      static_cast<std::uint64_t>(node.runtime_property_accessors.size());
  snapshot->runtime_instance_size_bytes =
      static_cast<std::uint64_t>(node.runtime_instance_size_bytes);
  snapshot->direct_protocol_count =
      (node.bundle != nullptr &&
       node.bundle->class_record.adopted_protocol_refs != nullptr)
          ? node.bundle->class_record.adopted_protocol_refs->count
          : 0;
  snapshot->module_name =
      objc3c::runtime::BorrowRuntimeCString(node.module_name);
  snapshot->translation_unit_identity_key =
      objc3c::runtime::BorrowRuntimeCString(
          node.translation_unit_identity_key);
  snapshot->class_name =
      objc3c::runtime::BorrowRuntimeCString(node.class_name);
  snapshot->class_owner_identity =
      objc3c::runtime::BorrowRuntimeCString(node.class_owner_identity);
  snapshot->metaclass_owner_identity =
      objc3c::runtime::BorrowRuntimeCString(node.metaclass_owner_identity);
  snapshot->super_class_owner_identity =
      objc3c::runtime::BorrowRuntimeCString(node.super_class_owner_identity);
  snapshot->super_metaclass_owner_identity =
      objc3c::runtime::BorrowRuntimeCString(
          node.super_metaclass_owner_identity);
  std::uint64_t attached_protocol_count = 0;
  for (const objc3c::runtime::EmittedCategoryRecord *category_record :
       node.attached_category_records) {
    if (category_record != nullptr &&
        category_record->adopted_protocol_refs != nullptr) {
      attached_protocol_count += category_record->adopted_protocol_refs->count;
    }
  }
  snapshot->attached_protocol_count = attached_protocol_count;
  if (!node.attached_category_records.empty()) {
    const objc3c::runtime::EmittedCategoryRecord *last_category =
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

extern "C" int objc3_runtime_copy_object_model_query_state_for_testing(
    objc3_runtime_object_model_query_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  snapshot->realized_class_count = 0;
  snapshot->reflectable_property_count = 0;
  snapshot->attached_category_count = 0;
  snapshot->protocol_conformance_edge_count = 0;
  snapshot->method_cache_entry_count = 0;
  snapshot->last_class_query_found = 0;
  snapshot->last_property_query_found = 0;
  snapshot->last_property_query_inherited = 0;
  snapshot->last_protocol_query_class_found = 0;
  snapshot->last_protocol_query_protocol_found = 0;
  snapshot->last_protocol_query_conforms = 0;
  snapshot->last_queried_class_name = nullptr;
  snapshot->last_resolved_class_name = nullptr;
  snapshot->last_resolved_class_owner_identity = nullptr;
  snapshot->last_queried_property_name = nullptr;
  snapshot->last_resolved_property_class_name = nullptr;
  snapshot->last_resolved_property_owner_identity = nullptr;
  snapshot->last_queried_protocol_class_name = nullptr;
  snapshot->last_queried_protocol_name = nullptr;
  snapshot->last_matched_protocol_owner_identity = nullptr;
  snapshot->last_matched_attachment_owner_identity = nullptr;

  objc3c::runtime::RuntimeState &state = objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  snapshot->realized_class_count =
      static_cast<std::uint64_t>(state.realized_class_nodes.size());
  snapshot->protocol_conformance_edge_count =
      state.realized_protocol_conformance_edge_count;
  snapshot->method_cache_entry_count =
      static_cast<std::uint64_t>(state.method_cache.size());
  snapshot->last_class_query_found = state.last_class_query_found ? 1 : 0;
  snapshot->last_property_query_found = state.last_property_query_found ? 1 : 0;
  snapshot->last_property_query_inherited =
      state.last_property_query_inherited ? 1 : 0;
  snapshot->last_protocol_query_class_found =
      state.last_protocol_query_class_found ? 1 : 0;
  snapshot->last_protocol_query_protocol_found =
      state.last_protocol_query_protocol_found ? 1 : 0;
  snapshot->last_protocol_query_conforms =
      state.last_protocol_query_conforms ? 1 : 0;
  for (const objc3c::runtime::RealizedClassNode &node :
       state.realized_class_nodes) {
    snapshot->attached_category_count +=
        static_cast<std::uint64_t>(node.attached_category_records.size());
    snapshot->reflectable_property_count +=
        static_cast<std::uint64_t>(node.runtime_property_accessors.size());
  }
  snapshot->last_queried_class_name =
      objc3c::runtime::BorrowRuntimeCString(state.last_queried_class_name);
  snapshot->last_resolved_class_name =
      objc3c::runtime::BorrowRuntimeCString(
          state.last_resolved_class_query_name);
  snapshot->last_resolved_class_owner_identity =
      objc3c::runtime::BorrowRuntimeCString(
          state.last_resolved_class_query_owner_identity);
  snapshot->last_queried_property_name =
      objc3c::runtime::BorrowRuntimeCString(state.last_queried_property_name);
  snapshot->last_resolved_property_class_name =
      objc3c::runtime::BorrowRuntimeCString(
          state.last_reflected_property_class_name);
  snapshot->last_resolved_property_owner_identity =
      objc3c::runtime::BorrowRuntimeCString(
          state.last_reflected_property_owner_identity);
  snapshot->last_queried_protocol_class_name =
      objc3c::runtime::BorrowRuntimeCString(
          state.last_protocol_conformance_class_name);
  snapshot->last_queried_protocol_name =
      objc3c::runtime::BorrowRuntimeCString(
          state.last_protocol_conformance_protocol_name);
  snapshot->last_matched_protocol_owner_identity =
      objc3c::runtime::BorrowRuntimeCString(
          state.last_protocol_conformance_owner_identity);
  snapshot->last_matched_attachment_owner_identity =
      objc3c::runtime::BorrowRuntimeCString(
          state.last_protocol_conformance_attachment_owner_identity);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}
