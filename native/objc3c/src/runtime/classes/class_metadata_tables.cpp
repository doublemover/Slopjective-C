#include "runtime/classes/class_metadata_tables.h"

#include "runtime/classes/metaclass_graph.h"
#include "runtime/images/image_descriptor.h"
#include "runtime/images/multi_image_ordering.h"
#include "runtime/metadata/runtime_emitted_records.h"
#include "runtime/metadata/runtime_registration_records.h"
#include "runtime/state/runtime_state_records.h"

#include <algorithm>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace objc3c::runtime {

namespace {

const void *ClassMetadataAggregateEntry(
    const objc3_runtime_pointer_aggregate *aggregate, std::uint64_t index) {
  return RuntimeAggregateEntry(aggregate, index);
}

bool RuntimeNonEmptyCString(const char *value) {
  return value != nullptr && value[0] != '\0';
}

void AddKnownClassBundleOwners(
    const objc3_runtime_pointer_aggregate *class_descriptor_root,
    std::uint64_t class_descriptor_count,
    std::unordered_map<const EmittedClassBundle *, const EmittedClassBundle *>
        &known_bundles) {
  for (std::uint64_t index = 0; index < class_descriptor_count; ++index) {
    const auto *bundle = static_cast<const EmittedClassBundle *>(
        ClassMetadataAggregateEntry(class_descriptor_root, index));
    if (bundle != nullptr) {
      known_bundles.emplace(bundle, bundle);
    }
  }
}

bool RuntimeClassBundleShapeIsSupported(
    const EmittedClassBundle &bundle, std::string &diagnostic_reason) {
  if (!RuntimeNonEmptyCString(bundle.class_record.class_name)) {
    diagnostic_reason = "class bundle is missing class name";
    return false;
  }
  if (!RuntimeNonEmptyCString(bundle.metaclass_record.class_name)) {
    diagnostic_reason = "class bundle is missing metaclass class name";
    return false;
  }
  if (std::string(bundle.class_record.class_name) !=
      bundle.metaclass_record.class_name) {
    diagnostic_reason =
        std::string("class bundle metaclass name mismatch for ") +
        bundle.class_record.class_name;
    return false;
  }
  if (!RuntimeNonEmptyCString(bundle.class_record.bundle_owner_identity)) {
    diagnostic_reason =
        std::string("class bundle is missing bundle owner for ") +
        bundle.class_record.class_name;
    return false;
  }
  if (!RuntimeMetaclassEdgeIsMaterializable(
          bundle.class_record.object_owner_identity,
          bundle.metaclass_record.object_owner_identity)) {
    diagnostic_reason =
        std::string("class/metaclass owner edge is incomplete for ") +
        bundle.class_record.class_name;
    return false;
  }
  return true;
}

bool RuntimeClassBundleHasImplementationBacking(
    const EmittedClassBundle &bundle) {
  return (bundle.class_record.method_list_ref != nullptr &&
          RuntimeImplementationOwnerIdentity(
              bundle.class_record.method_list_ref->owner_identity)) ||
         (bundle.metaclass_record.method_list_ref != nullptr &&
          RuntimeImplementationOwnerIdentity(
              bundle.metaclass_record.method_list_ref->owner_identity));
}

bool RuntimeSuperclassEdgeIsSupported(
    const EmittedClassBundle &bundle,
    const std::unordered_map<const EmittedClassBundle *,
                             const EmittedClassBundle *> &known_bundles,
    std::string &diagnostic_reason) {
  if (bundle.class_record.super_bundle == nullptr) {
    return true;
  }
  const auto *super_bundle = static_cast<const EmittedClassBundle *>(
      bundle.class_record.super_bundle);
  const auto found = known_bundles.find(super_bundle);
  if (found == known_bundles.end() || found->second == nullptr) {
    diagnostic_reason =
        std::string("superclass bundle is not registered before ") +
        bundle.class_record.class_name;
    return false;
  }
  if (!RuntimeNonEmptyCString(bundle.class_record.super_owner_identity) ||
      !RuntimeNonEmptyCString(bundle.metaclass_record.super_owner_identity)) {
    diagnostic_reason =
        std::string("superclass owner edge is incomplete for ") +
        bundle.class_record.class_name;
    return false;
  }
  if (found->second->class_record.object_owner_identity == nullptr ||
      found->second->metaclass_record.object_owner_identity == nullptr ||
      std::string(bundle.class_record.super_owner_identity) !=
          found->second->class_record.object_owner_identity ||
      std::string(bundle.metaclass_record.super_owner_identity) !=
          found->second->metaclass_record.object_owner_identity) {
    diagnostic_reason =
        std::string("superclass owner edge target mismatch for ") +
        bundle.class_record.class_name;
    return false;
  }
  return true;
}

struct RuntimeClassResolutionCounts {
  std::uint64_t declaration_bundle_count = 0;
  std::uint64_t implementation_bundle_count = 0;
};

bool AddClassResolutionCounts(
    const objc3_runtime_pointer_aggregate *class_descriptor_root,
    std::uint64_t class_descriptor_count,
    std::unordered_map<std::string, RuntimeClassResolutionCounts>
        &counts_by_class_name,
    std::string &diagnostic_reason) {
  for (std::uint64_t index = 0; index < class_descriptor_count; ++index) {
    const auto *bundle = static_cast<const EmittedClassBundle *>(
        ClassMetadataAggregateEntry(class_descriptor_root, index));
    if (bundle == nullptr || bundle->class_record.class_name == nullptr ||
        bundle->class_record.class_name[0] == '\0') {
      continue;
    }
    RuntimeClassResolutionCounts &counts =
        counts_by_class_name[bundle->class_record.class_name];
    if (RuntimeClassBundleHasImplementationBacking(*bundle)) {
      ++counts.implementation_bundle_count;
    } else {
      ++counts.declaration_bundle_count;
    }
  }

  for (const auto &entry : counts_by_class_name) {
    const RuntimeClassResolutionCounts &counts = entry.second;
    if (counts.implementation_bundle_count > 1u) {
      diagnostic_reason =
          "duplicate implementation-backed class metadata for " + entry.first;
      return false;
    }
    if (counts.implementation_bundle_count == 0u &&
        counts.declaration_bundle_count > 1u) {
      diagnostic_reason =
          "duplicate declaration-only class metadata for " + entry.first;
      return false;
    }
  }
  return true;
}

}  // namespace

