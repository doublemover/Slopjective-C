#pragma once

#include "probe_state.h"

namespace objc3c {
namespace runtime {
namespace probe {
namespace runtime_canonical_runnable_object {

inline void CaptureFixtureRuntimeSetup(RuntimeFixture &fixture) {
  (void)objc3_runtime_copy_realized_class_graph_state_for_testing(
      &fixture.graph_state);
  (void)objc3_runtime_copy_realized_class_entry_for_testing(
      "Widget", &fixture.widget_entry);

  fixture.widget_instance_receiver =
      fixture.widget_entry.found != 0
          ? static_cast<int>(fixture.widget_entry.base_identity + 1U)
          : 0;
  fixture.widget_class_receiver =
      fixture.widget_entry.found != 0
          ? static_cast<int>(fixture.widget_entry.base_identity + 2U)
          : 0;

  fixture.alloc_selector = objc3_runtime_lookup_selector("alloc");
  fixture.init_selector = objc3_runtime_lookup_selector("init");
  fixture.new_selector = objc3_runtime_lookup_selector("new");
  fixture.traced_selector = objc3_runtime_lookup_selector("tracedValue");
  fixture.inherited_selector =
      objc3_runtime_lookup_selector("inheritedValue");
  fixture.class_selector = objc3_runtime_lookup_selector("classValue");
}

} // namespace runtime_canonical_runnable_object
} // namespace probe
} // namespace runtime
} // namespace objc3c
