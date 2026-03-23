#include "runtime/objc3_runtime.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <cstdio>

extern "C" int runCase();

int main() {
  objc3_runtime_reset_for_testing();

  const int rc = runCase();
  objc3_runtime_error_bridge_state_snapshot snapshot{};
  const int status = objc3_runtime_copy_error_bridge_state_for_testing(&snapshot);

  std::printf("{");
  std::printf("\"rc\":%d,", rc);
  std::printf("\"status\":%d,", status);
  std::printf("\"store_call_count\":%llu,",
              static_cast<unsigned long long>(snapshot.store_call_count));
  std::printf("\"load_call_count\":%llu,",
              static_cast<unsigned long long>(snapshot.load_call_count));
  std::printf("\"status_bridge_call_count\":%llu,",
              static_cast<unsigned long long>(snapshot.status_bridge_call_count));
  std::printf("\"nserror_bridge_call_count\":%llu,",
              static_cast<unsigned long long>(snapshot.nserror_bridge_call_count));
  std::printf("\"catch_match_call_count\":%llu,",
              static_cast<unsigned long long>(snapshot.catch_match_call_count));
  std::printf("\"last_stored_error_value\":%d,", snapshot.last_stored_error_value);
  std::printf("\"last_loaded_error_value\":%d,", snapshot.last_loaded_error_value);
  std::printf("\"last_status_bridge_status_value\":%d,",
              snapshot.last_status_bridge_status_value);
  std::printf("\"last_status_bridge_error_value\":%d,",
              snapshot.last_status_bridge_error_value);
  std::printf("\"last_catch_match_kind\":%d,", snapshot.last_catch_match_kind);
  std::printf("\"last_catch_match_is_catch_all\":%d,",
              snapshot.last_catch_match_is_catch_all);
  std::printf("\"last_catch_match_result\":%d,",
              snapshot.last_catch_match_result);
  std::printf("\"last_catch_kind_name\":\"%s\"",
              snapshot.last_catch_kind_name != nullptr
                  ? snapshot.last_catch_kind_name
                  : "");
  std::printf("}\n");
  return 0;
}