bool RuntimeImplementationOwnerIdentity(const char *owner_identity) {
  if (owner_identity == nullptr) {
    return false;
  }
  static constexpr char kImplementationPrefix[] = "implementation:";
  return std::string(owner_identity).rfind(kImplementationPrefix, 0) == 0;
}

std::vector<const RegisteredImageMetadata *> OrderedClassGraphImages(
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
        ClassMetadataAggregateEntry(record.class_descriptor_root, index));
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

std::vector<const EmittedClassBundle *> CollectClassBundlesForImage(
    const RegisteredImageMetadata &record, const std::string &class_name) {
  std::vector<const EmittedClassBundle *> bundles;
  bundles.reserve(static_cast<std::size_t>(record.class_descriptor_count));
  for (std::uint64_t index = 0; index < record.class_descriptor_count; ++index) {
    const auto *bundle = static_cast<const EmittedClassBundle *>(
        ClassMetadataAggregateEntry(record.class_descriptor_root, index));
    if (bundle == nullptr || bundle->class_record.class_name == nullptr ||
        class_name != bundle->class_record.class_name) {
      continue;
    }
    bundles.push_back(bundle);
  }
  return bundles;
}

bool RuntimeClassMetadataTableIsSupported(
    const RuntimeState &state,
    const objc3_runtime_registration_table *registration_table,
    std::string &diagnostic_reason) {
  diagnostic_reason.clear();
  if (registration_table == nullptr ||
      registration_table->class_descriptor_root == nullptr ||
      registration_table->image_descriptor == nullptr) {
    diagnostic_reason = "registration table is missing class metadata";
    return false;
  }

  std::unordered_map<const EmittedClassBundle *, const EmittedClassBundle *>
      known_bundles;
  known_bundles.reserve(
      static_cast<std::size_t>(
          registration_table->image_descriptor->class_descriptor_count) +
      state.realized_class_nodes.size());
  AddKnownClassBundleOwners(
      registration_table->class_descriptor_root,
      registration_table->image_descriptor->class_descriptor_count,
      known_bundles);
  for (const RegisteredImageMetadata *record : OrderedClassGraphImages(state)) {
    if (record == nullptr) {
      continue;
    }
    AddKnownClassBundleOwners(record->class_descriptor_root,
                              record->class_descriptor_count, known_bundles);
  }

  for (std::uint64_t index = 0;
       index < registration_table->image_descriptor->class_descriptor_count;
       ++index) {
    const auto *bundle = static_cast<const EmittedClassBundle *>(
        ClassMetadataAggregateEntry(registration_table->class_descriptor_root,
                                    index));
    if (bundle == nullptr) {
      diagnostic_reason = "class descriptor root contains a null class bundle";
      return false;
    }
    if (!RuntimeClassBundleShapeIsSupported(*bundle, diagnostic_reason) ||
        !RuntimeSuperclassEdgeIsSupported(*bundle, known_bundles,
                                          diagnostic_reason)) {
      return false;
    }
  }

  std::unordered_map<std::string, RuntimeClassResolutionCounts>
      counts_by_class_name;
  counts_by_class_name.reserve(
      static_cast<std::size_t>(
          registration_table->image_descriptor->class_descriptor_count) +
      state.realized_class_node_indices_by_name.size());
  if (!AddClassResolutionCounts(
          registration_table->class_descriptor_root,
          registration_table->image_descriptor->class_descriptor_count,
          counts_by_class_name, diagnostic_reason)) {
    return false;
  }
  for (const RegisteredImageMetadata *record : OrderedClassGraphImages(state)) {
    if (record == nullptr) {
      continue;
    }
    if (!AddClassResolutionCounts(record->class_descriptor_root,
                                  record->class_descriptor_count,
                                  counts_by_class_name, diagnostic_reason)) {
      return false;
    }
  }
  return true;
}

