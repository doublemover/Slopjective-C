#include "runtime/classes/class_graph.h"

#include "runtime/classes/category_attachment.h"
#include "runtime/classes/class_metadata_tables.h"
#include "runtime/classes/metaclass_graph.h"
#include "runtime/classes/receiver_identity.h"
#include "runtime/dispatch/dispatch_family.h"
#include "runtime/metadata/runtime_emitted_records.h"
#include "runtime/metadata/runtime_registration_records.h"
#include "runtime/metadata/runtime_realized_records.h"
#include "runtime/state/runtime_cache_invalidation.h"
#include "runtime/state/runtime_state_clear.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/storage/property_layout_realization.h"

#include <algorithm>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace objc3c::runtime {

namespace {

struct ClassGraphBundleCandidate {
  const RegisteredImageMetadata *record = nullptr;
  const EmittedClassBundle *bundle = nullptr;
  bool implementation_backed = false;
};

std::vector<ClassGraphBundleCandidate> CollectClassGraphBundleCandidates(
    const std::vector<const RegisteredImageMetadata *> &ordered_images,
    const std::string &class_name) {
  std::vector<ClassGraphBundleCandidate> candidates;
  for (const RegisteredImageMetadata *record : ordered_images) {
    if (record == nullptr) {
      continue;
    }
    const std::vector<const EmittedClassBundle *> bundles =
        CollectClassBundlesForImage(*record, class_name);
    candidates.reserve(candidates.size() + bundles.size());
    for (const EmittedClassBundle *bundle : bundles) {
      if (bundle == nullptr) {
        continue;
      }
      candidates.push_back(
          {record, bundle,
           (bundle->class_record.method_list_ref != nullptr &&
            RuntimeImplementationOwnerIdentity(
                bundle->class_record.method_list_ref->owner_identity)) ||
               (bundle->metaclass_record.method_list_ref != nullptr &&
                RuntimeImplementationOwnerIdentity(
                    bundle->metaclass_record.method_list_ref->owner_identity))});
    }
  }
  return candidates;
}

const ClassGraphBundleCandidate *SelectPreferredClassGraphBundleCandidate(
    const std::vector<ClassGraphBundleCandidate> &candidates,
    const std::string &class_name,
    std::string &failure_reason) {
  const ClassGraphBundleCandidate *selected = nullptr;
  for (const ClassGraphBundleCandidate &candidate : candidates) {
    if (candidate.implementation_backed) {
      if (selected != nullptr) {
        failure_reason =
            "duplicate implementation-backed class metadata for " + class_name;
        return nullptr;
      }
      selected = &candidate;
    }
  }
  if (selected != nullptr) {
    return selected;
  }
  if (candidates.size() == 1u) {
    return &candidates.front();
  }
  failure_reason = "duplicate declaration-only class metadata for " + class_name;
  return nullptr;
}

std::string ResolveInterfaceOwnerIdentityForClassGraphCandidates(
    const std::vector<ClassGraphBundleCandidate> &candidates,
    const std::string &class_name,
    const std::string &default_owner_identity) {
  for (const ClassGraphBundleCandidate &candidate : candidates) {
    if (candidate.record == nullptr) {
      continue;
    }
    const std::string owner_identity = ResolveInterfaceOwnerIdentityForClass(
        *candidate.record, class_name, "");
    if (!owner_identity.empty()) {
      return owner_identity;
    }
  }
  return default_owner_identity;
}

bool AttachRealizedPropertyLayoutRecordsInSuperclassOrderUnlocked(
    RuntimeState &state,
    std::size_t node_index,
    std::vector<unsigned char> &visiting,
    std::vector<unsigned char> &attached) {
  if (node_index >= state.realized_class_nodes.size()) {
    return false;
  }
  if (attached[node_index] != 0u) {
    return true;
  }
  if (visiting[node_index] != 0u) {
    return false;
  }

  visiting[node_index] = 1u;
  RealizedClassNode &node = state.realized_class_nodes[node_index];
  if (node.has_super_node) {
    if (!AttachRealizedPropertyLayoutRecordsInSuperclassOrderUnlocked(
            state, node.super_node_index, visiting, attached)) {
      visiting[node_index] = 0u;
      return false;
    }
  }

  (void)AttachRealizedPropertyLayoutRecordsUnlocked(state, node);
  visiting[node_index] = 0u;
  attached[node_index] = 1u;
  return true;
}

void MarkMalformedRealizedClassGraphUnlocked(RuntimeState &state,
                                             std::string reason) {
  ClearRealizedClassGraphUnlocked(state);
  state.last_malformed_class_graph_metadata_surface.clear();
  state.last_malformed_class_graph_target_kind.clear();
  state.last_malformed_class_graph_visibility_state.clear();
  state.last_malformed_class_graph_availability_state.clear();
  ++state.malformed_class_metadata_rejection_count;
  state.last_malformed_class_graph_reason = std::move(reason);
  BumpRuntimeClassGraphGenerationUnlocked(state);
  BumpRuntimeMethodSurfaceGenerationUnlocked(state);
}

}  // namespace

