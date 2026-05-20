#include "runtime/stdlib/core_runtime_contract.h"

#include <algorithm>
#include <cstdint>
#include <mutex>

namespace {

struct RuntimeStdlibCoreState {
  std::mutex mutex;
  std::uint64_t reset_generation = 0;
  std::uint64_t total_call_count = 0;
  std::uint64_t revision_call_count = 0;
  std::uint64_t capability_call_count = 0;
  std::uint64_t option_call_count = 0;
  std::uint64_t count_call_count = 0;
  std::uint64_t prefix_call_count = 0;
  std::uint64_t map_call_count = 0;
  int last_input_a = 0;
  int last_input_b = 0;
  int last_input_c = 0;
  int last_result = 0;
};

RuntimeStdlibCoreState &State() {
  static RuntimeStdlibCoreState state;
  return state;
}

void RecordCall(RuntimeStdlibCoreState &state,
                std::uint64_t &family_count,
                int input_a,
                int input_b,
                int input_c,
                int result) {
  ++state.total_call_count;
  ++family_count;
  state.last_input_a = input_a;
  state.last_input_b = input_b;
  state.last_input_c = input_c;
  state.last_result = result;
}

int NormalizeCount(int count) {
  return std::max(count, 0);
}

}  // namespace

namespace objc3c::runtime {

void ResetRuntimeStdlibCoreStateForTesting() {
  RuntimeStdlibCoreState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  const std::uint64_t next_reset_generation = state.reset_generation + 1;
  state.total_call_count = 0;
  state.revision_call_count = 0;
  state.capability_call_count = 0;
  state.option_call_count = 0;
  state.count_call_count = 0;
  state.prefix_call_count = 0;
  state.map_call_count = 0;
  state.last_input_a = 0;
  state.last_input_b = 0;
  state.last_input_c = 0;
  state.last_result = 0;
  state.reset_generation = next_reset_generation;
}

}  // namespace objc3c::runtime

extern "C" int objc3_runtime_stdlib_core_language_revision_i32(void) {
  RuntimeStdlibCoreState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  constexpr int result = 1;
  RecordCall(state, state.revision_call_count, 0, 0, 0, result);
  return result;
}

extern "C" int objc3_runtime_stdlib_core_profile_revision_i32(void) {
  RuntimeStdlibCoreState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  constexpr int result = 1;
  RecordCall(state, state.revision_call_count, 0, 0, 0, result);
  return result;
}

extern "C" int objc3_runtime_stdlib_core_has_capability_i32(int capability) {
  RuntimeStdlibCoreState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  const int result = capability > 0 ? 1 : 0;
  RecordCall(state, state.capability_call_count, capability, 0, 0, result);
  return result;
}

extern "C" int objc3_runtime_stdlib_core_option_has_value_i32(int flag) {
  RuntimeStdlibCoreState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  const int result = flag != 0 ? 1 : 0;
  RecordCall(state, state.option_call_count, flag, 0, 0, result);
  return result;
}

extern "C" int objc3_runtime_stdlib_core_option_unwrap_or_i32(
    int flag,
    int value,
    int default_value) {
  RuntimeStdlibCoreState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  const int result = flag != 0 ? value : default_value;
  RecordCall(state, state.option_call_count, flag, value, default_value,
             result);
  return result;
}

extern "C" int objc3_runtime_stdlib_core_count_i32(int count) {
  RuntimeStdlibCoreState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  const int result = NormalizeCount(count);
  RecordCall(state, state.count_call_count, count, 0, 0, result);
  return result;
}

extern "C" int objc3_runtime_stdlib_core_prefix_count_i32(int count,
                                                          int requested) {
  RuntimeStdlibCoreState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  const int normalized_count = NormalizeCount(count);
  const int normalized_requested = NormalizeCount(requested);
  const int result = std::min(normalized_count, normalized_requested);
  RecordCall(state, state.prefix_call_count, count, requested, 0, result);
  return result;
}

extern "C" int objc3_runtime_stdlib_core_map_entry_present_i32(int flag) {
  RuntimeStdlibCoreState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  const int result = flag != 0 ? 1 : 0;
  RecordCall(state, state.map_call_count, flag, 0, 0, result);
  return result;
}

extern "C" int objc3_runtime_stdlib_core_map_entry_value_or_i32(
    int flag,
    int value,
    int default_value) {
  RuntimeStdlibCoreState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  const int result = flag != 0 ? value : default_value;
  RecordCall(state, state.map_call_count, flag, value, default_value, result);
  return result;
}

extern "C" int objc3_runtime_copy_stdlib_core_state_for_testing(
    objc3_runtime_stdlib_core_snapshot *out) {
  if (out == nullptr) {
    return -1;
  }
  RuntimeStdlibCoreState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  out->reset_generation = state.reset_generation;
  out->total_call_count = state.total_call_count;
  out->revision_call_count = state.revision_call_count;
  out->capability_call_count = state.capability_call_count;
  out->option_call_count = state.option_call_count;
  out->count_call_count = state.count_call_count;
  out->prefix_call_count = state.prefix_call_count;
  out->map_call_count = state.map_call_count;
  out->last_input_a = state.last_input_a;
  out->last_input_b = state.last_input_b;
  out->last_input_c = state.last_input_c;
  out->last_result = state.last_result;
  return 0;
}
