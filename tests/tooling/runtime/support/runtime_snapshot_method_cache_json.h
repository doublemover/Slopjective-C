#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_SUPPORT_RUNTIME_SNAPSHOT_METHOD_CACHE_JSON_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_SUPPORT_RUNTIME_SNAPSHOT_METHOD_CACHE_JSON_H_

#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "support/json_probe_writer.h"

#include <cstdio>

namespace objc3c::runtime::probe {

inline void PrintMethodCacheEntryWithProbeCounts(
    const objc3_runtime_method_cache_entry_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"found\":%d,", snapshot.found);
  std::printf("\"resolved\":%d,", snapshot.resolved);
  std::printf("\"dispatch_family_is_class\":%d,",
              snapshot.dispatch_family_is_class);
  std::printf(
      "\"lookup_start_base_identity\":%llu,",
      static_cast<unsigned long long>(snapshot.lookup_start_base_identity));
  std::printf(
      "\"normalized_receiver_identity\":%llu,",
      static_cast<unsigned long long>(snapshot.normalized_receiver_identity));
  std::printf("\"selector_stable_id\":%llu,",
              static_cast<unsigned long long>(snapshot.selector_stable_id));
  std::printf("\"parameter_count\":%llu,",
              static_cast<unsigned long long>(snapshot.parameter_count));
  std::printf("\"category_probe_count\":%llu,",
              static_cast<unsigned long long>(snapshot.category_probe_count));
  std::printf("\"protocol_probe_count\":%llu,",
              static_cast<unsigned long long>(snapshot.protocol_probe_count));
  std::printf("\"selector\":");
  PrintJsonStringOrNull(snapshot.selector);
  std::printf(",\"resolved_class_name\":");
  PrintJsonStringOrNull(snapshot.resolved_class_name);
  std::printf(",\"resolved_owner_identity\":");
  PrintJsonStringOrNull(snapshot.resolved_owner_identity);
  std::printf("}");
}

inline void PrintMethodCacheEntryBasic(
    const objc3_runtime_method_cache_entry_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"found\":%d,", snapshot.found);
  std::printf("\"resolved\":%d,", snapshot.resolved);
  std::printf("\"dispatch_family_is_class\":%d,",
              snapshot.dispatch_family_is_class);
  std::printf(
      "\"lookup_start_base_identity\":%llu,",
      static_cast<unsigned long long>(snapshot.lookup_start_base_identity));
  std::printf(
      "\"normalized_receiver_identity\":%llu,",
      static_cast<unsigned long long>(snapshot.normalized_receiver_identity));
  std::printf("\"selector_stable_id\":%llu,",
              static_cast<unsigned long long>(snapshot.selector_stable_id));
  std::printf("\"parameter_count\":%llu,",
              static_cast<unsigned long long>(snapshot.parameter_count));
  std::printf("\"selector\":");
  PrintJsonStringOrNull(snapshot.selector);
  std::printf(",\"resolved_class_name\":");
  PrintJsonStringOrNull(snapshot.resolved_class_name);
  std::printf(",\"resolved_owner_identity\":");
  PrintJsonStringOrNull(snapshot.resolved_owner_identity);
  std::printf("}");
}

inline void PrintMethodCacheEntryMetaclassMinimal(
    const objc3_runtime_method_cache_entry_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"found\":%d,", snapshot.found);
  std::printf("\"resolved\":%d,", snapshot.resolved);
  std::printf("\"dispatch_family_is_class\":%d,",
              snapshot.dispatch_family_is_class);
  std::printf(
      "\"lookup_start_base_identity\":%llu,",
      static_cast<unsigned long long>(snapshot.lookup_start_base_identity));
  std::printf(
      "\"normalized_receiver_identity\":%llu,",
      static_cast<unsigned long long>(snapshot.normalized_receiver_identity));
  std::printf("\"selector\":");
  PrintJsonStringOrNull(snapshot.selector);
  std::printf(",\"resolved_class_name\":");
  PrintJsonStringOrNull(snapshot.resolved_class_name);
  std::printf(",\"resolved_owner_identity\":");
  PrintJsonStringOrNull(snapshot.resolved_owner_identity);
  std::printf("}");
}

