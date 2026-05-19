#pragma once

#include "lower/metadata/metadata_object_format_helpers.h"
#include "lower/metadata/runtime_metadata_layout_policy_build_failure_support.h"

#include <string>

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
