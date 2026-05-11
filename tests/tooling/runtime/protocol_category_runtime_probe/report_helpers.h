#pragma once

#include "probe_state.h"
#include "support/runtime_snapshot_json.h"

#include <cstdio>

namespace objc3c::runtime::probe::protocol_category_runtime {

inline void PrintProtocolCategoryRuntimeReport(
    const ProtocolCategoryProbeRun &run) {
  const ProtocolCategoryValues &values = run.values;

  std::printf("{");
  std::printf("\"registration_state\":");
  ::objc3c::runtime::probe::PrintRegistrationStateBasic(
      run.registry.registration_state);
  std::printf(",\"selector_table_state\":");
  ::objc3c::runtime::probe::PrintSelectorTableStateBasic(
      run.registry.selector_table_state);
  std::printf(",\"instance_first\":%d,", values.instance_first);
  std::printf("\"instance_second\":%d,", values.instance_second);
  std::printf("\"class_self\":%d,", values.class_self);
  std::printf("\"known_class\":%d,", values.known_class);
  std::printf("\"strict_error_first\":%d,", values.strict_error_first);
  std::printf("\"strict_error_second\":%d,", values.strict_error_second);
  std::printf("\"strict_error_expected\":%d,",
              values.strict_error_expected);
  std::printf("\"instance_first_state\":");
  ::objc3c::runtime::probe::PrintMethodCacheStateProtocolCategory(
      run.instance_first_state.state);
  std::printf(",\"instance_second_state\":");
  ::objc3c::runtime::probe::PrintMethodCacheStateProtocolCategory(
      run.instance_second_state.state);
  std::printf(",\"class_self_state\":");
  ::objc3c::runtime::probe::PrintMethodCacheStateProtocolCategory(
      run.class_self_state.state);
  std::printf(",\"known_class_state\":");
  ::objc3c::runtime::probe::PrintMethodCacheStateProtocolCategory(
      run.known_class_state.state);
  std::printf(",\"strict_error_first_state\":");
  ::objc3c::runtime::probe::PrintMethodCacheStateProtocolCategory(
      run.strict_error_first_state.state);
  std::printf(",\"strict_error_second_state\":");
  ::objc3c::runtime::probe::PrintMethodCacheStateProtocolCategory(
      run.strict_error_second_state.state);
  std::printf(",\"instance_entry\":");
  ::objc3c::runtime::probe::PrintMethodCacheEntryWithProbeCounts(
      run.instance_entry.entry);
  std::printf(",\"class_entry\":");
  ::objc3c::runtime::probe::PrintMethodCacheEntryWithProbeCounts(
      run.class_entry.entry);
  std::printf(",\"strict_error_entry\":");
  ::objc3c::runtime::probe::PrintMethodCacheEntryWithProbeCounts(
      run.strict_error_entry.entry);
  std::printf("}\n");
}

}  // namespace objc3c::runtime::probe::protocol_category_runtime
