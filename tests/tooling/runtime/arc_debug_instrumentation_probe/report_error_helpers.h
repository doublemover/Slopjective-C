#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_ARC_DEBUG_INSTRUMENTATION_PROBE_REPORT_ERROR_HELPERS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_ARC_DEBUG_INSTRUMENTATION_PROBE_REPORT_ERROR_HELPERS_H_

#include "probe_state.h"

#include "support/json_probe_writer.h"

#include <cstdio>

namespace objc3c::runtime::probe::arc_debug_instrumentation {

inline void PrintArcDebugSnapshot(const StableArcDebugSnapshot &state) {
  const ::objc3_runtime_arc_debug_state_snapshot &snapshot = state.snapshot;

  // Stable JSON field anchors for tooling contracts:
  // "last_property_name"
  // "last_property_owner_identity"
  std::printf("{");
  std::printf("\"retain_call_count\":%llu,",
              static_cast<unsigned long long>(snapshot.retain_call_count));
  std::printf("\"release_call_count\":%llu,",
              static_cast<unsigned long long>(snapshot.release_call_count));
  std::printf("\"autorelease_call_count\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.autorelease_call_count));
  std::printf("\"autoreleasepool_push_count\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.autoreleasepool_push_count));
  std::printf("\"autoreleasepool_pop_count\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.autoreleasepool_pop_count));
  std::printf("\"current_property_read_count\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.current_property_read_count));
  std::printf("\"current_property_write_count\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.current_property_write_count));
  std::printf("\"current_property_exchange_count\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.current_property_exchange_count));
  std::printf("\"weak_current_property_load_count\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.weak_current_property_load_count));
  std::printf("\"weak_current_property_store_count\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.weak_current_property_store_count));
  std::printf("\"last_retain_value\":%d,", snapshot.last_retain_value);
  std::printf("\"last_release_value\":%d,", snapshot.last_release_value);
  std::printf("\"last_autorelease_value\":%d,",
              snapshot.last_autorelease_value);
  std::printf("\"last_property_read_value\":%d,",
              snapshot.last_property_read_value);
  std::printf("\"last_property_written_value\":%d,",
              snapshot.last_property_written_value);
  std::printf("\"last_property_exchange_previous_value\":%d,",
              snapshot.last_property_exchange_previous_value);
  std::printf("\"last_property_exchange_new_value\":%d,",
              snapshot.last_property_exchange_new_value);
  std::printf("\"last_weak_loaded_value\":%d,",
              snapshot.last_weak_loaded_value);
  std::printf("\"last_weak_stored_value\":%d,",
              snapshot.last_weak_stored_value);
  std::printf("\"last_property_receiver\":%d,",
              snapshot.last_property_receiver);
  std::printf("\"last_property_name\":");
  ::objc3c::runtime::probe::PrintJsonStringOrNull(
      snapshot.last_property_name);
  std::printf(",\"last_property_owner_identity\":");
  ::objc3c::runtime::probe::PrintJsonStringOrNull(
      snapshot.last_property_owner_identity);
  std::printf("}");
}

inline void PrintArcDebugInstrumentationReport(const ProbeRun &run) {
  const ArcDebugFixture &fixture = run.fixture;
  const ArcInstrumentationResults &instrumentation = run.instrumentation;

  std::printf("{");
  std::printf("\"parent\":%d,", fixture.parent);
  std::printf("\"child\":%d,", fixture.child);
  std::printf("\"bind_current_status\":%d,",
              instrumentation.bind_current_status);
  std::printf("\"strong_set_result\":%d,",
              instrumentation.strong_set_result);
  std::printf("\"release_local_result\":%d,",
              instrumentation.release_local_result);
  std::printf("\"retained\":%d,", instrumentation.retained);
  std::printf("\"autoreleased\":%d,", instrumentation.autoreleased);
  std::printf("\"getter_value\":%d,", instrumentation.getter_value);
  std::printf("\"bind_weak_status\":%d,",
              instrumentation.bind_weak_status);
  std::printf("\"weak_set_result\":%d,", instrumentation.weak_set_result);
  std::printf("\"rebind_current_status\":%d,",
              instrumentation.rebind_current_status);
  std::printf("\"clear_strong_result\":%d,",
              instrumentation.clear_strong_result);
  std::printf("\"rebind_weak_status\":%d,",
              instrumentation.rebind_weak_status);
  std::printf("\"weak_inside_pool\":%d,",
              instrumentation.weak_inside_pool);
  std::printf("\"weak_after_pool\":%d,",
              instrumentation.weak_after_pool);
  std::printf("\"released\":%d,", instrumentation.released);
  std::printf("\"parent_release_result\":%d,",
              instrumentation.parent_release_result);
  std::printf("\"inside\":");
  PrintArcDebugSnapshot(run.inside);
  std::printf(",\"after\":");
  PrintArcDebugSnapshot(run.after);
  std::printf("}\n");
}

}  // namespace objc3c::runtime::probe::arc_debug_instrumentation

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_ARC_DEBUG_INSTRUMENTATION_PROBE_REPORT_ERROR_HELPERS_H_
