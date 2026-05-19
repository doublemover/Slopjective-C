#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_MEMORY_MANAGEMENT_API_PROBE_MAIN_ORCHESTRATION_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_MEMORY_MANAGEMENT_API_PROBE_MAIN_ORCHESTRATION_H_

#include "api_call_scenarios.h"
#include "report_helpers.h"

namespace objc3c::runtime::probe::runtime_memory_management_api {

inline int RunProbeMain() {
  MemoryManagementProbeRun run;
  CaptureMemoryManagementApiProbe(run);
  PrintMemoryManagementApiProbeReport(run);
  return 0;
}

}  // namespace objc3c::runtime::probe::runtime_memory_management_api

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_MEMORY_MANAGEMENT_API_PROBE_MAIN_ORCHESTRATION_H_