inline void PrintMethodCacheEntryRuntimeCanonical(
    const objc3_runtime_method_cache_entry_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"found\":%d,", snapshot.found);
  std::printf("\"resolved\":%d,", snapshot.resolved);
  std::printf("\"dispatch_family_is_class\":%d,",
              snapshot.dispatch_family_is_class);
  std::printf(
      "\"lookup_start_base_identity\":%llu,",
      static_cast<unsigned long long>(snapshot.lookup_start_base_identity));
  std::printf(
      "\"normalized_receiver_identity\":%llu,",
      static_cast<unsigned long long>(snapshot.normalized_receiver_identity));
  std::printf("\"selector_stable_id\":%llu,",
              static_cast<unsigned long long>(snapshot.selector_stable_id));
  std::printf("\"category_probe_count\":%llu,",
              static_cast<unsigned long long>(snapshot.category_probe_count));
  std::printf("\"protocol_probe_count\":%llu,",
              static_cast<unsigned long long>(snapshot.protocol_probe_count));
  std::printf("\"selector\":");
  PrintJsonStringOrNull(snapshot.selector);
  std::printf(",\"resolved_class_name\":");
  PrintJsonStringOrNull(snapshot.resolved_class_name);
  std::printf(",\"resolved_owner_identity\":");
  PrintJsonStringOrNull(snapshot.resolved_owner_identity);
  std::printf("}");
}

inline void PrintMethodCacheStateCategoryAttachment(
    const objc3_runtime_method_cache_state_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"cache_entry_count\":%llu,",
              static_cast<unsigned long long>(snapshot.cache_entry_count));
  std::printf("\"cache_hit_count\":%llu,",
              static_cast<unsigned long long>(snapshot.cache_hit_count));
  std::printf("\"cache_miss_count\":%llu,",
              static_cast<unsigned long long>(snapshot.cache_miss_count));
  std::printf("\"slow_path_lookup_count\":%llu,",
              static_cast<unsigned long long>(snapshot.slow_path_lookup_count));
  std::printf("\"live_dispatch_count\":%llu,",
              static_cast<unsigned long long>(snapshot.live_dispatch_count));
  std::printf(
      "\"strict_dispatch_error_count\":%llu,",
      static_cast<unsigned long long>(snapshot.strict_dispatch_error_count));
  std::printf(
      "\"last_category_probe_count\":%llu,",
      static_cast<unsigned long long>(snapshot.last_category_probe_count));
  std::printf(
      "\"last_protocol_probe_count\":%llu,",
      static_cast<unsigned long long>(snapshot.last_protocol_probe_count));
  std::printf("\"last_dispatch_used_cache\":%d,",
              snapshot.last_dispatch_used_cache);
  std::printf("\"last_dispatch_resolved_live_method\":%d,",
              snapshot.last_dispatch_resolved_live_method);
  std::printf("\"last_dispatch_strict_error\":%d,",
              snapshot.last_dispatch_strict_error);
  std::printf("\"last_selector\":");
  PrintJsonStringOrNull(snapshot.last_selector);
  std::printf(",\"last_resolved_class_name\":");
  PrintJsonStringOrNull(snapshot.last_resolved_class_name);
  std::printf(",\"last_resolved_owner_identity\":");
  PrintJsonStringOrNull(snapshot.last_resolved_owner_identity);
  std::printf("}");
}

inline void PrintMethodCacheStateFull(
    const objc3_runtime_method_cache_state_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"cache_entry_count\":%llu,",
              static_cast<unsigned long long>(snapshot.cache_entry_count));
  std::printf("\"cache_hit_count\":%llu,",
              static_cast<unsigned long long>(snapshot.cache_hit_count));
  std::printf("\"cache_miss_count\":%llu,",
              static_cast<unsigned long long>(snapshot.cache_miss_count));
  std::printf("\"slow_path_lookup_count\":%llu,",
              static_cast<unsigned long long>(snapshot.slow_path_lookup_count));
  std::printf("\"live_dispatch_count\":%llu,",
              static_cast<unsigned long long>(snapshot.live_dispatch_count));
  std::printf(
      "\"strict_dispatch_error_count\":%llu,",
      static_cast<unsigned long long>(snapshot.strict_dispatch_error_count));
  std::printf(
      "\"last_selector_stable_id\":%llu,",
      static_cast<unsigned long long>(snapshot.last_selector_stable_id));
  std::printf("\"last_normalized_receiver_identity\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.last_normalized_receiver_identity));
  std::printf(
      "\"last_category_probe_count\":%llu,",
      static_cast<unsigned long long>(snapshot.last_category_probe_count));
  std::printf(
      "\"last_protocol_probe_count\":%llu,",
      static_cast<unsigned long long>(snapshot.last_protocol_probe_count));
  std::printf("\"last_dispatch_used_cache\":%d,",
              snapshot.last_dispatch_used_cache);
  std::printf("\"last_dispatch_resolved_live_method\":%d,",
              snapshot.last_dispatch_resolved_live_method);
  std::printf("\"last_dispatch_strict_error\":%d,",
              snapshot.last_dispatch_strict_error);
  std::printf("\"last_selector\":");
  PrintJsonStringOrNull(snapshot.last_selector);
  std::printf(",\"last_resolved_class_name\":");
  PrintJsonStringOrNull(snapshot.last_resolved_class_name);
  std::printf(",\"last_resolved_owner_identity\":");
  PrintJsonStringOrNull(snapshot.last_resolved_owner_identity);
  std::printf("}");
}

