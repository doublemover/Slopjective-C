#pragma once

#include "probe_state.h"

namespace objc3c {
namespace runtime {
namespace probe {
namespace block_arc_runtime_abi {

extern "C" inline int ProbeInvoke(void *storage,
                                  int a0,
                                  int a1,
                                  int a2,
                                  int a3) {
  auto *block = static_cast<ProbeBlockStorage *>(storage);
  if (block == nullptr || block->capture == nullptr) {
    return -1;
  }
  return block->capture->base + a0 + a1 + a2 + a3;
}

extern "C" inline void ProbeCopy(void *storage) {
  auto *block = static_cast<ProbeBlockStorage *>(storage);
  if (block == nullptr || block->capture == nullptr) {
    return;
  }
  ++block->capture->copy_count;
}

extern "C" inline void ProbeDispose(void *storage) {
  auto *block = static_cast<ProbeBlockStorage *>(storage);
  if (block == nullptr || block->capture == nullptr) {
    return;
  }
  ++block->capture->dispose_count;
}

inline ProbeBlockStorage SetUpProbeBlockStorage(ProbeCaptureState *capture) {
  return ProbeBlockStorage{&ProbeInvoke, &ProbeCopy, &ProbeDispose, capture};
}

inline ProbeCaptureState SetUpProbeCaptureState() {
  return ProbeCaptureState{7, 0, 0};
}

} // namespace block_arc_runtime_abi
} // namespace probe
} // namespace runtime
} // namespace objc3c
