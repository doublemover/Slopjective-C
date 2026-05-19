#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_STRICT_DISPATCH_ERROR_STATUS_PROBE_STATUS_ASSERTIONS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_STRICT_DISPATCH_ERROR_STATUS_PROBE_STATUS_ASSERTIONS_H_

#include "strict_error_scenarios.h"

#include "support/dispatch_expectations.h"

namespace objc3c::runtime::strict_dispatch_error_status_probe {

inline bool DispatchStatusMatches(const StrictDispatchScenario &scenario) {
  return ::objc3c::runtime::probe::HasDispatchStatus(
      InvokeScenario(scenario), scenario.expected_status, 0,
      scenario.expected_error_code, scenario.expected_error_message);
}

}  // namespace objc3c::runtime::strict_dispatch_error_status_probe

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_STRICT_DISPATCH_ERROR_STATUS_PROBE_STATUS_ASSERTIONS_H_
