#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_INSTANCE_ALLOCATION_RUNTIME_PROBE_REPORT_HELPERS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_INSTANCE_ALLOCATION_RUNTIME_PROBE_REPORT_HELPERS_H_

#include "probe_state.h"
#include "support/runtime_snapshot_json.h"

#include <cstdio>

namespace objc3c::runtime::probe::instance_allocation_runtime {

inline void PrintInstanceAllocationRuntimeReport(const ProbeRun &run) {
  const AllocationFixture &fixture = run.fixture;
  const AllocationMutationResults &mutations = run.mutations;
  const AllocationInvariantSnapshots &invariants = run.invariants;

  std::printf("{");
  std::printf("\"first_alloc\":%d,", fixture.first_alloc);
  std::printf("\"second_alloc\":%d,", fixture.second_alloc);
  std::printf("\"set_count_first\":%d,", mutations.set_count_first);
  std::printf("\"count_value_first\":%d,", mutations.count_value_first);
  std::printf("\"count_value_second_before\":%d,",
              mutations.count_value_second_before);
  std::printf("\"set_enabled_first\":%d,", mutations.set_enabled_first);
  std::printf("\"enabled_value_first\":%d,", mutations.enabled_value_first);
  std::printf("\"enabled_value_second\":%d,", mutations.enabled_value_second);
  std::printf("\"set_value_first\":%d,", mutations.set_value_first);
  std::printf("\"value_result_first\":%d,", mutations.value_result_first);
  std::printf("\"value_result_second_before\":%d,",
              mutations.value_result_second_before);
  std::printf("\"set_count_second\":%d,", mutations.set_count_second);
  std::printf("\"count_value_first_after_second\":%d,",
              mutations.count_value_first_after_second);
  std::printf("\"count_value_second_after\":%d,",
              mutations.count_value_second_after);
  std::printf("\"set_value_second\":%d,", mutations.set_value_second);
  std::printf("\"value_result_first_after_second\":%d,",
              mutations.value_result_first_after_second);
  std::printf("\"value_result_second_after\":%d,",
              mutations.value_result_second_after);
  std::printf("\"registration_state\":");
  ::objc3c::runtime::probe::PrintRegistrationStateBasic(
      run.registration_state.state);
  std::printf(",\"selector_table_state\":");
  ::objc3c::runtime::probe::PrintSelectorTableStateBasic(
      run.selector_table_state.state);
  std::printf(",\"graph_state\":");
  ::objc3c::runtime::probe::PrintRealizedGraphStateAllocation(
      invariants.graph_state.state);
  std::printf(",\"widget_entry\":");
  ::objc3c::runtime::probe::PrintRealizedClassEntryAllocation(
      invariants.widget_entry.entry);
  std::printf(",\"count_entry\":");
  ::objc3c::runtime::probe::PrintMethodCacheEntryBasic(
      invariants.count_entry.entry);
  std::printf(",\"set_count_entry\":");
  ::objc3c::runtime::probe::PrintMethodCacheEntryBasic(
      invariants.set_count_entry.entry);
  std::printf("}");
}

}  // namespace objc3c::runtime::probe::instance_allocation_runtime

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_INSTANCE_ALLOCATION_RUNTIME_PROBE_REPORT_HELPERS_H_
