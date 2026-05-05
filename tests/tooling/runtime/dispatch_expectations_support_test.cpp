#include "runtime/objc3_runtime.h"
#include "support/dispatch_expectations.h"
#include "support/output_expectations.h"

using objc3c::runtime::probe::ComputeFallbackDispatch;
using objc3c::runtime::probe::ExpectTrue;
using objc3c::runtime::probe::ExpectValueEqual;
using objc3c::runtime::probe::ExpectedDispatch;
using objc3c::runtime::probe::kStrictDispatchErrorI32;

int main() {
  if (ExpectValueEqual(kStrictDispatchErrorI32, 0, "strict dispatch error i32",
                       10) != 0) {
    return 10;
  }
  if (ExpectValueEqual(ExpectedDispatch(7, "copy", 1, 2, 3, 4), 0,
                       "copy expected dispatch", 12) != 0 ||
      ExpectValueEqual(ExpectedDispatch(5, "alpha:beta:", 1, 2, 3, 4),
                       0, "alpha:beta: expected dispatch", 12) != 0 ||
      ExpectValueEqual(ExpectedDispatch(1024, "missingDispatch:", 4, 5, 6, 7),
                       0, "missingDispatch expected dispatch", 12) != 0 ||
      ExpectValueEqual(ExpectedDispatch(1042, "ignoredValue", 0, 0, 0, 0),
                       0, "ignoredValue expected dispatch", 12) != 0 ||
      ExpectValueEqual(ExpectedDispatch(-3, "negative", -1, -2, -3, -4),
                       0, "negative expected dispatch", 12) != 0 ||
      ExpectValueEqual(ExpectedDispatch(0, "copy", 1, 2, 3, 4), 0,
                       "nil receiver expected dispatch", 12) != 0) {
    return 12;
  }
  if (ExpectTrue(ComputeFallbackDispatch(123, "missingDispatch:", 4, 5, 6, 7) ==
                     ExpectedDispatch(123, "missingDispatch:", 4, 5, 6, 7),
                 "legacy helper name remains strict-error compatible",
                 13) != 0) {
    return 13;
  }

  objc3_runtime_reset_for_testing();
  const int unresolved =
      objc3_runtime_dispatch_i32(123, "missingDispatch:", 4, 5, 6, 7);
  if (ExpectValueEqual(
          unresolved, ExpectedDispatch(123, "missingDispatch:", 4, 5, 6, 7),
          "live runtime strict dispatch error drift", 14) != 0) {
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
