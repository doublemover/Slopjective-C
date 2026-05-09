#pragma once

#include "lower/contracts/runtime_metadata_layout_policy_record_contracts.h"
#include "lower/metadata/metadata_replay_token_helpers.h"

#include <ostream>

static inline void AppendRuntimeMetadataLayoutPolicyReplayHeader(
    std::ostream &out, const Objc3RuntimeMetadataLayoutPolicy &policy) {
  out << "contract=" << policy.contract_id
      << ";abi_contract=" << policy.abi_contract_id
      << ";scaffold_contract=" << policy.scaffold_contract_id
      << ";object_format_contract="
      << policy.object_format_surface_contract_id
      << ";ready=" << BoolToken(policy.ready)
      << ";fail_closed=" << BoolToken(policy.fail_closed)
      << ";family_order=" << policy.family_ordering_model
      << ";descriptor_order=" << policy.descriptor_ordering_model
      << ";aggregate_relocation=" << policy.aggregate_relocation_policy
      << ";comdat=" << policy.comdat_policy
      << ";visibility_spelling=" << policy.visibility_spelling_policy
      << ";retention_order=" << policy.retention_ordering_model
      << ";object_format_model=" << policy.object_format_policy_model
      << ";object_format=" << policy.object_format
      << ";section_spelling_model=" << policy.section_spelling_model
      << ";retention_anchor_model=" << policy.retention_anchor_model
      << ";image_info=" << policy.image_info_symbol << "@"
      << policy.logical_image_info_section
      << ";image_info_emitted=" << policy.image_info_symbol << "@"
      << policy.emitted_image_info_section
      << ";descriptor_linkage=" << policy.descriptor_linkage
      << ";aggregate_linkage=" << policy.aggregate_linkage
      << ";metadata_visibility=" << policy.metadata_visibility
      << ";retention_root=" << policy.retention_root
      << ";total_retained_globals=" << policy.total_retained_global_count;
}