inline void PrintMethodCacheStateMetaclass(
    const objc3_runtime_method_cache_state_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"cache_entry_count\":%llu,",
              static_cast<unsigned long long>(snapshot.cache_entry_count));
  std::printf("\"cache_hit_count\":%llu,",
              static_cast<unsigned long long>(snapshot.cache_hit_count));
  std::printf("\"cache_miss_count\":%llu,",
              static_cast<unsigned long long>(snapshot.cache_miss_count));
  std::printf("\"slow_path_lookup_count\":%llu,",
              static_cast<unsigned long long>(snapshot.slow_path_lookup_count));
  std::printf("\"live_dispatch_count\":%llu,",
              static_cast<unsigned long long>(snapshot.live_dispatch_count));
  std::printf(
      "\"strict_dispatch_error_count\":%llu,",
      static_cast<unsigned long long>(snapshot.strict_dispatch_error_count));
  std::printf("\"last_normalized_receiver_identity\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.last_normalized_receiver_identity));
  std::printf("\"last_dispatch_used_cache\":%d,",
              snapshot.last_dispatch_used_cache);
  std::printf("\"last_dispatch_resolved_live_method\":%d,",
              snapshot.last_dispatch_resolved_live_method);
  std::printf("\"last_dispatch_strict_error\":%d,",
              snapshot.last_dispatch_strict_error);
  std::printf("\"last_selector\":");
  PrintJsonStringOrNull(snapshot.last_selector);
  std::printf(",\"last_resolved_class_name\":");
  PrintJsonStringOrNull(snapshot.last_resolved_class_name);
  std::printf(",\"last_resolved_owner_identity\":");
  PrintJsonStringOrNull(snapshot.last_resolved_owner_identity);
  std::printf("}");
}

inline void PrintMethodCacheStateMethodBinding(
    const objc3_runtime_method_cache_state_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"cache_entry_count\":%llu,",
              static_cast<unsigned long long>(snapshot.cache_entry_count));
  std::printf("\"cache_hit_count\":%llu,",
              static_cast<unsigned long long>(snapshot.cache_hit_count));
  std::printf("\"cache_miss_count\":%llu,",
              static_cast<unsigned long long>(snapshot.cache_miss_count));
  std::printf("\"slow_path_lookup_count\":%llu,",
              static_cast<unsigned long long>(snapshot.slow_path_lookup_count));
  std::printf("\"live_dispatch_count\":%llu,",
              static_cast<unsigned long long>(snapshot.live_dispatch_count));
  std::printf(
      "\"strict_dispatch_error_count\":%llu,",
      static_cast<unsigned long long>(snapshot.strict_dispatch_error_count));
  std::printf("\"last_selector\":");
  PrintJsonStringOrNull(snapshot.last_selector);
  std::printf(",\"last_normalized_receiver_identity\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.last_normalized_receiver_identity));
  std::printf("\"last_dispatch_used_cache\":%d,",
              snapshot.last_dispatch_used_cache);
  std::printf("\"last_dispatch_resolved_live_method\":%d,",
              snapshot.last_dispatch_resolved_live_method);
  std::printf("\"last_dispatch_strict_error\":%d,",
              snapshot.last_dispatch_strict_error);
  std::printf("\"last_resolved_class_name\":");
  PrintJsonStringOrNull(snapshot.last_resolved_class_name);
  std::printf(",\"last_resolved_owner_identity\":");
  PrintJsonStringOrNull(snapshot.last_resolved_owner_identity);
  std::printf("}");
}

