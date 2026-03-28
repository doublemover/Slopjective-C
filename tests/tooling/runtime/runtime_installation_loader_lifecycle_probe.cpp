#include "runtime/objc3_runtime.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <cstdio>
#include <string>

#ifndef OBJC3_RUNTIME_FIXTURE_MODULE_NAME
#error "OBJC3_RUNTIME_FIXTURE_MODULE_NAME must be provided by the generated fixture config"
#endif

#ifndef OBJC3_RUNTIME_FIXTURE_TRANSLATION_UNIT_IDENTITY_KEY
#error "OBJC3_RUNTIME_FIXTURE_TRANSLATION_UNIT_IDENTITY_KEY must be provided by the generated fixture config"
#endif

#ifndef OBJC3_RUNTIME_FIXTURE_REGISTRATION_ORDER_ORDINAL
#error "OBJC3_RUNTIME_FIXTURE_REGISTRATION_ORDER_ORDINAL must be provided by the generated fixture config"
#endif

#ifndef OBJC3_RUNTIME_FIXTURE_CLASS_DESCRIPTOR_COUNT
#error "OBJC3_RUNTIME_FIXTURE_CLASS_DESCRIPTOR_COUNT must be provided by the generated fixture config"
#endif

#ifndef OBJC3_RUNTIME_FIXTURE_PROTOCOL_DESCRIPTOR_COUNT
#error "OBJC3_RUNTIME_FIXTURE_PROTOCOL_DESCRIPTOR_COUNT must be provided by the generated fixture config"
#endif

#ifndef OBJC3_RUNTIME_FIXTURE_CATEGORY_DESCRIPTOR_COUNT
#error "OBJC3_RUNTIME_FIXTURE_CATEGORY_DESCRIPTOR_COUNT must be provided by the generated fixture config"
#endif

#ifndef OBJC3_RUNTIME_FIXTURE_PROPERTY_DESCRIPTOR_COUNT
#error "OBJC3_RUNTIME_FIXTURE_PROPERTY_DESCRIPTOR_COUNT must be provided by the generated fixture config"
#endif

#ifndef OBJC3_RUNTIME_FIXTURE_IVAR_DESCRIPTOR_COUNT
#error "OBJC3_RUNTIME_FIXTURE_IVAR_DESCRIPTOR_COUNT must be provided by the generated fixture config"
#endif

namespace {

template <std::size_t EntryCount>
struct ProbeAggregate {
  std::uint64_t count;
  const void *entries[EntryCount];
};

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

}  // namespace