std::vector<const EmittedClassBundle *> CollectPreferredClassBundlesForImage(
    const RegisteredImageMetadata &record, const std::string &class_name) {
  std::vector<const EmittedClassBundle *> implementation_bundles;
  std::vector<const EmittedClassBundle *> candidate_bundles;
  const std::vector<const EmittedClassBundle *> bundles =
      CollectClassBundlesForImage(record, class_name);
  implementation_bundles.reserve(bundles.size());
  candidate_bundles.reserve(bundles.size());
  for (const EmittedClassBundle *bundle : bundles) {
    if (bundle == nullptr) {
      continue;
    }
    if (RuntimeClassBundleHasImplementationBacking(*bundle)) {
      implementation_bundles.push_back(bundle);
    } else {
      candidate_bundles.push_back(bundle);
    }
  }
  return implementation_bundles.empty() ? candidate_bundles
                                        : implementation_bundles;
}

std::string ResolveInterfaceOwnerIdentityForClass(
    const RegisteredImageMetadata &record, const std::string &class_name,
    const std::string &default_owner_identity) {
  for (std::uint64_t index = 0; index < record.class_descriptor_count; ++index) {
    const auto *candidate = static_cast<const EmittedClassBundle *>(
        ClassMetadataAggregateEntry(record.class_descriptor_root, index));
    if (candidate == nullptr ||
        candidate->class_record.class_name == nullptr ||
        candidate->class_record.bundle_owner_identity == nullptr) {
      continue;
    }
    if (class_name != candidate->class_record.class_name) {
      continue;
    }
    if (std::string(candidate->class_record.bundle_owner_identity)
            .rfind("interface:", 0) == 0) {
      return candidate->class_record.bundle_owner_identity;
    }
  }
  return default_owner_identity;
}

}  // namespace objc3c::runtime
