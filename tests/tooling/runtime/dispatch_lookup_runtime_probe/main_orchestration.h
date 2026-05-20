#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_DISPATCH_LOOKUP_RUNTIME_PROBE_MAIN_ORCHESTRATION_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_DISPATCH_LOOKUP_RUNTIME_PROBE_MAIN_ORCHESTRATION_H_

#include "orchestration.h"
#include "probe_state.h"
#include "report_helpers.h"

#include <cstdio>

namespace objc3c::runtime::probe::dispatch_lookup_runtime_probe {

inline bool StatusIsMissingClassGraph(int status) {
  return status == OBJC3_RUNTIME_DISPATCH_STATUS_MISSING_CLASS_GRAPH;
}

inline bool ValidateDispatchLookupProbeResult(
    const DispatchLookupProbeResult &result) {
  if (result.register_status != OBJC3_RUNTIME_REGISTRATION_STATUS_OK ||
      !result.selectors.lookup_null_is_null ||
      !result.selectors.copy_selector_reused ||
      !result.selectors.copy_selector_spelling_matches ||
      result.selectors.copy_selector_stable_id == 0 ||
      result.selectors.gamma_selector_stable_id == 0) {
    std::fprintf(stderr, "dispatch lookup selector invariant failed\n");
    return false;
  }
  if (result.dispatch.dispatch_result !=
          result.dispatch.expected_dispatch_result ||
      result.dispatch.nil_dispatch_result != 0) {
    std::fprintf(stderr, "dispatch lookup base dispatch invariant failed\n");
    return false;
  }
  const FromClassDispatchCapture &from_class = result.from_class;
  if (from_class.typed_super_status != OBJC3_RUNTIME_DISPATCH_STATUS_OK ||
      from_class.typed_super_value != kExpectedRootValue ||
      from_class.typed_super_return_kind !=
          OBJC3_RUNTIME_DISPATCH_RETURN_KIND_I32 ||
      from_class.i32_super_status != OBJC3_RUNTIME_DISPATCH_STATUS_OK ||
      from_class.i32_super_value != kExpectedRootValue ||
      from_class.i32_super_return_kind !=
          OBJC3_RUNTIME_DISPATCH_RETURN_KIND_I32 ||
      from_class.typed_self_status != OBJC3_RUNTIME_DISPATCH_STATUS_OK ||
      from_class.typed_self_value != kExpectedWidgetValue ||
      from_class.i32_self_status != OBJC3_RUNTIME_DISPATCH_STATUS_OK ||
      from_class.i32_self_value != kExpectedWidgetValue) {
    std::fprintf(stderr, "dispatch lookup from-class positive invariant failed\n");
    return false;
  }
  if (!StatusIsMissingClassGraph(from_class.null_lookup_start_status) ||
      !StatusIsMissingClassGraph(from_class.empty_lookup_start_status) ||
      !StatusIsMissingClassGraph(from_class.missing_lookup_start_status) ||
      !StatusIsMissingClassGraph(from_class.unreachable_lookup_start_status)) {
    std::fprintf(stderr, "dispatch lookup from-class negative invariant failed\n");
    return false;
  }
  if (result.registration.snapshot_status !=
          OBJC3_RUNTIME_REGISTRATION_STATUS_OK ||
      result.registration.snapshot.registered_image_count == 0 ||
      result.reset_lookup.copy_after_reset_stable_id == 0) {
    std::fprintf(stderr, "dispatch lookup registration/reset invariant failed\n");
    return false;
  }
  return true;
}

inline int RunProbeMain() {
  const DispatchLookupProbeResult result = CaptureDispatchLookupProbeResult();
  PrintDispatchLookupProbeReport(result);
  return ValidateDispatchLookupProbeResult(result) ? 0 : 1;
}

}  // namespace objc3c::runtime::probe::dispatch_lookup_runtime_probe

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_DISPATCH_LOOKUP_RUNTIME_PROBE_MAIN_ORCHESTRATION_H_
