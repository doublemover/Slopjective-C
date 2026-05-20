#pragma once

#include <cstdint>
#include <string>

namespace objc3c::runtime {

struct RuntimeErrorBridgeState {
  std::uint64_t store_call_count = 0;
  std::uint64_t load_call_count = 0;
  std::uint64_t status_bridge_call_count = 0;
  std::uint64_t nserror_bridge_call_count = 0;
  std::uint64_t foreign_exception_bridge_call_count = 0;
  std::uint64_t catch_match_call_count = 0;
  int last_stored_error_value = 0;
  int last_loaded_error_value = 0;
  int last_status_bridge_status_value = 0;
  int last_status_bridge_error_value = 0;
  int last_nserror_bridge_error_value = 0;
  int last_foreign_exception_kind = 0;
  int last_foreign_exception_payload_value = 0;
  int last_foreign_exception_mapped_error_value = 0;
  int last_foreign_exception_bridge_result = 0;
  int last_catch_match_error_value = 0;
  int last_catch_match_kind = 0;
  int last_catch_match_is_catch_all = 0;
  int last_catch_match_result = 0;
  std::string last_foreign_exception_kind_name;
  std::string last_catch_kind_name;
};

RuntimeErrorBridgeState &RuntimeErrorBridgeThreadState();
void ResetRuntimeErrorBridgeThreadState();

}  // namespace objc3c::runtime
