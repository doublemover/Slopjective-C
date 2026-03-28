#include <cstdio>

#include "runtime/objc3_runtime.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"

int main() {
  objc3_runtime_reset_for_testing();

  int slot = 0;
  objc3_runtime_store_thrown_error_i32(&slot, 34);
  const int loaded = objc3_runtime_load_thrown_error_i32(&slot);
  const int bridged_status = objc3_runtime_bridge_status_error_i32(5, 45);
  const int bridged_nserror = objc3_runtime_bridge_nserror_error_i32(77);
  const int match_nserror =
      objc3_runtime_catch_matches_error_i32(bridged_status, 1, 0);
  const int match_protocol =
      objc3_runtime_catch_matches_error_i32(bridged_nserror, 2, 0);
  const int match_catch_all =
      objc3_runtime_catch_matches_error_i32(bridged_nserror, 0, 1);

  objc3_runtime_error_bridge_state_snapshot snapshot{};
  const int status = objc3_runtime_copy_error_bridge_state_for_testing(&snapshot);

  std::printf("status=%d\n", status);
  std::printf("loaded=%d\n", loaded);
  std::printf("bridged_status=%d\n", bridged_status);
  std::printf("bridged_nserror=%d\n", bridged_nserror);
  std::printf("match_nserror=%d\n", match_nserror);
  std::printf("match_protocol=%d\n", match_protocol);
  std::printf("match_catch_all=%d\n", match_catch_all);
  std::printf("store_call_count=%llu\n",
              static_cast<unsigned long long>(snapshot.store_call_count));
  std::printf("load_call_count=%llu\n",
              static_cast<unsigned long long>(snapshot.load_call_count));
  std::printf("status_bridge_call_count=%llu\n",
              static_cast<unsigned long long>(snapshot.status_bridge_call_count));
  std::printf("nserror_bridge_call_count=%llu\n",
              static_cast<unsigned long long>(snapshot.nserror_bridge_call_count));
  std::printf("catch_match_call_count=%llu\n",
              static_cast<unsigned long long>(snapshot.catch_match_call_count));
  std::printf("last_stored_error_value=%d\n", snapshot.last_stored_error_value);
  std::printf("last_loaded_error_value=%d\n", snapshot.last_loaded_error_value);
  std::printf("last_status_bridge_status_value=%d\n",
              snapshot.last_status_bridge_status_value);
  std::printf("last_status_bridge_error_value=%d\n",
              snapshot.last_status_bridge_error_value);
  std::printf("last_nserror_bridge_error_value=%d\n",
              snapshot.last_nserror_bridge_error_value);
  std::printf("last_catch_match_error_value=%d\n",
              snapshot.last_catch_match_error_value);
  std::printf("last_catch_match_kind=%d\n", snapshot.last_catch_match_kind);
  std::printf("last_catch_match_is_catch_all=%d\n",
              snapshot.last_catch_match_is_catch_all);
  std::printf("last_catch_match_result=%d\n",
              snapshot.last_catch_match_result);
  std::printf("last_catch_kind_name=%s\n",
              snapshot.last_catch_kind_name != nullptr
                  ? snapshot.last_catch_kind_name
                  : "<null>");

  if (status != OBJC3_RUNTIME_REGISTRATION_STATUS_OK || loaded != 34 ||
      bridged_status != 45 || bridged_nserror != 77 || match_nserror != 1 ||
      match_protocol != 1 || match_catch_all != 1 ||
      snapshot.store_call_count != 1 || snapshot.load_call_count != 1 ||
      snapshot.status_bridge_call_count != 1 ||
      snapshot.nserror_bridge_call_count != 1 ||
      snapshot.catch_match_call_count != 3 ||
      snapshot.last_status_bridge_status_value != 5 ||
      snapshot.last_status_bridge_error_value != 45 ||
      snapshot.last_nserror_bridge_error_value != 77 ||
      snapshot.last_catch_match_kind != 0 ||
      snapshot.last_catch_match_is_catch_all != 1 ||
      snapshot.last_catch_match_result != 1) {
    return 1;
  }
  return 0;
}
