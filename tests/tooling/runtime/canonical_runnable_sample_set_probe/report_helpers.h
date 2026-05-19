#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_CANONICAL_RUNNABLE_SAMPLE_SET_PROBE_REPORT_HELPERS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_CANONICAL_RUNNABLE_SAMPLE_SET_PROBE_REPORT_HELPERS_H_

#include "sample_fixture_definitions.h"
#include "support/runtime_snapshot_json.h"

#include <cstdio>

namespace objc3c::runtime::probe::canonical_runnable_sample_set {

inline void PrintProbeReport(const ProbeResult &result) {
  const RunnableSampleExecution &execution = result.execution;
  const RunnableSampleAssertions &assertions = result.assertions;

  std::printf("{");
  std::printf("\"widget_entry\":");
  ::objc3c::runtime::probe::PrintRealizedClassEntryCanonicalSummary(
      result.fixture.widget_entry.entry);
  std::printf(",\"init_value\":%d", execution.init_value);
  std::printf(",\"traced_value\":%d", execution.traced_value);
  std::printf(",\"inherited_value\":%d", execution.inherited_value);
  std::printf(",\"class_value\":%d", execution.class_value);
  std::printf(",\"shared_value\":%d", execution.shared_value);
  std::printf(",\"count_value\":%d", execution.count_value);
  std::printf(",\"enabled_value\":%d", execution.enabled_value);
  std::printf(",\"current_value\":%d", execution.current_value);
  std::printf(",\"token_value\":%d", execution.token_value);
  std::printf(",\"worker_query\":");
  ::objc3c::runtime::probe::PrintConformanceQueryCanonicalSummary(
      assertions.worker_query.query);
  std::printf(",\"tracer_query\":");
  ::objc3c::runtime::probe::PrintConformanceQueryCanonicalSummary(
      assertions.tracer_query.query);
  std::printf(",\"count_property\":");
  ::objc3c::runtime::probe::PrintPropertyEntryCanonicalSummary(
      assertions.count_property.entry);
  std::printf(",\"value_property\":");
  ::objc3c::runtime::probe::PrintPropertyEntryCanonicalSummary(
      assertions.value_property.entry);
  std::printf(",\"token_property\":");
  ::objc3c::runtime::probe::PrintPropertyEntryCanonicalSummary(
      assertions.token_property.entry);
  std::printf("}\n");
}

}  // namespace objc3c::runtime::probe::canonical_runnable_sample_set

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_CANONICAL_RUNNABLE_SAMPLE_SET_PROBE_REPORT_HELPERS_H_
