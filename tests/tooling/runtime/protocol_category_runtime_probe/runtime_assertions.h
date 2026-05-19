#pragma once

#include "protocol_category_fixtures.h"
#include "support/dispatch_expectations.h"

namespace objc3c::runtime::probe::protocol_category_runtime {

inline int ExpectedStrictErrorDispatchValue() {
  return ::objc3c::runtime::probe::ExpectedStrictDispatchErrorValue(
      kStrictErrorDispatch.receiver, kStrictErrorDispatch.selector, 0, 0, 0,
      0);
}

}  // namespace objc3c::runtime::probe::protocol_category_runtime
