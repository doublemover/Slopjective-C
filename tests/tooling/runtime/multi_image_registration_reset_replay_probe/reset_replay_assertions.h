#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_MULTI_IMAGE_REGISTRATION_RESET_REPLAY_PROBE_RESET_REPLAY_ASSERTIONS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_MULTI_IMAGE_REGISTRATION_RESET_REPLAY_PROBE_RESET_REPLAY_ASSERTIONS_H_

#include "registration_lifecycle_helpers.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"

namespace objc3c::runtime::probe::multi_image_registration_reset_replay {

struct ResetReplayCycleResult {
  PostResetLifecycleState reset;
  int replay_status = 0;
  RuntimeImageLifecycleState replayed;
};

struct BlockedReplayResult {
  int replay_status = 0;
  ResetReplayState replay_state;
};

inline ResetReplayCycleResult RunResetReplayCycle() {
  objc3_runtime_reset_for_testing();
  ResetReplayCycleResult result;
  result.reset = CapturePostResetLifecycleState();
  result.replay_status = objc3_runtime_replay_registered_images_for_testing();
  result.replayed = CaptureRuntimeImageLifecycleState();
  return result;
}

inline BlockedReplayResult RunBlockedReplayAssertion() {
  BlockedReplayResult result;
  result.replay_status = objc3_runtime_replay_registered_images_for_testing();
  result.replay_state = CaptureResetReplayState();
  return result;
}

}  // namespace objc3c::runtime::probe::multi_image_registration_reset_replay

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_MULTI_IMAGE_REGISTRATION_RESET_REPLAY_PROBE_RESET_REPLAY_ASSERTIONS_H_
