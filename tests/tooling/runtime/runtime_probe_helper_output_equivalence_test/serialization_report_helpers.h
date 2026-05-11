#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_PROBE_HELPER_OUTPUT_EQUIVALENCE_TEST_SERIALIZATION_REPORT_HELPERS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_PROBE_HELPER_OUTPUT_EQUIVALENCE_TEST_SERIALIZATION_REPORT_HELPERS_H_

#include "runtime_snapshot_fixtures.h"
#include "../support/json_probe_writer.h"
#include "../support/runtime_snapshot_text.h"

#include <sstream>
#include <string>

namespace objc3c::runtime::probe::runtime_probe_helper_output_equivalence {

inline std::string SerializeRepresentativeJsonObject() {
  using ::objc3c::runtime::probe::JsonFieldSeparator;
  using ::objc3c::runtime::probe::WriteJsonBoolField;
  using ::objc3c::runtime::probe::WriteJsonIntField;
  using ::objc3c::runtime::probe::WriteJsonStringField;

  std::ostringstream json;
  JsonFieldSeparator fields;
  json << '{';
  WriteJsonStringField(json, fields, "selector", "copy");
  WriteJsonIntField(json, fields, "strict_error", 22535);
  WriteJsonBoolField(json, fields, "cache_hit", true);
  WriteJsonStringField(json, fields, "missing", nullptr);
  json << '}';
  return json.str();
}

inline std::string SerializeLabeledMethodCacheState() {
  std::ostringstream method_cache;
  ::objc3c::runtime::probe::WriteLabeledMethodCacheState(
      method_cache, "direct", MakeRepresentativeMethodCacheState(), "copy");
  return method_cache.str();
}

inline std::string SerializeLabeledFastPathMethodCacheState() {
  std::ostringstream fast_path_cache;
  ::objc3c::runtime::probe::WriteLabeledFastPathMethodCacheState(
      fast_path_cache, "fast", MakeRepresentativeFastPathMethodCacheState(),
      "copy", "selector-table-hit");
  return fast_path_cache.str();
}

inline std::string SerializeLabeledDispatchState() {
  std::ostringstream dispatch_state;
  ::objc3c::runtime::probe::WriteLabeledDispatchState(
      dispatch_state, "mixed", MakeRepresentativeDispatchState(), "copy",
      "selector-table-hit", "cache", "method", "Widget");
  return dispatch_state.str();
}

}  // namespace objc3c::runtime::probe::runtime_probe_helper_output_equivalence

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_PROBE_HELPER_OUTPUT_EQUIVALENCE_TEST_SERIALIZATION_REPORT_HELPERS_H_
