#include "runtime/objc3_runtime.h"
#include "support/dispatch_expectations.h"
#include "support/output_expectations.h"

using objc3c::runtime::probe::ComputeFallbackDispatch;
using objc3c::runtime::probe::ComputeFallbackDispatchFormula;
using objc3c::runtime::probe::ComputeSelectorScore;
using objc3c::runtime::probe::ExpectTrue;
using objc3c::runtime::probe::ExpectValueEqual;
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
  if (ExpectValueEqual(kDispatchModulus, 2147483629LL, "dispatch modulus",
                       10) != 0 ||
      ExpectValueEqual(kDispatchSeed, 41, "dispatch seed", 10) != 0 ||
      ExpectValueEqual(kDispatchReceiverMultiplier, 97,
                       "dispatch receiver multiplier", 10) != 0 ||
      ExpectValueEqual(kDispatchArgument0Multiplier, 7,
                       "dispatch arg0 multiplier", 10) != 0 ||
      ExpectValueEqual(kDispatchArgument1Multiplier, 11,
                       "dispatch arg1 multiplier", 10) != 0 ||
      ExpectValueEqual(kDispatchArgument2Multiplier, 13,
                       "dispatch arg2 multiplier", 10) != 0 ||
      ExpectValueEqual(kDispatchArgument3Multiplier, 17,
                       "dispatch arg3 multiplier", 10) != 0 ||
      ExpectValueEqual(kDispatchSelectorMultiplier, 19,
                       "dispatch selector multiplier", 10) != 0) {
    return 10;
  }
  if (ExpectValueEqual(ComputeSelectorScore(nullptr), 0,
                       "null selector score", 11) != 0 ||
      ExpectValueEqual(ComputeSelectorScore(""), 0, "empty selector score",
                       11) != 0 ||
      ExpectValueEqual(ComputeSelectorScore("copy"), 1141,
                       "copy selector score", 11) != 0 ||
      ExpectValueEqual(ComputeSelectorScore("alpha:beta:"), 6044,
                       "alpha:beta: selector score", 11) != 0 ||
      ExpectValueEqual(ComputeSelectorScore("missingDispatch:"), 13447,
                       "missingDispatch: selector score", 11) != 0 ||
      ExpectValueEqual(ComputeSelectorScore("negative"), 3853,
                       "negative selector score", 11) != 0) {
    return 11;
  }
  if (ExpectValueEqual(ExpectedDispatch(7, "copy", 1, 2, 3, 4), 22535,
                       "copy expected dispatch", 12) != 0 ||
      ExpectValueEqual(ExpectedDispatch(5, "alpha:beta:", 1, 2, 3, 4),
                       115498, "alpha:beta: expected dispatch", 12) != 0 ||
      ExpectValueEqual(ExpectedDispatch(1024, "missingDispatch:", 4, 5, 6, 7),
                       355142, "missingDispatch expected dispatch", 12) != 0 ||
      ExpectValueEqual(ExpectedDispatch(1042, "ignoredValue", 0, 0, 0, 0),
                       255034, "ignoredValue expected dispatch", 12) != 0 ||
      ExpectValueEqual(ExpectedDispatch(-3, "negative", -1, -2, -3, -4),
                       72821, "negative expected dispatch", 12) != 0 ||
      ExpectValueEqual(ExpectedDispatch(0, "copy", 1, 2, 3, 4), 0,
                       "nil receiver expected dispatch", 12) != 0) {
    return 12;
  }
  if (ExpectTrue(ComputeFallbackDispatchFormula(0, "copy", 1, 2, 3, 4) !=
                     ExpectedDispatch(0, "copy", 1, 2, 3, 4),
                 "raw formula remains distinct from nil receiver dispatch",
                 13) != 0) {
    return 13;
  }

  objc3_runtime_reset_for_testing();
  const int fallback =
      objc3_runtime_dispatch_i32(123, "missingDispatch:", 4, 5, 6, 7);
  if (ExpectValueEqual(
          fallback, ComputeFallbackDispatch(123, "missingDispatch:", 4, 5, 6, 7),
          "live runtime fallback dispatch drift", 14) != 0) {
    return 14;
  }
  if (ExpectValueEqual(
          objc3_runtime_dispatch_i32(0, "missingDispatch:", 4, 5, 6, 7),
          ExpectedDispatch(0, "missingDispatch:", 4, 5, 6, 7),
          "live runtime nil receiver dispatch drift", 15) != 0) {
    return 15;
  }
  return 0;
}
