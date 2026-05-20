#pragma once

#include "category_protocol_fixture_definitions.h"
#include "probe_state.h"
#include "support/dispatch_expectations.h"

#include <cstring>

namespace objc3c::runtime::probe::category_attachment_protocol_runtime {

inline int ExpectedProtocolStrictDispatchErrorValue(
    const RuntimeDispatch &dispatch) {
  return ::objc3c::runtime::probe::ExpectedStrictDispatchErrorValue(
      dispatch.receiver, dispatch.selector, 0, 0, 0, 0);
}

inline void CaptureProtocolRuntimeAssertions(
    CategoryAttachmentProtocolProbeRun &run) {
  const RuntimeDispatch strict_error_dispatch{
      static_cast<int>(run.widget_entry.entry.instance_receiver_identity),
      kProtocolStrictErrorSelector};
  run.values.protocol_strict_error_expected =
      ExpectedProtocolStrictDispatchErrorValue(strict_error_dispatch);
}

inline bool TextEquals(const char *actual, const char *expected) {
  return actual != nullptr && expected != nullptr &&
         std::strcmp(actual, expected) == 0;
}

inline bool TextPresent(const char *actual) {
  return actual != nullptr && actual[0] != '\0';
}

inline bool I32StrictErrorResultPassed(
    const objc3_runtime_dispatch_i32_result &result) {
  return result.abi_version == OBJC3_RUNTIME_DISPATCH_I32_RESULT_ABI_VERSION &&
         result.result_size == sizeof(objc3_runtime_dispatch_i32_result) &&
         result.status_code == OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_SELECTOR &&
         result.return_kind ==
             OBJC3_RUNTIME_DISPATCH_RETURN_KIND_UNSUPPORTED &&
         result.value == 0 && TextEquals(result.diagnostic_code, "O3RT001") &&
         TextEquals(result.diagnostic_message,
                    "runtime dispatch failed: unknown selector") &&
         TextEquals(result.result_contract,
                    "typed-dispatch-strict-error-result");
}

inline bool TypedStrictErrorValueFieldsAreZero(
    const objc3_runtime_dispatch_typed_result &result) {
  return result.i32_value == 0 && result.bool_value == 0 &&
         result.object_reference == 0 && result.class_reference == 0 &&
         result.selector_reference == 0 && result.protocol_reference == 0;
}

inline bool TypedStrictErrorResultPassed(
    const objc3_runtime_dispatch_typed_result &result) {
  return result.abi_version ==
             OBJC3_RUNTIME_DISPATCH_TYPED_RESULT_ABI_VERSION &&
         result.result_size == sizeof(objc3_runtime_dispatch_typed_result) &&
         result.status_code == OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_SELECTOR &&
         result.return_kind ==
             OBJC3_RUNTIME_DISPATCH_RETURN_KIND_UNSUPPORTED &&
         TextEquals(result.return_kind_name, "unsupported") &&
         TypedStrictErrorValueFieldsAreZero(result) &&
         TextEquals(result.diagnostic_code, "O3RT001") &&
         TextEquals(result.diagnostic_message,
                    "runtime dispatch failed: unknown selector") &&
         TextEquals(result.result_contract,
                    "typed-dispatch-strict-error-result");
}

inline bool CategoryAttachmentProtocolRuntimeProbePassed(
    const CategoryAttachmentProtocolProbeRun &run) {
  const CategoryAttachmentProtocolValues &values = run.values;
  const objc3_runtime_realized_class_graph_state_snapshot &graph =
      run.graph_state.state;
  const objc3_runtime_realized_class_entry_snapshot &widget =
      run.widget_entry.entry;
  const objc3_runtime_realized_class_entry_snapshot &base =
      run.base_entry.entry;
  const objc3_runtime_protocol_conformance_query_snapshot &worker =
      run.worker_query.query;
  const objc3_runtime_protocol_conformance_query_snapshot &tracer =
      run.tracer_query.query;
  const objc3_runtime_protocol_conformance_query_snapshot &base_worker =
      run.base_worker_query.query;
  const objc3_runtime_protocol_conformance_query_snapshot &derived_worker =
      run.derived_worker_query.query;
  const objc3_runtime_method_cache_state_snapshot &category_first =
      run.category_first_state.state;
  const objc3_runtime_method_cache_state_snapshot &category_second =
      run.category_second_state.state;
  const objc3_runtime_method_cache_state_snapshot &strict_state =
      run.method_state.state;
  const objc3_runtime_method_cache_entry_snapshot &category_entry =
      run.category_entry.entry;
  const objc3_runtime_method_cache_entry_snapshot &strict_entry =
      run.strict_error_entry.entry;

  return values.category_value == 13 && values.category_cached_value == 13 &&
         values.class_value == 11 &&
         values.protocol_strict_error ==
             values.protocol_strict_error_expected &&
         I32StrictErrorResultPassed(
             values.protocol_strict_error_i32_result) &&
         TypedStrictErrorResultPassed(
             values.protocol_strict_error_typed_result) &&
         graph.attached_category_count == 1 &&
         graph.category_attachment_generation > 0 &&
         graph.method_surface_generation > 0 &&
         graph.protocol_conformance_edge_count >= 2 &&
         widget.attached_category_count == 1 &&
         widget.direct_protocol_count == 1 &&
         widget.attached_protocol_count == 1 && base.found == 1 &&
         base.attached_protocol_count == 0 && worker.conforms == 1 &&
         worker.malformed_metadata == 0 &&
         TextPresent(worker.matched_protocol_owner_identity) &&
         worker.matched_protocol_depth == 0 &&
         worker.matched_from_category == 0 && tracer.conforms == 1 &&
         tracer.malformed_metadata == 0 &&
         tracer.visited_protocol_count >= 2 &&
         TextEquals(tracer.matched_attachment_owner_identity,
                    "category:Widget(Tracing)") &&
         base_worker.class_found == 1 && base_worker.protocol_found == 1 &&
         base_worker.conforms == 0 && base_worker.malformed_metadata == 0 &&
         derived_worker.class_found == 1 &&
         derived_worker.protocol_found == 1 && derived_worker.conforms == 1 &&
         derived_worker.malformed_metadata == 0 &&
         TextEquals(derived_worker.matched_protocol_owner_identity,
                    "protocol:Worker") &&
         TextEquals(derived_worker.matched_class_name, "Derived") &&
         derived_worker.matched_protocol_depth >= 1 &&
         derived_worker.matched_via_inherited_protocol == 1 &&
         derived_worker.matched_from_category == 0 &&
         TextEquals(strict_state.last_selector, "ignoredValue") &&
         strict_state.last_dispatch_strict_error == 1 &&
         TextEquals(category_first.last_selector, "tracedValue") &&
         category_first.last_dispatch_used_cache == 0 &&
         category_first.last_dispatch_resolved_live_method == 1 &&
         category_first.last_category_probe_count >= 1 &&
         TextEquals(category_second.last_selector, "tracedValue") &&
         category_second.last_dispatch_used_cache == 1 &&
         category_second.last_dispatch_resolved_live_method == 1 &&
         category_entry.found == 1 && category_entry.resolved == 1 &&
         TextEquals(category_entry.selector, "tracedValue") &&
         TextEquals(category_entry.resolved_class_name, "Widget") &&
         TextEquals(category_entry.resolved_owner_identity,
                    "implementation:Widget(Tracing)::instance_method:"
                    "tracedValue") &&
         category_entry.cache_category_attachment_generation ==
             graph.category_attachment_generation &&
         category_entry.cache_method_surface_generation ==
             graph.method_surface_generation &&
         category_entry.category_probe_count >= 1 && strict_entry.found == 1 &&
         strict_entry.resolved == 0 &&
         TextEquals(strict_entry.selector, "ignoredValue") &&
         strict_entry.cache_category_attachment_generation ==
             graph.category_attachment_generation &&
         strict_entry.cache_method_surface_generation ==
             graph.method_surface_generation &&
         strict_entry.category_probe_count >= 1 &&
         strict_entry.protocol_probe_count >= 1;
}

}  // namespace objc3c::runtime::probe::category_attachment_protocol_runtime
