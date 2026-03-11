#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <cstdio>
#include <string>

namespace {

void PrintJsonStringOrNull(const char *value) {
  if (value == nullptr) {
    std::printf("null");
    return;
  }
  std::printf("\"");
  for (const unsigned char *cursor =
           reinterpret_cast<const unsigned char *>(value);
       *cursor != 0U; ++cursor) {
    switch (*cursor) {
      case '\\':
        std::printf("\\\\");
        break;
      case '"':
        std::printf("\\\"");
        break;
      case '\n':
        std::printf("\\n");
        break;
      case '\r':
        std::printf("\\r");
        break;
      case '\t':
        std::printf("\\t");
        break;
      default:
        std::printf("%c", static_cast<char>(*cursor));
        break;
    }
  }
  std::printf("\"");
}

void StabilizeNullableCString(const char *source, std::string &storage,
                              const char *&field) {
  storage = source != nullptr ? source : "";
  field = storage.empty() ? nullptr : storage.c_str();
}

void StabilizeGraph(objc3_runtime_realized_class_graph_state_snapshot &snapshot,
                    std::string &class_storage) {
  StabilizeNullableCString(snapshot.last_allocated_class_name, class_storage,
                           snapshot.last_allocated_class_name);
}

void StabilizePropertyEntry(objc3_runtime_property_entry_snapshot &snapshot,
                            std::string &queried_class_storage,
                            std::string &resolved_class_storage,
                            std::string &property_name_storage,
                            std::string &owner_storage,
                            std::string &lifetime_storage,
                            std::string &hook_storage,
                            std::string &accessor_storage) {
  StabilizeNullableCString(snapshot.queried_class_name, queried_class_storage,
                           snapshot.queried_class_name);
  StabilizeNullableCString(snapshot.resolved_class_name, resolved_class_storage,
                           snapshot.resolved_class_name);
  StabilizeNullableCString(snapshot.property_name, property_name_storage,
                           snapshot.property_name);
  StabilizeNullableCString(snapshot.declaration_owner_identity, owner_storage,
                           snapshot.declaration_owner_identity);
  StabilizeNullableCString(snapshot.ownership_lifetime_profile,
                           lifetime_storage,
                           snapshot.ownership_lifetime_profile);
  StabilizeNullableCString(snapshot.ownership_runtime_hook_profile,
                           hook_storage,
                           snapshot.ownership_runtime_hook_profile);
  StabilizeNullableCString(snapshot.accessor_ownership_profile,
                           accessor_storage,
                           snapshot.accessor_ownership_profile);
}

void PrintGraph(const objc3_runtime_realized_class_graph_state_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"live_instance_count\":%llu,",
              static_cast<unsigned long long>(snapshot.live_instance_count));
  std::printf("\"last_allocated_class_name\":");
  PrintJsonStringOrNull(snapshot.last_allocated_class_name);
  std::printf("}");
}

void PrintMemoryState(
    const objc3_runtime_memory_management_state_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"live_runtime_instance_count\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.live_runtime_instance_count));
  std::printf("\"weak_target_count\":%llu,",
              static_cast<unsigned long long>(snapshot.weak_target_count));
  std::printf("\"weak_slot_ref_count\":%llu,",
              static_cast<unsigned long long>(snapshot.weak_slot_ref_count));
  std::printf("\"autoreleasepool_depth\":%llu,",
              static_cast<unsigned long long>(snapshot.autoreleasepool_depth));
  std::printf("\"autoreleasepool_max_depth\":%llu,",
              static_cast<unsigned long long>(snapshot.autoreleasepool_max_depth));
  std::printf("\"queued_autorelease_value_count\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.queued_autorelease_value_count));
  std::printf("\"drained_autorelease_value_count\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.drained_autorelease_value_count));
  std::printf("\"last_autoreleased_value\":%d,",
              snapshot.last_autoreleased_value);
  std::printf("\"last_drained_autorelease_value\":%d",
              snapshot.last_drained_autorelease_value);
  std::printf("}");
}

void PrintPropertyEntry(const objc3_runtime_property_entry_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"found\":%d,", snapshot.found);
  std::printf("\"setter_available\":%d,", snapshot.setter_available);
  std::printf("\"has_runtime_getter\":%d,", snapshot.has_runtime_getter);
  std::printf("\"has_runtime_setter\":%d,", snapshot.has_runtime_setter);
  std::printf("\"ownership_lifetime_profile\":");
  PrintJsonStringOrNull(snapshot.ownership_lifetime_profile);
  std::printf(",\"ownership_runtime_hook_profile\":");
  PrintJsonStringOrNull(snapshot.ownership_runtime_hook_profile);
  std::printf(",\"accessor_ownership_profile\":");
  PrintJsonStringOrNull(snapshot.accessor_ownership_profile);
  std::printf("}");
}

}  // namespace

