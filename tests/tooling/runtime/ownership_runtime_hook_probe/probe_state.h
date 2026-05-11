#pragma once

#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <string>

namespace objc3c::runtime::probe::ownership_runtime_hook {

struct OwnershipFixture {
  int parent = 0;
  int child = 0;
};

struct RealizedClassGraphCapture {
  objc3_runtime_realized_class_graph_state_snapshot snapshot{};
  std::string class_name_storage;
};

struct OwnershipGraphCaptures {
  RealizedClassGraphCapture after_alloc;
  RealizedClassGraphCapture after_drop_local;
  RealizedClassGraphCapture after_clear;
  RealizedClassGraphCapture after_parent_release;
};

struct PropertyEntryOwnershipCapture {
  objc3_runtime_property_entry_snapshot entry{};
  std::string queried_class_storage;
  std::string resolved_class_storage;
  std::string property_name_storage;
  std::string declaration_owner_storage;
  std::string getter_owner_storage;
  std::string setter_owner_storage;
  std::string lifetime_storage;
  std::string hook_storage;
  std::string accessor_storage;
};

struct OwnershipOperationResults {
  int parent = 0;
  int child = 0;
  int strong_set_result = 0;
  int weak_set_result = 0;
  int weak_before_clear = 0;
  int retain_result = 0;
  int release_after_retain_result = 0;
  int release_local_result = 0;
  int strong_before_clear = 0;
  int clear_strong_result = 0;
  int strong_after_clear = 0;
  int weak_after_clear = 0;
  int parent_release_result = 0;
};

struct OwnershipProbeRun {
  OwnershipOperationResults operations;
  OwnershipGraphCaptures graphs;
  PropertyEntryOwnershipCapture current_value_entry;
  PropertyEntryOwnershipCapture weak_value_entry;
};

}  // namespace objc3c::runtime::probe::ownership_runtime_hook
