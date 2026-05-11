#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_DISPATCH_EXPECTATIONS_SUPPORT_TEST_FAILURE_REPORT_HELPERS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_DISPATCH_EXPECTATIONS_SUPPORT_TEST_FAILURE_REPORT_HELPERS_H_

#include <string_view>

#include "support/output_expectations.h"

namespace objc3c::runtime::probe::dispatch_expectations_support {

template <typename Actual, typename Expected>
inline int ReportValueExpectation(const Actual &actual,
                                  const Expected &expected,
                                  const char *label,
                                  int report_exit_code) {
  return ::objc3c::runtime::probe::ExpectValueEqual(
      actual, expected, label, report_exit_code);
}

inline int ReportTextExpectation(std::string_view actual,
                                 std::string_view expected,
                                 const char *label,
                                 int report_exit_code) {
  return ::objc3c::runtime::probe::ExpectTextEqual(
      actual, expected, label, report_exit_code);
}

inline int ReportBooleanExpectation(bool condition,
                                    const char *label,
                                    int report_exit_code) {
  return ::objc3c::runtime::probe::ExpectTrue(
      condition, label, report_exit_code);
}

}  // namespace objc3c::runtime::probe::dispatch_expectations_support

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_DISPATCH_EXPECTATIONS_SUPPORT_TEST_FAILURE_REPORT_HELPERS_H_
