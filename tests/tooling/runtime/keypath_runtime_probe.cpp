#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <cstdio>

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
    if (*cursor == '\\' || *cursor == '"') {
      std::printf("\\%c", *cursor);
    } else {
      std::printf("%c", *cursor);
    }
  }
  std::printf("\"");
}

}  // namespace

int main() {
  objc3_runtime_keypath_registry_state_snapshot registry{};
  objc3_runtime_keypath_entry_snapshot entry{};
  objc3_runtime_keypath_entry_snapshot missing{};

  (void)objc3_runtime_copy_keypath_registry_state_for_testing(&registry);
  (void)objc3_runtime_copy_keypath_entry_for_testing(1, &entry);
  (void)objc3_runtime_copy_keypath_entry_for_testing(99, &missing);

  const int component_count = objc3_runtime_keypath_component_count_for_testing(1);
  const int root_is_self = objc3_runtime_keypath_root_is_self_for_testing(1);

  std::printf("{");
  std::printf("\"keypath_table_entry_count\":%llu,",
              static_cast<unsigned long long>(registry.keypath_table_entry_count));
  std::printf("\"image_backed_keypath_count\":%llu,",
              static_cast<unsigned long long>(registry.image_backed_keypath_count));
  std::printf("\"ambiguous_keypath_handle_count\":%llu,",
              static_cast<unsigned long long>(registry.ambiguous_keypath_handle_count));
  std::printf("\"last_materialized_handle\":%llu,",
              static_cast<unsigned long long>(registry.last_materialized_handle));
  std::printf("\"entry_found\":%d,", entry.found);
  std::printf("\"entry_ambiguous\":%d,", entry.ambiguous);
  std::printf("\"entry_root_is_self\":%d,", entry.root_is_self);
  std::printf("\"entry_component_count\":%llu,",
              static_cast<unsigned long long>(entry.component_count));
  std::printf("\"entry_metadata_provider_count\":%llu,",
              static_cast<unsigned long long>(entry.metadata_provider_count));
  std::printf("\"component_count_helper\":%d,", component_count);
  std::printf("\"root_is_self_helper\":%d,", root_is_self);
  std::printf("\"missing_found\":%d,", missing.found);
  std::printf("\"root_name\":");
  PrintJsonStringOrNull(entry.root_name);
  std::printf(",\"component_path\":");
  PrintJsonStringOrNull(entry.component_path);
  std::printf(",");
  std::printf("\"profile_present\":%d,",
              entry.profile != nullptr && entry.profile[0] != '\0' ? 1 : 0);
  std::printf("\"generic_metadata_replay_key_present\":%d",
              entry.generic_metadata_replay_key != nullptr &&
                      entry.generic_metadata_replay_key[0] != '\0'
                  ? 1
                  : 0);
  std::printf("}");
  return entry.found == 1 && entry.ambiguous == 0 && component_count == 1 &&
                 root_is_self == 0
             ? 0
             : 1;
}
