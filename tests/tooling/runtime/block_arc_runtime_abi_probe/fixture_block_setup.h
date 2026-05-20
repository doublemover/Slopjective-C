#pragma once

#include "probe_state.h"

namespace objc3c {
namespace runtime {
namespace probe {
namespace block_arc_runtime_abi {

inline ProbeCaptureState &ProbeCounters() {
  static ProbeCaptureState counters;
  return counters;
}

inline void ResetProbeCounters(int base) {
  ProbeCounters() = ProbeCaptureState{base, 0, 0};
}

inline ProbeCaptureState CaptureProbeCounters() {
  return ProbeCounters();
}

extern "C" inline int ProbeInvoke(void *storage,
                                  int a0,
                                  int a1,
                                  int a2,
                                  int a3) {
  auto *block = static_cast<ProbeBlockStorage *>(storage);
  if (block == nullptr || block->captured_base == nullptr) {
    return -1;
  }
  return *block->captured_base + a0 + a1 + a2 + a3;
}

extern "C" inline void ProbeCopy(void *storage) {
  auto *block = static_cast<ProbeBlockStorage *>(storage);
  if (block == nullptr || block->captured_base == nullptr) {
    return;
  }
  ++ProbeCounters().copy_count;
}

extern "C" inline void ProbeDispose(void *storage) {
  auto *block = static_cast<ProbeBlockStorage *>(storage);
  if (block == nullptr || block->captured_base == nullptr) {
    return;
  }
  ++ProbeCounters().dispose_count;
}

inline const ::objc3c::runtime::RuntimeBlockDescriptor &ProbeDescriptor() {
  static const ::objc3c::runtime::RuntimeBlockDescriptor descriptor{
      sizeof(ProbeBlockStorage),
      1,
      4,
      ::objc3c::runtime::kRuntimeBlockDescriptorPointerCaptureStorageFlag |
          ::objc3c::runtime::kRuntimeBlockDescriptorCopyHelperFlag |
          ::objc3c::runtime::kRuntimeBlockDescriptorDisposeHelperFlag,
      0,
      &ProbeInvoke};
  return descriptor;
}

inline ProbeBlockStorage SetUpProbeBlockStorage(int *captured_base) {
  return ProbeBlockStorage{&ProbeDescriptor(), &ProbeCopy, &ProbeDispose,
                           captured_base};
}

inline int SetUpCapturedBaseCell() {
  return 7;
}

} // namespace block_arc_runtime_abi
} // namespace probe
} // namespace runtime
} // namespace objc3c
