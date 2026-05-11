#pragma once

#include "probe_state.h"

#include "support/json_probe_writer.h"

#include <cstdio>

namespace objc3c {
namespace runtime {
namespace probe {
namespace block_arc_runtime_abi {

inline void PrintRuntimeInvocationReport(
    const RuntimeInvocationResult &runtime) {
  std::printf("\"retained\":%d,", runtime.retained);
  std::printf("\"autoreleased\":%d,", runtime.autoreleased);
  std::printf("\"released\":%d,", runtime.released);
  std::printf("\"handle\":%d,", runtime.handle);
  std::printf("\"invoke_result\":%d,", runtime.invoke_result);
  std::printf("\"retain_handle_result\":%d,", runtime.retain_handle_result);
  std::printf("\"release_handle_result\":%d,", runtime.release_handle_result);
  std::printf("\"final_release_result\":%d,", runtime.final_release_result);
  std::printf("\"copy_count\":%d,", runtime.capture.copy_count);
  std::printf("\"dispose_count\":%d,", runtime.capture.dispose_count);
}

inline void PrintAbiStatusAndCounterReport(
    const ::objc3_runtime_block_arc_runtime_abi_snapshot &abi,
    int abi_status,
    int arc_status) {
  std::printf("\"abi_status\":%d,", abi_status);
  std::printf("\"arc_status\":%d,", arc_status);
  std::printf("\"private_runtime_abi_ready\":%llu,",
              static_cast<unsigned long long>(abi.private_runtime_abi_ready));
  std::printf("\"public_runtime_header_unchanged\":%llu,",
              static_cast<unsigned long long>(
                  abi.public_runtime_header_unchanged));
  std::printf("\"deterministic\":%llu,",
              static_cast<unsigned long long>(abi.deterministic));
  std::printf("\"live_runtime_block_handle_count\":%llu,",
              static_cast<unsigned long long>(
                  abi.live_runtime_block_handle_count));
  std::printf("\"block_promote_call_count\":%llu,",
              static_cast<unsigned long long>(abi.block_promote_call_count));
  std::printf("\"block_invoke_call_count\":%llu,",
              static_cast<unsigned long long>(abi.block_invoke_call_count));
  std::printf("\"retain_call_count\":%llu,",
              static_cast<unsigned long long>(abi.retain_call_count));
  std::printf("\"release_call_count\":%llu,",
              static_cast<unsigned long long>(abi.release_call_count));
  std::printf("\"autorelease_call_count\":%llu,",
              static_cast<unsigned long long>(abi.autorelease_call_count));
  std::printf("\"autoreleasepool_push_count\":%llu,",
              static_cast<unsigned long long>(abi.autoreleasepool_push_count));
  std::printf("\"autoreleasepool_pop_count\":%llu,",
              static_cast<unsigned long long>(abi.autoreleasepool_pop_count));
  std::printf("\"current_property_read_count\":%llu,",
              static_cast<unsigned long long>(abi.current_property_read_count));
  std::printf("\"current_property_write_count\":%llu,",
              static_cast<unsigned long long>(abi.current_property_write_count));
  std::printf("\"current_property_exchange_count\":%llu,",
              static_cast<unsigned long long>(
                  abi.current_property_exchange_count));
  std::printf("\"weak_current_property_load_count\":%llu,",
              static_cast<unsigned long long>(
                  abi.weak_current_property_load_count));
  std::printf("\"weak_current_property_store_count\":%llu,",
              static_cast<unsigned long long>(
                  abi.weak_current_property_store_count));
}

inline void PrintAbiLastValueReport(
    const ::objc3_runtime_block_arc_runtime_abi_snapshot &abi) {
  std::printf("\"last_promoted_block_handle\":%d,",
              abi.last_promoted_block_handle);
  std::printf("\"last_promote_has_pointer_capture_storage\":%d,",
              abi.last_promote_has_pointer_capture_storage);
  std::printf("\"last_invoked_block_handle\":%d,",
              abi.last_invoked_block_handle);
  std::printf("\"last_block_invoke_result\":%d,",
              abi.last_block_invoke_result);
  std::printf("\"last_retain_value\":%d,", abi.last_retain_value);
  std::printf("\"last_release_value\":%d,", abi.last_release_value);
  std::printf("\"last_autorelease_value\":%d,", abi.last_autorelease_value);
}

inline void PrintArcCounterReport(
    const ::objc3_runtime_arc_debug_state_snapshot &arc) {
  std::printf("\"arc_retain_call_count\":%llu,",
              static_cast<unsigned long long>(arc.retain_call_count));
  std::printf("\"arc_release_call_count\":%llu,",
              static_cast<unsigned long long>(arc.release_call_count));
  std::printf("\"arc_autorelease_call_count\":%llu,",
              static_cast<unsigned long long>(arc.autorelease_call_count));
  std::printf("\"arc_autoreleasepool_push_count\":%llu,",
              static_cast<unsigned long long>(arc.autoreleasepool_push_count));
  std::printf("\"arc_autoreleasepool_pop_count\":%llu,",
              static_cast<unsigned long long>(arc.autoreleasepool_pop_count));
}

inline void PrintAbiSymbolReport(
    const ::objc3_runtime_block_arc_runtime_abi_snapshot &abi) {
  std::printf("\"block_promote_symbol\":");
  ::objc3c::runtime::probe::PrintJsonStringOrNull(abi.block_promote_symbol);
  std::printf(",\"block_invoke_symbol\":");
  ::objc3c::runtime::probe::PrintJsonStringOrNull(abi.block_invoke_symbol);
  std::printf(",\"retain_symbol\":");
  ::objc3c::runtime::probe::PrintJsonStringOrNull(abi.retain_symbol);
  std::printf(",\"release_symbol\":");
  ::objc3c::runtime::probe::PrintJsonStringOrNull(abi.release_symbol);
  std::printf(",\"autorelease_symbol\":");
  ::objc3c::runtime::probe::PrintJsonStringOrNull(abi.autorelease_symbol);
  std::printf(",\"autoreleasepool_push_symbol\":");
  ::objc3c::runtime::probe::PrintJsonStringOrNull(
      abi.autoreleasepool_push_symbol);
  std::printf(",\"autoreleasepool_pop_symbol\":");
  ::objc3c::runtime::probe::PrintJsonStringOrNull(
      abi.autoreleasepool_pop_symbol);
  std::printf(",\"current_property_read_symbol\":");
  ::objc3c::runtime::probe::PrintJsonStringOrNull(
      abi.current_property_read_symbol);
  std::printf(",\"current_property_write_symbol\":");
  ::objc3c::runtime::probe::PrintJsonStringOrNull(
      abi.current_property_write_symbol);
  std::printf(",\"current_property_exchange_symbol\":");
  ::objc3c::runtime::probe::PrintJsonStringOrNull(
      abi.current_property_exchange_symbol);
  std::printf(",\"bind_current_property_context_symbol\":");
  ::objc3c::runtime::probe::PrintJsonStringOrNull(
      abi.bind_current_property_context_symbol);
  std::printf(",\"clear_current_property_context_symbol\":");
  ::objc3c::runtime::probe::PrintJsonStringOrNull(
      abi.clear_current_property_context_symbol);
  std::printf(",\"weak_current_property_load_symbol\":");
  ::objc3c::runtime::probe::PrintJsonStringOrNull(
      abi.weak_current_property_load_symbol);
  std::printf(",\"weak_current_property_store_symbol\":");
  ::objc3c::runtime::probe::PrintJsonStringOrNull(
      abi.weak_current_property_store_symbol);
  std::printf(",\"arc_debug_state_snapshot_symbol\":");
  ::objc3c::runtime::probe::PrintJsonStringOrNull(
      abi.arc_debug_state_snapshot_symbol);
  std::printf(",\"runtime_abi_boundary_model\":");
  ::objc3c::runtime::probe::PrintJsonStringOrNull(
      abi.runtime_abi_boundary_model);
  std::printf(",\"block_runtime_model\":");
  ::objc3c::runtime::probe::PrintJsonStringOrNull(abi.block_runtime_model);
  std::printf(",\"arc_runtime_model\":");
  ::objc3c::runtime::probe::PrintJsonStringOrNull(abi.arc_runtime_model);
  std::printf(",\"fail_closed_model\":");
  ::objc3c::runtime::probe::PrintJsonStringOrNull(abi.fail_closed_model);
}

inline void PrintBlockArcRuntimeAbiProbeReport(const ProbeResult &result) {
  std::printf("{");
  PrintRuntimeInvocationReport(result.runtime);
  PrintAbiStatusAndCounterReport(result.abi, result.abi_status,
                                 result.arc_status);
  PrintAbiLastValueReport(result.abi);
  PrintArcCounterReport(result.arc);
  PrintAbiSymbolReport(result.abi);
  std::printf("}");
}

} // namespace block_arc_runtime_abi
} // namespace probe
} // namespace runtime
} // namespace objc3c
