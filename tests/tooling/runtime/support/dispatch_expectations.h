#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_SUPPORT_DISPATCH_EXPECTATIONS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_SUPPORT_DISPATCH_EXPECTATIONS_H_

#include <cstdint>

namespace objc3c::runtime::probe {

inline constexpr std::int64_t kDispatchModulus = 2147483629LL;
inline constexpr std::int64_t kDispatchSeed = 41;
inline constexpr std::int64_t kDispatchReceiverMultiplier = 97;
inline constexpr std::int64_t kDispatchArgument0Multiplier = 7;
inline constexpr std::int64_t kDispatchArgument1Multiplier = 11;
inline constexpr std::int64_t kDispatchArgument2Multiplier = 13;
inline constexpr std::int64_t kDispatchArgument3Multiplier = 17;
inline constexpr std::int64_t kDispatchSelectorMultiplier = 19;

inline std::int64_t ComputeSelectorScore(const char *selector) {
  if (selector == nullptr) {
    return 0;
  }

  std::int64_t selector_score = 0;
  std::int64_t index = 1;
  const unsigned char *cursor =
      reinterpret_cast<const unsigned char *>(selector);
  while (*cursor != 0U) {
    selector_score =
        (selector_score + (static_cast<std::int64_t>(*cursor) * index)) %
        kDispatchModulus;
    ++cursor;
    ++index;
  }
  return selector_score;
}

inline int ComputeFallbackDispatchFormula(int receiver, const char *selector,
                                          int a0, int a1, int a2, int a3) {
  std::int64_t value = kDispatchSeed;
  value += static_cast<std::int64_t>(receiver) * kDispatchReceiverMultiplier;
  value += static_cast<std::int64_t>(a0) * kDispatchArgument0Multiplier;
  value += static_cast<std::int64_t>(a1) * kDispatchArgument1Multiplier;
  value += static_cast<std::int64_t>(a2) * kDispatchArgument2Multiplier;
  value += static_cast<std::int64_t>(a3) * kDispatchArgument3Multiplier;
  value += ComputeSelectorScore(selector) * kDispatchSelectorMultiplier;
  value %= kDispatchModulus;
  if (value < 0) {
    value += kDispatchModulus;
  }
  return static_cast<int>(value);
}

inline int ComputeFallbackDispatch(int receiver, const char *selector, int a0,
                                   int a1, int a2, int a3) {
  return receiver == 0 ? 0
                       : ComputeFallbackDispatchFormula(receiver, selector, a0,
                                                        a1, a2, a3);
}

inline int ExpectedDispatch(int receiver, const char *selector, int a0, int a1,
                            int a2, int a3) {
  return ComputeFallbackDispatch(receiver, selector, a0, a1, a2, a3);
}

} // namespace objc3c::runtime::probe

#endif // OBJC3C_TESTS_TOOLING_RUNTIME_SUPPORT_DISPATCH_EXPECTATIONS_H_