int main() {
  objc3_runtime_registration_state_snapshot startup_registration{};
  const int startup_registration_copy_status =
      objc3_runtime_copy_registration_state_for_testing(&startup_registration);
  const std::string startup_module_name =
      startup_registration.last_registered_module_name != nullptr
          ? startup_registration.last_registered_module_name
          : "";
  const std::string startup_identity_key =
      startup_registration.last_registered_translation_unit_identity_key != nullptr
          ? startup_registration.last_registered_translation_unit_identity_key
          : "";
  objc3_runtime_image_walk_state_snapshot startup_walk{};
  const int startup_image_walk_copy_status =
      objc3_runtime_copy_image_walk_state_for_testing(&startup_walk);
  objc3_runtime_reset_replay_state_snapshot startup_reset_replay{};
  const int startup_reset_replay_copy_status =
      objc3_runtime_copy_reset_replay_state_for_testing(&startup_reset_replay);

  const std::uint64_t startup_registration_ordinal =
      startup_registration.last_successful_registration_order_ordinal == 0
          ? 1u
          : startup_registration.last_successful_registration_order_ordinal;
  const objc3_runtime_image_descriptor duplicate_image{
      startup_module_name.empty() ? "invalid-module" : startup_module_name.c_str(),
      startup_identity_key.empty() ? "invalid-identity" : startup_identity_key.c_str(),
      startup_registration_ordinal,
      startup_walk.last_walked_class_descriptor_count,
      startup_walk.last_walked_protocol_descriptor_count,
      startup_walk.last_walked_category_descriptor_count,
      startup_walk.last_walked_property_descriptor_count,
      startup_walk.last_walked_ivar_descriptor_count,
  };
  const std::string out_of_order_identity =
      startup_identity_key.empty()
          ? "out-of-order-identity"
          : startup_identity_key + "-out-of-order";
  const objc3_runtime_image_descriptor out_of_order_image{
      "out-of-order-module",
      out_of_order_identity.c_str(),
      startup_registration.next_expected_registration_order_ordinal + 1u,
      startup_walk.last_walked_class_descriptor_count,
      startup_walk.last_walked_protocol_descriptor_count,
      startup_walk.last_walked_category_descriptor_count,
      startup_walk.last_walked_property_descriptor_count,
      startup_walk.last_walked_ivar_descriptor_count,
  };
  const int duplicate_status = objc3_runtime_register_image(&duplicate_image);
  objc3_runtime_registration_state_snapshot after_duplicate_registration{};
  objc3_runtime_image_walk_state_snapshot after_duplicate_walk{};
  const int after_duplicate_registration_copy_status =
      objc3_runtime_copy_registration_state_for_testing(
          &after_duplicate_registration);
  const std::string after_duplicate_last_rejected_module_name =
      after_duplicate_registration.last_rejected_module_name != nullptr
          ? after_duplicate_registration.last_rejected_module_name
          : "";
  const std::string after_duplicate_last_rejected_translation_unit_identity_key =
      after_duplicate_registration.last_rejected_translation_unit_identity_key !=
              nullptr
          ? after_duplicate_registration.last_rejected_translation_unit_identity_key
          : "";
  const int after_duplicate_image_walk_copy_status =
      objc3_runtime_copy_image_walk_state_for_testing(&after_duplicate_walk);
  const int out_of_order_status =
      objc3_runtime_register_image(&out_of_order_image);
  objc3_runtime_registration_state_snapshot after_out_of_order_registration{};
  objc3_runtime_image_walk_state_snapshot after_out_of_order_walk{};
  const int after_out_of_order_registration_copy_status =
      objc3_runtime_copy_registration_state_for_testing(
          &after_out_of_order_registration);
  const std::string after_out_of_order_last_rejected_module_name =
      after_out_of_order_registration.last_rejected_module_name != nullptr
          ? after_out_of_order_registration.last_rejected_module_name
          : "";
  const std::string after_out_of_order_last_rejected_translation_unit_identity_key =
      after_out_of_order_registration
                  .last_rejected_translation_unit_identity_key != nullptr
          ? after_out_of_order_registration
                .last_rejected_translation_unit_identity_key
          : "";
  const int after_out_of_order_image_walk_copy_status =
      objc3_runtime_copy_image_walk_state_for_testing(&after_out_of_order_walk);

  objc3_runtime_reset_for_testing();

  objc3_runtime_registration_state_snapshot post_reset_registration{};
  objc3_runtime_reset_replay_state_snapshot post_reset_reset_replay{};
  const int post_reset_registration_copy_status =
      objc3_runtime_copy_registration_state_for_testing(&post_reset_registration);
  const int post_reset_reset_replay_copy_status =
      objc3_runtime_copy_reset_replay_state_for_testing(
          &post_reset_reset_replay);

  static const int kClassDescriptorSlots[4] = {1, 2, 3, 4};
  static const int kProtocolDescriptorSlots[2] = {5, 6};
  static const int kCategoryDescriptorSlots[2] = {7, 8};
  static const int kDiscoveryPaddingSlots[2] = {9, 10};
  static unsigned char kImageLocalInitState = 0;
  const objc3_runtime_image_descriptor compiled_image_descriptor{
      OBJC3_RUNTIME_FIXTURE_MODULE_NAME,
      OBJC3_RUNTIME_FIXTURE_TRANSLATION_UNIT_IDENTITY_KEY,
      OBJC3_RUNTIME_FIXTURE_REGISTRATION_ORDER_ORDINAL,
      OBJC3_RUNTIME_FIXTURE_CLASS_DESCRIPTOR_COUNT,
      OBJC3_RUNTIME_FIXTURE_PROTOCOL_DESCRIPTOR_COUNT,
      OBJC3_RUNTIME_FIXTURE_CATEGORY_DESCRIPTOR_COUNT,
      OBJC3_RUNTIME_FIXTURE_PROPERTY_DESCRIPTOR_COUNT,
      OBJC3_RUNTIME_FIXTURE_IVAR_DESCRIPTOR_COUNT,
  };
  const ProbeAggregate<4> class_root = {
      OBJC3_RUNTIME_FIXTURE_CLASS_DESCRIPTOR_COUNT,
      {
          &kClassDescriptorSlots[0],
          &kClassDescriptorSlots[1],
          &kClassDescriptorSlots[2],
          &kClassDescriptorSlots[3],
      },
  };
  const ProbeAggregate<2> protocol_root = {
      OBJC3_RUNTIME_FIXTURE_PROTOCOL_DESCRIPTOR_COUNT,
      {
          &kProtocolDescriptorSlots[0],
          &kProtocolDescriptorSlots[1],
      },
  };
  const ProbeAggregate<2> category_root = {
      OBJC3_RUNTIME_FIXTURE_CATEGORY_DESCRIPTOR_COUNT,
      {
          &kCategoryDescriptorSlots[0],
          &kCategoryDescriptorSlots[1],
      },
  };
  const ProbeAggregate<1> property_root = {
      OBJC3_RUNTIME_FIXTURE_PROPERTY_DESCRIPTOR_COUNT,
      {nullptr},
  };
  const ProbeAggregate<1> ivar_root = {
      OBJC3_RUNTIME_FIXTURE_IVAR_DESCRIPTOR_COUNT,
      {nullptr},
  };
  const ProbeAggregate<6> valid_discovery_root = {
      6u,
      {
          &compiled_image_descriptor,
          &class_root,
          &protocol_root,
          &category_root,
          &property_root,
          &ivar_root,
      },
  };
  const void *valid_linker_anchor_target = &valid_discovery_root;
  const void *invalid_linker_anchor_target = &class_root;
  const ProbeAggregate<6> invalid_discovery_root = {
      6u,
      {
          &compiled_image_descriptor,
          &class_root,
          &protocol_root,
          &property_root,
          &ivar_root,
          &kDiscoveryPaddingSlots[0],
      },
  };

  objc3_runtime_registration_table invalid_anchor_table = {
      2u,
      12u,
      &compiled_image_descriptor,
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
          &valid_discovery_root),
      &invalid_linker_anchor_target,
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(&class_root),
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(&protocol_root),
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(&category_root),
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(&property_root),
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(&ivar_root),
      nullptr,
      nullptr,
      nullptr,
      &kImageLocalInitState,
  };
  objc3_runtime_stage_registration_table_for_bootstrap(&invalid_anchor_table);
  const int post_reset_invalid_anchor_status =
      objc3_runtime_register_image(&compiled_image_descriptor);
  objc3_runtime_registration_state_snapshot after_invalid_anchor_registration{};
  objc3_runtime_image_walk_state_snapshot after_invalid_anchor_walk{};
  const int after_invalid_anchor_registration_copy_status =
      objc3_runtime_copy_registration_state_for_testing(
          &after_invalid_anchor_registration);
  const int after_invalid_anchor_image_walk_copy_status =
      objc3_runtime_copy_image_walk_state_for_testing(&after_invalid_anchor_walk);

  objc3_runtime_registration_table invalid_discovery_root_table = {
      2u,
      12u,
      &compiled_image_descriptor,
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(
          &invalid_discovery_root),
      &valid_linker_anchor_target,
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(&class_root),
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(&protocol_root),
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(&category_root),
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(&property_root),
      reinterpret_cast<const objc3_runtime_pointer_aggregate *>(&ivar_root),
      nullptr,
      nullptr,
      nullptr,
      &kImageLocalInitState,
  };
  objc3_runtime_stage_registration_table_for_bootstrap(
      &invalid_discovery_root_table);
  const int post_reset_invalid_discovery_root_status =
      objc3_runtime_register_image(&compiled_image_descriptor);
  objc3_runtime_registration_state_snapshot
      after_invalid_discovery_root_registration{};
  objc3_runtime_image_walk_state_snapshot after_invalid_discovery_root_walk{};
  const int after_invalid_discovery_root_registration_copy_status =
      objc3_runtime_copy_registration_state_for_testing(
          &after_invalid_discovery_root_registration);
  const int after_invalid_discovery_root_image_walk_copy_status =
      objc3_runtime_copy_image_walk_state_for_testing(
          &after_invalid_discovery_root_walk);

  const int replay_status = objc3_runtime_replay_registered_images_for_testing();

  objc3_runtime_registration_state_snapshot post_replay_registration{};
  const int post_replay_registration_copy_status =
      objc3_runtime_copy_registration_state_for_testing(&post_replay_registration);
  const std::string post_replay_last_registered_module_name =
      post_replay_registration.last_registered_module_name != nullptr
          ? post_replay_registration.last_registered_module_name
          : "";
  const std::string post_replay_last_registered_translation_unit_identity_key =
      post_replay_registration.last_registered_translation_unit_identity_key !=
              nullptr
          ? post_replay_registration.last_registered_translation_unit_identity_key
          : "";
  objc3_runtime_image_walk_state_snapshot post_replay_walk{};
  const int post_replay_image_walk_copy_status =
      objc3_runtime_copy_image_walk_state_for_testing(&post_replay_walk);
  const std::string post_replay_last_walked_module_name =
      post_replay_walk.last_walked_module_name != nullptr
          ? post_replay_walk.last_walked_module_name
          : "";
  const std::string post_replay_last_walked_translation_unit_identity_key =
      post_replay_walk.last_walked_translation_unit_identity_key != nullptr
          ? post_replay_walk.last_walked_translation_unit_identity_key
          : "";
  objc3_runtime_reset_replay_state_snapshot post_replay_reset_replay{};
  const int post_replay_reset_replay_copy_status =
      objc3_runtime_copy_reset_replay_state_for_testing(
          &post_replay_reset_replay);
  const std::string post_replay_last_replayed_module_name =
      post_replay_reset_replay.last_replayed_module_name != nullptr
          ? post_replay_reset_replay.last_replayed_module_name
          : "";
  const std::string post_replay_last_replayed_translation_unit_identity_key =
      post_replay_reset_replay.last_replayed_translation_unit_identity_key !=
              nullptr
          ? post_replay_reset_replay.last_replayed_translation_unit_identity_key
          : "";

  std::printf("{");
  std::printf("\"startup_registration_copy_status\":%d,",
              startup_registration_copy_status);
  std::printf("\"startup_image_walk_copy_status\":%d,",
              startup_image_walk_copy_status);
  std::printf("\"startup_reset_replay_copy_status\":%d,",
              startup_reset_replay_copy_status);
  std::printf("\"startup_registered_image_count\":%llu,",
              static_cast<unsigned long long>(
                  startup_registration.registered_image_count));
  std::printf("\"startup_next_expected_registration_order_ordinal\":%llu,",
              static_cast<unsigned long long>(
                  startup_registration.next_expected_registration_order_ordinal));
  std::printf("\"startup_walked_image_count\":%llu,",
              static_cast<unsigned long long>(startup_walk.walked_image_count));
  std::printf("\"startup_last_discovery_root_entry_count\":%llu,",
              static_cast<unsigned long long>(
                  startup_walk.last_discovery_root_entry_count));
  std::printf("\"startup_last_registration_used_staged_table\":%d,",
              startup_walk.last_registration_used_staged_table);
  std::printf("\"startup_retained_bootstrap_image_count\":%llu,",
              static_cast<unsigned long long>(
                  startup_reset_replay.retained_bootstrap_image_count));
  std::printf("\"startup_last_registered_module_name\":");
  PrintJsonStringOrNull(startup_registration.last_registered_module_name);
  std::printf(",\"startup_last_registered_translation_unit_identity_key\":");
  PrintJsonStringOrNull(
      startup_registration.last_registered_translation_unit_identity_key);
  std::printf(",\"duplicate_status\":%d,", duplicate_status);
  std::printf("\"after_duplicate_registration_copy_status\":%d,",
              after_duplicate_registration_copy_status);
  std::printf("\"after_duplicate_image_walk_copy_status\":%d,",
              after_duplicate_image_walk_copy_status);
  std::printf("\"after_duplicate_registered_image_count\":%llu,",
              static_cast<unsigned long long>(
                  after_duplicate_registration.registered_image_count));
  std::printf("\"after_duplicate_next_expected_registration_order_ordinal\":%llu,",
              static_cast<unsigned long long>(after_duplicate_registration
                                                  .next_expected_registration_order_ordinal));
  std::printf("\"after_duplicate_last_successful_registration_order_ordinal\":%llu,",
              static_cast<unsigned long long>(after_duplicate_registration
                                                  .last_successful_registration_order_ordinal));
  std::printf("\"after_duplicate_last_registration_status\":%d,",
              after_duplicate_registration.last_registration_status);
  std::printf("\"after_duplicate_last_rejected_module_name\":");
  PrintJsonStringOrNull(after_duplicate_last_rejected_module_name.c_str());
  std::printf(",\"after_duplicate_last_rejected_translation_unit_identity_key\":");
  PrintJsonStringOrNull(
      after_duplicate_last_rejected_translation_unit_identity_key.c_str());
  std::printf(",\"after_duplicate_last_rejected_registration_order_ordinal\":%llu,",
              static_cast<unsigned long long>(after_duplicate_registration
                                                  .last_rejected_registration_order_ordinal));
  std::printf("\"after_duplicate_walked_image_count\":%llu,",
              static_cast<unsigned long long>(
                  after_duplicate_walk.walked_image_count));
  std::printf("\"out_of_order_status\":%d,", out_of_order_status);
  std::printf("\"after_out_of_order_registration_copy_status\":%d,",
              after_out_of_order_registration_copy_status);
  std::printf("\"after_out_of_order_image_walk_copy_status\":%d,",
              after_out_of_order_image_walk_copy_status);
  std::printf("\"after_out_of_order_registered_image_count\":%llu,",
              static_cast<unsigned long long>(
                  after_out_of_order_registration.registered_image_count));
  std::printf("\"after_out_of_order_next_expected_registration_order_ordinal\":%llu,",
              static_cast<unsigned long long>(after_out_of_order_registration
                                                  .next_expected_registration_order_ordinal));
  std::printf("\"after_out_of_order_last_successful_registration_order_ordinal\":%llu,",
              static_cast<unsigned long long>(after_out_of_order_registration
                                                  .last_successful_registration_order_ordinal));
  std::printf("\"after_out_of_order_last_registration_status\":%d,",
              after_out_of_order_registration.last_registration_status);
  std::printf("\"after_out_of_order_last_rejected_module_name\":");
  PrintJsonStringOrNull(after_out_of_order_last_rejected_module_name.c_str());
  std::printf(",\"after_out_of_order_last_rejected_translation_unit_identity_key\":");
  PrintJsonStringOrNull(
      after_out_of_order_last_rejected_translation_unit_identity_key.c_str());
  std::printf(
      ",\"after_out_of_order_last_rejected_registration_order_ordinal\":%llu,",
      static_cast<unsigned long long>(after_out_of_order_registration
                                          .last_rejected_registration_order_ordinal));
  std::printf("\"after_out_of_order_walked_image_count\":%llu,",
              static_cast<unsigned long long>(
                  after_out_of_order_walk.walked_image_count));

  std::printf("\"post_reset_registration_copy_status\":%d,",
              post_reset_registration_copy_status);
  std::printf("\"post_reset_reset_replay_copy_status\":%d,",
              post_reset_reset_replay_copy_status);
  std::printf("\"post_reset_registered_image_count\":%llu,",
              static_cast<unsigned long long>(
                  post_reset_registration.registered_image_count));
  std::printf("\"post_reset_next_expected_registration_order_ordinal\":%llu,",
              static_cast<unsigned long long>(
                  post_reset_registration.next_expected_registration_order_ordinal));
  std::printf("\"post_reset_retained_bootstrap_image_count\":%llu,",
              static_cast<unsigned long long>(
                  post_reset_reset_replay.retained_bootstrap_image_count));
  std::printf("\"post_reset_last_reset_cleared_image_local_init_state_count\":%llu,",
              static_cast<unsigned long long>(
                  post_reset_reset_replay
                      .last_reset_cleared_image_local_init_state_count));
  std::printf("\"post_reset_invalid_anchor_status\":%d,",
              post_reset_invalid_anchor_status);
  std::printf("\"after_invalid_anchor_registration_copy_status\":%d,",
              after_invalid_anchor_registration_copy_status);
  std::printf("\"after_invalid_anchor_image_walk_copy_status\":%d,",
              after_invalid_anchor_image_walk_copy_status);
  std::printf("\"after_invalid_anchor_registered_image_count\":%llu,",
              static_cast<unsigned long long>(
                  after_invalid_anchor_registration.registered_image_count));
  std::printf("\"after_invalid_anchor_next_expected_registration_order_ordinal\":%llu,",
              static_cast<unsigned long long>(
                  after_invalid_anchor_registration
                      .next_expected_registration_order_ordinal));
  std::printf("\"after_invalid_anchor_last_registration_status\":%d,",
              after_invalid_anchor_registration.last_registration_status);
  std::printf("\"after_invalid_anchor_walked_image_count\":%llu,",
              static_cast<unsigned long long>(
                  after_invalid_anchor_walk.walked_image_count));
  std::printf("\"after_invalid_anchor_last_linker_anchor_matches_discovery_root\":%d,",
              after_invalid_anchor_walk.last_linker_anchor_matches_discovery_root);
  std::printf("\"post_reset_invalid_discovery_root_status\":%d,",
              post_reset_invalid_discovery_root_status);
  std::printf("\"after_invalid_discovery_root_registration_copy_status\":%d,",
              after_invalid_discovery_root_registration_copy_status);
  std::printf("\"after_invalid_discovery_root_image_walk_copy_status\":%d,",
              after_invalid_discovery_root_image_walk_copy_status);
  std::printf("\"after_invalid_discovery_root_registered_image_count\":%llu,",
              static_cast<unsigned long long>(
                  after_invalid_discovery_root_registration
                      .registered_image_count));
  std::printf("\"after_invalid_discovery_root_next_expected_registration_order_ordinal\":%llu,",
              static_cast<unsigned long long>(
                  after_invalid_discovery_root_registration
                      .next_expected_registration_order_ordinal));
  std::printf("\"after_invalid_discovery_root_last_registration_status\":%d,",
              after_invalid_discovery_root_registration.last_registration_status);
  std::printf("\"after_invalid_discovery_root_walked_image_count\":%llu,",
              static_cast<unsigned long long>(
                  after_invalid_discovery_root_walk.walked_image_count));
  std::printf("\"after_invalid_discovery_root_last_linker_anchor_matches_discovery_root\":%d,",
              after_invalid_discovery_root_walk
                  .last_linker_anchor_matches_discovery_root);

  std::printf("\"replay_status\":%d,", replay_status);
  std::printf("\"post_replay_registration_copy_status\":%d,",
              post_replay_registration_copy_status);
  std::printf("\"post_replay_image_walk_copy_status\":%d,",
              post_replay_image_walk_copy_status);
  std::printf("\"post_replay_reset_replay_copy_status\":%d,",
              post_replay_reset_replay_copy_status);
  std::printf("\"post_replay_registered_image_count\":%llu,",
              static_cast<unsigned long long>(
                  post_replay_registration.registered_image_count));
  std::printf("\"post_replay_next_expected_registration_order_ordinal\":%llu,",
              static_cast<unsigned long long>(
                  post_replay_registration
                      .next_expected_registration_order_ordinal));
  std::printf("\"post_replay_walked_image_count\":%llu,",
              static_cast<unsigned long long>(post_replay_walk.walked_image_count));
  std::printf("\"post_replay_last_discovery_root_entry_count\":%llu,",
              static_cast<unsigned long long>(
                  post_replay_walk.last_discovery_root_entry_count));
  std::printf("\"post_replay_last_registration_used_staged_table\":%d,",
              post_replay_walk.last_registration_used_staged_table);
  std::printf("\"post_replay_retained_bootstrap_image_count\":%llu,",
              static_cast<unsigned long long>(
                  post_replay_reset_replay.retained_bootstrap_image_count));
  std::printf("\"post_replay_last_replayed_image_count\":%llu,",
              static_cast<unsigned long long>(
                  post_replay_reset_replay.last_replayed_image_count));
  std::printf("\"post_replay_replay_generation\":%llu,",
              static_cast<unsigned long long>(
                  post_replay_reset_replay.replay_generation));
  std::printf("\"post_replay_last_replay_status\":%d,",
              post_replay_reset_replay.last_replay_status);
  std::printf("\"post_replay_last_registered_module_name\":");
  PrintJsonStringOrNull(post_replay_last_registered_module_name.c_str());
  std::printf(",\"post_replay_last_walked_module_name\":");
  PrintJsonStringOrNull(post_replay_last_walked_module_name.c_str());
  std::printf(",\"post_replay_last_replayed_module_name\":");
  PrintJsonStringOrNull(post_replay_last_replayed_module_name.c_str());
  std::printf(",\"post_replay_last_registered_translation_unit_identity_key\":");
  PrintJsonStringOrNull(
      post_replay_last_registered_translation_unit_identity_key.c_str());
  std::printf(",\"post_replay_last_walked_translation_unit_identity_key\":");
  PrintJsonStringOrNull(
      post_replay_last_walked_translation_unit_identity_key.c_str());
  std::printf(",\"post_replay_last_replayed_translation_unit_identity_key\":");
  PrintJsonStringOrNull(
      post_replay_last_replayed_translation_unit_identity_key.c_str());
  std::printf("}\n");
  return 0;
}
