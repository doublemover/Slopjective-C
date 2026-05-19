#pragma once

#include "probe_state.h"
#include "support/runtime_snapshot_json.h"

#include <cstdio>

namespace objc3c::runtime::probe::class_realization_runtime {

inline void PrintClassRealizationRuntimeReport(
    const ClassRealizationProbeRun &run) {
  const ClassRealizationValues &values = run.values;

  std::printf("{");
  std::printf("\"inherited_value\":%d,", values.inherited_value);
  std::printf("\"category_value\":%d,", values.category_value);
  std::printf("\"class_value\":%d,", values.class_value);
  std::printf("\"known_class_value\":%d,", values.known_class_value);
  std::printf("\"protocol_strict_error\":%d,",
              values.protocol_strict_error);
  std::printf("\"protocol_strict_error_cached\":%d,",
              values.protocol_strict_error_cached);
  std::printf("\"protocol_strict_error_expected\":%d,",
              values.protocol_strict_error_expected);
  std::printf("\"registration_state\":");
  ::objc3c::runtime::probe::PrintRegistrationStateFull(
      run.registry.registration_state);
  std::printf(",\"selector_table_state\":");
  ::objc3c::runtime::probe::PrintSelectorTableStateFull(
      run.registry.selector_table_state);
  std::printf(",\"inherited_state\":");
  ::objc3c::runtime::probe::PrintMethodCacheStateFull(
      run.inherited_state.state);
  std::printf(",\"category_state\":");
  ::objc3c::runtime::probe::PrintMethodCacheStateFull(
      run.category_state.state);
  std::printf(",\"class_state\":");
  ::objc3c::runtime::probe::PrintMethodCacheStateFull(run.class_state.state);
  std::printf(",\"known_class_state\":");
  ::objc3c::runtime::probe::PrintMethodCacheStateFull(
      run.known_class_state.state);
  std::printf(",\"protocol_strict_error_state\":");
  ::objc3c::runtime::probe::PrintMethodCacheStateFull(
      run.protocol_strict_error_state.state);
  std::printf(",\"protocol_strict_error_cached_state\":");
  ::objc3c::runtime::probe::PrintMethodCacheStateFull(
      run.protocol_strict_error_cached_state.state);
  std::printf(",\"inherited_entry\":");
  ::objc3c::runtime::probe::PrintMethodCacheEntryWithProbeCounts(
      run.inherited_entry.entry);
  std::printf(",\"category_entry\":");
  ::objc3c::runtime::probe::PrintMethodCacheEntryWithProbeCounts(
      run.category_entry.entry);
  std::printf(",\"known_class_entry\":");
  ::objc3c::runtime::probe::PrintMethodCacheEntryWithProbeCounts(
      run.known_class_entry.entry);
  std::printf(",\"protocol_strict_error_entry\":");
  ::objc3c::runtime::probe::PrintMethodCacheEntryWithProbeCounts(
      run.protocol_strict_error_entry.entry);
  std::printf("}\n");
}

}  // namespace objc3c::runtime::probe::class_realization_runtime
