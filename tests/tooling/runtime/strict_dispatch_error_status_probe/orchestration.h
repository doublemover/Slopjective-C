#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_STRICT_DISPATCH_ERROR_STATUS_PROBE_ORCHESTRATION_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_STRICT_DISPATCH_ERROR_STATUS_PROBE_ORCHESTRATION_H_

#include "report_helpers.h"
#include "status_assertions.h"
#include "strict_error_scenarios.h"

namespace objc3c::runtime::strict_dispatch_error_status_probe {

inline int RunStrictDispatchErrorStatusScenarios() {
  const StrictDispatchScenarioList scenarios = StrictDispatchScenarios();
  for (std::size_t index = 0; index < scenarios.count; ++index) {
    const StrictDispatchScenario &scenario = scenarios.items[index];
    ManualImageCase image_case = MakeImageCase(
        scenario.module_name, scenario.identity_key, scenario.selector,
        scenario.return_type_name, scenario.parameter_count,
        scenario.implementation, scenario.method_header_count);

    if (!RegisterCase(image_case)) {
      return RegistrationFailureCode(scenario);
    }

    if (!DispatchStatusMatches(scenario)) {
      return StatusFailureCode(scenario);
    }
  }

  return 0;
}

}  // namespace objc3c::runtime::strict_dispatch_error_status_probe

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_STRICT_DISPATCH_ERROR_STATUS_PROBE_ORCHESTRATION_H_
