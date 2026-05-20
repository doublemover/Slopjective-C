#pragma once

#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <string>

namespace objc3c::runtime::probe::reference_counting_weak_autoreleasepool {

struct ReferenceCountingFixture {
  int parent = 0;
  int child = 0;
};

struct RealizedClassGraphCapture {
  objc3_runtime_realized_class_graph_state_snapshot snapshot{};
  std::string allocated_class_name_storage;
};

struct MemoryManagementCapture {
  objc3_runtime_memory_management_state_snapshot snapshot{};
};

struct WeakPropertyEntryCapture {
  objc3_runtime_property_entry_snapshot snapshot{};
  std::string queried_class_name_storage;
  std::string resolved_class_name_storage;
  std::string property_name_storage;
  std::string declaration_owner_storage;
  std::string ownership_lifetime_storage;
  std::string ownership_runtime_hook_storage;
  std::string accessor_ownership_storage;
};

struct ReferenceCountingOperationResults {
  int parent = 0;
  int child = 0;
  int strong_set_result = 0;
  int release_local_result = 0;
  int getter_value = 0;
  int weak_set_result = 0;
  int clear_strong_result = 0;
  int weak_inside_pool = 0;
  int weak_after_pool = 0;
  int weak_stale_zeroed = 0;
  int parent_release_result = 0;
  int nested_outer_retained = 0;
  int nested_inner_retained = 0;
  int nested_outer_autoreleased = 0;
  int nested_inner_autoreleased = 0;
  int nested_lifo_drain_order_observed = 0;
};

struct ReferenceCountingSnapshotCaptures {
  RealizedClassGraphCapture graph_after_setup;
  RealizedClassGraphCapture graph_inside_pool;
  RealizedClassGraphCapture graph_after_pool;
  RealizedClassGraphCapture graph_after_parent_release;
  MemoryManagementCapture memory_inside_pool;
  MemoryManagementCapture memory_after_pool;
  MemoryManagementCapture memory_after_parent_release;
  MemoryManagementCapture memory_before_nested_pool;
  MemoryManagementCapture memory_nested_pool;
  MemoryManagementCapture memory_after_inner_pool;
  MemoryManagementCapture memory_after_outer_pool;
  MemoryManagementCapture memory_after_nested_release_cleanup;
  WeakPropertyEntryCapture weak_value_entry;
};

struct ReferenceCountingWeakAutoreleasepoolRun {
  ReferenceCountingFixture fixture;
  ReferenceCountingOperationResults operations;
  ReferenceCountingSnapshotCaptures snapshots;
};

}  // namespace objc3c::runtime::probe::reference_counting_weak_autoreleasepool
