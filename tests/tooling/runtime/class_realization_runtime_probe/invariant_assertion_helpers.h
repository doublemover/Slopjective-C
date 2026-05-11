#pragma once

#include "class_metadata_definitions.h"
#include "support/dispatch_expectations.h"

namespace objc3c::runtime::probe::class_realization_runtime {

inline int ExpectedProtocolStrictDispatchErrorValue() {
  return ::objc3c::runtime::probe::ExpectedStrictDispatchErrorValue(
      kProtocolStrictErrorDispatch.receiver,
      kProtocolStrictErrorDispatch.selector, 0, 0, 0, 0);
}

}  // namespace objc3c::runtime::probe::class_realization_runtime
