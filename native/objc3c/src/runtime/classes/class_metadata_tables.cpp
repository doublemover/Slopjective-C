#include "runtime/classes/class_metadata_tables.h"

#include "runtime/images/image_descriptor.h"
#include "runtime/images/multi_image_ordering.h"
#include "runtime/metadata/runtime_emitted_records.h"
#include "runtime/metadata/runtime_registration_records.h"
#include "runtime/state/runtime_state_records.h"

#include <algorithm>
#include <cstdint>
#include <string>
#include <unordered_set>
#include <vector>

namespace objc3c::runtime {

namespace {

const void *ClassMetadataAggregateEntry(
    const objc3_runtime_pointer_aggregate *aggregate, std::uint64_t index) {
  return RuntimeAggregateEntry(aggregate, index);
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
        ClassMetadataAggregateEntry(record.class_descriptor_root, index));
    if (bundle == nullptr || bundle->class_record.class_name == nullptr ||
        class_name != bundle->class_record.class_name) {
      continue;
    }
    const bool implementation_backed =
        bundle->class_record.method_list_ref != nullptr &&
        RuntimeImplementationOwnerIdentity(
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
