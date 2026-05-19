#pragma once

#include "archive_static_link_fixture_setup.h"
#include "bootstrap_replay_scenarios.h"
#include "report_error_helpers.h"

namespace objc3c::runtime::probe::archive_static_link_bootstrap_replay {

inline ProbeResult RunArchiveStaticLinkBootstrapReplayProbe() {
  ProbeResult result;
  result.startup = CaptureStartupArchiveStaticLinkFixture();
  result.replay = RunBootstrapReplayScenario();
  return result;
}

inline int RunProbeMain() {
  const ProbeResult result = RunArchiveStaticLinkBootstrapReplayProbe();
  PrintProbeReport(result);
  return 0;
}

}  // namespace objc3c::runtime::probe::archive_static_link_bootstrap_replay
