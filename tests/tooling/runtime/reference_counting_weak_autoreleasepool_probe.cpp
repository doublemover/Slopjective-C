#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "support/json_probe_writer.h"
#include "support/runtime_snapshot_json.h"
#include "support/runtime_snapshot_stabilizers.h"

#include <cstdio>
#include <string>

namespace {

using objc3c::runtime::probe::PrintAllocationGraph;
using objc3c::runtime::probe::PrintMemoryManagementState;
using objc3c::runtime::probe::PrintPropertyEntryWeakAutoreleasepool;

using objc3c::runtime::probe::StabilizeGraph;
using objc3c::runtime::probe::StabilizeNullableCString;
using objc3c::runtime::probe::StabilizePropertyEntry;

using objc3c::runtime::probe::PrintJsonStringOrNull;

} // namespace

int main() {
  objc3_runtime_reset_for_testing();
  (void)objc3_runtime_replay_registered_images_for_testing();

  objc3_runtime_realized_class_graph_state_snapshot graph_after_setup{};
  objc3_runtime_realized_class_graph_state_snapshot graph_inside_pool{};
  objc3_runtime_realized_class_graph_state_snapshot graph_after_pool{};
  objc3_runtime_realized_class_graph_state_snapshot
      graph_after_parent_release{};
  objc3_runtime_memory_management_state_snapshot memory_inside_pool{};
  objc3_runtime_memory_management_state_snapshot memory_after_pool{};
  objc3_runtime_memory_management_state_snapshot memory_after_parent_release{};
  objc3_runtime_property_entry_snapshot weak_value_entry{};

  std::string setup_class_storage;
  std::string inside_pool_class_storage;
  std::string after_pool_class_storage;
  std::string after_parent_release_class_storage;
  std::string weak_queried_class_storage;
  std::string weak_resolved_class_storage;
  std::string weak_property_name_storage;
  std::string weak_owner_storage;
  std::string weak_lifetime_storage;
  std::string weak_hook_storage;
  std::string weak_accessor_storage;

  const int parent = objc3_runtime_dispatch_i32(1024, "alloc", 0, 0, 0, 0);
  const int child = objc3_runtime_dispatch_i32(1024, "alloc", 0, 0, 0, 0);
  const int strong_set_result =
      objc3_runtime_dispatch_i32(parent, "setCurrentValue:", child, 0, 0, 0);
  const int release_local_result = objc3_runtime_release_i32(child);
  (void)objc3_runtime_copy_realized_class_graph_state_for_testing(
      &graph_after_setup);
  StabilizeGraph(graph_after_setup, setup_class_storage);

  objc3_runtime_push_autoreleasepool_scope();
  const int getter_value =
      objc3_runtime_dispatch_i32(parent, "currentValue", 0, 0, 0, 0);
  const int weak_set_result = objc3_runtime_dispatch_i32(
      parent, "setWeakValue:", getter_value, 0, 0, 0);
  const int clear_strong_result =
      objc3_runtime_dispatch_i32(parent, "setCurrentValue:", 0, 0, 0, 0);
  const int weak_inside_pool =
      objc3_runtime_dispatch_i32(parent, "weakValue", 0, 0, 0, 0);
  (void)objc3_runtime_copy_realized_class_graph_state_for_testing(
      &graph_inside_pool);
  (void)objc3_runtime_copy_memory_management_state_for_testing(
      &memory_inside_pool);
  StabilizeGraph(graph_inside_pool, inside_pool_class_storage);

  objc3_runtime_pop_autoreleasepool_scope();
  const int weak_after_pool =
      objc3_runtime_dispatch_i32(parent, "weakValue", 0, 0, 0, 0);
  (void)objc3_runtime_copy_realized_class_graph_state_for_testing(
      &graph_after_pool);
  (void)objc3_runtime_copy_memory_management_state_for_testing(
      &memory_after_pool);
  StabilizeGraph(graph_after_pool, after_pool_class_storage);

  const int parent_release_result = objc3_runtime_release_i32(parent);
  (void)objc3_runtime_copy_realized_class_graph_state_for_testing(
      &graph_after_parent_release);
  (void)objc3_runtime_copy_memory_management_state_for_testing(
      &memory_after_parent_release);
  StabilizeGraph(graph_after_parent_release,
                 after_parent_release_class_storage);

  (void)objc3_runtime_copy_property_entry_for_testing("Box", "weakValue",
                                                      &weak_value_entry);
  StabilizePropertyEntry(
      weak_value_entry, weak_queried_class_storage, weak_resolved_class_storage,
      weak_property_name_storage, weak_owner_storage, weak_lifetime_storage,
      weak_hook_storage, weak_accessor_storage);

  std::printf("{");
  std::printf("\"parent\":%d,", parent);
  std::printf("\"child\":%d,", child);
  std::printf("\"strong_set_result\":%d,", strong_set_result);
  std::printf("\"release_local_result\":%d,", release_local_result);
  std::printf("\"getter_value\":%d,", getter_value);
  std::printf("\"weak_set_result\":%d,", weak_set_result);
  std::printf("\"clear_strong_result\":%d,", clear_strong_result);
  std::printf("\"weak_inside_pool\":%d,", weak_inside_pool);
  std::printf("\"weak_after_pool\":%d,", weak_after_pool);
  std::printf("\"parent_release_result\":%d,", parent_release_result);
  std::printf("\"graph_after_setup\":");
  PrintAllocationGraph(graph_after_setup);
  std::printf(",\"graph_inside_pool\":");
  PrintAllocationGraph(graph_inside_pool);
  std::printf(",\"graph_after_pool\":");
  PrintAllocationGraph(graph_after_pool);
  std::printf(",\"graph_after_parent_release\":");
  PrintAllocationGraph(graph_after_parent_release);
  std::printf(",\"memory_inside_pool\":");
  PrintMemoryManagementState(memory_inside_pool);
  std::printf(",\"memory_after_pool\":");
  PrintMemoryManagementState(memory_after_pool);
  std::printf(",\"memory_after_parent_release\":");
  PrintMemoryManagementState(memory_after_parent_release);
  std::printf(",\"weak_value_entry\":");
  PrintPropertyEntryWeakAutoreleasepool(weak_value_entry);
  std::printf("}");
  return 0;
}
