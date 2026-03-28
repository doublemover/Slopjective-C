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

void StabilizeRealizedClassGraph(
    objc3_runtime_realized_class_graph_state_snapshot &snapshot,
    std::string &class_storage) {
  StabilizeNullableCString(snapshot.last_allocated_class_name, class_storage,
                           snapshot.last_allocated_class_name);
}

void StabilizePropertyEntry(objc3_runtime_property_entry_snapshot &snapshot,
                            std::string &queried_class_storage,
                            std::string &resolved_class_storage,
                            std::string &property_name_storage,
                            std::string &declaration_owner_storage,
                            std::string &getter_owner_storage,
                            std::string &setter_owner_storage,
                            std::string &lifetime_storage,
                            std::string &hook_storage,
                            std::string &accessor_storage) {
  StabilizeNullableCString(snapshot.queried_class_name, queried_class_storage,
                           snapshot.queried_class_name);
  StabilizeNullableCString(snapshot.resolved_class_name, resolved_class_storage,
                           snapshot.resolved_class_name);
  StabilizeNullableCString(snapshot.property_name, property_name_storage,
                           snapshot.property_name);
  StabilizeNullableCString(snapshot.declaration_owner_identity,
                           declaration_owner_storage,
                           snapshot.declaration_owner_identity);
  StabilizeNullableCString(snapshot.getter_owner_identity, getter_owner_storage,
                           snapshot.getter_owner_identity);
  StabilizeNullableCString(snapshot.setter_owner_identity, setter_owner_storage,
                           snapshot.setter_owner_identity);
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

void PrintPropertyEntry(const objc3_runtime_property_entry_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"found\":%d,", snapshot.found);
  std::printf("\"inherited\":%d,", snapshot.inherited);
  std::printf("\"setter_available\":%d,", snapshot.setter_available);
  std::printf("\"has_runtime_getter\":%d,", snapshot.has_runtime_getter);
  std::printf("\"has_runtime_setter\":%d,", snapshot.has_runtime_setter);
  std::printf("\"slot_index\":%llu,",
              static_cast<unsigned long long>(snapshot.slot_index));
  std::printf("\"offset_bytes\":%llu,",
              static_cast<unsigned long long>(snapshot.offset_bytes));
  std::printf("\"ownership_lifetime_profile\":");
  PrintJsonStringOrNull(snapshot.ownership_lifetime_profile);
  std::printf(",\"ownership_runtime_hook_profile\":");
  PrintJsonStringOrNull(snapshot.ownership_runtime_hook_profile);
  std::printf(",\"accessor_ownership_profile\":");
  PrintJsonStringOrNull(snapshot.accessor_ownership_profile);
  std::printf(",\"getter_owner_identity\":");
  PrintJsonStringOrNull(snapshot.getter_owner_identity);
  std::printf(",\"setter_owner_identity\":");
  PrintJsonStringOrNull(snapshot.setter_owner_identity);
  std::printf("}");
}

}  // namespace

