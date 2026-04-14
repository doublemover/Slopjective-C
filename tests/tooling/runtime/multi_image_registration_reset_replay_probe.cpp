#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <cstdio>
#include <string>

namespace {

std::string CopyJsonString(const char *value) {
  return value == nullptr ? std::string() : std::string(value);
}

const char *NullableCString(const std::string &value) {
  return value.empty() ? nullptr : value.c_str();
}

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

void PrintUint64Field(const char *name, unsigned long long value) {
  std::printf("\"%s\":%llu,", name, value);
}

void PrintIntField(const char *name, int value) {
  std::printf("\"%s\":%d,", name, value);
}

void PrintStringField(const char *name, const char *value) {
  std::printf("\"%s\":", name);
  PrintJsonStringOrNull(value);
  std::printf(",");
}

}  // namespace

int main() {
  objc3_runtime_registration_state_snapshot startup_registration{};
  objc3_runtime_image_walk_state_snapshot startup_walk{};
  objc3_runtime_realized_class_graph_state_snapshot startup_graph{};
  objc3_runtime_realized_class_entry_snapshot startup_provider_entry{};
  objc3_runtime_realized_class_entry_snapshot startup_consumer_entry{};
  objc3_runtime_reset_replay_state_snapshot startup_replay{};

  const int startup_registration_status =
      objc3_runtime_copy_registration_state_for_testing(&startup_registration);
  const int startup_walk_status =
      objc3_runtime_copy_image_walk_state_for_testing(&startup_walk);
  const int startup_graph_status =
      objc3_runtime_copy_realized_class_graph_state_for_testing(&startup_graph);
  const int startup_provider_status =
      objc3_runtime_copy_realized_class_entry_for_testing(
          "ImportedProvider", &startup_provider_entry);
  const int startup_consumer_status =
      objc3_runtime_copy_realized_class_entry_for_testing(
          "LocalConsumer", &startup_consumer_entry);
  const int startup_replay_status =
      objc3_runtime_copy_reset_replay_state_for_testing(&startup_replay);
  const std::string startup_last_walked_module_name =
      CopyJsonString(startup_walk.last_walked_module_name);
  const std::string startup_provider_identity =
      CopyJsonString(startup_provider_entry.translation_unit_identity_key);
  const std::string startup_consumer_identity =
      CopyJsonString(startup_consumer_entry.translation_unit_identity_key);

  objc3_runtime_reset_for_testing();

  objc3_runtime_registration_state_snapshot first_reset_registration{};
  objc3_runtime_reset_replay_state_snapshot first_reset_replay{};
  const int first_reset_registration_status =
      objc3_runtime_copy_registration_state_for_testing(
          &first_reset_registration);
  const int first_reset_replay_status =
      objc3_runtime_copy_reset_replay_state_for_testing(&first_reset_replay);

  const int first_replay_status =
      objc3_runtime_replay_registered_images_for_testing();

  objc3_runtime_registration_state_snapshot first_replay_registration{};
  objc3_runtime_image_walk_state_snapshot first_replay_walk{};
  objc3_runtime_realized_class_graph_state_snapshot first_replay_graph{};
  objc3_runtime_realized_class_entry_snapshot first_replay_provider_entry{};
  objc3_runtime_realized_class_entry_snapshot first_replay_consumer_entry{};
  objc3_runtime_reset_replay_state_snapshot first_replay_state{};
  const int first_replay_registration_status =
      objc3_runtime_copy_registration_state_for_testing(
          &first_replay_registration);
  const int first_replay_walk_status =
      objc3_runtime_copy_image_walk_state_for_testing(&first_replay_walk);
  const int first_replay_graph_status =
      objc3_runtime_copy_realized_class_graph_state_for_testing(
          &first_replay_graph);
  const int first_replay_provider_status =
      objc3_runtime_copy_realized_class_entry_for_testing(
          "ImportedProvider", &first_replay_provider_entry);
  const int first_replay_consumer_status =
      objc3_runtime_copy_realized_class_entry_for_testing(
          "LocalConsumer", &first_replay_consumer_entry);
  const int first_replay_state_status =
      objc3_runtime_copy_reset_replay_state_for_testing(&first_replay_state);
  const std::string first_replay_last_walked_module_name =
      CopyJsonString(first_replay_walk.last_walked_module_name);
  const std::string first_replay_last_replayed_module_name =
      CopyJsonString(first_replay_state.last_replayed_module_name);
  const std::string first_replay_provider_identity =
      CopyJsonString(first_replay_provider_entry.translation_unit_identity_key);
  const std::string first_replay_consumer_identity =
      CopyJsonString(first_replay_consumer_entry.translation_unit_identity_key);

  const int replay_without_reset_status =
      objc3_runtime_replay_registered_images_for_testing();
  objc3_runtime_reset_replay_state_snapshot blocked_replay_state{};
  const int blocked_replay_state_status =
      objc3_runtime_copy_reset_replay_state_for_testing(&blocked_replay_state);

  objc3_runtime_reset_for_testing();

  objc3_runtime_registration_state_snapshot second_reset_registration{};
  objc3_runtime_reset_replay_state_snapshot second_reset_replay{};
  const int second_reset_registration_status =
      objc3_runtime_copy_registration_state_for_testing(
          &second_reset_registration);
  const int second_reset_replay_status =
      objc3_runtime_copy_reset_replay_state_for_testing(&second_reset_replay);

  const int second_replay_status =
      objc3_runtime_replay_registered_images_for_testing();

  objc3_runtime_registration_state_snapshot second_replay_registration{};
  objc3_runtime_image_walk_state_snapshot second_replay_walk{};
  objc3_runtime_realized_class_graph_state_snapshot second_replay_graph{};
  objc3_runtime_realized_class_entry_snapshot second_replay_provider_entry{};
  objc3_runtime_realized_class_entry_snapshot second_replay_consumer_entry{};
  objc3_runtime_reset_replay_state_snapshot second_replay_state{};
  const int second_replay_registration_status =
      objc3_runtime_copy_registration_state_for_testing(
          &second_replay_registration);
  const int second_replay_walk_status =
      objc3_runtime_copy_image_walk_state_for_testing(&second_replay_walk);
  const int second_replay_graph_status =
      objc3_runtime_copy_realized_class_graph_state_for_testing(
          &second_replay_graph);
  const int second_replay_provider_status =
      objc3_runtime_copy_realized_class_entry_for_testing(
          "ImportedProvider", &second_replay_provider_entry);
  const int second_replay_consumer_status =
      objc3_runtime_copy_realized_class_entry_for_testing(
          "LocalConsumer", &second_replay_consumer_entry);
  const int second_replay_state_status =
      objc3_runtime_copy_reset_replay_state_for_testing(&second_replay_state);
  const std::string second_replay_last_walked_module_name =
      CopyJsonString(second_replay_walk.last_walked_module_name);
  const std::string second_replay_last_replayed_module_name =
      CopyJsonString(second_replay_state.last_replayed_module_name);
  const std::string second_replay_provider_identity =
      CopyJsonString(second_replay_provider_entry.translation_unit_identity_key);
  const std::string second_replay_consumer_identity =
      CopyJsonString(second_replay_consumer_entry.translation_unit_identity_key);

  std::printf("{");
  PrintIntField("startup_registration_status", startup_registration_status);
  PrintIntField("startup_walk_status", startup_walk_status);
  PrintIntField("startup_graph_status", startup_graph_status);
  PrintIntField("startup_provider_status", startup_provider_status);
  PrintIntField("startup_consumer_status", startup_consumer_status);
  PrintIntField("startup_replay_status", startup_replay_status);
  PrintUint64Field("startup_registered_image_count",
                   startup_registration.registered_image_count);
  PrintUint64Field("startup_next_expected_registration_order_ordinal",
                   startup_registration.next_expected_registration_order_ordinal);
  PrintUint64Field("startup_walked_image_count", startup_walk.walked_image_count);
  PrintUint64Field("startup_realized_class_count",
                   startup_graph.realized_class_count);
  PrintUint64Field("startup_retained_bootstrap_image_count",
                   startup_replay.retained_bootstrap_image_count);
  PrintStringField("startup_last_walked_module_name",
                   NullableCString(startup_last_walked_module_name));
  PrintUint64Field("startup_provider_registration_order_ordinal",
                   startup_provider_entry.registration_order_ordinal);
  PrintUint64Field("startup_consumer_registration_order_ordinal",
                   startup_consumer_entry.registration_order_ordinal);
  PrintStringField("startup_provider_identity",
                   NullableCString(startup_provider_identity));
  PrintStringField("startup_consumer_identity",
                   NullableCString(startup_consumer_identity));

  PrintIntField("first_reset_registration_status",
                first_reset_registration_status);
  PrintIntField("first_reset_replay_status", first_reset_replay_status);
  PrintUint64Field("first_reset_registered_image_count",
                   first_reset_registration.registered_image_count);
  PrintUint64Field("first_reset_next_expected_registration_order_ordinal",
                   first_reset_registration.next_expected_registration_order_ordinal);
  PrintUint64Field("first_reset_retained_bootstrap_image_count",
                   first_reset_replay.retained_bootstrap_image_count);
  PrintUint64Field("first_reset_cleared_image_local_init_state_count",
                   first_reset_replay.last_reset_cleared_image_local_init_state_count);
  PrintUint64Field("first_reset_generation", first_reset_replay.reset_generation);

  PrintIntField("first_replay_status", first_replay_status);
  PrintIntField("first_replay_registration_status",
                first_replay_registration_status);
  PrintIntField("first_replay_walk_status", first_replay_walk_status);
  PrintIntField("first_replay_graph_status", first_replay_graph_status);
  PrintIntField("first_replay_provider_status", first_replay_provider_status);
  PrintIntField("first_replay_consumer_status", first_replay_consumer_status);
  PrintIntField("first_replay_state_status", first_replay_state_status);
  PrintUint64Field("first_replay_registered_image_count",
                   first_replay_registration.registered_image_count);
  PrintUint64Field("first_replay_next_expected_registration_order_ordinal",
                   first_replay_registration.next_expected_registration_order_ordinal);
  PrintUint64Field("first_replay_walked_image_count",
                   first_replay_walk.walked_image_count);
  PrintUint64Field("first_replay_realized_class_count",
                   first_replay_graph.realized_class_count);
  PrintUint64Field("first_replay_last_replayed_image_count",
                   first_replay_state.last_replayed_image_count);
  PrintUint64Field("first_replay_generation",
                   first_replay_state.replay_generation);
  PrintStringField("first_replay_last_walked_module_name",
                   NullableCString(first_replay_last_walked_module_name));
  PrintStringField("first_replay_last_replayed_module_name",
                   NullableCString(first_replay_last_replayed_module_name));
  PrintUint64Field("first_replay_provider_registration_order_ordinal",
                   first_replay_provider_entry.registration_order_ordinal);
  PrintUint64Field("first_replay_consumer_registration_order_ordinal",
                   first_replay_consumer_entry.registration_order_ordinal);
  PrintStringField("first_replay_provider_identity",
                   NullableCString(first_replay_provider_identity));
  PrintStringField("first_replay_consumer_identity",
                   NullableCString(first_replay_consumer_identity));

  PrintIntField("replay_without_reset_status", replay_without_reset_status);
  PrintIntField("blocked_replay_state_status", blocked_replay_state_status);
  PrintIntField("blocked_replay_last_replay_status",
                blocked_replay_state.last_replay_status);
  PrintUint64Field("blocked_replay_last_replayed_image_count",
                   blocked_replay_state.last_replayed_image_count);

  PrintIntField("second_reset_registration_status",
                second_reset_registration_status);
  PrintIntField("second_reset_replay_status", second_reset_replay_status);
  PrintUint64Field("second_reset_registered_image_count",
                   second_reset_registration.registered_image_count);
  PrintUint64Field("second_reset_next_expected_registration_order_ordinal",
                   second_reset_registration.next_expected_registration_order_ordinal);
  PrintUint64Field("second_reset_retained_bootstrap_image_count",
                   second_reset_replay.retained_bootstrap_image_count);
  PrintUint64Field("second_reset_cleared_image_local_init_state_count",
                   second_reset_replay.last_reset_cleared_image_local_init_state_count);
  PrintUint64Field("second_reset_generation", second_reset_replay.reset_generation);

  PrintIntField("second_replay_status", second_replay_status);
  PrintIntField("second_replay_registration_status",
                second_replay_registration_status);
  PrintIntField("second_replay_walk_status", second_replay_walk_status);
  PrintIntField("second_replay_graph_status", second_replay_graph_status);
  PrintIntField("second_replay_provider_status", second_replay_provider_status);
  PrintIntField("second_replay_consumer_status", second_replay_consumer_status);
  PrintIntField("second_replay_state_status", second_replay_state_status);
  PrintUint64Field("second_replay_registered_image_count",
                   second_replay_registration.registered_image_count);
  PrintUint64Field("second_replay_next_expected_registration_order_ordinal",
                   second_replay_registration.next_expected_registration_order_ordinal);
  PrintUint64Field("second_replay_walked_image_count",
                   second_replay_walk.walked_image_count);
  PrintUint64Field("second_replay_realized_class_count",
                   second_replay_graph.realized_class_count);
  PrintUint64Field("second_replay_last_replayed_image_count",
                   second_replay_state.last_replayed_image_count);
  PrintUint64Field("second_replay_generation",
                   second_replay_state.replay_generation);
  PrintStringField("second_replay_last_walked_module_name",
                   NullableCString(second_replay_last_walked_module_name));
  PrintStringField("second_replay_last_replayed_module_name",
                   NullableCString(second_replay_last_replayed_module_name));
  PrintUint64Field("second_replay_provider_registration_order_ordinal",
                   second_replay_provider_entry.registration_order_ordinal);
  PrintUint64Field("second_replay_consumer_registration_order_ordinal",
                   second_replay_consumer_entry.registration_order_ordinal);
  PrintStringField("second_replay_provider_identity",
                   NullableCString(second_replay_provider_identity));
  std::printf("\"second_replay_consumer_identity\":");
  PrintJsonStringOrNull(NullableCString(second_replay_consumer_identity));
  std::printf("}\n");
  return 0;
}
