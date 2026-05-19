#pragma once

#include "probe_state.h"
#include "support/runtime_snapshot_stabilizers.h"

namespace objc3c {
namespace runtime {
namespace probe {
namespace metaclass_graph_root_class {

inline void CaptureRootClassInvariants(RootClassInvariants &invariants) {
  invariants.widget_inherited_instance_value = objc3_runtime_dispatch_i32(
      kWidgetInstanceReceiver, kRootValueSelector, 0, 0, 0, 0);
  (void)objc3_runtime_copy_method_cache_state_for_testing(
      &invariants.widget_inherited_state);
  ::objc3c::runtime::probe::StabilizeMethodCacheState(
      invariants.widget_inherited_state,
      invariants.widget_inherited_state_storage.selector,
      invariants.widget_inherited_state_storage.class_name,
      invariants.widget_inherited_state_storage.owner_identity);

  invariants.widget_own_instance_value = objc3_runtime_dispatch_i32(
      kWidgetInstanceReceiver, kWidgetValueSelector, 0, 0, 0, 0);
  (void)objc3_runtime_copy_method_cache_state_for_testing(
      &invariants.widget_own_state);
  ::objc3c::runtime::probe::StabilizeMethodCacheState(
      invariants.widget_own_state, invariants.widget_own_state_storage.selector,
      invariants.widget_own_state_storage.class_name,
      invariants.widget_own_state_storage.owner_identity);

  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      kRootClassReceiver, kSharedSelector, &invariants.root_shared_entry);
  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      kWidgetKnownClassReceiver, kSharedSelector,
      &invariants.widget_shared_entry);
  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      kWidgetInstanceReceiver, kRootValueSelector,
      &invariants.widget_inherited_entry);
  (void)objc3_runtime_copy_method_cache_entry_for_testing(
      kWidgetInstanceReceiver, kWidgetValueSelector,
      &invariants.widget_own_entry);
  ::objc3c::runtime::probe::StabilizeMethodCacheEntry(
      invariants.root_shared_entry,
      invariants.root_shared_entry_storage.selector,
      invariants.root_shared_entry_storage.class_name,
      invariants.root_shared_entry_storage.owner_identity);
  ::objc3c::runtime::probe::StabilizeMethodCacheEntry(
      invariants.widget_shared_entry,
      invariants.widget_shared_entry_storage.selector,
      invariants.widget_shared_entry_storage.class_name,
      invariants.widget_shared_entry_storage.owner_identity);
  ::objc3c::runtime::probe::StabilizeMethodCacheEntry(
      invariants.widget_inherited_entry,
      invariants.widget_inherited_entry_storage.selector,
      invariants.widget_inherited_entry_storage.class_name,
      invariants.widget_inherited_entry_storage.owner_identity);
  ::objc3c::runtime::probe::StabilizeMethodCacheEntry(
      invariants.widget_own_entry,
      invariants.widget_own_entry_storage.selector,
      invariants.widget_own_entry_storage.class_name,
      invariants.widget_own_entry_storage.owner_identity);
}

}  // namespace metaclass_graph_root_class
}  // namespace probe
}  // namespace runtime
}  // namespace objc3c