int main() {
  objc3_runtime_reset_for_testing();
  (void)objc3_runtime_replay_registered_images_for_testing();

  objc3_runtime_realized_class_graph_state_snapshot graph_after_alloc{};
  objc3_runtime_realized_class_graph_state_snapshot graph_after_drop_local{};
  objc3_runtime_realized_class_graph_state_snapshot graph_after_clear{};
  objc3_runtime_realized_class_graph_state_snapshot graph_after_parent_release{};
  objc3_runtime_property_entry_snapshot current_value_entry{};
  objc3_runtime_property_entry_snapshot weak_value_entry{};

  std::string alloc_class_storage;
  std::string drop_local_class_storage;
  std::string clear_class_storage;
  std::string parent_release_class_storage;
  std::string current_queried_class_storage;
  std::string current_resolved_class_storage;
  std::string current_property_name_storage;
  std::string current_declaration_owner_storage;
  std::string current_getter_owner_storage;
  std::string current_setter_owner_storage;
  std::string current_lifetime_storage;
  std::string current_hook_storage;
  std::string current_accessor_storage;
  std::string weak_queried_class_storage;
  std::string weak_resolved_class_storage;
  std::string weak_property_name_storage;
  std::string weak_declaration_owner_storage;
  std::string weak_getter_owner_storage;
  std::string weak_setter_owner_storage;
  std::string weak_lifetime_storage;
  std::string weak_hook_storage;
  std::string weak_accessor_storage;

  const int parent = objc3_runtime_dispatch_i32(1024, "alloc", 0, 0, 0, 0);
  const int child = objc3_runtime_dispatch_i32(1024, "alloc", 0, 0, 0, 0);
  (void)objc3_runtime_copy_realized_class_graph_state_for_testing(&graph_after_alloc);
  StabilizeRealizedClassGraph(graph_after_alloc, alloc_class_storage);

  const int strong_set_result =
      objc3_runtime_dispatch_i32(parent, "setCurrentValue:", child, 0, 0, 0);
  const int weak_set_result =
      objc3_runtime_dispatch_i32(parent, "setWeakValue:", child, 0, 0, 0);
  const int weak_before_clear =
      objc3_runtime_dispatch_i32(parent, "weakValue", 0, 0, 0, 0);
  const int retain_result = objc3_runtime_retain_i32(child);
  const int release_after_retain_result = objc3_runtime_release_i32(child);
  const int release_local_result = objc3_runtime_release_i32(child);
  (void)objc3_runtime_copy_realized_class_graph_state_for_testing(
      &graph_after_drop_local);
  StabilizeRealizedClassGraph(graph_after_drop_local, drop_local_class_storage);

  const int strong_before_clear =
      objc3_runtime_dispatch_i32(parent, "currentValue", 0, 0, 0, 0);
  const int clear_strong_result =
      objc3_runtime_dispatch_i32(parent, "setCurrentValue:", 0, 0, 0, 0);
  const int strong_after_clear =
      objc3_runtime_dispatch_i32(parent, "currentValue", 0, 0, 0, 0);
  const int weak_after_clear =
      objc3_runtime_dispatch_i32(parent, "weakValue", 0, 0, 0, 0);
  (void)objc3_runtime_copy_realized_class_graph_state_for_testing(&graph_after_clear);
  StabilizeRealizedClassGraph(graph_after_clear, clear_class_storage);

  const int parent_release_result = objc3_runtime_release_i32(parent);
  (void)objc3_runtime_copy_realized_class_graph_state_for_testing(
      &graph_after_parent_release);
  StabilizeRealizedClassGraph(graph_after_parent_release,
                              parent_release_class_storage);

  (void)objc3_runtime_copy_property_entry_for_testing("Box", "currentValue",
                                                      &current_value_entry);
  StabilizePropertyEntry(current_value_entry, current_queried_class_storage,
                         current_resolved_class_storage,
                         current_property_name_storage,
                         current_declaration_owner_storage,
                         current_getter_owner_storage,
                         current_setter_owner_storage,
                         current_lifetime_storage, current_hook_storage,
                         current_accessor_storage);
  (void)objc3_runtime_copy_property_entry_for_testing("Box", "weakValue",
                                                      &weak_value_entry);
  StabilizePropertyEntry(weak_value_entry, weak_queried_class_storage,
                         weak_resolved_class_storage,
                         weak_property_name_storage,
                         weak_declaration_owner_storage,
                         weak_getter_owner_storage,
                         weak_setter_owner_storage, weak_lifetime_storage,
                         weak_hook_storage, weak_accessor_storage);

  std::printf("{");
  std::printf("\"parent\":%d,", parent);
  std::printf("\"child\":%d,", child);
  std::printf("\"strong_set_result\":%d,", strong_set_result);
  std::printf("\"weak_set_result\":%d,", weak_set_result);
  std::printf("\"weak_before_clear\":%d,", weak_before_clear);
  std::printf("\"retain_result\":%d,", retain_result);
  std::printf("\"release_after_retain_result\":%d,", release_after_retain_result);
  std::printf("\"release_local_result\":%d,", release_local_result);
  std::printf("\"strong_before_clear\":%d,", strong_before_clear);
  std::printf("\"clear_strong_result\":%d,", clear_strong_result);
  std::printf("\"strong_after_clear\":%d,", strong_after_clear);
  std::printf("\"weak_after_clear\":%d,", weak_after_clear);
  std::printf("\"parent_release_result\":%d,", parent_release_result);
  std::printf("\"graph_after_alloc\":");
  PrintGraph(graph_after_alloc);
  std::printf(",\"graph_after_drop_local\":");
  PrintGraph(graph_after_drop_local);
  std::printf(",\"graph_after_clear\":");
  PrintGraph(graph_after_clear);
  std::printf(",\"graph_after_parent_release\":");
  PrintGraph(graph_after_parent_release);
  std::printf(",\"current_value_entry\":");
  PrintPropertyEntry(current_value_entry);
  std::printf(",\"weak_value_entry\":");
  PrintPropertyEntry(weak_value_entry);
  std::printf("}");
  return 0;
}
