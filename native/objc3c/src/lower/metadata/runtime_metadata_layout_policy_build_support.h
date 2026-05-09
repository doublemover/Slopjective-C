#pragma once

#include "lower/metadata/metadata_descriptor_count_helpers.h"
#include "lower/metadata/metadata_family_order_helpers.h"
#include "lower/metadata/metadata_object_format_helpers.h"
#include "lower/metadata/metadata_retention_flag_helpers.h"

#include <cstddef>
#include <string>

static inline bool FailRuntimeMetadataLayoutPolicyBuild(
    Objc3RuntimeMetadataLayoutPolicy &policy, std::string &error,
    const std::string &message) {
  error = message;
  policy.failure_reason = error;
  return false;
}

static inline void InitializeRuntimeMetadataLayoutPolicyFromInput(
    const Objc3RuntimeMetadataLayoutPolicyInput &input,
    Objc3RuntimeMetadataLayoutPolicy &policy) {
  policy = Objc3RuntimeMetadataLayoutPolicy{};
  policy.abi_contract_id = input.abi_contract_id;
  policy.scaffold_contract_id = input.scaffold_contract_id;
  policy.object_format = HostRuntimeMetadataObjectFormat();
  policy.section_spelling_model = HostRuntimeMetadataSectionSpellingModel();
  policy.retention_anchor_model = HostRuntimeMetadataRetentionAnchorModel();
  policy.image_info_symbol = input.image_info_symbol;
  policy.logical_image_info_section = input.image_info_section;
  policy.emitted_image_info_section = MapRuntimeMetadataSectionForObjectFormat(
      policy.object_format, input.image_info_section);
  policy.descriptor_symbol_prefix = input.descriptor_symbol_prefix;
  policy.descriptor_linkage = input.descriptor_linkage;
  policy.aggregate_linkage = input.aggregate_linkage;
  policy.metadata_visibility = input.metadata_visibility;
  policy.retention_root = input.retention_root;
  policy.total_retained_global_count = input.total_retained_global_count;
  policy.fail_closed = input.scaffold_fail_closed;
}

static inline bool ValidateRuntimeMetadataLayoutPolicyInputPrerequisites(
    const Objc3RuntimeMetadataLayoutPolicyInput &input,
    Objc3RuntimeMetadataLayoutPolicy &policy, std::string &error) {
  if (input.abi_contract_id.empty()) {
    return FailRuntimeMetadataLayoutPolicyBuild(
        policy, error,
        "runtime metadata layout policy requires a non-empty ABI contract id");
  }
  if (input.scaffold_contract_id.empty()) {
    return FailRuntimeMetadataLayoutPolicyBuild(
        policy, error,
        "runtime metadata layout policy requires a non-empty scaffold contract id");
  }
  if (!input.section_boundary_ready || !input.runtime_export_ready ||
      !input.scaffold_emitted || !input.scaffold_fail_closed ||
      !input.uses_llvm_used || !input.image_info_emitted) {
    return FailRuntimeMetadataLayoutPolicyBuild(
        policy, error,
        "runtime metadata layout policy prerequisites are not ready");
  }
  if (input.image_info_symbol.empty() || input.image_info_section.empty() ||
      policy.emitted_image_info_section.empty()) {
    return FailRuntimeMetadataLayoutPolicyBuild(
        policy, error,
        "runtime metadata layout policy requires image-info symbol and section");
  }
  if (!IsSupportedRuntimeMetadataObjectFormat(policy.object_format) ||
      policy.section_spelling_model.empty() ||
      policy.retention_anchor_model.empty()) {
    return FailRuntimeMetadataLayoutPolicyBuild(
        policy, error,
        "runtime metadata layout policy requires a supported explicit object-format surface");
  }
  if (input.descriptor_symbol_prefix.empty()) {
    return FailRuntimeMetadataLayoutPolicyBuild(
        policy, error,
        "runtime metadata layout policy requires a descriptor symbol prefix");
  }
  if (input.descriptor_linkage != "private") {
    return FailRuntimeMetadataLayoutPolicyBuild(
        policy, error,
        "runtime metadata layout policy requires descriptor linkage private");
  }
  if (input.aggregate_linkage != "internal") {
    return FailRuntimeMetadataLayoutPolicyBuild(
        policy, error,
        "runtime metadata layout policy requires aggregate linkage internal");
  }
  if (input.metadata_visibility != "hidden") {
    return FailRuntimeMetadataLayoutPolicyBuild(
        policy, error,
        "runtime metadata layout policy requires metadata visibility hidden");
  }
  if (input.retention_root != "llvm.used") {
    return FailRuntimeMetadataLayoutPolicyBuild(
        policy, error,
        "runtime metadata layout policy requires llvm.used retention root");
  }
  return true;
}

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
