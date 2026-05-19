#pragma once

#include "probe_state.h"
#include "support/runtime_snapshot_json.h"

#include <cstdio>

namespace objc3c::runtime::probe::category_attachment_protocol_runtime {

inline void PrintCategoryAttachmentProtocolRuntimeReport(
    const CategoryAttachmentProtocolProbeRun &run) {
  const CategoryAttachmentProtocolValues &values = run.values;

  std::printf("{");
  std::printf("\"category_value\":%d,", values.category_value);
  std::printf("\"category_cached_value\":%d,",
              values.category_cached_value);
  std::printf("\"class_value\":%d,", values.class_value);
  std::printf("\"protocol_strict_error\":%d,",
              values.protocol_strict_error);
  std::printf("\"protocol_strict_error_expected\":%d,",
              values.protocol_strict_error_expected);
  std::printf("\"graph_state\":");
  ::objc3c::runtime::probe::PrintGraphStateProtocolCategory(
      run.graph_state.state);
  std::printf(",\"widget_entry\":");
  ::objc3c::runtime::probe::PrintRealizedEntryProtocolCategory(
      run.widget_entry.entry);
  std::printf(",\"base_entry\":");
  ::objc3c::runtime::probe::PrintRealizedEntryProtocolCategory(
      run.base_entry.entry);
  std::printf(",\"worker_query\":");
  ::objc3c::runtime::probe::PrintConformanceQueryProtocolCategory(
      run.worker_query.query);
  std::printf(",\"tracer_query\":");
  ::objc3c::runtime::probe::PrintConformanceQueryProtocolCategory(
      run.tracer_query.query);
  std::printf(",\"base_worker_query\":");
  ::objc3c::runtime::probe::PrintConformanceQueryProtocolCategory(
      run.base_worker_query.query);
  std::printf(",\"derived_worker_query\":");
  ::objc3c::runtime::probe::PrintConformanceQueryProtocolInheritance(
      run.derived_worker_query.query);
  std::printf(",\"category_first_state\":");
  ::objc3c::runtime::probe::PrintMethodCacheStateCategoryAttachment(
      run.category_first_state.state);
  std::printf(",\"category_second_state\":");
  ::objc3c::runtime::probe::PrintMethodCacheStateCategoryAttachment(
      run.category_second_state.state);
  std::printf(",\"method_state\":");
  ::objc3c::runtime::probe::PrintMethodCacheStateCategoryAttachment(
      run.method_state.state);
  std::printf(",\"category_entry\":");
  ::objc3c::runtime::probe::PrintMethodCacheEntryWithProbeCounts(
      run.category_entry.entry);
  std::printf(",\"strict_error_entry\":");
  ::objc3c::runtime::probe::PrintMethodCacheEntryWithProbeCounts(
      run.strict_error_entry.entry);
  std::printf("}\n");
}

}  // namespace objc3c::runtime::probe::category_attachment_protocol_runtime
