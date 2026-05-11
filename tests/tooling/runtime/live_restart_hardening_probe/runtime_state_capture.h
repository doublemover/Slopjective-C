#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_LIVE_RESTART_HARDENING_PROBE_RUNTIME_STATE_CAPTURE_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_LIVE_RESTART_HARDENING_PROBE_RUNTIME_STATE_CAPTURE_H_

#include "probe_state.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"

namespace objc3c::runtime::live_restart_hardening_probe {

inline RegistrationState CaptureRegistrationState() {
  RegistrationState state;
  state.copy_status =
      objc3_runtime_copy_registration_state_for_testing(&state.snapshot);
  return state;
}

inline ImageWalkState CaptureImageWalkState() {
  ImageWalkState state;
  state.copy_status =
      objc3_runtime_copy_image_walk_state_for_testing(&state.snapshot);
  return state;
}

inline ResetReplayState CaptureResetReplayState() {
  ResetReplayState state;
  state.copy_status =
      objc3_runtime_copy_reset_replay_state_for_testing(&state.snapshot);
  return state;
}

inline BootstrapState CaptureBootstrapState() {
  return {
      CaptureRegistrationState(),
      CaptureResetReplayState(),
  };
}

inline ResetState CaptureResetState() {
  return {
      CaptureRegistrationState(),
      CaptureResetReplayState(),
  };
}

inline RestartState CaptureRestartState(int replay_status) {
  return {
      replay_status,
      CaptureRegistrationState(),
      CaptureImageWalkState(),
      CaptureResetReplayState(),
  };
}

inline UnsupportedReplayState CaptureUnsupportedReplayState(int replay_status) {
  return {
      replay_status,
      CaptureRegistrationState(),
      CaptureResetReplayState(),
  };
}

}  // namespace objc3c::runtime::live_restart_hardening_probe

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_LIVE_RESTART_HARDENING_PROBE_RUNTIME_STATE_CAPTURE_H_
