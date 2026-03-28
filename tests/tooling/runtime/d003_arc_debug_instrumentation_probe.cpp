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

void StabilizeArcDebugSnapshot(objc3_runtime_arc_debug_state_snapshot &snapshot,
                               std::string &property_name_storage,
                               std::string &owner_storage) {
  StabilizeNullableCString(snapshot.last_property_name, property_name_storage,
                           snapshot.last_property_name);
  StabilizeNullableCString(snapshot.last_property_owner_identity, owner_storage,
                           snapshot.last_property_owner_identity);
}

void PrintArcDebugSnapshot(const objc3_runtime_arc_debug_state_snapshot &snapshot) {
  // Stable JSON field anchors for tooling contracts:
  // "last_property_name"
  // "last_property_owner_identity"
  std::printf("{");
  std::printf("\"retain_call_count\":%llu,",
              static_cast<unsigned long long>(snapshot.retain_call_count));
  std::printf("\"release_call_count\":%llu,",
              static_cast<unsigned long long>(snapshot.release_call_count));
  std::printf("\"autorelease_call_count\":%llu,",
              static_cast<unsigned long long>(snapshot.autorelease_call_count));
  std::printf("\"autoreleasepool_push_count\":%llu,",
              static_cast<unsigned long long>(snapshot.autoreleasepool_push_count));
  std::printf("\"autoreleasepool_pop_count\":%llu,",
              static_cast<unsigned long long>(snapshot.autoreleasepool_pop_count));
  std::printf("\"current_property_read_count\":%llu,",
              static_cast<unsigned long long>(snapshot.current_property_read_count));
  std::printf("\"current_property_write_count\":%llu,",
              static_cast<unsigned long long>(snapshot.current_property_write_count));
  std::printf("\"current_property_exchange_count\":%llu,",
              static_cast<unsigned long long>(snapshot.current_property_exchange_count));
  std::printf("\"weak_current_property_load_count\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.weak_current_property_load_count));
  std::printf("\"weak_current_property_store_count\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.weak_current_property_store_count));
  std::printf("\"last_retain_value\":%d,", snapshot.last_retain_value);
  std::printf("\"last_release_value\":%d,", snapshot.last_release_value);
  std::printf("\"last_autorelease_value\":%d,",
              snapshot.last_autorelease_value);
  std::printf("\"last_property_read_value\":%d,",
              snapshot.last_property_read_value);
  std::printf("\"last_property_written_value\":%d,",
              snapshot.last_property_written_value);
  std::printf("\"last_property_exchange_previous_value\":%d,",
              snapshot.last_property_exchange_previous_value);
  std::printf("\"last_property_exchange_new_value\":%d,",
              snapshot.last_property_exchange_new_value);
  std::printf("\"last_weak_loaded_value\":%d,",
              snapshot.last_weak_loaded_value);
  std::printf("\"last_weak_stored_value\":%d,",
              snapshot.last_weak_stored_value);
  std::printf("\"last_property_receiver\":%d,",
              snapshot.last_property_receiver);
  std::printf("\"last_property_name\":");
  PrintJsonStringOrNull(snapshot.last_property_name);
  std::printf(",\"last_property_owner_identity\":");
  PrintJsonStringOrNull(snapshot.last_property_owner_identity);
  std::printf("}");
}

}  // namespace

int main() {
  objc3_runtime_reset_for_testing();
  (void)objc3_runtime_replay_registered_images_for_testing();

  objc3_runtime_arc_debug_state_snapshot inside{};
  objc3_runtime_arc_debug_state_snapshot after{};
  std::string inside_property_name_storage;
  std::string inside_owner_storage;
  std::string after_property_name_storage;
  std::string after_owner_storage;

  const int parent = objc3_runtime_dispatch_i32(1024, "alloc", 0, 0, 0, 0);
  const int child = objc3_runtime_dispatch_i32(1024, "alloc", 0, 0, 0, 0);
  const int bind_current_status = objc3_runtime_bind_current_property_context_for_testing(
      parent, "ArcBox", "currentValue");
  const int strong_set_result = objc3_runtime_exchange_current_property_i32(child);
  const int release_local_result = objc3_runtime_release_i32(child);
  const int retained = objc3_runtime_retain_i32(9);

  objc3_runtime_push_autoreleasepool_scope();
  const int autoreleased = objc3_runtime_autorelease_i32(retained);
  const int getter_value = objc3_runtime_read_current_property_i32();
  const int bind_weak_status = objc3_runtime_bind_current_property_context_for_testing(
      parent, "ArcBox", "weakValue");
  objc3_runtime_store_weak_current_property_i32(getter_value);
  const int weak_set_result = getter_value;
  const int weak_inside_pool = objc3_runtime_load_weak_current_property_i32();
  const int rebind_current_status = objc3_runtime_bind_current_property_context_for_testing(
      parent, "ArcBox", "currentValue");
  const int clear_strong_result = objc3_runtime_exchange_current_property_i32(0);
  const int rebind_weak_status = objc3_runtime_bind_current_property_context_for_testing(
      parent, "ArcBox", "weakValue");
  (void)objc3_runtime_copy_arc_debug_state_for_testing(&inside);
  StabilizeArcDebugSnapshot(inside, inside_property_name_storage,
                            inside_owner_storage);

  objc3_runtime_pop_autoreleasepool_scope();
  const int weak_after_pool = objc3_runtime_load_weak_current_property_i32();
  objc3_runtime_clear_current_property_context_for_testing();
  const int released = objc3_runtime_release_i32(retained);
  const int parent_release_result = objc3_runtime_release_i32(parent);
  (void)objc3_runtime_copy_arc_debug_state_for_testing(&after);
  StabilizeArcDebugSnapshot(after, after_property_name_storage,
                            after_owner_storage);

  std::printf("{");
  std::printf("\"parent\":%d,", parent);
  std::printf("\"child\":%d,", child);
  std::printf("\"bind_current_status\":%d,", bind_current_status);
  std::printf("\"strong_set_result\":%d,", strong_set_result);
  std::printf("\"release_local_result\":%d,", release_local_result);
  std::printf("\"retained\":%d,", retained);
  std::printf("\"autoreleased\":%d,", autoreleased);
  std::printf("\"getter_value\":%d,", getter_value);
  std::printf("\"bind_weak_status\":%d,", bind_weak_status);
  std::printf("\"weak_set_result\":%d,", weak_set_result);
  std::printf("\"rebind_current_status\":%d,", rebind_current_status);
  std::printf("\"clear_strong_result\":%d,", clear_strong_result);
  std::printf("\"rebind_weak_status\":%d,", rebind_weak_status);
  std::printf("\"weak_inside_pool\":%d,", weak_inside_pool);
  std::printf("\"weak_after_pool\":%d,", weak_after_pool);
  std::printf("\"released\":%d,", released);
  std::printf("\"parent_release_result\":%d,", parent_release_result);
  std::printf("\"inside\":");
  PrintArcDebugSnapshot(inside);
  std::printf(",\"after\":");
  PrintArcDebugSnapshot(after);
  std::printf("}\n");
  return 0;
}
