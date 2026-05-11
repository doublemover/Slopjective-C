#pragma once

#include "category_protocol_fixture_definitions.h"
#include "probe_state.h"
#include "support/dispatch_expectations.h"

namespace objc3c::runtime::probe::category_attachment_protocol_runtime {

inline int ExpectedProtocolStrictDispatchErrorValue(
    const RuntimeDispatch &dispatch) {
  return ::objc3c::runtime::probe::ExpectedStrictDispatchErrorValue(
      dispatch.receiver, dispatch.selector, 0, 0, 0, 0);
}

inline void CaptureProtocolRuntimeAssertions(
    CategoryAttachmentProtocolProbeRun &run) {
  run.values.protocol_strict_error_expected =
      ExpectedProtocolStrictDispatchErrorValue(kProtocolStrictErrorDispatch);
}

}  // namespace objc3c::runtime::probe::category_attachment_protocol_runtime
