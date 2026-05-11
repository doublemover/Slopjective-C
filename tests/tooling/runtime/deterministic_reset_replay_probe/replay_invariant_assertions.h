#pragma once

#include "state_snapshots.h"

namespace objc3c::runtime::probe::deterministic_reset_replay {

struct ResetReplayCycleResult {
  PostResetState post_reset;
  int replay_status = 0;
  PostReplayState post_replay;
};

inline ResetReplayCycleResult RunResetReplayCycle() {
  objc3_runtime_reset_for_testing();
  ResetReplayCycleResult result;
  result.post_reset = CapturePostResetState();
  result.replay_status = objc3_runtime_replay_registered_images_for_testing();
  result.post_replay = CapturePostReplayState();
  return result;
}

}  // namespace objc3c::runtime::probe::deterministic_reset_replay
