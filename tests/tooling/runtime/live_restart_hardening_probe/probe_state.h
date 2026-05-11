#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_LIVE_RESTART_HARDENING_PROBE_PROBE_STATE_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_LIVE_RESTART_HARDENING_PROBE_PROBE_STATE_H_

#include "runtime/objc3_runtime_bootstrap_internal.h"

namespace objc3c::runtime::live_restart_hardening_probe {

struct RegistrationState {
  objc3_runtime_registration_state_snapshot snapshot{};
  int copy_status = 0;
};

struct ImageWalkState {
  objc3_runtime_image_walk_state_snapshot snapshot{};
  int copy_status = 0;
};

struct ResetReplayState {
  objc3_runtime_reset_replay_state_snapshot snapshot{};
  int copy_status = 0;
};

struct BootstrapState {
  RegistrationState registration;
  ResetReplayState reset_replay;
};

struct UnsupportedReplayState {
  int replay_status = 0;
  RegistrationState registration;
  ResetReplayState reset_replay;
};

struct ResetState {
  RegistrationState registration;
  ResetReplayState reset_replay;
};

struct RestartState {
  int replay_status = 0;
  RegistrationState registration;
  ImageWalkState image_walk;
  ResetReplayState reset_replay;
};

struct RestartCycleState {
  ResetState reset;
  RestartState restart;
};

struct ProbeRun {
  BootstrapState startup;
  UnsupportedReplayState unsupported_replay;
  RestartCycleState first_cycle;
  UnsupportedReplayState second_unsupported_replay;
  RestartCycleState second_cycle;
};

}  // namespace objc3c::runtime::live_restart_hardening_probe

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_LIVE_RESTART_HARDENING_PROBE_PROBE_STATE_H_
