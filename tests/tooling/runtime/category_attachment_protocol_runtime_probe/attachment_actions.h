#pragma once

#include "category_protocol_fixture_definitions.h"
#include "probe_state.h"

namespace objc3c::runtime::probe::category_attachment_protocol_runtime {

inline int DispatchRuntimeValue(const RuntimeDispatch &dispatch) {
  return objc3_runtime_dispatch_i32(dispatch.receiver, dispatch.selector, 0, 0,
                                    0, 0);
}

inline int DispatchRuntimeValueChecked(const RuntimeDispatch &dispatch) {
  const objc3_runtime_dispatch_i32_result result =
      objc3_runtime_dispatch_i32_checked(dispatch.receiver, dispatch.selector,
                                         0, 0, 0, 0);
  return result.value;
}

inline void CaptureCategoryAttachmentActions(
    CategoryAttachmentProtocolProbeRun &run) {
  run.values.category_value = DispatchRuntimeValue(kCategoryDispatch);
  run.values.class_value = DispatchRuntimeValue(kClassDispatch);
  run.values.protocol_strict_error =
      DispatchRuntimeValueChecked(kProtocolStrictErrorDispatch);
}

}  // namespace objc3c::runtime::probe::category_attachment_protocol_runtime
