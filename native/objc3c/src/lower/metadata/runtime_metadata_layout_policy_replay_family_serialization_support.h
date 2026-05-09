#pragma once

#include "lower/contracts/runtime_metadata_layout_policy_record_contracts.h"

#include <ostream>

static inline void AppendRuntimeMetadataLayoutPolicyReplayFamilies(
    std::ostream &out, const Objc3RuntimeMetadataLayoutPolicy &policy) {
  for (const auto &family : policy.families) {
    out << ";family=" << family.kind << "|" << family.logical_section_name
        << "|" << family.aggregate_symbol_name << "|" << family.descriptor_count
        << ";family_emitted=" << family.kind << "|"
        << family.emitted_section_name << "|" << family.aggregate_symbol_name
        << "|" << family.descriptor_count;
  }
}

static inline void AppendRuntimeMetadataLayoutPolicyReplayFailure(
    std::ostream &out, const Objc3RuntimeMetadataLayoutPolicy &policy) {
  if (!policy.failure_reason.empty()) {
    out << ";failure=" << policy.failure_reason;
  }
}
