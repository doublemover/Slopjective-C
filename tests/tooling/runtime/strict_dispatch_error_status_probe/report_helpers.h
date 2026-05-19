#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_STRICT_DISPATCH_ERROR_STATUS_PROBE_REPORT_HELPERS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_STRICT_DISPATCH_ERROR_STATUS_PROBE_REPORT_HELPERS_H_

#include "strict_error_scenarios.h"

namespace objc3c::runtime::strict_dispatch_error_status_probe {

inline int RegistrationFailureCode(const StrictDispatchScenario &scenario) {
  return scenario.exit_code_base;
}

inline int StatusFailureCode(const StrictDispatchScenario &scenario) {
  return scenario.exit_code_base + 1;
}

}  // namespace objc3c::runtime::strict_dispatch_error_status_probe

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_STRICT_DISPATCH_ERROR_STATUS_PROBE_REPORT_HELPERS_H_
