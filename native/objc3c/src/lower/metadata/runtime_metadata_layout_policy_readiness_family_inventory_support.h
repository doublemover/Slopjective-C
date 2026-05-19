#pragma once

#include "lower/contracts/runtime_metadata_layout_policy_record_contracts.h"
#include "lower/metadata/metadata_descriptor_count_helpers.h"
#include "lower/metadata/metadata_family_order_helpers.h"

#include <cstddef>

static inline bool HasReadyRuntimeMetadataLayoutPolicyFamilies(
    const Objc3RuntimeMetadataLayoutPolicy &policy) {
  for (std::size_t i = 0; i < kCanonicalRuntimeMetadataFamilyOrder.size(); ++i) {
    const auto &family = policy.families[i];
    if (family.kind != kCanonicalRuntimeMetadataFamilyOrder[i] ||
        family.logical_section_name.empty() ||
        family.emitted_section_name.empty() ||
        family.aggregate_symbol_name.empty()) {
      return false;
    }
  }
  return true;
}

static inline bool HasReadyRuntimeMetadataLayoutPolicyDescriptorInventory(
    const Objc3RuntimeMetadataLayoutPolicy &policy) {
  return policy.total_retained_global_count ==
         CountRuntimeMetadataLayoutDescriptors(policy.families) + 6u;
}
