#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_SUPPORT_RUNTIME_SNAPSHOT_TEXT_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_SUPPORT_RUNTIME_SNAPSHOT_TEXT_H_

#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <ostream>
#include <string>

namespace objc3c::runtime::probe {

inline void
WriteLabeledDispatchState(std::ostream &out, const char *label,
                          const objc3_runtime_dispatch_state_snapshot &snapshot,
                          const std::string &last_selector,
                          const std::string &last_fast_path_reason,
                          const std::string &last_dispatch_path,
                          const std::string &last_implementation_kind,
                          const std::string &last_resolved_class_name) {
  out << label << "_cache_entry_count=" << snapshot.cache_entry_count << "\n";
  out << label << "_fast_path_seed_count=" << snapshot.fast_path_seed_count
      << "\n";
  out << label << "_fast_path_hit_count=" << snapshot.fast_path_hit_count
      << "\n";
  out << label << "_live_dispatch_count=" << snapshot.live_dispatch_count
      << "\n";
  out << label
      << "_strict_dispatch_error_count=" << snapshot.strict_dispatch_error_count
      << "\n";
  out << label << "_last_resolved_parameter_count="
      << snapshot.last_resolved_parameter_count << "\n";
  out << label
      << "_last_dispatch_used_cache=" << snapshot.last_dispatch_used_cache
      << "\n";
  out << label << "_last_dispatch_used_fast_path="
      << snapshot.last_dispatch_used_fast_path << "\n";
  out << label << "_last_dispatch_resolved_live_method="
      << snapshot.last_dispatch_resolved_live_method << "\n";
  out << label
      << "_last_dispatch_strict_error=" << snapshot.last_dispatch_strict_error
      << "\n";
  out << label << "_last_effective_direct_dispatch="
      << snapshot.last_effective_direct_dispatch << "\n";
  out << label << "_last_used_builtin=" << snapshot.last_used_builtin << "\n";
  out << label << "_last_selector=" << last_selector << "\n";
  out << label << "_last_fast_path_reason=" << last_fast_path_reason << "\n";
  out << label << "_last_dispatch_path=" << last_dispatch_path << "\n";
  out << label << "_last_implementation_kind=" << last_implementation_kind
      << "\n";
  out << label << "_last_resolved_class_name=" << last_resolved_class_name
      << "\n";
}

inline void WriteLabeledMethodCacheState(
    std::ostream &out, const char *label,
    const objc3_runtime_method_cache_state_snapshot &snapshot,
    const std::string &last_selector) {
  out << label << "_cache_entry_count=" << snapshot.cache_entry_count << "\n";
  out << label << "_cache_hit_count=" << snapshot.cache_hit_count << "\n";
  out << label << "_cache_miss_count=" << snapshot.cache_miss_count << "\n";
  out << label << "_slow_path_lookup_count=" << snapshot.slow_path_lookup_count
      << "\n";
  out << label << "_live_dispatch_count=" << snapshot.live_dispatch_count
      << "\n";
  out << label
      << "_strict_dispatch_error_count=" << snapshot.strict_dispatch_error_count
      << "\n";
  out << label
      << "_last_dispatch_used_cache=" << snapshot.last_dispatch_used_cache
      << "\n";
  out << label << "_last_dispatch_resolved_live_method="
      << snapshot.last_dispatch_resolved_live_method << "\n";
  out << label
      << "_last_dispatch_strict_error=" << snapshot.last_dispatch_strict_error
      << "\n";
  out << label << "_last_selector=" << last_selector << "\n";
}

inline void WriteLabeledFastPathMethodCacheState(
    std::ostream &out, const char *label,
    const objc3_runtime_method_cache_state_snapshot &snapshot,
    const std::string &last_selector,
    const std::string &last_fast_path_reason) {
  out << label << "_cache_entry_count=" << snapshot.cache_entry_count << "\n";
  out << label << "_cache_hit_count=" << snapshot.cache_hit_count << "\n";
  out << label << "_cache_miss_count=" << snapshot.cache_miss_count << "\n";
  out << label << "_slow_path_lookup_count=" << snapshot.slow_path_lookup_count
      << "\n";
  out << label << "_live_dispatch_count=" << snapshot.live_dispatch_count
      << "\n";
  out << label
      << "_strict_dispatch_error_count=" << snapshot.strict_dispatch_error_count
      << "\n";
  out << label << "_fast_path_seed_count=" << snapshot.fast_path_seed_count
      << "\n";
  out << label << "_fast_path_hit_count=" << snapshot.fast_path_hit_count
      << "\n";
  out << label
      << "_last_dispatch_used_cache=" << snapshot.last_dispatch_used_cache
      << "\n";
  out << label << "_last_dispatch_used_fast_path="
      << snapshot.last_dispatch_used_fast_path << "\n";
  out << label << "_last_dispatch_resolved_live_method="
      << snapshot.last_dispatch_resolved_live_method << "\n";
  out << label
      << "_last_dispatch_strict_error=" << snapshot.last_dispatch_strict_error
      << "\n";
  out << label << "_last_selector=" << last_selector << "\n";
  out << label << "_last_fast_path_reason=" << last_fast_path_reason << "\n";
}

} // namespace objc3c::runtime::probe

#endif // OBJC3C_TESTS_TOOLING_RUNTIME_SUPPORT_RUNTIME_SNAPSHOT_TEXT_H_
