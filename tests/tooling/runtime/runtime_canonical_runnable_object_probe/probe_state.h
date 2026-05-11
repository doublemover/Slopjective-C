#pragma once

#include "runtime/objc3_runtime_bootstrap_internal.h"

namespace objc3c {
namespace runtime {
namespace probe {
namespace runtime_canonical_runnable_object {

struct RuntimeFixture {
  objc3_runtime_realized_class_graph_state_snapshot graph_state{};
  objc3_runtime_realized_class_entry_snapshot widget_entry{};
  int widget_instance_receiver = 0;
  int widget_class_receiver = 0;
  const objc3_runtime_selector_handle *alloc_selector = nullptr;
  const objc3_runtime_selector_handle *init_selector = nullptr;
  const objc3_runtime_selector_handle *new_selector = nullptr;
  const objc3_runtime_selector_handle *traced_selector = nullptr;
  const objc3_runtime_selector_handle *inherited_selector = nullptr;
  const objc3_runtime_selector_handle *class_selector = nullptr;
};

struct RunnableInvocationAssertions {
  objc3_runtime_method_cache_state_snapshot inherited_state{};
  objc3_runtime_method_cache_state_snapshot traced_state{};
  objc3_runtime_method_cache_state_snapshot class_state{};
  objc3_runtime_method_cache_state_snapshot ignored_state{};
  objc3_runtime_method_cache_state_snapshot ignored_cached_state{};
  objc3_runtime_dispatch_i32_result ignored_result{};
  objc3_runtime_dispatch_i32_result ignored_cached_result{};
  int alloc_value = 0;
  int init_value = 0;
  int new_value = 0;
  int traced_value = 0;
  int inherited_value = 0;
  int class_value = 0;
  int ignored_value = 0;
  int ignored_cached_value = 0;
  int ignored_expected = 0;
};

struct ObjectClassAssertions {
  objc3_runtime_protocol_conformance_query_snapshot worker_query{};
  objc3_runtime_protocol_conformance_query_snapshot tracer_query{};
  objc3_runtime_method_cache_state_snapshot method_state{};
  objc3_runtime_method_cache_entry_snapshot alloc_entry{};
  objc3_runtime_method_cache_entry_snapshot init_entry{};
  objc3_runtime_method_cache_entry_snapshot new_entry{};
  objc3_runtime_method_cache_entry_snapshot traced_entry{};
  objc3_runtime_method_cache_entry_snapshot inherited_entry{};
  objc3_runtime_method_cache_entry_snapshot class_entry{};
  objc3_runtime_method_cache_entry_snapshot ignored_entry{};
  objc3_runtime_selector_lookup_table_state_snapshot selector_table_state{};
  objc3_runtime_selector_lookup_entry_snapshot traced_selector_entry{};
  objc3_runtime_selector_lookup_entry_snapshot inherited_selector_entry{};
  objc3_runtime_selector_lookup_entry_snapshot class_selector_entry{};
  objc3_runtime_selector_lookup_entry_snapshot ignored_selector_entry{};
};

struct ProbeRun {
  RuntimeFixture fixture;
  RunnableInvocationAssertions runnable;
  ObjectClassAssertions object_class;
};

} // namespace runtime_canonical_runnable_object
} // namespace probe
} // namespace runtime
} // namespace objc3c
