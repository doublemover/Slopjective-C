#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_BOOTSTRAP_API_PROBE_RUNTIME_ASSERTION_HELPERS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_BOOTSTRAP_API_PROBE_RUNTIME_ASSERTION_HELPERS_H_

#include "probe_state.h"
#include "../support/dispatch_expectations.h"

#include <cstdint>

namespace objc3c::runtime::probe::runtime_bootstrap_api {

inline RuntimeStringObservation CopyRuntimeString(const char *value) {
  if (value == nullptr) {
    return {};
  }
  return {false, value};
}

inline std::uint64_t SelectorStableId(
    const objc3_runtime_selector_handle *selector) {
  return selector == nullptr ? 0u : selector->stable_id;
}

inline int ExpectedBootstrapDispatchResult(int receiver, const char *selector,
                                           int a0, int a1, int a2, int a3) {
  return ExpectedDispatch(receiver, selector, a0, a1, a2, a3);
}

}  // namespace objc3c::runtime::probe::runtime_bootstrap_api

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_BOOTSTRAP_API_PROBE_RUNTIME_ASSERTION_HELPERS_H_
