#pragma once

#include "lower/metadata/metadata_family_order_helpers.h"
#include "lower/metadata/metadata_object_format_helpers.h"
#include "lower/metadata/runtime_metadata_layout_policy_build_failure_support.h"

#include <cstddef>
#include <string>

static inline bool PopulateRuntimeMetadataLayoutPolicyFamilies(
    const Objc3RuntimeMetadataLayoutPolicyInput &input,
    Objc3RuntimeMetadataLayoutPolicy &policy, std::string &error) {
  for (std::size_t i = 0; i < kCanonicalRuntimeMetadataFamilyOrder.size(); ++i) {
    const auto &family_input = input.families[i];
    auto &family = policy.families[i];
    family.kind = kCanonicalRuntimeMetadataFamilyOrder[i];
    family.logical_section_name = family_input.section_name;
    family.emitted_section_name = MapRuntimeMetadataSectionForObjectFormat(
        policy.object_format, family_input.section_name);
    family.aggregate_symbol_name = family_input.aggregate_symbol_name;
    family.descriptor_count = family_input.descriptor_count;

    if (family_input.kind != kCanonicalRuntimeMetadataFamilyOrder[i]) {
      return FailRuntimeMetadataLayoutPolicyBuild(
          policy, error,
          "runtime metadata layout policy family order mismatch at index " +
              std::to_string(i) + ": expected " +
              kCanonicalRuntimeMetadataFamilyOrder[i] + " but saw " +
              family_input.kind);
    }
    if (family.logical_section_name.empty() ||
        family.emitted_section_name.empty() ||
        family.aggregate_symbol_name.empty()) {
      return FailRuntimeMetadataLayoutPolicyBuild(
          policy, error,
          "runtime metadata layout policy requires non-empty family section and aggregate names");
    }
  }
  return true;
}
