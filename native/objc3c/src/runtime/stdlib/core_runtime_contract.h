#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct objc3_runtime_stdlib_core_snapshot {
  uint64_t reset_generation;
  uint64_t total_call_count;
  uint64_t revision_call_count;
  uint64_t capability_call_count;
  uint64_t option_call_count;
  uint64_t count_call_count;
  uint64_t prefix_call_count;
  uint64_t map_call_count;
  int last_input_a;
  int last_input_b;
  int last_input_c;
  int last_result;
} objc3_runtime_stdlib_core_snapshot;

typedef enum objc3_runtime_stdlib_core_capability_i32 {
  OBJC3_RUNTIME_STDLIB_CORE_CAPABILITY_CORE = 1,
  OBJC3_RUNTIME_STDLIB_CORE_CAPABILITY_ERRORS = 2,
  OBJC3_RUNTIME_STDLIB_CORE_CAPABILITY_CONCURRENCY = 3,
  OBJC3_RUNTIME_STDLIB_CORE_CAPABILITY_KEYPATH = 4,
  OBJC3_RUNTIME_STDLIB_CORE_CAPABILITY_SYSTEM = 5,
} objc3_runtime_stdlib_core_capability_i32;

int objc3_runtime_stdlib_core_language_revision_i32(void);
int objc3_runtime_stdlib_core_profile_revision_i32(void);
int objc3_runtime_stdlib_core_has_capability_i32(int capability);
int objc3_runtime_stdlib_core_option_has_value_i32(int flag);
int objc3_runtime_stdlib_core_option_unwrap_or_i32(int flag,
                                                   int value,
                                                   int default_value);
int objc3_runtime_stdlib_core_count_i32(int count);
int objc3_runtime_stdlib_core_prefix_count_i32(int count, int requested);
int objc3_runtime_stdlib_core_map_entry_present_i32(int flag);
int objc3_runtime_stdlib_core_map_entry_value_or_i32(int flag,
                                                     int value,
                                                     int default_value);
int objc3_runtime_copy_stdlib_core_state_for_testing(
    objc3_runtime_stdlib_core_snapshot *out);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
namespace objc3c::runtime {

void ResetRuntimeStdlibCoreStateForTesting();

}  // namespace objc3c::runtime
#endif
