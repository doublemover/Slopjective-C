#include "runtime/public/objc3_runtime_ownership_contract.h"
#include "runtime/stdlib/core_runtime_contract.h"

#include <iostream>

namespace {

int Fail(const char *message) {
  std::cerr << "stdlib-core-runtime-probe: " << message << "\n";
  return 1;
}

}  // namespace

int main() {
  objc3_runtime_reset_for_testing();

  if (objc3_runtime_stdlib_core_language_revision_i32() != 1) {
    return Fail("language revision did not come from runtime helper");
  }
  if (objc3_runtime_stdlib_core_profile_revision_i32() != 1) {
    return Fail("profile revision did not come from runtime helper");
  }
  if (objc3_runtime_stdlib_core_has_capability_i32(
          OBJC3_RUNTIME_STDLIB_CORE_CAPABILITY_CORE) != 1 ||
      objc3_runtime_stdlib_core_has_capability_i32(
          OBJC3_RUNTIME_STDLIB_CORE_CAPABILITY_KEYPATH) != 1 ||
      objc3_runtime_stdlib_core_has_capability_i32(
          OBJC3_RUNTIME_STDLIB_CORE_CAPABILITY_SYSTEM) != 0 ||
      objc3_runtime_stdlib_core_has_capability_i32(99) != 0 ||
      objc3_runtime_stdlib_core_has_capability_i32(-7) != 0) {
    return Fail("capability helper did not fail closed for unsupported ids");
  }
  if (objc3_runtime_stdlib_core_option_has_value_i32(4) != 1 ||
      objc3_runtime_stdlib_core_option_has_value_i32(0) != 0) {
    return Fail("option presence helper drifted");
  }
  if (objc3_runtime_stdlib_core_option_unwrap_or_i32(1, 11, 13) != 11 ||
      objc3_runtime_stdlib_core_option_unwrap_or_i32(0, 11, 13) != 13) {
    return Fail("option unwrap helper drifted");
  }
  if (objc3_runtime_stdlib_core_count_i32(-5) != 0 ||
      objc3_runtime_stdlib_core_count_i32(6) != 6) {
    return Fail("count helper did not apply zero floor");
  }
  if (objc3_runtime_stdlib_core_prefix_count_i32(9, 4) != 4 ||
      objc3_runtime_stdlib_core_prefix_count_i32(-9, 4) != 0 ||
      objc3_runtime_stdlib_core_prefix_count_i32(9, -4) != 0) {
    return Fail("prefix helper did not clamp count/requested values");
  }
  if (objc3_runtime_stdlib_core_map_entry_present_i32(3) != 1 ||
      objc3_runtime_stdlib_core_map_entry_present_i32(0) != 0) {
    return Fail("map presence helper drifted");
  }
  if (objc3_runtime_stdlib_core_map_entry_value_or_i32(1, 17, 19) != 17 ||
      objc3_runtime_stdlib_core_map_entry_value_or_i32(0, 17, 19) != 19) {
    return Fail("map value helper drifted");
  }

  objc3_runtime_stdlib_core_snapshot snapshot{};
  if (objc3_runtime_copy_stdlib_core_state_for_testing(&snapshot) != 0) {
    return Fail("snapshot copy failed");
  }
  if (snapshot.total_call_count != 20 || snapshot.revision_call_count != 2 ||
      snapshot.capability_call_count != 5 || snapshot.option_call_count != 4 ||
      snapshot.count_call_count != 2 || snapshot.prefix_call_count != 3 ||
      snapshot.map_call_count != 4) {
    return Fail("runtime helper call counters drifted");
  }
  if (snapshot.last_input_a != 0 || snapshot.last_input_b != 17 ||
      snapshot.last_input_c != 19 || snapshot.last_result != 19) {
    return Fail("runtime helper snapshot did not preserve last call");
  }

  std::cout << "{"
            << "\"total_call_count\":" << snapshot.total_call_count
            << ",\"revision_call_count\":" << snapshot.revision_call_count
            << ",\"capability_call_count\":" << snapshot.capability_call_count
            << ",\"option_call_count\":" << snapshot.option_call_count
            << ",\"count_call_count\":" << snapshot.count_call_count
            << ",\"prefix_call_count\":" << snapshot.prefix_call_count
            << ",\"map_call_count\":" << snapshot.map_call_count
            << ",\"last_result\":" << snapshot.last_result << "}\n";
  return 0;
}