inline void PrintMethodCacheStateSlowPath(
    const objc3_runtime_method_cache_state_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"cache_entry_count\":%llu,",
              static_cast<unsigned long long>(snapshot.cache_entry_count));
  std::printf("\"cache_hit_count\":%llu,",
              static_cast<unsigned long long>(snapshot.cache_hit_count));
  std::printf("\"cache_miss_count\":%llu,",
              static_cast<unsigned long long>(snapshot.cache_miss_count));
  std::printf("\"slow_path_lookup_count\":%llu,",
              static_cast<unsigned long long>(snapshot.slow_path_lookup_count));
  std::printf("\"live_dispatch_count\":%llu,",
              static_cast<unsigned long long>(snapshot.live_dispatch_count));
  std::printf(
      "\"strict_dispatch_error_count\":%llu,",
      static_cast<unsigned long long>(snapshot.strict_dispatch_error_count));
  std::printf("\"last_selector\":");
  PrintJsonStringOrNull(snapshot.last_selector);
  std::printf(
      ",\"last_selector_stable_id\":%llu,",
      static_cast<unsigned long long>(snapshot.last_selector_stable_id));
  std::printf("\"last_normalized_receiver_identity\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.last_normalized_receiver_identity));
  std::printf("\"last_dispatch_used_cache\":%d,",
              snapshot.last_dispatch_used_cache);
  std::printf("\"last_dispatch_resolved_live_method\":%d,",
              snapshot.last_dispatch_resolved_live_method);
  std::printf("\"last_dispatch_strict_error\":%d,",
              snapshot.last_dispatch_strict_error);
  std::printf("\"last_resolved_class_name\":");
  PrintJsonStringOrNull(snapshot.last_resolved_class_name);
  std::printf(",\"last_resolved_owner_identity\":");
  PrintJsonStringOrNull(snapshot.last_resolved_owner_identity);
  std::printf("}");
}

inline void PrintMethodCacheStateProtocolCategory(
    const objc3_runtime_method_cache_state_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"cache_entry_count\":%llu,",
              static_cast<unsigned long long>(snapshot.cache_entry_count));
  std::printf("\"cache_hit_count\":%llu,",
              static_cast<unsigned long long>(snapshot.cache_hit_count));
  std::printf("\"cache_miss_count\":%llu,",
              static_cast<unsigned long long>(snapshot.cache_miss_count));
  std::printf("\"slow_path_lookup_count\":%llu,",
              static_cast<unsigned long long>(snapshot.slow_path_lookup_count));
  std::printf("\"live_dispatch_count\":%llu,",
              static_cast<unsigned long long>(snapshot.live_dispatch_count));
  std::printf(
      "\"strict_dispatch_error_count\":%llu,",
      static_cast<unsigned long long>(snapshot.strict_dispatch_error_count));
  std::printf("\"last_selector\":");
  PrintJsonStringOrNull(snapshot.last_selector);
  std::printf(
      ",\"last_selector_stable_id\":%llu,",
      static_cast<unsigned long long>(snapshot.last_selector_stable_id));
  std::printf("\"last_normalized_receiver_identity\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.last_normalized_receiver_identity));
  std::printf(
      "\"last_category_probe_count\":%llu,",
      static_cast<unsigned long long>(snapshot.last_category_probe_count));
  std::printf(
      "\"last_protocol_probe_count\":%llu,",
      static_cast<unsigned long long>(snapshot.last_protocol_probe_count));
  std::printf("\"last_dispatch_used_cache\":%d,",
              snapshot.last_dispatch_used_cache);
  std::printf("\"last_dispatch_resolved_live_method\":%d,",
              snapshot.last_dispatch_resolved_live_method);
  std::printf("\"last_dispatch_strict_error\":%d,",
              snapshot.last_dispatch_strict_error);
  std::printf("\"last_resolved_class_name\":");
  PrintJsonStringOrNull(snapshot.last_resolved_class_name);
  std::printf(",\"last_resolved_owner_identity\":");
  PrintJsonStringOrNull(snapshot.last_resolved_owner_identity);
  std::printf("}");
}


} // namespace objc3c::runtime::probe

#endif // OBJC3C_TESTS_TOOLING_RUNTIME_SUPPORT_RUNTIME_SNAPSHOT_METHOD_CACHE_JSON_H_
