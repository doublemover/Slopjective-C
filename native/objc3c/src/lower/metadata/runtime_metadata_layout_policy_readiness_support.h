#pragma once

#include "lower/metadata/lowering_metadata_helpers.h"

static inline bool HasReadyRuntimeMetadataLayoutPolicyHeader(
    const Objc3RuntimeMetadataLayoutPolicy &policy) {
  return policy.ready && policy.fail_closed && !policy.contract_id.empty() &&
         !policy.abi_contract_id.empty() &&
         !policy.scaffold_contract_id.empty() &&
         !policy.object_format_surface_contract_id.empty() &&
         IsSupportedRuntimeMetadataObjectFormat(policy.object_format) &&
         !policy.section_spelling_model.empty() &&
         !policy.retention_anchor_model.empty() &&
         !policy.image_info_symbol.empty() &&
         !policy.logical_image_info_section.empty() &&
         !policy.emitted_image_info_section.empty() &&
         !policy.descriptor_symbol_prefix.empty() &&
         policy.descriptor_linkage == "private" &&
         policy.aggregate_linkage == "internal" &&
         policy.metadata_visibility == "hidden" &&
         policy.retention_root == "llvm.used" &&
         policy.family_ordering_model ==
             kObjc3RuntimeMetadataLayoutFamilyOrderingModel &&
         policy.descriptor_ordering_model ==
             kObjc3RuntimeMetadataDescriptorOrderingModel &&
         policy.aggregate_relocation_policy ==
             kObjc3RuntimeMetadataAggregateRelocationPolicy &&
         policy.comdat_policy == kObjc3RuntimeMetadataComdatPolicy &&
         policy.visibility_spelling_policy ==
             kObjc3RuntimeMetadataVisibilitySpellingPolicy &&
         policy.retention_ordering_model ==
             kObjc3RuntimeMetadataRetentionOrderingModel &&
         policy.object_format_policy_model ==
             kObjc3RuntimeMetadataObjectFormatPolicyModel &&
         policy.failure_reason.empty();
}

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
