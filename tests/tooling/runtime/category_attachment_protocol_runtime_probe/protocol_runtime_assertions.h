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
  const RuntimeDispatch strict_error_dispatch{
      static_cast<int>(run.widget_entry.entry.instance_receiver_identity),
      kProtocolStrictErrorSelector};
  run.values.protocol_strict_error_expected =
      ExpectedProtocolStrictDispatchErrorValue(strict_error_dispatch);
}

}  // namespace objc3c::runtime::probe::category_attachment_protocol_runtime
