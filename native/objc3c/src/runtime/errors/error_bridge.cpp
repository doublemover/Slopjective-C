#include "runtime/errors/error_bridge.h"

#include "runtime/errors/catch_filter.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <cstdint>
#include <string>

namespace {

thread_local std::uint64_t g_store_call_count = 0;
thread_local std::uint64_t g_load_call_count = 0;
thread_local std::uint64_t g_status_bridge_call_count = 0;
thread_local std::uint64_t g_nserror_bridge_call_count = 0;
thread_local std::uint64_t g_catch_match_call_count = 0;
thread_local int g_last_stored_error_value = 0;
thread_local int g_last_loaded_error_value = 0;
thread_local int g_last_status_bridge_status_value = 0;
thread_local int g_last_status_bridge_error_value = 0;
thread_local int g_last_nserror_bridge_error_value = 0;
thread_local int g_last_catch_match_error_value = 0;
thread_local int g_last_catch_match_kind = 0;
thread_local int g_last_catch_match_is_catch_all = 0;
thread_local int g_last_catch_match_result = 0;
thread_local std::string g_last_catch_kind_name;

const char *RuntimeErrorCatchKindName(int catch_kind) {
  switch (catch_kind) {
  case 1:
    return "nserror";
  case 2:
    return "id<error>";
  default:
    return "unknown";
  }
}

}  // namespace

namespace objc3c::runtime {

void ResetRuntimeErrorBridgeStateForTesting() {
  g_store_call_count = 0;
  g_load_call_count = 0;
  g_status_bridge_call_count = 0;
  g_nserror_bridge_call_count = 0;
  g_catch_match_call_count = 0;
  g_last_stored_error_value = 0;
  g_last_loaded_error_value = 0;
  g_last_status_bridge_status_value = 0;
  g_last_status_bridge_error_value = 0;
  g_last_nserror_bridge_error_value = 0;
  g_last_catch_match_error_value = 0;
  g_last_catch_match_kind = 0;
  g_last_catch_match_is_catch_all = 0;
  g_last_catch_match_result = 0;
  g_last_catch_kind_name.clear();
}

}  // namespace objc3c::runtime

extern "C" void objc3_runtime_store_thrown_error_i32(int *slot, int value) {
  ++g_store_call_count;
  g_last_stored_error_value = value;
  if (slot != nullptr) {
    *slot = value;
  }
}

extern "C" int objc3_runtime_load_thrown_error_i32(const int *slot) {
  ++g_load_call_count;
  const int value = slot != nullptr ? *slot : 0;
  g_last_loaded_error_value = value;
  return value;
}

extern "C" int objc3_runtime_bridge_status_error_i32(
    int status_value, int mapped_error_value) {
  ++g_status_bridge_call_count;
  g_last_status_bridge_status_value = status_value;
  const int bridged_error =
      mapped_error_value != 0 ? mapped_error_value : status_value;
  g_last_status_bridge_error_value = bridged_error;
  return bridged_error;
}

extern "C" int objc3_runtime_bridge_nserror_error_i32(int error_value) {
  ++g_nserror_bridge_call_count;
  g_last_nserror_bridge_error_value = error_value;
  return error_value;
}

extern "C" int objc3_runtime_catch_matches_error_i32(int error_value,
                                                     int catch_kind,
                                                     int catch_all) {
  ++g_catch_match_call_count;
  g_last_catch_match_error_value = error_value;
  g_last_catch_match_kind = catch_kind;
  g_last_catch_match_is_catch_all = catch_all != 0 ? 1 : 0;
  g_last_catch_kind_name = RuntimeErrorCatchKindName(catch_kind);
  const int matches =
      objc3c::runtime::RuntimeCatchFilterMatches(error_value, catch_kind,
                                                 catch_all)
          ? 1
          : 0;
  g_last_catch_match_result = matches;
  return matches;
}

extern "C" int objc3_runtime_copy_error_bridge_state_for_testing(
    objc3_runtime_error_bridge_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  snapshot->store_call_count = g_store_call_count;
  snapshot->load_call_count = g_load_call_count;
  snapshot->status_bridge_call_count = g_status_bridge_call_count;
  snapshot->nserror_bridge_call_count = g_nserror_bridge_call_count;
  snapshot->catch_match_call_count = g_catch_match_call_count;
  snapshot->last_stored_error_value = g_last_stored_error_value;
  snapshot->last_loaded_error_value = g_last_loaded_error_value;
  snapshot->last_status_bridge_status_value =
      g_last_status_bridge_status_value;
  snapshot->last_status_bridge_error_value = g_last_status_bridge_error_value;
  snapshot->last_nserror_bridge_error_value = g_last_nserror_bridge_error_value;
  snapshot->last_catch_match_error_value = g_last_catch_match_error_value;
  snapshot->last_catch_match_kind = g_last_catch_match_kind;
  snapshot->last_catch_match_is_catch_all = g_last_catch_match_is_catch_all;
  snapshot->last_catch_match_result = g_last_catch_match_result;
  snapshot->last_catch_kind_name =
      g_last_catch_kind_name.empty() ? nullptr : g_last_catch_kind_name.c_str();
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}
