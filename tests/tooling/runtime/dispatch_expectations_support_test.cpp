#include "runtime/objc3_runtime.h"
#include "support/dispatch_expectations.h"

using objc3c::runtime::probe::ComputeFallbackDispatch;
using objc3c::runtime::probe::ComputeFallbackDispatchFormula;
using objc3c::runtime::probe::ComputeSelectorScore;
using objc3c::runtime::probe::ExpectedDispatch;
using objc3c::runtime::probe::kDispatchArgument0Multiplier;
using objc3c::runtime::probe::kDispatchArgument1Multiplier;
using objc3c::runtime::probe::kDispatchArgument2Multiplier;
using objc3c::runtime::probe::kDispatchArgument3Multiplier;
using objc3c::runtime::probe::kDispatchModulus;
using objc3c::runtime::probe::kDispatchReceiverMultiplier;
using objc3c::runtime::probe::kDispatchSeed;
using objc3c::runtime::probe::kDispatchSelectorMultiplier;

int main() {
  if (kDispatchModulus != 2147483629LL || kDispatchSeed != 41 ||
      kDispatchReceiverMultiplier != 97 || kDispatchArgument0Multiplier != 7 ||
      kDispatchArgument1Multiplier != 11 ||
      kDispatchArgument2Multiplier != 13 ||
      kDispatchArgument3Multiplier != 17 || kDispatchSelectorMultiplier != 19) {
    return 10;
  }
  if (ComputeSelectorScore(nullptr) != 0 || ComputeSelectorScore("") != 0 ||
      ComputeSelectorScore("copy") != 1141 ||
      ComputeSelectorScore("alpha:beta:") != 6044 ||
      ComputeSelectorScore("missingDispatch:") != 13447 ||
      ComputeSelectorScore("negative") != 3853) {
    return 11;
  }
  if (ExpectedDispatch(7, "copy", 1, 2, 3, 4) != 22535 ||
      ExpectedDispatch(5, "alpha:beta:", 1, 2, 3, 4) != 115498 ||
      ExpectedDispatch(1024, "missingDispatch:", 4, 5, 6, 7) != 355142 ||
      ExpectedDispatch(1042, "ignoredValue", 0, 0, 0, 0) != 255034 ||
      ExpectedDispatch(-3, "negative", -1, -2, -3, -4) != 72821 ||
      ExpectedDispatch(0, "copy", 1, 2, 3, 4) != 0) {
    return 12;
  }
  if (ComputeFallbackDispatchFormula(0, "copy", 1, 2, 3, 4) ==
      ExpectedDispatch(0, "copy", 1, 2, 3, 4)) {
    return 13;
  }

  objc3_runtime_reset_for_testing();
  const int fallback =
      objc3_runtime_dispatch_i32(123, "missingDispatch:", 4, 5, 6, 7);
  if (fallback !=
      ComputeFallbackDispatch(123, "missingDispatch:", 4, 5, 6, 7)) {
    return 14;
  }
  if (objc3_runtime_dispatch_i32(0, "missingDispatch:", 4, 5, 6, 7) !=
      ExpectedDispatch(0, "missingDispatch:", 4, 5, 6, 7)) {
    return 15;
  }
  return 0;
}
