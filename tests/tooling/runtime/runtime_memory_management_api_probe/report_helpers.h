#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_MEMORY_MANAGEMENT_API_PROBE_REPORT_HELPERS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_MEMORY_MANAGEMENT_API_PROBE_REPORT_HELPERS_H_

#include "probe_state.h"
#include "support/runtime_snapshot_json.h"

#include <cstdio>

namespace objc3c::runtime::probe::runtime_memory_management_api {

inline void PrintMemoryManagementApiProbeReport(
    const MemoryManagementProbeRun &run) {
  const MemoryManagementOperationResults &operations = run.operations;

  std::printf("{");
  std::printf("\"parent\":%d,", operations.parent);
  std::printf("\"child\":%d,", operations.child);
  std::printf("\"strong_set_result\":%d,", operations.strong_set_result);
  std::printf("\"weak_set_result\":%d,", operations.weak_set_result);
  std::printf("\"retain_result\":%d,", operations.retain_result);
  std::printf("\"autorelease_result\":%d,", operations.autorelease_result);
  std::printf("\"release_after_helper_result\":%d,",
              operations.release_after_helper_result);
  std::printf("\"release_local_result\":%d,", operations.release_local_result);
  std::printf("\"strong_before_clear\":%d,", operations.strong_before_clear);
  std::printf("\"weak_before_clear\":%d,", operations.weak_before_clear);
  std::printf("\"clear_strong_result\":%d,", operations.clear_strong_result);
  std::printf("\"strong_after_clear\":%d,", operations.strong_after_clear);
  std::printf("\"weak_after_clear\":%d,", operations.weak_after_clear);
  std::printf("\"parent_release_result\":%d,",
              operations.parent_release_result);
  std::printf("\"registration_state\":");
  ::objc3c::runtime::probe::PrintRegistrationStateCountsOnly(
      run.registration_state);
  std::printf(",\"graph_after_alloc\":");
  ::objc3c::runtime::probe::PrintAllocationGraph(
      run.graphs.after_alloc.snapshot);
  std::printf(",\"graph_after_helper_release\":");
  ::objc3c::runtime::probe::PrintAllocationGraph(
      run.graphs.after_helper_release.snapshot);
  std::printf(",\"graph_after_clear\":");
  ::objc3c::runtime::probe::PrintAllocationGraph(
      run.graphs.after_clear.snapshot);
  std::printf(",\"graph_after_parent_release\":");
  ::objc3c::runtime::probe::PrintAllocationGraph(
      run.graphs.after_parent_release.snapshot);
  std::printf("}");
}

}  // namespace objc3c::runtime::probe::runtime_memory_management_api

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_MEMORY_MANAGEMENT_API_PROBE_REPORT_HELPERS_H_
