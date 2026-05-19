#pragma once

#include "bootstrap_record_definitions.h"
#include "consumption_invariant_assertions.h"
#include "report_error_helpers.h"
#include "table_fixture_setup.h"

namespace objc3c::runtime::probe::runtime_bootstrap_table_consumption {

inline BootstrapTableConsumptionProbeResult RunProbeLifecycle() {
  BootstrapTableConsumptionProbeResult result;
  result.startup = CaptureStartupTableState();
  result.duplicate = RunDuplicateRegistrationConsumption(result.startup);
  return result;
}

inline int RunProbeMain() {
  const BootstrapTableConsumptionProbeResult result = RunProbeLifecycle();
  PrintProbeReport(result);
  return 0;
}

}  // namespace objc3c::runtime::probe::runtime_bootstrap_table_consumption
