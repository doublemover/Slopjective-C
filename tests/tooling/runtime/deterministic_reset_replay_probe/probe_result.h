#pragma once

#include "replay_invariant_assertions.h"

namespace objc3c::runtime::probe::deterministic_reset_replay {

struct ProbeResult {
  StartupState startup;
  ResetReplayCycleResult reset_replay;
};

}  // namespace objc3c::runtime::probe::deterministic_reset_replay
