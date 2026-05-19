#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_PROPERTY_IVAR_EXECUTION_MATRIX_PROBE_PROBE_REPORT_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_PROPERTY_IVAR_EXECUTION_MATRIX_PROBE_PROBE_REPORT_H_

#include "probe_result.h"
#include "support/runtime_snapshot_json.h"

#include <cstdio>

namespace objc3c::runtime::probe::property_ivar_execution_matrix {

inline void PrintProbeReport(const ProbeResult &result) {
  const PropertyIvarExecutionCases &execution = result.execution;
  const PropertyIvarExecutionAssertions &assertions = result.assertions;

  std::printf("{");
  std::printf("\"widget_instance\":%d,", result.fixture.widget_instance);
  std::printf("\"set_base_count_result\":%d,",
              execution.set_base_count_result);
  std::printf("\"base_count_value\":%d,", execution.base_count_value);
  std::printf("\"set_count_result\":%d,", execution.set_count_result);
  std::printf("\"count_value\":%d,", execution.count_value);
  std::printf("\"set_enabled_result\":%d,", execution.set_enabled_result);
  std::printf("\"enabled_value\":%d,", execution.enabled_value);
  std::printf("\"set_value_result\":%d,", execution.set_value_result);
  std::printf("\"value_result\":%d,", execution.value_result);
  std::printf("\"set_token_result\":%d,", execution.set_token_result);
  std::printf("\"token_value\":%d,", execution.token_value);
  std::printf("\"widget_entry\":");
  ::objc3c::runtime::probe::PrintRealizedClassEntryPropertySummary(
      result.fixture.widget_entry.entry);
  std::printf(",\"registry_state\":");
  ::objc3c::runtime::probe::PrintPropertyRegistryStateFull(
      assertions.registry_state.state);
  std::printf(",\"base_count_property\":");
  ::objc3c::runtime::probe::PrintPropertyEntryFull(
      assertions.base_count_property.entry);
  std::printf(",\"count_property\":");
  ::objc3c::runtime::probe::PrintPropertyEntryFull(
      assertions.count_property.entry);
  std::printf(",\"enabled_property\":");
  ::objc3c::runtime::probe::PrintPropertyEntryFull(
      assertions.enabled_property.entry);
  std::printf(",\"value_property\":");
  ::objc3c::runtime::probe::PrintPropertyEntryFull(
      assertions.value_property.entry);
  std::printf(",\"token_property\":");
  ::objc3c::runtime::probe::PrintPropertyEntryFull(
      assertions.token_property.entry);
  std::printf(",\"base_count_method\":");
  ::objc3c::runtime::probe::PrintMethodCacheEntryBasic(
      assertions.base_count_method.entry);
  std::printf(",\"count_method\":");
  ::objc3c::runtime::probe::PrintMethodCacheEntryBasic(
      assertions.count_method.entry);
  std::printf(",\"enabled_method\":");
  ::objc3c::runtime::probe::PrintMethodCacheEntryBasic(
      assertions.enabled_method.entry);
  std::printf(",\"value_method\":");
  ::objc3c::runtime::probe::PrintMethodCacheEntryBasic(
      assertions.value_method.entry);
  std::printf(",\"set_token_method\":");
  ::objc3c::runtime::probe::PrintMethodCacheEntryBasic(
      assertions.set_token_method.entry);
  std::printf(",\"token_method\":");
  ::objc3c::runtime::probe::PrintMethodCacheEntryBasic(
      assertions.token_method.entry);
  std::printf(",\"set_base_count_dispatch\":");
  ::objc3c::runtime::probe::PrintDispatchStatePropertyExecution(
      execution.set_base_count_dispatch.state);
  std::printf(",\"base_count_dispatch\":");
  ::objc3c::runtime::probe::PrintDispatchStatePropertyExecution(
      execution.base_count_dispatch.state);
  std::printf(",\"set_count_dispatch\":");
  ::objc3c::runtime::probe::PrintDispatchStatePropertyExecution(
      execution.set_count_dispatch.state);
  std::printf(",\"count_dispatch\":");
  ::objc3c::runtime::probe::PrintDispatchStatePropertyExecution(
      execution.count_dispatch.state);
  std::printf(",\"set_enabled_dispatch\":");
  ::objc3c::runtime::probe::PrintDispatchStatePropertyExecution(
      execution.set_enabled_dispatch.state);
  std::printf(",\"enabled_dispatch\":");
  ::objc3c::runtime::probe::PrintDispatchStatePropertyExecution(
      execution.enabled_dispatch.state);
  std::printf(",\"set_value_dispatch\":");
  ::objc3c::runtime::probe::PrintDispatchStatePropertyExecution(
      execution.set_value_dispatch.state);
  std::printf(",\"value_dispatch\":");
  ::objc3c::runtime::probe::PrintDispatchStatePropertyExecution(
      execution.value_dispatch.state);
  std::printf(",\"set_token_dispatch\":");
  ::objc3c::runtime::probe::PrintDispatchStatePropertyExecution(
      execution.set_token_dispatch.state);
  std::printf(",\"token_dispatch\":");
  ::objc3c::runtime::probe::PrintDispatchStatePropertyExecution(
      execution.token_dispatch.state);
  std::printf("}");
}

}  // namespace objc3c::runtime::probe::property_ivar_execution_matrix

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_PROPERTY_IVAR_EXECUTION_MATRIX_PROBE_PROBE_REPORT_H_
