#pragma once

#include "lower/metadata/metadata_descriptor_count_helpers.h"
#include "lower/metadata/runtime_metadata_layout_policy_build_failure_support.h"

#include <cstddef>
#include <string>

static inline bool ValidateRuntimeMetadataLayoutPolicyDescriptorInventory(
    const Objc3RuntimeMetadataLayoutPolicyInput &input,
    Objc3RuntimeMetadataLayoutPolicy &policy, std::string &error) {
  const std::size_t total_descriptor_count =
      CountRuntimeMetadataLayoutDescriptors(policy.families);
  if (input.total_retained_global_count != total_descriptor_count + 6u) {
    return FailRuntimeMetadataLayoutPolicyBuild(
        policy, error,
        "runtime metadata layout policy retained-global count drifted from descriptor inventory");
  }
  return true;
}