int main() {
  objc3_runtime_reset_for_testing();
  (void)objc3_runtime_replay_registered_images_for_testing();

  objc3_runtime_realized_class_graph_state_snapshot graph_after_setup{};
  objc3_runtime_realized_class_graph_state_snapshot graph_inside_pool{};
  objc3_runtime_realized_class_graph_state_snapshot graph_after_pool{};
  objc3_runtime_realized_class_graph_state_snapshot graph_after_parent_release{};
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
  (void)objc3_runtime_copy_realized_class_graph_state_for_testing(&graph_after_setup);
  StabilizeGraph(graph_after_setup, setup_class_storage);

  objc3_runtime_push_autoreleasepool_scope();
  const int getter_value =
      objc3_runtime_dispatch_i32(parent, "currentValue", 0, 0, 0, 0);
  const int weak_set_result =
      objc3_runtime_dispatch_i32(parent, "setWeakValue:", getter_value, 0, 0, 0);
  const int clear_strong_result =
      objc3_runtime_dispatch_i32(parent, "setCurrentValue:", 0, 0, 0, 0);
  const int weak_inside_pool =
      objc3_runtime_dispatch_i32(parent, "weakValue", 0, 0, 0, 0);
  (void)objc3_runtime_copy_realized_class_graph_state_for_testing(&graph_inside_pool);
  (void)objc3_runtime_copy_memory_management_state_for_testing(&memory_inside_pool);
  StabilizeGraph(graph_inside_pool, inside_pool_class_storage);

  objc3_runtime_pop_autoreleasepool_scope();
  const int weak_after_pool =
      objc3_runtime_dispatch_i32(parent, "weakValue", 0, 0, 0, 0);
  (void)objc3_runtime_copy_realized_class_graph_state_for_testing(&graph_after_pool);
  (void)objc3_runtime_copy_memory_management_state_for_testing(&memory_after_pool);
  StabilizeGraph(graph_after_pool, after_pool_class_storage);

  const int parent_release_result = objc3_runtime_release_i32(parent);
  (void)objc3_runtime_copy_realized_class_graph_state_for_testing(
      &graph_after_parent_release);
  (void)objc3_runtime_copy_memory_management_state_for_testing(
      &memory_after_parent_release);
  StabilizeGraph(graph_after_parent_release, after_parent_release_class_storage);

  (void)objc3_runtime_copy_property_entry_for_testing("Box", "weakValue",
                                                      &weak_value_entry);
  StabilizePropertyEntry(weak_value_entry, weak_queried_class_storage,
                         weak_resolved_class_storage,
                         weak_property_name_storage, weak_owner_storage,
                         weak_lifetime_storage, weak_hook_storage,
                         weak_accessor_storage);

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
  PrintGraph(graph_after_setup);
  std::printf(",\"graph_inside_pool\":");
  PrintGraph(graph_inside_pool);
  std::printf(",\"graph_after_pool\":");
  PrintGraph(graph_after_pool);
  std::printf(",\"graph_after_parent_release\":");
  PrintGraph(graph_after_parent_release);
  std::printf(",\"memory_inside_pool\":");
  PrintMemoryState(memory_inside_pool);
  std::printf(",\"memory_after_pool\":");
  PrintMemoryState(memory_after_pool);
  std::printf(",\"memory_after_parent_release\":");
  PrintMemoryState(memory_after_parent_release);
  std::printf(",\"weak_value_entry\":");
  PrintPropertyEntry(weak_value_entry);
  std::printf("}");
  return 0;
}
