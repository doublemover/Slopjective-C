#pragma once

#include "lower/metadata/metadata_object_format_helpers.h"
#include "lower/metadata/metadata_retention_flag_helpers.h"

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
