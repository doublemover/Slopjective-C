#pragma once

#include "selector_class_fixtures.h"

#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "support/dispatch_expectations.h"

namespace objc3c {
namespace tooling {
namespace method_cache_slow_path_probe {

inline int ExpectedStrictDispatchValue() {
  return ::objc3c::runtime::probe::ExpectedStrictDispatchErrorValue(
      kWidgetInstanceClassId, kStrictErrorSelector, 4, 5, 6, 7);
}

inline int DispatchStrictErrorSelector() {
  const objc3_runtime_dispatch_i32_result result =
      objc3_runtime_dispatch_i32_checked(kWidgetInstanceClassId,
                                         kStrictErrorSelector, 4, 5, 6, 7);
  return result.value;
}

} // namespace method_cache_slow_path_probe
} // namespace tooling
} // namespace objc3c
