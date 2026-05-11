#pragma once

#include "probe_state.h"

#include "support/runtime_snapshot_json.h"

#include <cstdio>

namespace objc3c {
namespace tooling {
namespace method_binding_probe {

inline void PrintMethodBindingProbeReport(const MethodBindingProbeRun &run) {
  std::printf("{");
  std::printf("\"instance_first\":%d,", run.values.instance_first);
  std::printf("\"instance_second\":%d,", run.values.instance_second);
  std::printf("\"class_value\":%d,", run.values.class_value);
  std::printf("\"known_class_value\":%d,", run.values.known_class_value);
  std::printf("\"category_value\":%d,", run.values.category_value);
  std::printf("\"registration_state\":");
  ::objc3c::runtime::probe::PrintRegistrationStateBasic(
      run.registration.state);
  std::printf(",\"selector_table_state\":");
  ::objc3c::runtime::probe::PrintSelectorTableStateBasic(
      run.selector_table.state);
  std::printf(",\"instance_first_state\":");
  ::objc3c::runtime::probe::PrintMethodCacheStateMethodBinding(
      run.instance_first_state.state);
  std::printf(",\"instance_second_state\":");
  ::objc3c::runtime::probe::PrintMethodCacheStateMethodBinding(
      run.instance_second_state.state);
  std::printf(",\"class_state\":");
  ::objc3c::runtime::probe::PrintMethodCacheStateMethodBinding(
      run.class_state.state);
  std::printf(",\"known_class_state\":");
  ::objc3c::runtime::probe::PrintMethodCacheStateMethodBinding(
      run.known_class_state.state);
  std::printf(",\"category_state\":");
  ::objc3c::runtime::probe::PrintMethodCacheStateMethodBinding(
      run.category_state.state);
  std::printf(",\"instance_entry\":");
  ::objc3c::runtime::probe::PrintMethodCacheEntryBasic(
      run.instance_entry.entry);
  std::printf(",\"class_entry\":");
  ::objc3c::runtime::probe::PrintMethodCacheEntryBasic(run.class_entry.entry);
  std::printf(",\"category_entry\":");
  ::objc3c::runtime::probe::PrintMethodCacheEntryBasic(
      run.category_entry.entry);
  std::printf("}");
}

} // namespace method_binding_probe
} // namespace tooling
} // namespace objc3c
