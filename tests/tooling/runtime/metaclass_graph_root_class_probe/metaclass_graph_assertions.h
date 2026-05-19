#pragma once

#include "probe_state.h"
#include "support/runtime_snapshot_stabilizers.h"

namespace objc3c {
namespace runtime {
namespace probe {
namespace metaclass_graph_root_class {

inline void CaptureMetaclassGraphAssertions(
    MetaclassGraphAssertions &assertions) {
  assertions.root_class_value = objc3_runtime_dispatch_i32(
      kRootClassReceiver, kSharedSelector, 0, 0, 0, 0);
  (void)objc3_runtime_copy_method_cache_state_for_testing(
      &assertions.root_class_state);
  ::objc3c::runtime::probe::StabilizeMethodCacheState(
      assertions.root_class_state,
      assertions.root_class_state_storage.selector,
      assertions.root_class_state_storage.class_name,
      assertions.root_class_state_storage.owner_identity);

  assertions.widget_class_value = objc3_runtime_dispatch_i32(
      kWidgetClassReceiver, kSharedSelector, 0, 0, 0, 0);
  (void)objc3_runtime_copy_method_cache_state_for_testing(
      &assertions.widget_class_state);
  ::objc3c::runtime::probe::StabilizeMethodCacheState(
      assertions.widget_class_state,
      assertions.widget_class_state_storage.selector,
      assertions.widget_class_state_storage.class_name,
      assertions.widget_class_state_storage.owner_identity);

  assertions.widget_known_class_value = objc3_runtime_dispatch_i32(
      kWidgetKnownClassReceiver, kSharedSelector, 0, 0, 0, 0);
  (void)objc3_runtime_copy_method_cache_state_for_testing(
      &assertions.widget_known_class_state);
  ::objc3c::runtime::probe::StabilizeMethodCacheState(
      assertions.widget_known_class_state,
      assertions.widget_known_class_state_storage.selector,
      assertions.widget_known_class_state_storage.class_name,
      assertions.widget_known_class_state_storage.owner_identity);
}

}  // namespace metaclass_graph_root_class
}  // namespace probe
}  // namespace runtime
}  // namespace objc3c
