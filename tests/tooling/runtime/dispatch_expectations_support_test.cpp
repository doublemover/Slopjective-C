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
  const objc3_runtime_dispatch_i32_result unresolved =
      objc3_runtime_dispatch_i32_checked(123, "missingDispatch:", 4, 5, 6, 7);
  if (ExpectValueEqual(unresolved.status_code,
                       OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_RECEIVER_CLASS,
                       "live runtime typed dispatch error status", 14) != 0) {
    return 14;
  }
  if (ExpectValueEqual(
          unresolved.value,
          ExpectedDispatch(123, "missingDispatch:", 4, 5, 6, 7),
          "live runtime strict dispatch error value projection", 16) != 0) {
    return 16;
  }
  const objc3_runtime_dispatch_i32_result nil_receiver =
      objc3_runtime_dispatch_i32_checked(0, "missingDispatch:", 4, 5, 6, 7);
  if (ExpectValueEqual(nil_receiver.status_code,
                       OBJC3_RUNTIME_DISPATCH_STATUS_NIL_RECEIVER,
                       "live runtime nil receiver dispatch status", 17) != 0) {
    return 17;
  }
  if (ExpectValueEqual(
          nil_receiver.value,
          ExpectedDispatch(0, "missingDispatch:", 4, 5, 6, 7),
          "live runtime nil receiver dispatch value", 18) != 0) {
    return 18;
  }
  return 0;
}
