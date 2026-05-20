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

inline bool I32NilReceiverResultPassed(
    const objc3_runtime_dispatch_i32_result &result) {
  return result.abi_version == OBJC3_RUNTIME_DISPATCH_I32_RESULT_ABI_VERSION &&
         result.result_size == sizeof(objc3_runtime_dispatch_i32_result) &&
         result.status_code == OBJC3_RUNTIME_DISPATCH_STATUS_NIL_RECEIVER &&
         result.return_kind ==
             OBJC3_RUNTIME_DISPATCH_RETURN_KIND_UNSUPPORTED &&
         result.value == 0 && TextEquals(result.diagnostic_code, "O3RT008") &&
         TextEquals(
             result.diagnostic_message,
             "runtime dispatch failed: nil receiver has no value dispatch result") &&
         TextEquals(result.result_contract, "typed-dispatch-value-result");
}

inline bool I32SuperDispatchResultPassed(
    const objc3_runtime_dispatch_i32_result &result) {
  return result.abi_version == OBJC3_RUNTIME_DISPATCH_I32_RESULT_ABI_VERSION &&
         result.result_size == sizeof(objc3_runtime_dispatch_i32_result) &&
         result.status_code == OBJC3_RUNTIME_DISPATCH_STATUS_OK &&
         result.return_kind == OBJC3_RUNTIME_DISPATCH_RETURN_KIND_I32 &&
         result.value == 7 && TextEquals(result.result_contract,
                                         "typed-dispatch-value-result");
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

inline bool TypedNilReceiverResultPassed(
    const objc3_runtime_dispatch_typed_result &result) {
  return result.abi_version ==
             OBJC3_RUNTIME_DISPATCH_TYPED_RESULT_ABI_VERSION &&
         result.result_size == sizeof(objc3_runtime_dispatch_typed_result) &&
         result.status_code == OBJC3_RUNTIME_DISPATCH_STATUS_NIL_RECEIVER &&
         result.return_kind ==
             OBJC3_RUNTIME_DISPATCH_RETURN_KIND_UNSUPPORTED &&
         TextEquals(result.return_kind_name, "unsupported") &&
         TypedStrictErrorValueFieldsAreZero(result) &&
         TextEquals(result.diagnostic_code, "O3RT008") &&
         TextEquals(
             result.diagnostic_message,
             "runtime dispatch failed: nil receiver has no value dispatch result") &&
         TextEquals(result.result_contract, "typed-dispatch-value-result");
}

inline bool TypedBoolCategoryResultPassed(
    const objc3_runtime_dispatch_typed_result &result) {
  return result.abi_version ==
             OBJC3_RUNTIME_DISPATCH_TYPED_RESULT_ABI_VERSION &&
         result.result_size == sizeof(objc3_runtime_dispatch_typed_result) &&
         result.status_code == OBJC3_RUNTIME_DISPATCH_STATUS_OK &&
         result.return_kind == OBJC3_RUNTIME_DISPATCH_RETURN_KIND_BOOL &&
         TextEquals(result.return_kind_name, "bool") &&
         result.i32_value == 0 && result.bool_value == 1 &&
         result.object_reference == 0 && result.class_reference == 0 &&
         result.selector_reference == 0 && result.protocol_reference == 0 &&
         TextEquals(result.result_contract, "typed-dispatch-value-result");
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
  const objc3_runtime_protocol_conformance_query_snapshot &leaf_worker =
      run.leaf_worker_query.query;
  const objc3_runtime_protocol_conformance_query_snapshot &missing_protocol =
      run.missing_protocol_query.query;
  const objc3_runtime_protocol_conformance_query_snapshot &missing_class =
      run.missing_class_query.query;
  const objc3_runtime_method_cache_state_snapshot &category_first =
      run.category_first_state.state;
  const objc3_runtime_method_cache_state_snapshot &category_second =
      run.category_second_state.state;
  const objc3_runtime_method_cache_state_snapshot &super_first =
      run.super_first_state.state;
  const objc3_runtime_method_cache_state_snapshot &super_second =
      run.super_second_state.state;
  const objc3_runtime_method_cache_state_snapshot &strict_state =
      run.method_state.state;
  const objc3_runtime_method_cache_entry_snapshot &category_entry =
      run.category_entry.entry;
  const objc3_runtime_method_cache_entry_snapshot &strict_entry =
      run.strict_error_entry.entry;

  return values.category_value == 13 && values.category_cached_value == 13 &&
         values.auxiliary_category_value == 29 &&
         values.category_bool_value == 1 &&
         TypedBoolCategoryResultPassed(values.category_bool_typed_result) &&
         values.class_value == 11 && values.super_inherited_value == 7 &&
         values.super_cached_inherited_value == 7 &&
         I32SuperDispatchResultPassed(values.super_inherited_i32_result) &&
         values.nil_receiver_value == 0 &&
         I32NilReceiverResultPassed(values.nil_receiver_i32_result) &&
         TypedNilReceiverResultPassed(values.nil_receiver_typed_result) &&
         values.protocol_strict_error ==
             values.protocol_strict_error_expected &&
         I32StrictErrorResultPassed(
             values.protocol_strict_error_i32_result) &&
         TypedStrictErrorResultPassed(
             values.protocol_strict_error_typed_result) &&
         graph.attached_category_count == 2 &&
         graph.category_attachment_generation > 0 &&
         graph.method_surface_generation > 0 &&
         graph.protocol_conformance_edge_count >= 2 &&
         widget.attached_category_count == 2 &&
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
         leaf_worker.class_found == 1 && leaf_worker.protocol_found == 1 &&
         leaf_worker.conforms == 1 && leaf_worker.malformed_metadata == 0 &&
         TextEquals(leaf_worker.matched_protocol_owner_identity,
                    "protocol:Worker") &&
         TextEquals(leaf_worker.matched_class_name, "Derived") &&
         TextEquals(leaf_worker.matched_class_owner_identity,
                    "class:Derived") &&
         leaf_worker.matched_protocol_depth >= 1 &&
         leaf_worker.matched_via_inherited_protocol == 1 &&
         leaf_worker.matched_from_superclass == 1 &&
         leaf_worker.matched_from_category == 0 &&
         missing_protocol.class_found == 1 &&
         missing_protocol.protocol_found == 0 &&
         missing_protocol.conforms == 0 &&
         missing_protocol.malformed_metadata == 0 &&
         TextEquals(missing_protocol.class_name, "Widget") &&
         TextEquals(missing_protocol.protocol_name, "MissingProtocol") &&
         missing_class.class_found == 0 && missing_class.protocol_found == 1 &&
         missing_class.conforms == 0 &&
         missing_class.malformed_metadata == 0 &&
         TextEquals(missing_class.class_name, "MissingConformanceClass") &&
         TextEquals(missing_class.protocol_name, "Worker") &&
         TextEquals(strict_state.last_selector, "ignoredValue") &&
         strict_state.last_dispatch_strict_error == 1 &&
         TextEquals(category_first.last_selector, "tracedValue") &&
         category_first.last_dispatch_used_cache == 0 &&
         category_first.last_dispatch_resolved_live_method == 1 &&
         category_first.last_category_probe_count == 1 &&
         TextEquals(category_second.last_selector, "tracedValue") &&
         category_second.last_dispatch_used_cache == 1 &&
         category_second.last_dispatch_resolved_live_method == 1 &&
         category_second.last_category_probe_count == 1 &&
         TextEquals(super_first.last_selector, "inheritedValue") &&
         super_first.last_dispatch_used_cache == 0 &&
         super_first.last_dispatch_resolved_live_method == 1 &&
         super_first.last_category_probe_count == 0 &&
         TextEquals(super_first.last_resolved_class_name, "Base") &&
         TextEquals(super_first.last_resolved_owner_identity,
                    "implementation:Base::instance_method:inheritedValue") &&
         TextEquals(super_second.last_selector, "inheritedValue") &&
         super_second.last_dispatch_used_cache == 1 &&
         super_second.last_dispatch_resolved_live_method == 1 &&
         super_second.last_category_probe_count == 0 &&
         TextEquals(super_second.last_resolved_class_name, "Base") &&
         TextEquals(super_second.last_resolved_owner_identity,
                    "implementation:Base::instance_method:inheritedValue") &&
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
         category_entry.category_probe_count == 1 && strict_entry.found == 1 &&
         strict_entry.resolved == 0 &&
         TextEquals(strict_entry.selector, "ignoredValue") &&
         strict_entry.cache_category_attachment_generation ==
             graph.category_attachment_generation &&
         strict_entry.cache_method_surface_generation ==
             graph.method_surface_generation &&
         strict_entry.category_probe_count >= 2 &&
         strict_entry.protocol_probe_count >= 1;
}

}  // namespace objc3c::runtime::probe::category_attachment_protocol_runtime