void RebuildRealizedClassGraphUnlocked(RuntimeState &state) {
  // metaclass-graph-root-class anchor: runtime now republishes a
  // realized class/metaclass graph keyed by stable receiver base identities,
  // preserving root classes as explicit graph nodes rather than rediscovering
  // the class family from emitted bundles on every dispatch.
  ClearRealizedClassGraphUnlocked(state);

  std::vector<const RegisteredImageMetadata *> ordered_images =
      OrderedClassGraphImages(state);
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
      MarkMalformedRealizedClassGraphUnlocked(
          state, "registered image contains malformed class names");
      return;
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
  state.realized_class_nodes.reserve(global_class_names.size());
  state.realized_class_node_indices_by_name.reserve(global_class_names.size());
  for (const std::string &class_name : global_class_names) {
    const auto ordinal_it = global_ordinal_by_class_name.find(class_name);
    if (ordinal_it == global_ordinal_by_class_name.end()) {
      continue;
    }
    const std::vector<ClassGraphBundleCandidate> candidates =
        CollectClassGraphBundleCandidates(ordered_images, class_name);
    if (candidates.empty()) {
      MarkMalformedRealizedClassGraphUnlocked(
          state, "missing class metadata for " + class_name);
      return;
    }
    std::string failure_reason;
    const ClassGraphBundleCandidate *selected =
        SelectPreferredClassGraphBundleCandidate(candidates, class_name,
                                                 failure_reason);
    if (selected == nullptr || selected->record == nullptr ||
        selected->bundle == nullptr) {
      MarkMalformedRealizedClassGraphUnlocked(state, failure_reason);
      return;
    }

    const RegisteredImageMetadata *record = selected->record;
    const EmittedClassBundle *bundle = selected->bundle;
    RealizedClassNode node;
    node.module_name = record->module_name;
    node.translation_unit_identity_key =
        record->translation_unit_identity_key;
    node.class_name = class_name;
    node.bundle_owner_identity =
        bundle->class_record.bundle_owner_identity != nullptr
            ? bundle->class_record.bundle_owner_identity
            : "";
    node.interface_owner_identity =
        ResolveInterfaceOwnerIdentityForClassGraphCandidates(
            candidates, class_name, node.bundle_owner_identity);
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
    node.is_root_class = bundle->class_record.super_bundle == nullptr;
    std::string metaclass_failure_reason;
    if (!RuntimeMetaclassMetadataIsConsistent(
            node.class_name.c_str(), node.class_owner_identity.c_str(),
            node.metaclass_owner_identity.c_str(),
            node.super_class_owner_identity.c_str(),
            node.super_metaclass_owner_identity.c_str(), node.is_root_class,
            &metaclass_failure_reason)) {
      MarkMalformedRealizedClassGraphUnlocked(state, metaclass_failure_reason);
      return;
    }
    node.registration_order_ordinal = record->registration_order_ordinal;
    node.base_identity = BuildReceiverBaseIdentity(ordinal_it->second);
    node.implementation_backed = selected->implementation_backed;
    node.objc_final_declared = bundle->class_record.objc_final_declared;
    node.objc_sealed_declared = bundle->class_record.objc_sealed_declared;
    node.image = record;
    node.bundle = bundle;
    const std::size_t node_index = state.realized_class_nodes.size();
    state.realized_class_nodes.push_back(std::move(node));
    for (const ClassGraphBundleCandidate &candidate : candidates) {
      if (candidate.bundle != nullptr) {
        node_index_by_bundle.emplace(candidate.bundle, node_index);
      }
    }
    state.realized_class_node_indices_by_name[class_name].push_back(
        node_index);
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
        const RealizedClassNode &super_node =
            state.realized_class_nodes[node.super_node_index];
        std::string metaclass_failure_reason;
        if (!RuntimeMetaclassSuperclassLinkIsConsistent(
                node.class_name.c_str(),
                node.super_class_owner_identity.c_str(),
                node.super_metaclass_owner_identity.c_str(),
                super_node.class_owner_identity.c_str(),
                super_node.metaclass_owner_identity.c_str(),
                &metaclass_failure_reason)) {
          MarkMalformedRealizedClassGraphUnlocked(state,
                                                  metaclass_failure_reason);
          return;
        }
        ++state.realized_metaclass_edge_count;
      } else {
        MarkMalformedRealizedClassGraphUnlocked(
            state, "realized superclass link is missing for " +
                       node.class_name);
        return;
      }
    }
    if (node.is_root_class) {
      ++state.realized_root_class_count;
    }
    if (!AttachRealizedCategoryRecordsUnlocked(state, node)) {
      const std::string reason =
          state.last_malformed_class_graph_reason.empty()
              ? "category attachment failed for " + node.class_name
              : state.last_malformed_class_graph_reason;
      ClearRealizedClassGraphUnlocked(state);
      state.last_malformed_class_graph_reason = reason;
      BumpRuntimeClassGraphGenerationUnlocked(state);
      BumpRuntimeMethodSurfaceGenerationUnlocked(state);
      return;
    }
  }

  std::vector<unsigned char> property_layout_visiting(
      state.realized_class_nodes.size(), 0u);
  std::vector<unsigned char> property_layout_attached(
      state.realized_class_nodes.size(), 0u);
  for (std::size_t index = 0; index < state.realized_class_nodes.size();
       ++index) {
    (void)AttachRealizedPropertyLayoutRecordsInSuperclassOrderUnlocked(
        state, index, property_layout_visiting, property_layout_attached);
  }

  if (!state.realized_class_nodes.empty()) {
    const RealizedClassNode &last_node = state.realized_class_nodes.back();
    state.last_realized_class_name = last_node.class_name;
    state.last_realized_class_owner_identity = last_node.class_owner_identity;
    state.last_realized_metaclass_owner_identity =
        last_node.metaclass_owner_identity;
  }
  BumpRuntimeClassGraphGenerationUnlocked(state);
  BumpRuntimeMethodSurfaceGenerationUnlocked(state);
}

}  // namespace objc3c::runtime
