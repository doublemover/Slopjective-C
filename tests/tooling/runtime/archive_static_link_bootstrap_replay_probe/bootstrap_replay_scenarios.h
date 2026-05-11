#pragma once

#include "archive_static_link_fixture_setup.h"

namespace objc3c::runtime::probe::archive_static_link_bootstrap_replay {

inline BootstrapReplayScenarioResult RunBootstrapReplayScenario() {
  objc3_runtime_reset_for_testing();

  BootstrapReplayScenarioResult result;
  result.post_reset = CapturePostResetBootstrapReplayState();
  result.replay_status = objc3_runtime_replay_registered_images_for_testing();
  result.post_replay = CapturePostReplayBootstrapReplayState();
  return result;
}

}  // namespace objc3c::runtime::probe::archive_static_link_bootstrap_replay
