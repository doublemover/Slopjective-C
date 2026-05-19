#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_DISPATCH_EXPECTATIONS_SUPPORT_TEST_MAIN_ORCHESTRATION_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_DISPATCH_EXPECTATIONS_SUPPORT_TEST_MAIN_ORCHESTRATION_H_

#include "matching_assertion_helpers.h"

namespace objc3c::runtime::probe::dispatch_expectations_support {

inline int RunHelperExpectationChecks() {
  int result = VerifyStrictDispatchErrorConstant();
  if (result != 0) {
    return result;
  }

  result = VerifyExpectedDispatchCases();
  if (result != 0) {
    return result;
  }

  result = VerifyStrictDispatchAliasMatchesExpectedDispatch();
  if (result != 0) {
    return result;
  }

  result = VerifyRuntimeDiagnosticTextCases();
  if (result != 0) {
    return result;
  }

  result = VerifyDispatchStatusCases();
  if (result != 0) {
    return result;
  }

  return VerifyDispatchResultAbiFields();
}

inline int RunLiveRuntimeDispatchChecks() {
  objc3_runtime_reset_for_testing();

  int result = VerifyUnknownReceiverStrictDispatchError(
      ExecuteCheckedDispatch(kUnknownReceiverDispatchCall));
  if (result != 0) {
    return result;
  }

  result = VerifyMissingGraphStrictDispatchError(
      ExecuteCheckedDispatch(kMissingGraphDispatchCall));
  if (result != 0) {
    return result;
  }

  return VerifyNilReceiverStrictDispatchError(
      ExecuteCheckedDispatch(kNilReceiverDispatchCall));
}

inline int RunTestMain() {
  const int helper_result = RunHelperExpectationChecks();
  if (helper_result != 0) {
    return helper_result;
  }

  return RunLiveRuntimeDispatchChecks();
}

}  // namespace objc3c::runtime::probe::dispatch_expectations_support

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_DISPATCH_EXPECTATIONS_SUPPORT_TEST_MAIN_ORCHESTRATION_H_
