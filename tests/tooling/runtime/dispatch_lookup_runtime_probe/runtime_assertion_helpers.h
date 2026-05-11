#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_DISPATCH_LOOKUP_RUNTIME_PROBE_RUNTIME_ASSERTION_HELPERS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_DISPATCH_LOOKUP_RUNTIME_PROBE_RUNTIME_ASSERTION_HELPERS_H_

#include "runtime/public/objc3_runtime_api.h"
#include "../support/dispatch_expectations.h"

#include <cstdint>
#include <cstring>

namespace objc3c::runtime::probe::dispatch_lookup_runtime_probe {

inline constexpr const char *kCopySelector = "copy";
inline constexpr const char *kGammaSelector = "gamma";
inline constexpr int kDispatchReceiver = 7;
inline constexpr int kNilDispatchReceiver = 0;
inline constexpr int kDispatchArg0 = 1;
inline constexpr int kDispatchArg1 = 2;
inline constexpr int kDispatchArg2 = 3;
inline constexpr int kDispatchArg3 = 4;

inline std::uint64_t SelectorStableId(
    const objc3_runtime_selector_handle *selector) {
  return selector != nullptr ? selector->stable_id : 0;
}

inline bool SelectorSpellingMatches(
    const objc3_runtime_selector_handle *selector, const char *expected) {
  return selector != nullptr && std::strcmp(selector->selector, expected) == 0;
}

inline int ExpectedCopyDispatchResult() {
  return ::objc3c::runtime::probe::ExpectedDispatch(
      kDispatchReceiver, kCopySelector, kDispatchArg0, kDispatchArg1,
      kDispatchArg2, kDispatchArg3);
}

}  // namespace objc3c::runtime::probe::dispatch_lookup_runtime_probe

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_DISPATCH_LOOKUP_RUNTIME_PROBE_RUNTIME_ASSERTION_HELPERS_H_
