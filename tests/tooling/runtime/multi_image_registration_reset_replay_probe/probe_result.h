#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_MULTI_IMAGE_REGISTRATION_RESET_REPLAY_PROBE_PROBE_RESULT_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_MULTI_IMAGE_REGISTRATION_RESET_REPLAY_PROBE_PROBE_RESULT_H_

#include "reset_replay_assertions.h"

namespace objc3c::runtime::probe::multi_image_registration_reset_replay {

struct ProbeResult {
  RuntimeImageLifecycleState startup;
  ResetReplayCycleResult first_cycle;
  BlockedReplayResult blocked_replay;
  ResetReplayCycleResult second_cycle;
};

}  // namespace objc3c::runtime::probe::multi_image_registration_reset_replay

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_MULTI_IMAGE_REGISTRATION_RESET_REPLAY_PROBE_PROBE_RESULT_H_
