#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_MULTI_IMAGE_REGISTRATION_RESET_REPLAY_PROBE_PROBE_REPORT_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_MULTI_IMAGE_REGISTRATION_RESET_REPLAY_PROBE_PROBE_REPORT_H_

#include "../support/json_probe_writer.h"
#include "image_fixture_setup.h"
#include "probe_result.h"

#include <cstdio>
#include <string>

namespace objc3c::runtime::probe::multi_image_registration_reset_replay {

inline void PrintStartupFields(const RuntimeImageLifecycleState &startup) {
  using objc3c::runtime::probe::PrintIntField;
  using objc3c::runtime::probe::PrintStringField;
  using objc3c::runtime::probe::PrintUint64Field;

  PrintIntField("startup_registration_status",
                startup.registration.copy_status);
  PrintIntField("startup_walk_status", startup.walk.copy_status);
  PrintIntField("startup_graph_status", startup.graph.copy_status);
  PrintIntField("startup_provider_status", startup.provider_entry.copy_status);
  PrintIntField("startup_consumer_status", startup.consumer_entry.copy_status);
  PrintIntField("startup_replay_status", startup.replay.copy_status);
  PrintUint64Field("startup_registered_image_count",
                   startup.registration.snapshot.registered_image_count);
  PrintUint64Field(
      "startup_next_expected_registration_order_ordinal",
      startup.registration.snapshot.next_expected_registration_order_ordinal);
  PrintUint64Field("startup_walked_image_count",
                   startup.walk.snapshot.walked_image_count);
  PrintUint64Field("startup_realized_class_count",
                   startup.graph.snapshot.realized_class_count);
  PrintUint64Field("startup_class_graph_generation",
                   startup.graph.snapshot.class_graph_generation);
  PrintUint64Field("startup_category_attachment_generation",
                   startup.graph.snapshot.category_attachment_generation);
  PrintUint64Field("startup_protocol_declaration_generation",
                   startup.graph.snapshot.protocol_declaration_generation);
  PrintUint64Field("startup_storage_surface_generation",
                   startup.graph.snapshot.storage_surface_generation);
  PrintUint64Field("startup_method_surface_generation",
                   startup.graph.snapshot.method_surface_generation);
  PrintUint64Field("startup_retained_bootstrap_image_count",
                   startup.replay.snapshot.retained_bootstrap_image_count);
  PrintStringField("startup_last_walked_module_name",
                   NullableRuntimeString(startup.walk.last_walked_module_name));
  PrintUint64Field("startup_provider_registration_order_ordinal",
                   startup.provider_entry.snapshot.registration_order_ordinal);
  PrintUint64Field("startup_consumer_registration_order_ordinal",
                   startup.consumer_entry.snapshot.registration_order_ordinal);
  PrintStringField("startup_provider_identity",
                   NullableRuntimeString(
                       startup.provider_entry.translation_unit_identity_key));
  PrintStringField("startup_consumer_identity",
                   NullableRuntimeString(
                       startup.consumer_entry.translation_unit_identity_key));
}

inline void PrintResetFields(const char *prefix,
                             const PostResetLifecycleState &reset) {
  using objc3c::runtime::probe::PrintIntField;
  using objc3c::runtime::probe::PrintUint64Field;

  std::string registration_status_field(prefix);
  registration_status_field += "_reset_registration_status";
  std::string replay_status_field(prefix);
  replay_status_field += "_reset_replay_status";
  std::string image_count_field(prefix);
  image_count_field += "_reset_registered_image_count";
  std::string next_ordinal_field(prefix);
  next_ordinal_field += "_reset_next_expected_registration_order_ordinal";
  std::string retained_bootstrap_count_field(prefix);
  retained_bootstrap_count_field += "_reset_retained_bootstrap_image_count";
  std::string live_registration_order_count_field(prefix);
  live_registration_order_count_field +=
      "_reset_live_registration_order_entry_count";
  std::string live_metadata_count_field(prefix);
  live_metadata_count_field += "_reset_live_registered_metadata_entry_count";
  std::string live_selector_count_field(prefix);
  live_selector_count_field += "_reset_live_selector_table_entry_count";
  std::string live_keypath_count_field(prefix);
  live_keypath_count_field += "_reset_live_keypath_entry_count";
  std::string live_class_count_field(prefix);
  live_class_count_field += "_reset_live_realized_class_count";
  std::string live_method_cache_count_field(prefix);
  live_method_cache_count_field += "_reset_live_method_cache_entry_count";
  std::string live_property_cache_count_field(prefix);
  live_property_cache_count_field +=
      "_reset_live_property_lookup_cache_entry_count";
  std::string cleared_init_state_count_field(prefix);
  cleared_init_state_count_field +=
      "_reset_cleared_image_local_init_state_count";
  std::string generation_field(prefix);
  generation_field += "_reset_generation";

  PrintIntField(registration_status_field.c_str(),
                reset.registration.copy_status);
  PrintIntField(replay_status_field.c_str(), reset.replay.copy_status);
  PrintUint64Field(image_count_field.c_str(),
                   reset.registration.snapshot.registered_image_count);
  PrintUint64Field(
      next_ordinal_field.c_str(),
      reset.registration.snapshot.next_expected_registration_order_ordinal);
  PrintUint64Field(retained_bootstrap_count_field.c_str(),
                   reset.replay.snapshot.retained_bootstrap_image_count);
  PrintUint64Field(live_registration_order_count_field.c_str(),
                   reset.replay.snapshot.live_registration_order_entry_count);
  PrintUint64Field(live_metadata_count_field.c_str(),
                   reset.replay.snapshot.live_registered_metadata_entry_count);
  PrintUint64Field(live_selector_count_field.c_str(),
                   reset.replay.snapshot.live_selector_table_entry_count);
  PrintUint64Field(live_keypath_count_field.c_str(),
                   reset.replay.snapshot.live_keypath_entry_count);
  PrintUint64Field(live_class_count_field.c_str(),
                   reset.replay.snapshot.live_realized_class_count);
  PrintUint64Field(live_method_cache_count_field.c_str(),
                   reset.replay.snapshot.live_method_cache_entry_count);
  PrintUint64Field(
      live_property_cache_count_field.c_str(),
      reset.replay.snapshot.live_property_lookup_cache_entry_count);
  PrintUint64Field(
      cleared_init_state_count_field.c_str(),
      reset.replay.snapshot.last_reset_cleared_image_local_init_state_count);
  PrintUint64Field(generation_field.c_str(),
                   reset.replay.snapshot.reset_generation);
}

inline void PrintReplayFields(const char *prefix,
                              const ResetReplayCycleResult &cycle,
                              bool print_consumer_identity = true) {
  using objc3c::runtime::probe::PrintIntField;
  using objc3c::runtime::probe::PrintStringField;
  using objc3c::runtime::probe::PrintUint64Field;

  const RuntimeImageLifecycleState &replayed = cycle.replayed;
  std::string replay_status_field(prefix);
  replay_status_field += "_replay_status";
  std::string registration_status_field(prefix);
  registration_status_field += "_replay_registration_status";
  std::string walk_status_field(prefix);
  walk_status_field += "_replay_walk_status";
  std::string graph_status_field(prefix);
  graph_status_field += "_replay_graph_status";
  std::string provider_status_field(prefix);
  provider_status_field += "_replay_provider_status";
  std::string consumer_status_field(prefix);
  consumer_status_field += "_replay_consumer_status";
  std::string state_status_field(prefix);
  state_status_field += "_replay_state_status";
  std::string registered_image_count_field(prefix);
  registered_image_count_field += "_replay_registered_image_count";
  std::string next_ordinal_field(prefix);
  next_ordinal_field += "_replay_next_expected_registration_order_ordinal";
  std::string walked_image_count_field(prefix);
  walked_image_count_field += "_replay_walked_image_count";
  std::string realized_class_count_field(prefix);
  realized_class_count_field += "_replay_realized_class_count";
  std::string class_graph_generation_field(prefix);
  class_graph_generation_field += "_replay_class_graph_generation";
  std::string category_generation_field(prefix);
  category_generation_field += "_replay_category_attachment_generation";
  std::string protocol_generation_field(prefix);
  protocol_generation_field += "_replay_protocol_declaration_generation";
  std::string storage_generation_field(prefix);
  storage_generation_field += "_replay_storage_surface_generation";
  std::string method_generation_field(prefix);
  method_generation_field += "_replay_method_surface_generation";
  std::string last_replayed_image_count_field(prefix);
  last_replayed_image_count_field += "_replay_last_replayed_image_count";
  std::string generation_field(prefix);
  generation_field += "_replay_generation";
  std::string last_walked_module_field(prefix);
  last_walked_module_field += "_replay_last_walked_module_name";
  std::string last_replayed_module_field(prefix);
  last_replayed_module_field += "_replay_last_replayed_module_name";
  std::string provider_ordinal_field(prefix);
  provider_ordinal_field += "_replay_provider_registration_order_ordinal";
  std::string consumer_ordinal_field(prefix);
  consumer_ordinal_field += "_replay_consumer_registration_order_ordinal";
  std::string provider_identity_field(prefix);
  provider_identity_field += "_replay_provider_identity";
  std::string consumer_identity_field(prefix);
  consumer_identity_field += "_replay_consumer_identity";

  PrintIntField(replay_status_field.c_str(), cycle.replay_status);
  PrintIntField(registration_status_field.c_str(),
                replayed.registration.copy_status);
  PrintIntField(walk_status_field.c_str(), replayed.walk.copy_status);
  PrintIntField(graph_status_field.c_str(), replayed.graph.copy_status);
  PrintIntField(provider_status_field.c_str(),
                replayed.provider_entry.copy_status);
  PrintIntField(consumer_status_field.c_str(),
                replayed.consumer_entry.copy_status);
  PrintIntField(state_status_field.c_str(), replayed.replay.copy_status);
  PrintUint64Field(registered_image_count_field.c_str(),
                   replayed.registration.snapshot.registered_image_count);
  PrintUint64Field(
      next_ordinal_field.c_str(),
      replayed.registration.snapshot.next_expected_registration_order_ordinal);
  PrintUint64Field(walked_image_count_field.c_str(),
                   replayed.walk.snapshot.walked_image_count);
  PrintUint64Field(realized_class_count_field.c_str(),
                   replayed.graph.snapshot.realized_class_count);
  PrintUint64Field(class_graph_generation_field.c_str(),
                   replayed.graph.snapshot.class_graph_generation);
  PrintUint64Field(category_generation_field.c_str(),
                   replayed.graph.snapshot.category_attachment_generation);
  PrintUint64Field(protocol_generation_field.c_str(),
                   replayed.graph.snapshot.protocol_declaration_generation);
  PrintUint64Field(storage_generation_field.c_str(),
                   replayed.graph.snapshot.storage_surface_generation);
  PrintUint64Field(method_generation_field.c_str(),
                   replayed.graph.snapshot.method_surface_generation);
  PrintUint64Field(last_replayed_image_count_field.c_str(),
                   replayed.replay.snapshot.last_replayed_image_count);
  PrintUint64Field(generation_field.c_str(),
                   replayed.replay.snapshot.replay_generation);
  PrintStringField(
      last_walked_module_field.c_str(),
      NullableRuntimeString(replayed.walk.last_walked_module_name));
  PrintStringField(
      last_replayed_module_field.c_str(),
      NullableRuntimeString(replayed.replay.last_replayed_module_name));
  PrintUint64Field(provider_ordinal_field.c_str(),
                   replayed.provider_entry.snapshot.registration_order_ordinal);
  PrintUint64Field(consumer_ordinal_field.c_str(),
                   replayed.consumer_entry.snapshot.registration_order_ordinal);
  PrintStringField(provider_identity_field.c_str(),
                   NullableRuntimeString(
                       replayed.provider_entry.translation_unit_identity_key));
  if (print_consumer_identity) {
    PrintStringField(
        consumer_identity_field.c_str(),
        NullableRuntimeString(
            replayed.consumer_entry.translation_unit_identity_key));
  }
}

inline void
PrintBlockedReplayFields(const BlockedReplayResult &blocked_replay) {
  using objc3c::runtime::probe::PrintIntField;
  using objc3c::runtime::probe::PrintUint64Field;

  PrintIntField("replay_without_reset_status", blocked_replay.replay_status);
  PrintIntField("blocked_replay_state_status",
                blocked_replay.replay_state.copy_status);
  PrintIntField("blocked_replay_last_replay_status",
                blocked_replay.replay_state.snapshot.last_replay_status);
  PrintUint64Field(
      "blocked_replay_last_replayed_image_count",
      blocked_replay.replay_state.snapshot.last_replayed_image_count);
  PrintUint64Field(
      "blocked_replay_live_registration_order_entry_count",
      blocked_replay.replay_state.snapshot.live_registration_order_entry_count);
  PrintUint64Field("blocked_replay_live_registered_metadata_entry_count",
                   blocked_replay.replay_state.snapshot
                       .live_registered_metadata_entry_count);
  PrintUint64Field(
      "blocked_replay_live_selector_table_entry_count",
      blocked_replay.replay_state.snapshot.live_selector_table_entry_count);
  PrintUint64Field(
      "blocked_replay_live_keypath_entry_count",
      blocked_replay.replay_state.snapshot.live_keypath_entry_count);
  PrintUint64Field(
      "blocked_replay_live_realized_class_count",
      blocked_replay.replay_state.snapshot.live_realized_class_count);
  PrintUint64Field(
      "blocked_replay_live_method_cache_entry_count",
      blocked_replay.replay_state.snapshot.live_method_cache_entry_count);
  PrintUint64Field("blocked_replay_live_property_lookup_cache_entry_count",
                   blocked_replay.replay_state.snapshot
                       .live_property_lookup_cache_entry_count);
}

inline void PrintSecondReplayConsumerIdentityFinalField(
    const ResetReplayCycleResult &second_cycle) {
  using objc3c::runtime::probe::PrintStringField;

  PrintStringField(
      "second_replay_consumer_identity",
      NullableRuntimeString(
          second_cycle.replayed.consumer_entry.translation_unit_identity_key),
      false);
}

inline void PrintProbeReport(const ProbeResult &result) {
  std::printf("{");
  PrintStartupFields(result.startup);
  PrintResetFields("first", result.first_cycle.reset);
  PrintReplayFields("first", result.first_cycle);
  PrintBlockedReplayFields(result.blocked_replay);
  PrintResetFields("second", result.second_cycle.reset);
  PrintReplayFields("second", result.second_cycle, false);
  PrintSecondReplayConsumerIdentityFinalField(result.second_cycle);
  std::printf("}\n");
}

} // namespace objc3c::runtime::probe::multi_image_registration_reset_replay

#endif // OBJC3C_TESTS_TOOLING_RUNTIME_MULTI_IMAGE_REGISTRATION_RESET_REPLAY_PROBE_PROBE_REPORT_H_
