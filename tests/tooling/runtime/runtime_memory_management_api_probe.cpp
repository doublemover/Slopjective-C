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

void PrintGraph(const objc3_runtime_realized_class_graph_state_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"live_instance_count\":%llu,",
              static_cast<unsigned long long>(snapshot.live_instance_count));
  std::printf("\"last_allocated_class_name\":");
  PrintJsonStringOrNull(snapshot.last_allocated_class_name);
  std::printf("}");
}

void PrintRegistrationState(
    const objc3_runtime_registration_state_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"registered_image_count\":%llu,",
              static_cast<unsigned long long>(snapshot.registered_image_count));
  std::printf(
      "\"registered_descriptor_total\":%llu,",
      static_cast<unsigned long long>(snapshot.registered_descriptor_total));
  std::printf("\"last_registration_status\":%d",
              snapshot.last_registration_status);
  std::printf("}");
}

}  // namespace

int main() {
  objc3_runtime_reset_for_testing();
  (void)objc3_runtime_replay_registered_images_for_testing();

  objc3_runtime_registration_state_snapshot registration_state{};
  objc3_runtime_realized_class_graph_state_snapshot graph_after_alloc{};
  objc3_runtime_realized_class_graph_state_snapshot graph_after_helper_release{};
  objc3_runtime_realized_class_graph_state_snapshot graph_after_clear{};
  objc3_runtime_realized_class_graph_state_snapshot graph_after_parent_release{};

  std::string alloc_class_storage;
  std::string helper_release_class_storage;
  std::string clear_class_storage;
  std::string parent_release_class_storage;

  (void)objc3_runtime_copy_registration_state_for_testing(&registration_state);

  const int parent = objc3_runtime_dispatch_i32(1024, "alloc", 0, 0, 0, 0);
  const int child = objc3_runtime_dispatch_i32(1024, "alloc", 0, 0, 0, 0);
  (void)objc3_runtime_copy_realized_class_graph_state_for_testing(&graph_after_alloc);
  StabilizeRealizedClassGraph(graph_after_alloc, alloc_class_storage);

  const int strong_set_result =
      objc3_runtime_dispatch_i32(parent, "setCurrentValue:", child, 0, 0, 0);
  const int weak_set_result =
      objc3_runtime_dispatch_i32(parent, "setWeakValue:", child, 0, 0, 0);
  const int retain_result = objc3_runtime_retain_i32(child);
  const int autorelease_result = objc3_runtime_autorelease_i32(child);
  const int release_after_helper_result = objc3_runtime_release_i32(child);
  const int release_local_result = objc3_runtime_release_i32(child);
  (void)objc3_runtime_copy_realized_class_graph_state_for_testing(
      &graph_after_helper_release);
  StabilizeRealizedClassGraph(graph_after_helper_release,
                              helper_release_class_storage);

  const int strong_before_clear =
      objc3_runtime_dispatch_i32(parent, "currentValue", 0, 0, 0, 0);
  const int weak_before_clear =
      objc3_runtime_dispatch_i32(parent, "weakValue", 0, 0, 0, 0);
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

  std::printf("{");
  std::printf("\"parent\":%d,", parent);
  std::printf("\"child\":%d,", child);
  std::printf("\"strong_set_result\":%d,", strong_set_result);
  std::printf("\"weak_set_result\":%d,", weak_set_result);
  std::printf("\"retain_result\":%d,", retain_result);
  std::printf("\"autorelease_result\":%d,", autorelease_result);
  std::printf("\"release_after_helper_result\":%d,",
              release_after_helper_result);
  std::printf("\"release_local_result\":%d,", release_local_result);
  std::printf("\"strong_before_clear\":%d,", strong_before_clear);
  std::printf("\"weak_before_clear\":%d,", weak_before_clear);
  std::printf("\"clear_strong_result\":%d,", clear_strong_result);
  std::printf("\"strong_after_clear\":%d,", strong_after_clear);
  std::printf("\"weak_after_clear\":%d,", weak_after_clear);
  std::printf("\"parent_release_result\":%d,", parent_release_result);
  std::printf("\"registration_state\":");
  PrintRegistrationState(registration_state);
  std::printf(",\"graph_after_alloc\":");
  PrintGraph(graph_after_alloc);
  std::printf(",\"graph_after_helper_release\":");
  PrintGraph(graph_after_helper_release);
  std::printf(",\"graph_after_clear\":");
  PrintGraph(graph_after_clear);
  std::printf(",\"graph_after_parent_release\":");
  PrintGraph(graph_after_parent_release);
  std::printf("}");
  return 0;
}
