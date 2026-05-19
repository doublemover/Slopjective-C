#pragma once

#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "runtime/public/objc3_runtime_api.h"

namespace objc3c::runtime::probe::live_registration_replay_tracking {

inline void ResetRuntimeRegistrationFixture() {
  objc3_runtime_reset_for_testing();
}

inline int ReplayRegisteredImagesForTracking() {
  return objc3_runtime_replay_registered_images_for_testing();
}

}  // namespace objc3c::runtime::probe::live_registration_replay_tracking
