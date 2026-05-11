#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_SYNTHESIZED_ACCESSOR_PROBE_REPORT_HELPERS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_SYNTHESIZED_ACCESSOR_PROBE_REPORT_HELPERS_H_

#include "probe_result.h"
#include "support/runtime_snapshot_json.h"

#include <cstdio>

namespace objc3c::runtime::probe::synthesized_accessor {

inline void PrintSynthesizedAccessorReport(const ProbeResult &result) {
  const AccessorActions &actions = result.actions;
  const AccessorSetup &setup = result.setup;
  const AccessorAssertions &assertions = result.assertions;

  std::printf("{");
  std::printf("\"widget_instance\":%d,", actions.widget_instance);
  std::printf("\"set_count_result\":%d,", actions.set_count_result);
  std::printf("\"count_value\":%d,", actions.count_value);
  std::printf("\"set_enabled_result\":%d,", actions.set_enabled_result);
  std::printf("\"enabled_value\":%d,", actions.enabled_value);
  std::printf("\"set_value_result\":%d,", actions.set_value_result);
  std::printf("\"value_result\":%d,", actions.value_result);
  std::printf("\"registration_state\":");
  ::objc3c::runtime::probe::PrintRegistrationStateBasic(
      setup.registration_state.state);
  std::printf(",\"selector_table_state\":");
  ::objc3c::runtime::probe::PrintSelectorTableStateBasic(
      setup.selector_table_state.state);
  std::printf(",\"count_entry\":");
  ::objc3c::runtime::probe::PrintMethodCacheEntryBasic(
      assertions.count_entry.entry);
  std::printf(",\"set_count_entry\":");
  ::objc3c::runtime::probe::PrintMethodCacheEntryBasic(
      assertions.set_count_entry.entry);
  std::printf(",\"enabled_entry\":");
  ::objc3c::runtime::probe::PrintMethodCacheEntryBasic(
      assertions.enabled_entry.entry);
  std::printf(",\"set_enabled_entry\":");
  ::objc3c::runtime::probe::PrintMethodCacheEntryBasic(
      assertions.set_enabled_entry.entry);
  std::printf(",\"value_entry\":");
  ::objc3c::runtime::probe::PrintMethodCacheEntryBasic(
      assertions.value_entry.entry);
  std::printf(",\"set_value_entry\":");
  ::objc3c::runtime::probe::PrintMethodCacheEntryBasic(
      assertions.set_value_entry.entry);
  std::printf("}");
}

}  // namespace objc3c::runtime::probe::synthesized_accessor

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_SYNTHESIZED_ACCESSOR_PROBE_REPORT_HELPERS_H_
