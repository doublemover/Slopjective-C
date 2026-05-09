#pragma once

#include "runtime/state/runtime_bootstrap_contracts.h"

#include <cstdint>

namespace objc3c::runtime {

struct RuntimeRegistrationTableDescriptorCounts {
  std::uint64_t discovery_root_entry_count = 0;
  std::uint64_t class_descriptor_count = 0;
  std::uint64_t protocol_descriptor_count = 0;
  std::uint64_t category_descriptor_count = 0;
  std::uint64_t property_descriptor_count = 0;
  std::uint64_t ivar_descriptor_count = 0;
  std::uint64_t selector_pool_count = 0;
  std::uint64_t string_pool_count = 0;
  std::uint64_t keypath_descriptor_count = 0;
};

bool RuntimeRegistrationTableShapeIsSupported(
    const objc3_runtime_registration_table *registration_table,
    const objc3_runtime_image_descriptor *image);
RuntimeRegistrationTableDescriptorCounts
ReadRuntimeRegistrationTableDescriptorCounts(
    const objc3_runtime_registration_table *registration_table);
bool RuntimeRegistrationTableDescriptorCountsMatchImage(
    const RuntimeRegistrationTableDescriptorCounts &counts,
    const objc3_runtime_image_descriptor *image);
bool RuntimeRegistrationTableDiscoveryRootsAreClosed(
    const objc3_runtime_registration_table *registration_table,
    bool &linker_anchor_matches_discovery_root);

}  // namespace objc3c::runtime
