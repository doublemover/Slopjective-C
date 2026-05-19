#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_DISPATCH_EXPECTATIONS_SUPPORT_TEST_MATCHING_ASSERTION_HELPERS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_DISPATCH_EXPECTATIONS_SUPPORT_TEST_MATCHING_ASSERTION_HELPERS_H_

#include "expectation_fixture_cases.h"
#include "failure_report_helpers.h"
#include "runtime/public/objc3_runtime_result_contract.h"
#include "support/dispatch_expectations.h"

namespace objc3c::runtime::probe::dispatch_expectations_support {

inline objc3_runtime_dispatch_i32_result MakeFixtureDispatchResult(
    const DispatchStatusCase &test_case) {
  return ::objc3c::runtime::MakeRuntimeDispatchI32Result(
      test_case.result_status, test_case.result_value);
}

inline objc3_runtime_dispatch_i32_result ExecuteCheckedDispatch(
    const CheckedDispatchCall &call) {
  return objc3_runtime_dispatch_i32_checked(call.receiver, call.selector,
                                            call.argument0, call.argument1,
                                            call.argument2, call.argument3);
}

inline bool DispatchStatusMatches(const objc3_runtime_dispatch_i32_result &result,
                                  const DispatchStatusCase &test_case) {
  return ::objc3c::runtime::probe::HasDispatchStatus(
      result, test_case.expected_status, test_case.expected_value,
      test_case.expected_code, test_case.expected_message);
}

inline int VerifyStrictDispatchErrorConstant() {
  return ReportValueExpectation(
      ::objc3c::runtime::probe::kStrictDispatchErrorValueI32, 0,
      "strict dispatch error i32", 10);
}

inline int VerifyExpectedDispatchCases() {
  for (const DispatchExpectationCase &test_case : kExpectedDispatchCases) {
    const int actual = ::objc3c::runtime::probe::ExpectedDispatch(
        test_case.receiver, test_case.selector, test_case.argument0,
        test_case.argument1, test_case.argument2, test_case.argument3);
    const int result =
        ReportValueExpectation(actual, 0, test_case.label, 12);
    if (result != 0) {
      return 12;
    }
  }
  return 0;
}

inline int VerifyStrictDispatchAliasMatchesExpectedDispatch() {
  const int strict_error_value =
      ::objc3c::runtime::probe::ExpectedStrictDispatchErrorValue(
          kUnknownReceiverDispatchCall.receiver,
          kUnknownReceiverDispatchCall.selector,
          kUnknownReceiverDispatchCall.argument0,
          kUnknownReceiverDispatchCall.argument1,
          kUnknownReceiverDispatchCall.argument2,
          kUnknownReceiverDispatchCall.argument3);
  const int expected_dispatch_value = ::objc3c::runtime::probe::ExpectedDispatch(
      kUnknownReceiverDispatchCall.receiver,
      kUnknownReceiverDispatchCall.selector,
      kUnknownReceiverDispatchCall.argument0,
      kUnknownReceiverDispatchCall.argument1,
      kUnknownReceiverDispatchCall.argument2,
      kUnknownReceiverDispatchCall.argument3);
  return ReportBooleanExpectation(
      strict_error_value == expected_dispatch_value,
      "strict dispatch error helper matches expected dispatch value", 13);
}

inline int VerifyRuntimeDiagnosticTextCases() {
  for (const DiagnosticTextCase &test_case : kDiagnosticTextCases) {
    if (ReportTextExpectation(
            ::objc3c::runtime::RuntimeDispatchDiagnosticCode(test_case.status),
            test_case.expected_code, test_case.code_label,
            test_case.code_report_exit) != 0) {
      return 19;
    }
    if (ReportTextExpectation(
            ::objc3c::runtime::RuntimeDispatchDiagnosticMessage(
                test_case.status),
            test_case.expected_message, test_case.message_label,
            test_case.message_report_exit) != 0) {
      return 19;
    }
  }
  return 0;
}

inline int VerifyDispatchStatusCases() {
  for (const DispatchStatusCase &test_case : kDispatchStatusCases) {
    if (!DispatchStatusMatches(MakeFixtureDispatchResult(test_case), test_case)) {
      return 31;
    }
  }
  return 0;
}

inline int VerifyDispatchResultAbiFields() {
  const objc3_runtime_dispatch_i32_result success =
      MakeRuntimeDispatchI32TypedResult(
          OBJC3_RUNTIME_DISPATCH_STATUS_OK, 77,
          OBJC3_RUNTIME_DISPATCH_RETURN_KIND_BOOL);
  if (ReportValueExpectation(
          success.abi_version, OBJC3_RUNTIME_DISPATCH_I32_RESULT_ABI_VERSION,
          "checked dispatch result abi version", 35) != 0) {
    return 35;
  }
  if (ReportValueExpectation(
          success.result_size,
          static_cast<int>(sizeof(objc3_runtime_dispatch_i32_result)),
          "checked dispatch result abi size", 36) != 0) {
    return 36;
  }
  if (ReportValueExpectation(success.status_code,
                             OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_RETURN_TYPE,
                             "checked dispatch result rejects non-i32 status",
                             37) != 0) {
    return 37;
  }
  if (ReportValueExpectation(success.value, 0,
                             "checked dispatch result rejects non-i32 value",
                             38) != 0) {
    return 38;
  }
  if (ReportTextExpectation(success.result_contract,
                            "typed-dispatch-strict-error-result",
                            "checked dispatch result non-i32 contract",
                            41) != 0) {
    return 41;
  }

  const objc3_runtime_dispatch_i32_result failure =
      MakeRuntimeDispatchI32TypedResult(
          OBJC3_RUNTIME_DISPATCH_STATUS_STALE_METHOD_CACHE, 77,
          OBJC3_RUNTIME_DISPATCH_RETURN_KIND_I32);
  if (ReportValueExpectation(
          failure.value, 0,
          "checked dispatch strict error zeroes value", 39) != 0) {
    return 39;
  }
  if (ReportTextExpectation(failure.result_contract,
                            "typed-dispatch-strict-error-result",
                            "checked dispatch strict error contract",
                            40) != 0) {
    return 40;
  }
  return 0;
}

inline int VerifyUnknownReceiverStrictDispatchError(
    const objc3_runtime_dispatch_i32_result &result) {
  if (!::objc3c::runtime::probe::IsStrictDispatchError(
          result, OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_RECEIVER_CLASS)) {
    return 14;
  }
  if (ReportValueExpectation(
          result.value, 0,
          "live runtime strict dispatch error carries no value", 16) != 0) {
    return 16;
  }
  if (ReportTextExpectation(result.diagnostic_code, "O3RT002",
                            "live runtime unknown receiver diagnostic code",
                            27) != 0 ||
      ReportTextExpectation(
          result.diagnostic_message,
          "runtime dispatch failed: unknown receiver class",
          "live runtime unknown receiver diagnostic message", 28) != 0) {
    return 27;
  }
  return 0;
}

inline int VerifyMissingGraphStrictDispatchError(
    const objc3_runtime_dispatch_i32_result &result) {
  if (!::objc3c::runtime::probe::HasDispatchStatus(
          result, OBJC3_RUNTIME_DISPATCH_STATUS_MISSING_CLASS_GRAPH, 0,
          "O3RT003", "runtime dispatch failed: missing class graph")) {
    return 32;
  }
  return 0;
}

inline int VerifyNilReceiverStrictDispatchError(
    const objc3_runtime_dispatch_i32_result &result) {
  if (ReportValueExpectation(result.status_code,
                             OBJC3_RUNTIME_DISPATCH_STATUS_NIL_RECEIVER,
                             "live runtime nil receiver dispatch status",
                             17) != 0) {
    return 17;
  }
  if (ReportValueExpectation(result.value, 0,
                             "live runtime nil receiver dispatch value",
                             18) != 0) {
    return 18;
  }
  if (ReportTextExpectation(result.diagnostic_code, "O3RT008",
                            "live runtime nil receiver diagnostic code",
                            29) != 0 ||
      ReportTextExpectation(
          result.diagnostic_message,
          "runtime dispatch failed: nil receiver has no value dispatch result",
          "live runtime nil receiver diagnostic message", 30) != 0) {
    return 29;
  }
  return 0;
}

}  // namespace objc3c::runtime::probe::dispatch_expectations_support

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_DISPATCH_EXPECTATIONS_SUPPORT_TEST_MATCHING_ASSERTION_HELPERS_H_
