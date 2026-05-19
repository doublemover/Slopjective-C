#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_DISPATCH_EXPECTATIONS_SUPPORT_TEST_EXPECTATION_FIXTURE_CASES_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_DISPATCH_EXPECTATIONS_SUPPORT_TEST_EXPECTATION_FIXTURE_CASES_H_

#include "runtime/public/objc3_runtime_api.h"

namespace objc3c::runtime::probe::dispatch_expectations_support {

struct DispatchExpectationCase {
  int receiver;
  const char *selector;
  int argument0;
  int argument1;
  int argument2;
  int argument3;
  const char *label;
};

struct DiagnosticTextCase {
  objc3_runtime_dispatch_status_code status;
  const char *expected_code;
  const char *code_label;
  int code_report_exit;
  const char *expected_message;
  const char *message_label;
  int message_report_exit;
};

struct DispatchStatusCase {
  objc3_runtime_dispatch_status_code result_status;
  int result_value;
  objc3_runtime_dispatch_status_code expected_status;
  int expected_value;
  const char *expected_code;
  const char *expected_message;
};

struct CheckedDispatchCall {
  int receiver;
  const char *selector;
  int argument0;
  int argument1;
  int argument2;
  int argument3;
};

inline constexpr DispatchExpectationCase kExpectedDispatchCases[] = {
    {7, "copy", 1, 2, 3, 4, "copy expected dispatch"},
    {5, "alpha:beta:", 1, 2, 3, 4, "alpha:beta: expected dispatch"},
    {1024, "missingDispatch:", 4, 5, 6, 7,
     "missingDispatch expected dispatch"},
    {1042, "ignoredValue", 0, 0, 0, 0, "ignoredValue expected dispatch"},
    {-3, "negative", -1, -2, -3, -4, "negative expected dispatch"},
    {0, "copy", 1, 2, 3, 4, "nil receiver expected dispatch"},
};

inline constexpr DiagnosticTextCase kDiagnosticTextCases[] = {
    {OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_SELECTOR,
     "O3RT001",
     "unknown selector diagnostic code",
     19,
     "runtime dispatch failed: unknown selector",
     "unknown selector diagnostic message",
     20},
    {OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_RECEIVER_CLASS,
     "O3RT002",
     "unknown receiver diagnostic code",
     21,
     "runtime dispatch failed: unknown receiver class",
     "unknown receiver diagnostic message",
     22},
    {OBJC3_RUNTIME_DISPATCH_STATUS_CATEGORY_CONFLICT,
     "O3RT007",
     "category conflict diagnostic code",
     23,
     "runtime dispatch failed: category conflict",
     "category conflict diagnostic message",
     24},
    {OBJC3_RUNTIME_DISPATCH_STATUS_NIL_RECEIVER,
     "O3RT008",
     "nil receiver diagnostic code",
     25,
     "runtime dispatch failed: nil receiver has no value dispatch result",
     "nil receiver diagnostic message",
     26},
};

inline constexpr DispatchStatusCase kDispatchStatusCases[] = {
    {OBJC3_RUNTIME_DISPATCH_STATUS_OK,
     77,
     OBJC3_RUNTIME_DISPATCH_STATUS_OK,
     77,
     "",
     ""},
    {OBJC3_RUNTIME_DISPATCH_STATUS_NIL_RECEIVER,
     0,
     OBJC3_RUNTIME_DISPATCH_STATUS_NIL_RECEIVER,
     0,
     "O3RT008",
     "runtime dispatch failed: nil receiver has no value dispatch result"},
    {OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_SELECTOR,
     0,
     OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_SELECTOR,
     0,
     "O3RT001",
     "runtime dispatch failed: unknown selector"},
    {OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_RECEIVER_CLASS,
     0,
     OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_RECEIVER_CLASS,
     0,
     "O3RT002",
     "runtime dispatch failed: unknown receiver class"},
    {OBJC3_RUNTIME_DISPATCH_STATUS_MISSING_CLASS_GRAPH,
     0,
     OBJC3_RUNTIME_DISPATCH_STATUS_MISSING_CLASS_GRAPH,
     0,
     "O3RT003",
     "runtime dispatch failed: missing class graph"},
    {OBJC3_RUNTIME_DISPATCH_STATUS_MALFORMED_METADATA,
     0,
     OBJC3_RUNTIME_DISPATCH_STATUS_MALFORMED_METADATA,
     0,
     "O3RT004",
     "runtime dispatch failed: malformed metadata"},
    {OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_RETURN_TYPE,
     0,
     OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_RETURN_TYPE,
     0,
     "O3RT005",
     "runtime dispatch failed: rejected return shape"},
    {OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_ARGUMENT_LAYOUT,
     0,
     OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_ARGUMENT_LAYOUT,
     0,
     "O3RT006",
     "runtime dispatch failed: rejected argument layout"},
    {OBJC3_RUNTIME_DISPATCH_STATUS_CATEGORY_CONFLICT,
     0,
     OBJC3_RUNTIME_DISPATCH_STATUS_CATEGORY_CONFLICT,
     0,
     "O3RT007",
     "runtime dispatch failed: category conflict"},
};

inline constexpr CheckedDispatchCall kUnknownReceiverDispatchCall = {
    123, "missingDispatch:", 4, 5, 6, 7};

inline constexpr CheckedDispatchCall kMissingGraphDispatchCall = {
    1024, "missingDispatch:", 4, 5, 6, 7};

inline constexpr CheckedDispatchCall kNilReceiverDispatchCall = {
    0, "missingDispatch:", 4, 5, 6, 7};

}  // namespace objc3c::runtime::probe::dispatch_expectations_support

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_DISPATCH_EXPECTATIONS_SUPPORT_TEST_EXPECTATION_FIXTURE_CASES_H_
