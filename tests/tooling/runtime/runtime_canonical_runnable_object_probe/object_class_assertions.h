#pragma once

#include "probe_state.h"

namespace objc3c {
namespace runtime {
namespace probe {
namespace runtime_canonical_runnable_object {

inline void CaptureObjectClassAssertions(ProbeRun &run) {
  const RuntimeFixture &fixture = run.fixture;
  const RunnableInvocationAssertions &runnable = run.runnable;
  ObjectClassAssertions &object_class = run.object_class;

  (void)objc3_runtime_copy_protocol_conformance_query_for_testing(
      "Widget", "Worker", &object_class.worker_query);
  (void)objc3_runtime_copy_protocol_conformance_query_for_testing(
      "Widget", "Tracer", &object_class.tracer_query);
  (void)objc3_runtime_copy_method_cache_state_for_testing(
      &object_class.method_state);
  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      fixture.widget_class_receiver, "alloc", &object_class.alloc_entry);
  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      runnable.init_value, "init", &object_class.init_entry);
  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      fixture.widget_class_receiver, "new", &object_class.new_entry);
  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      runnable.init_value, "tracedValue", &object_class.traced_entry);
  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      runnable.init_value, "inheritedValue", &object_class.inherited_entry);
  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      fixture.widget_class_receiver, "classValue", &object_class.class_entry);
  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      runnable.init_value, "ignoredValue", &object_class.ignored_entry);
  (void)objc3_runtime_copy_selector_lookup_table_state_for_testing(
      &object_class.selector_table_state);
  (void)objc3_runtime_copy_selector_lookup_entry_for_testing(
      "tracedValue", &object_class.traced_selector_entry);
  (void)objc3_runtime_copy_selector_lookup_entry_for_testing(
      "inheritedValue", &object_class.inherited_selector_entry);
  (void)objc3_runtime_copy_selector_lookup_entry_for_testing(
      "classValue", &object_class.class_selector_entry);
  (void)objc3_runtime_copy_selector_lookup_entry_for_testing(
      "ignoredValue", &object_class.ignored_selector_entry);
}

} // namespace runtime_canonical_runnable_object
} // namespace probe
} // namespace runtime
} // namespace objc3c
