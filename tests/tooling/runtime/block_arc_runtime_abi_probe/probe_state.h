#pragma once

#include "runtime/objc3_runtime_bootstrap_internal.h"

namespace objc3c {
namespace runtime {
namespace probe {
namespace block_arc_runtime_abi {

struct ProbeCaptureState {
  int base = 0;
  int copy_count = 0;
  int dispose_count = 0;
};

struct ProbeBlockStorage {
  int (*invoke)(void *, int, int, int, int) = nullptr;
  void (*copy)(void *) = nullptr;
  void (*dispose)(void *) = nullptr;
  ProbeCaptureState *capture = nullptr;
};

struct RuntimeInvocationResult {
  int retained = 0;
  int autoreleased = 0;
  int released = 0;
  ProbeCaptureState capture;
  int handle = 0;
  int invoke_result = 0;
  int retain_handle_result = 0;
  int release_handle_result = 0;
  int final_release_result = 0;
};

struct ProbeResult {
  RuntimeInvocationResult runtime;
  ::objc3_runtime_block_arc_runtime_abi_snapshot abi{};
  ::objc3_runtime_arc_debug_state_snapshot arc{};
  int abi_status = 0;
  int arc_status = 0;
};

} // namespace block_arc_runtime_abi
} // namespace probe
} // namespace runtime
} // namespace objc3c
