#pragma once

#include "probe_state.h"
#include "support/json_probe_writer.h"
#include "support/runtime_snapshot_json.h"

#include <cstdio>

namespace objc3c::runtime::probe::category_attachment_protocol_runtime {

inline void PrintI32DispatchResult(
    const objc3_runtime_dispatch_i32_result &result) {
  using ::objc3c::runtime::probe::PrintIntField;
  using ::objc3c::runtime::probe::PrintStringField;

  std::printf("{");
  PrintIntField("abi_version", static_cast<int>(result.abi_version));
  PrintIntField("result_size", static_cast<int>(result.result_size));
  PrintIntField("status_code", static_cast<int>(result.status_code));
  PrintIntField("return_kind", static_cast<int>(result.return_kind));
  PrintIntField("value", result.value);
  PrintStringField("diagnostic_code", result.diagnostic_code);
  PrintStringField("diagnostic_message", result.diagnostic_message);
  PrintStringField("result_contract", result.result_contract);
  PrintStringField("diagnostic_owner_model", result.diagnostic_owner_model);
  PrintStringField("fail_closed_ownership_model",
                   result.fail_closed_ownership_model, false);
  std::printf("}");
}

inline void PrintTypedDispatchResult(
    const objc3_runtime_dispatch_typed_result &result) {
  using ::objc3c::runtime::probe::PrintIntField;
  using ::objc3c::runtime::probe::PrintStringField;

  std::printf("{");
  PrintIntField("abi_version", static_cast<int>(result.abi_version));
  PrintIntField("result_size", static_cast<int>(result.result_size));
  PrintIntField("status_code", static_cast<int>(result.status_code));
  PrintIntField("return_kind", static_cast<int>(result.return_kind));
  PrintStringField("return_kind_name", result.return_kind_name);
  PrintIntField("i32_value", result.i32_value);
  PrintIntField("bool_value", result.bool_value);
  PrintIntField("object_reference", result.object_reference);
  PrintIntField("class_reference", result.class_reference);
  PrintIntField("selector_reference", result.selector_reference);
  PrintIntField("protocol_reference", result.protocol_reference);
  PrintStringField("diagnostic_code", result.diagnostic_code);
  PrintStringField("diagnostic_message", result.diagnostic_message);
  PrintStringField("result_contract", result.result_contract);
  PrintStringField("diagnostic_owner_model", result.diagnostic_owner_model);
  PrintStringField("fail_closed_ownership_model",
                   result.fail_closed_ownership_model, false);
  std::printf("}");
}

inline void PrintCategoryAttachmentProtocolRuntimeReport(
    const CategoryAttachmentProtocolProbeRun &run) {
  const CategoryAttachmentProtocolValues &values = run.values;

  std::printf("{");
  std::printf("\"report_contract\":\"objc3c.runtime.category-attachment-protocol.report.v2\",");
  std::printf("\"source_path\":\"tests/tooling/runtime/category_attachment_protocol_runtime_probe.cpp\",");
  std::printf("\"category_value\":%d,", values.category_value);
  std::printf("\"category_cached_value\":%d,",
              values.category_cached_value);
  std::printf("\"auxiliary_category_value\":%d,",
              values.auxiliary_category_value);
  std::printf("\"category_bool_value\":%d,", values.category_bool_value);
  std::printf("\"category_bool_typed_result\":");
  PrintTypedDispatchResult(values.category_bool_typed_result);
  std::printf(",");
  std::printf("\"class_value\":%d,", values.class_value);
  std::printf("\"super_inherited_value\":%d,",
              values.super_inherited_value);
  std::printf("\"nil_receiver_value\":%d,", values.nil_receiver_value);
  std::printf("\"protocol_strict_error\":%d,",
              values.protocol_strict_error);
  std::printf("\"protocol_strict_error_expected\":%d,",
              values.protocol_strict_error_expected);
  std::printf("\"protocol_strict_error_i32_result\":");
  PrintI32DispatchResult(values.protocol_strict_error_i32_result);
  std::printf(",");
  std::printf("\"protocol_strict_error_typed_result\":");
  PrintTypedDispatchResult(values.protocol_strict_error_typed_result);
  std::printf(",");
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
  std::printf(",\"missing_protocol_query\":");
  ::objc3c::runtime::probe::PrintConformanceQueryProtocolCategory(
      run.missing_protocol_query.query);
  std::printf(",\"missing_class_query\":");
  ::objc3c::runtime::probe::PrintConformanceQueryProtocolCategory(
      run.missing_class_query.query);
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
