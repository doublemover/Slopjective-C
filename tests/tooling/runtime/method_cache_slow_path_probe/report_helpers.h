#pragma once

#include "probe_state.h"

#include "support/runtime_snapshot_json.h"

#include <cstdio>

namespace objc3c {
namespace tooling {
namespace method_cache_slow_path_probe {

inline void PrintMethodCacheSlowPathProbeReport(const SlowPathProbeRun &run) {
  std::printf("{");
  std::printf("\"registration_state\":");
  ::objc3c::runtime::probe::PrintRegistrationStateBasic(
      run.registration.state);
  std::printf(",\"selector_table_state\":");
  ::objc3c::runtime::probe::PrintSelectorTableStateBasic(
      run.selector_table.state);
  std::printf(",\"instance_first\":%d,", run.instance_first);
  std::printf("\"instance_second\":%d,", run.instance_second);
  std::printf("\"instance_after_stale\":%d,", run.instance_after_stale);
  std::printf("\"class_self\":%d,", run.class_self);
  std::printf("\"known_class\":%d,", run.known_class);
  std::printf("\"strict_error_first\":%d,", run.strict_error_first);
  std::printf("\"strict_error_second\":%d,", run.strict_error_second);
  std::printf("\"strict_error_expected\":%d,", run.strict_error_expected);
  std::printf("\"mutation_registration_status\":%d,",
              run.mutation_registration_status);
  std::printf("\"registration_after_mutation_state\":");
  ::objc3c::runtime::probe::PrintRegistrationStateBasic(
      run.registration_after_mutation.state);
  std::printf(",\"instance_first_state\":");
  ::objc3c::runtime::probe::PrintMethodCacheStateSlowPath(
      run.instance_first_state.state);
  std::printf(",\"instance_second_state\":");
  ::objc3c::runtime::probe::PrintMethodCacheStateSlowPath(
      run.instance_second_state.state);
  std::printf(",\"instance_after_stale_state\":");
  ::objc3c::runtime::probe::PrintMethodCacheStateSlowPath(
      run.instance_after_stale_state.state);
  std::printf(",\"class_self_state\":");
  ::objc3c::runtime::probe::PrintMethodCacheStateSlowPath(
      run.class_self_state.state);
  std::printf(",\"known_class_state\":");
  ::objc3c::runtime::probe::PrintMethodCacheStateSlowPath(
      run.known_class_state.state);
  std::printf(",\"strict_error_first_state\":");
  ::objc3c::runtime::probe::PrintMethodCacheStateSlowPath(
      run.strict_error_first_state.state);
  std::printf(",\"strict_error_second_state\":");
  ::objc3c::runtime::probe::PrintMethodCacheStateSlowPath(
      run.strict_error_second_state.state);
  std::printf(",\"instance_entry\":");
  ::objc3c::runtime::probe::PrintMethodCacheEntryBasic(
      run.instance_entry.entry);
  std::printf(",\"instance_after_stale_entry\":");
  ::objc3c::runtime::probe::PrintMethodCacheEntryBasic(
      run.instance_after_stale_entry.entry);
  std::printf(",\"class_entry\":");
  ::objc3c::runtime::probe::PrintMethodCacheEntryBasic(run.class_entry.entry);
  std::printf(",\"strict_error_entry\":");
  ::objc3c::runtime::probe::PrintMethodCacheEntryBasic(
      run.strict_error_entry.entry);
  std::printf("}\n");
}

} // namespace method_cache_slow_path_probe
} // namespace tooling
} // namespace objc3c
