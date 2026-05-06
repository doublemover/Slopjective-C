#include "runtime/public/objc3_runtime_api.h"
#include "runtime/dispatch/dispatch_errors.h"
#include "support/dispatch_expectations.h"
#include "support/output_expectations.h"

using objc3c::runtime::probe::ExpectedStrictDispatchErrorValue;
using objc3c::runtime::probe::ExpectTextEqual;
using objc3c::runtime::probe::ExpectTrue;
using objc3c::runtime::probe::ExpectValueEqual;
using objc3c::runtime::probe::ExpectedDispatch;
using objc3c::runtime::probe::HasDispatchStatus;
using objc3c::runtime::probe::IsStrictDispatchError;
using objc3c::runtime::probe::kStrictDispatchErrorValueI32;
using objc3c::runtime::MakeDispatchI32Result;

int main() {
  if (ExpectValueEqual(kStrictDispatchErrorValueI32, 0, "strict dispatch error i32",
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
  if (ExpectTrue(ExpectedStrictDispatchErrorValue(123, "missingDispatch:", 4, 5, 6, 7) ==
                     ExpectedDispatch(123, "missingDispatch:", 4, 5, 6, 7),
                 "strict dispatch error helper matches expected dispatch value",
                 13) != 0) {
    return 13;
  }
  if (ExpectTextEqual(
          objc3c::runtime::DispatchDiagnosticCode(
              OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_SELECTOR),
          "O3RT001", "unknown selector diagnostic code", 19) != 0 ||
      ExpectTextEqual(
          objc3c::runtime::DispatchDiagnosticMessage(
              OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_SELECTOR),
          "runtime dispatch failed: unknown selector",
          "unknown selector diagnostic message", 20) != 0 ||
      ExpectTextEqual(
          objc3c::runtime::DispatchDiagnosticCode(
              OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_RECEIVER_CLASS),
          "O3RT002", "unknown receiver diagnostic code", 21) != 0 ||
      ExpectTextEqual(
          objc3c::runtime::DispatchDiagnosticMessage(
              OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_RECEIVER_CLASS),
          "runtime dispatch failed: unknown receiver class",
          "unknown receiver diagnostic message", 22) != 0 ||
      ExpectTextEqual(
          objc3c::runtime::DispatchDiagnosticCode(
              OBJC3_RUNTIME_DISPATCH_STATUS_CATEGORY_CONFLICT),
          "O3RT007", "category conflict diagnostic code", 23) != 0 ||
      ExpectTextEqual(
          objc3c::runtime::DispatchDiagnosticMessage(
              OBJC3_RUNTIME_DISPATCH_STATUS_CATEGORY_CONFLICT),
          "runtime dispatch failed: category conflict",
          "category conflict diagnostic message", 24) != 0 ||
      ExpectTextEqual(
          objc3c::runtime::DispatchDiagnosticCode(
              OBJC3_RUNTIME_DISPATCH_STATUS_NIL_RECEIVER),
          "", "nil receiver diagnostic code", 25) != 0 ||
      ExpectTextEqual(
          objc3c::runtime::DispatchDiagnosticMessage(
              OBJC3_RUNTIME_DISPATCH_STATUS_NIL_RECEIVER),
          "", "nil receiver diagnostic message", 26) != 0) {
    return 19;
  }
  const objc3_runtime_dispatch_i32_result diagnostic_contracts[] = {
      MakeDispatchI32Result(OBJC3_RUNTIME_DISPATCH_STATUS_OK, 77),
      MakeDispatchI32Result(OBJC3_RUNTIME_DISPATCH_STATUS_NIL_RECEIVER, 0),
      MakeDispatchI32Result(OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_SELECTOR, 0),
      MakeDispatchI32Result(
          OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_RECEIVER_CLASS, 0),
      MakeDispatchI32Result(OBJC3_RUNTIME_DISPATCH_STATUS_MALFORMED_METADATA,
                            0),
      MakeDispatchI32Result(
          OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_RETURN_TYPE, 0),
      MakeDispatchI32Result(
          OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_ARGUMENT_LAYOUT, 0),
      MakeDispatchI32Result(OBJC3_RUNTIME_DISPATCH_STATUS_CATEGORY_CONFLICT,
                            0),
  };
  if (!HasDispatchStatus(diagnostic_contracts[0],
                         OBJC3_RUNTIME_DISPATCH_STATUS_OK, 77, "", "") ||
      !HasDispatchStatus(diagnostic_contracts[1],
                         OBJC3_RUNTIME_DISPATCH_STATUS_NIL_RECEIVER, 0, "",
                         "") ||
      !HasDispatchStatus(diagnostic_contracts[2],
                         OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_SELECTOR, 0,
                         "O3RT001",
                         "runtime dispatch failed: unknown selector") ||
      !HasDispatchStatus(
          diagnostic_contracts[3],
          OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_RECEIVER_CLASS, 0, "O3RT002",
          "runtime dispatch failed: unknown receiver class") ||
      !HasDispatchStatus(
          diagnostic_contracts[4],
          OBJC3_RUNTIME_DISPATCH_STATUS_MALFORMED_METADATA, 0, "O3RT004",
          "runtime dispatch failed: malformed metadata") ||
      !HasDispatchStatus(
          diagnostic_contracts[5],
          OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_RETURN_TYPE, 0, "O3RT005",
          "runtime dispatch failed: unsupported return type") ||
      !HasDispatchStatus(
          diagnostic_contracts[6],
          OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_ARGUMENT_LAYOUT, 0,
          "O3RT006",
          "runtime dispatch failed: unsupported argument layout") ||
      !HasDispatchStatus(
          diagnostic_contracts[7],
          OBJC3_RUNTIME_DISPATCH_STATUS_CATEGORY_CONFLICT, 0, "O3RT007",
          "runtime dispatch failed: category conflict")) {
    return 31;
  }

  objc3_runtime_reset_for_testing();
  const objc3_runtime_dispatch_i32_result unresolved =
      objc3_runtime_dispatch_i32_checked(123, "missingDispatch:", 4, 5, 6, 7);
  if (!IsStrictDispatchError(
          unresolved, OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_RECEIVER_CLASS)) {
    return 14;
  }
  if (ExpectValueEqual(unresolved.value, 0,
                       "live runtime strict dispatch error carries no value",
                       16) != 0) {
    return 16;
  }
  if (ExpectTextEqual(unresolved.diagnostic_code, "O3RT002",
                      "live runtime unknown receiver diagnostic code", 27) !=
          0 ||
      ExpectTextEqual(unresolved.diagnostic_message,
                      "runtime dispatch failed: unknown receiver class",
                      "live runtime unknown receiver diagnostic message", 28) !=
          0) {
    return 27;
  }
  const objc3_runtime_dispatch_i32_result nil_receiver =
      objc3_runtime_dispatch_i32_checked(0, "missingDispatch:", 4, 5, 6, 7);
  if (ExpectValueEqual(nil_receiver.status_code,
                       OBJC3_RUNTIME_DISPATCH_STATUS_NIL_RECEIVER,
                       "live runtime nil receiver dispatch status", 17) != 0) {
    return 17;
  }
  if (ExpectValueEqual(nil_receiver.value, 0,
                       "live runtime nil receiver dispatch value", 18) != 0) {
    return 18;
  }
  if (ExpectTextEqual(nil_receiver.diagnostic_code, "",
                      "live runtime nil receiver diagnostic code", 29) != 0 ||
      ExpectTextEqual(nil_receiver.diagnostic_message, "",
                      "live runtime nil receiver diagnostic message", 30) != 0) {
    return 29;
  }
  return 0;
}
