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

void PrintUint64Field(const char *name, unsigned long long value,
                      bool trailing_comma = true) {
  std::printf("\"%s\":%llu%s", name, value, trailing_comma ? "," : "");
}

void PrintIntField(const char *name, int value, bool trailing_comma = true) {
  std::printf("\"%s\":%d%s", name, value, trailing_comma ? "," : "");
}

void PrintStringField(const char *name, const char *value,
                      bool trailing_comma = true) {
  std::printf("\"%s\":", name);
  PrintJsonStringOrNull(value);
  if (trailing_comma) {
    std::printf(",");
  }
}

}  // namespace

int main() {
  objc3_runtime_registration_state_snapshot startup_registration{};
  objc3_runtime_image_walk_state_snapshot startup_image_walk{};
  objc3_runtime_realized_class_graph_state_snapshot startup_graph{};
  objc3_runtime_realized_class_entry_snapshot imported_entry{};
  objc3_runtime_realized_class_entry_snapshot local_entry{};
  objc3_runtime_selector_lookup_table_state_snapshot selector_table{};
  objc3_runtime_selector_lookup_entry_snapshot provider_selector{};
  objc3_runtime_selector_lookup_entry_snapshot imported_protocol_selector{};
  objc3_runtime_selector_lookup_entry_snapshot local_selector{};
  objc3_runtime_method_cache_state_snapshot method_cache_state{};
  objc3_runtime_method_cache_entry_snapshot provider_method{};
  objc3_runtime_method_cache_entry_snapshot imported_protocol_method{};
  objc3_runtime_method_cache_entry_snapshot local_method{};
  objc3_runtime_protocol_conformance_query_snapshot protocol_query{};

  const int startup_registration_copy_status =
      objc3_runtime_copy_registration_state_for_testing(&startup_registration);
  const int startup_image_walk_status =
      objc3_runtime_copy_image_walk_state_for_testing(&startup_image_walk);
  const int startup_graph_status =
      objc3_runtime_copy_realized_class_graph_state_for_testing(&startup_graph);
  const int imported_entry_status =
      objc3_runtime_copy_realized_class_entry_for_testing("ImportedProvider",
                                                          &imported_entry);
  const int local_entry_status =
      objc3_runtime_copy_realized_class_entry_for_testing("LocalConsumer",
                                                          &local_entry);

  const int imported_class_receiver =
      imported_entry.found != 0
          ? static_cast<int>(imported_entry.base_identity + 2U)
          : 0;
  const int local_class_receiver =
      local_entry.found != 0 ? static_cast<int>(local_entry.base_identity + 2U)
                             : 0;

  const int imported_provider_class_value =
      objc3_runtime_dispatch_i32(imported_class_receiver, "providerClassValue",
                                 0, 0, 0, 0);
  const int imported_provider_protocol_value =
      objc3_runtime_dispatch_i32(imported_class_receiver,
                                 "importedProtocolValue", 0, 0, 0, 0);
  const int local_consumer_class_value =
      objc3_runtime_dispatch_i32(local_class_receiver, "localClassValue", 0, 0,
                                 0, 0);

  const int selector_table_status =
      objc3_runtime_copy_selector_lookup_table_state_for_testing(&selector_table);
  const int provider_selector_status =
      objc3_runtime_copy_selector_lookup_entry_for_testing("providerClassValue",
                                                           &provider_selector);
  const int imported_protocol_selector_status =
      objc3_runtime_copy_selector_lookup_entry_for_testing(
          "importedProtocolValue", &imported_protocol_selector);
  const int local_selector_status =
      objc3_runtime_copy_selector_lookup_entry_for_testing("localClassValue",
                                                           &local_selector);

  const int method_cache_state_status =
      objc3_runtime_copy_method_cache_state_for_testing(&method_cache_state);
  const int provider_method_status =
      objc3_runtime_copy_method_cache_entry_for_testing(
          imported_class_receiver, "providerClassValue", &provider_method);
  const int imported_protocol_method_status =
      objc3_runtime_copy_method_cache_entry_for_testing(
          imported_class_receiver, "importedProtocolValue",
          &imported_protocol_method);
  const int local_method_status =
      objc3_runtime_copy_method_cache_entry_for_testing(
          local_class_receiver, "localClassValue", &local_method);

  const int protocol_query_status =
      objc3_runtime_copy_protocol_conformance_query_for_testing(
          "ImportedProvider", "ImportedWorker", &protocol_query);

  const std::string startup_imported_module_name =
      imported_entry.module_name != nullptr ? imported_entry.module_name : "";
  const std::string startup_imported_translation_unit_identity_key =
      imported_entry.translation_unit_identity_key != nullptr
          ? imported_entry.translation_unit_identity_key
          : "";
  const std::string startup_imported_class_owner_identity =
      imported_entry.class_owner_identity != nullptr
          ? imported_entry.class_owner_identity
          : "";
  const std::string startup_local_module_name =
      local_entry.module_name != nullptr ? local_entry.module_name : "";
  const std::string startup_local_translation_unit_identity_key =
      local_entry.translation_unit_identity_key != nullptr
          ? local_entry.translation_unit_identity_key
          : "";
  const std::string startup_local_class_owner_identity =
      local_entry.class_owner_identity != nullptr ? local_entry.class_owner_identity
                                                  : "";

  objc3_runtime_reset_for_testing();
  objc3_runtime_registration_state_snapshot post_reset_registration{};
  objc3_runtime_reset_replay_state_snapshot post_reset_replay{};
  const int post_reset_registration_copy_status =
      objc3_runtime_copy_registration_state_for_testing(&post_reset_registration);
  const int post_reset_replay_copy_status =
      objc3_runtime_copy_reset_replay_state_for_testing(&post_reset_replay);

  const int replay_status = objc3_runtime_replay_registered_images_for_testing();

  objc3_runtime_registration_state_snapshot post_replay_registration{};
  objc3_runtime_image_walk_state_snapshot post_replay_image_walk{};
  objc3_runtime_realized_class_graph_state_snapshot post_replay_graph{};
  objc3_runtime_reset_replay_state_snapshot post_replay_replay{};
  objc3_runtime_realized_class_entry_snapshot post_replay_imported_entry{};
  objc3_runtime_realized_class_entry_snapshot post_replay_local_entry{};
  const int post_replay_registration_copy_status =
      objc3_runtime_copy_registration_state_for_testing(&post_replay_registration);
  const int post_replay_image_walk_status =
      objc3_runtime_copy_image_walk_state_for_testing(&post_replay_image_walk);
  const int post_replay_graph_status =
      objc3_runtime_copy_realized_class_graph_state_for_testing(&post_replay_graph);
  const int post_replay_replay_copy_status =
      objc3_runtime_copy_reset_replay_state_for_testing(&post_replay_replay);
  const int post_replay_imported_entry_status =
      objc3_runtime_copy_realized_class_entry_for_testing(
          "ImportedProvider", &post_replay_imported_entry);
  const int post_replay_local_entry_status =
      objc3_runtime_copy_realized_class_entry_for_testing("LocalConsumer",
                                                          &post_replay_local_entry);

  const int post_replay_imported_class_receiver =
      post_replay_imported_entry.found != 0
          ? static_cast<int>(post_replay_imported_entry.base_identity + 2U)
          : 0;
  const int post_replay_local_class_receiver =
      post_replay_local_entry.found != 0
          ? static_cast<int>(post_replay_local_entry.base_identity + 2U)
          : 0;

  const int post_replay_imported_provider_class_value =
      objc3_runtime_dispatch_i32(post_replay_imported_class_receiver,
                                 "providerClassValue", 0, 0, 0, 0);
  const int post_replay_imported_provider_protocol_value =
      objc3_runtime_dispatch_i32(post_replay_imported_class_receiver,
                                 "importedProtocolValue", 0, 0, 0, 0);
  const int post_replay_local_consumer_class_value =
      objc3_runtime_dispatch_i32(post_replay_local_class_receiver,
                                 "localClassValue", 0, 0, 0, 0);

  objc3_runtime_selector_lookup_table_state_snapshot post_replay_selector_table{};
  objc3_runtime_selector_lookup_entry_snapshot post_replay_provider_selector{};
  objc3_runtime_selector_lookup_entry_snapshot post_replay_imported_protocol_selector{};
  objc3_runtime_selector_lookup_entry_snapshot post_replay_local_selector{};
  objc3_runtime_method_cache_state_snapshot post_replay_method_cache_state{};
  objc3_runtime_method_cache_entry_snapshot post_replay_provider_method{};
  objc3_runtime_method_cache_entry_snapshot post_replay_imported_protocol_method{};
  objc3_runtime_method_cache_entry_snapshot post_replay_local_method{};

  const int post_replay_selector_table_status =
      objc3_runtime_copy_selector_lookup_table_state_for_testing(
          &post_replay_selector_table);
  const int post_replay_provider_selector_status =
      objc3_runtime_copy_selector_lookup_entry_for_testing(
          "providerClassValue", &post_replay_provider_selector);
  const int post_replay_imported_protocol_selector_status =
      objc3_runtime_copy_selector_lookup_entry_for_testing(
          "importedProtocolValue", &post_replay_imported_protocol_selector);
  const int post_replay_local_selector_status =
      objc3_runtime_copy_selector_lookup_entry_for_testing(
          "localClassValue", &post_replay_local_selector);
  const int post_replay_method_cache_state_status =
      objc3_runtime_copy_method_cache_state_for_testing(
          &post_replay_method_cache_state);
  const int post_replay_provider_method_status =
      objc3_runtime_copy_method_cache_entry_for_testing(
          post_replay_imported_class_receiver, "providerClassValue",
          &post_replay_provider_method);
  const int post_replay_imported_protocol_method_status =
      objc3_runtime_copy_method_cache_entry_for_testing(
          post_replay_imported_class_receiver, "importedProtocolValue",
          &post_replay_imported_protocol_method);
  const int post_replay_local_method_status =
      objc3_runtime_copy_method_cache_entry_for_testing(
          post_replay_local_class_receiver, "localClassValue",
          &post_replay_local_method);

  std::printf("{");
  PrintIntField("startup_registration_copy_status",
                startup_registration_copy_status);
  PrintUint64Field("startup_registered_image_count",
                   startup_registration.registered_image_count);
  PrintUint64Field("startup_next_expected_registration_order_ordinal",
                   startup_registration.next_expected_registration_order_ordinal);
  PrintIntField("startup_image_walk_status", startup_image_walk_status);
  PrintUint64Field("startup_walked_image_count",
                   startup_image_walk.walked_image_count);
  PrintStringField("startup_last_walked_module_name",
                   startup_image_walk.last_walked_module_name);
  PrintIntField("startup_graph_status", startup_graph_status);
  PrintUint64Field("startup_realized_class_count",
                   startup_graph.realized_class_count);
  PrintUint64Field("startup_root_class_count", startup_graph.root_class_count);
  PrintUint64Field("startup_metaclass_edge_count",
                   startup_graph.metaclass_edge_count);
  PrintIntField("imported_entry_status", imported_entry_status);
  PrintIntField("imported_entry_found", imported_entry.found);
  PrintUint64Field("imported_registration_order_ordinal",
                   imported_entry.registration_order_ordinal);
  PrintUint64Field("imported_direct_protocol_count",
                   imported_entry.direct_protocol_count);
  PrintUint64Field("imported_attached_protocol_count",
                   imported_entry.attached_protocol_count);
  PrintUint64Field("imported_runtime_property_accessor_count",
                   imported_entry.runtime_property_accessor_count);
  PrintStringField("imported_module_name", startup_imported_module_name.c_str());
  PrintStringField("imported_translation_unit_identity_key",
                   startup_imported_translation_unit_identity_key.c_str());
  PrintStringField("imported_class_owner_identity",
                   startup_imported_class_owner_identity.c_str());
  PrintIntField("local_entry_status", local_entry_status);
  PrintIntField("local_entry_found", local_entry.found);
  PrintUint64Field("local_registration_order_ordinal",
                   local_entry.registration_order_ordinal);
  PrintUint64Field("local_direct_protocol_count",
                   local_entry.direct_protocol_count);
  PrintUint64Field("local_attached_protocol_count",
                   local_entry.attached_protocol_count);
  PrintUint64Field("local_runtime_property_accessor_count",
                   local_entry.runtime_property_accessor_count);
  PrintStringField("local_module_name", startup_local_module_name.c_str());
  PrintStringField("local_translation_unit_identity_key",
                   startup_local_translation_unit_identity_key.c_str());
  PrintStringField("local_class_owner_identity",
                   startup_local_class_owner_identity.c_str());
  PrintIntField("imported_provider_class_value",
                imported_provider_class_value);
  PrintIntField("imported_provider_protocol_value",
                imported_provider_protocol_value);
  PrintIntField("local_consumer_class_value", local_consumer_class_value);
  PrintIntField("selector_table_status", selector_table_status);
  PrintUint64Field("selector_table_entry_count",
                   selector_table.selector_table_entry_count);
  PrintUint64Field("selector_metadata_backed_selector_count",
                   selector_table.metadata_backed_selector_count);
  PrintUint64Field("selector_dynamic_selector_count",
                   selector_table.dynamic_selector_count);
  PrintIntField("provider_selector_status", provider_selector_status);
  PrintIntField("provider_selector_found", provider_selector.found);
  PrintIntField("provider_selector_metadata_backed",
                provider_selector.metadata_backed);
  PrintUint64Field("provider_selector_provider_count",
                   provider_selector.metadata_provider_count);
  PrintUint64Field("provider_selector_first_ordinal",
                   provider_selector.first_registration_order_ordinal);
  PrintUint64Field("provider_selector_last_ordinal",
                   provider_selector.last_registration_order_ordinal);
  PrintIntField("imported_protocol_selector_status",
                imported_protocol_selector_status);
  PrintIntField("imported_protocol_selector_found",
                imported_protocol_selector.found);
  PrintIntField("imported_protocol_selector_metadata_backed",
                imported_protocol_selector.metadata_backed);
  PrintUint64Field("imported_protocol_selector_provider_count",
                   imported_protocol_selector.metadata_provider_count);
  PrintUint64Field("imported_protocol_selector_first_ordinal",
                   imported_protocol_selector.first_registration_order_ordinal);
  PrintUint64Field("imported_protocol_selector_last_ordinal",
                   imported_protocol_selector.last_registration_order_ordinal);
  PrintIntField("local_selector_status", local_selector_status);
  PrintIntField("local_selector_found", local_selector.found);
  PrintIntField("local_selector_metadata_backed",
                local_selector.metadata_backed);
  PrintUint64Field("local_selector_provider_count",
                   local_selector.metadata_provider_count);
  PrintUint64Field("local_selector_first_ordinal",
                   local_selector.first_registration_order_ordinal);
  PrintUint64Field("local_selector_last_ordinal",
                   local_selector.last_registration_order_ordinal);
  PrintIntField("method_cache_state_status", method_cache_state_status);
  PrintUint64Field("method_cache_entry_count",
                   method_cache_state.cache_entry_count);
  PrintUint64Field("method_cache_live_dispatch_count",
                   method_cache_state.live_dispatch_count);
  PrintUint64Field("method_cache_fallback_dispatch_count",
                   method_cache_state.fallback_dispatch_count);
  PrintStringField("method_cache_last_selector",
                   method_cache_state.last_selector);
  PrintStringField("method_cache_last_resolved_class_name",
                   method_cache_state.last_resolved_class_name);
  PrintStringField("method_cache_last_resolved_owner_identity",
                   method_cache_state.last_resolved_owner_identity);
  PrintIntField("provider_method_status", provider_method_status);
  PrintIntField("provider_method_found", provider_method.found);
  PrintIntField("provider_method_resolved", provider_method.resolved);
  PrintStringField("provider_method_owner_identity",
                   provider_method.resolved_owner_identity);
  PrintIntField("imported_protocol_method_status",
                imported_protocol_method_status);
  PrintIntField("imported_protocol_method_found",
                imported_protocol_method.found);
  PrintIntField("imported_protocol_method_resolved",
                imported_protocol_method.resolved);
  PrintStringField("imported_protocol_method_owner_identity",
                   imported_protocol_method.resolved_owner_identity);
  PrintIntField("local_method_status", local_method_status);
  PrintIntField("local_method_found", local_method.found);
  PrintIntField("local_method_resolved", local_method.resolved);
  PrintStringField("local_method_owner_identity",
                   local_method.resolved_owner_identity);
  PrintIntField("protocol_query_status", protocol_query_status);
  PrintIntField("protocol_query_class_found", protocol_query.class_found);
  PrintIntField("protocol_query_protocol_found", protocol_query.protocol_found);
  PrintIntField("protocol_query_conforms", protocol_query.conforms);
  PrintUint64Field("protocol_query_visited_protocol_count",
                   protocol_query.visited_protocol_count);
  PrintUint64Field("protocol_query_attached_category_count",
                   protocol_query.attached_category_count);
  PrintStringField("protocol_query_matched_protocol_owner_identity",
                   protocol_query.matched_protocol_owner_identity);
  PrintIntField("post_reset_registration_copy_status",
                post_reset_registration_copy_status);
  PrintUint64Field("post_reset_registered_image_count",
                   post_reset_registration.registered_image_count);
  PrintIntField("post_reset_replay_copy_status",
                post_reset_replay_copy_status);
  PrintUint64Field("post_reset_retained_bootstrap_image_count",
                   post_reset_replay.retained_bootstrap_image_count);
  PrintUint64Field("post_reset_generation", post_reset_replay.reset_generation);
  PrintIntField("replay_status", replay_status);
  PrintIntField("post_replay_registration_copy_status",
                post_replay_registration_copy_status);
  PrintUint64Field("post_replay_registered_image_count",
                   post_replay_registration.registered_image_count);
  PrintUint64Field("post_replay_next_expected_registration_order_ordinal",
                   post_replay_registration
                       .next_expected_registration_order_ordinal);
  PrintIntField("post_replay_image_walk_status",
                post_replay_image_walk_status);
  PrintUint64Field("post_replay_walked_image_count",
                   post_replay_image_walk.walked_image_count);
  PrintStringField("post_replay_last_walked_module_name",
                   post_replay_image_walk.last_walked_module_name);
  PrintIntField("post_replay_graph_status", post_replay_graph_status);
  PrintUint64Field("post_replay_realized_class_count",
                   post_replay_graph.realized_class_count);
  PrintIntField("post_replay_replay_copy_status",
                post_replay_replay_copy_status);
  PrintUint64Field("post_replay_replay_generation",
                   post_replay_replay.replay_generation);
  PrintUint64Field("post_replay_retained_bootstrap_image_count",
                   post_replay_replay.retained_bootstrap_image_count);
  PrintIntField("post_replay_imported_entry_status",
                post_replay_imported_entry_status);
  PrintIntField("post_replay_imported_entry_found",
                post_replay_imported_entry.found);
  PrintStringField("post_replay_imported_module_name",
                   post_replay_imported_entry.module_name);
  PrintStringField("post_replay_imported_translation_unit_identity_key",
                   post_replay_imported_entry.translation_unit_identity_key);
  PrintIntField("post_replay_local_entry_status",
                post_replay_local_entry_status);
  PrintIntField("post_replay_local_entry_found",
                post_replay_local_entry.found);
  PrintStringField("post_replay_local_module_name",
                   post_replay_local_entry.module_name);
  PrintStringField("post_replay_local_translation_unit_identity_key",
                   post_replay_local_entry.translation_unit_identity_key);
  PrintIntField("post_replay_imported_provider_class_value",
                post_replay_imported_provider_class_value);
  PrintIntField("post_replay_imported_provider_protocol_value",
                post_replay_imported_provider_protocol_value);
  PrintIntField("post_replay_local_consumer_class_value",
                post_replay_local_consumer_class_value);
  PrintIntField("post_replay_selector_table_status",
                post_replay_selector_table_status);
  PrintUint64Field("post_replay_selector_table_entry_count",
                   post_replay_selector_table.selector_table_entry_count);
  PrintUint64Field("post_replay_selector_metadata_backed_selector_count",
                   post_replay_selector_table.metadata_backed_selector_count);
  PrintIntField("post_replay_provider_selector_status",
                post_replay_provider_selector_status);
  PrintIntField("post_replay_provider_selector_found",
                post_replay_provider_selector.found);
  PrintIntField("post_replay_imported_protocol_selector_status",
                post_replay_imported_protocol_selector_status);
  PrintIntField("post_replay_imported_protocol_selector_found",
                post_replay_imported_protocol_selector.found);
  PrintIntField("post_replay_local_selector_status",
                post_replay_local_selector_status);
  PrintIntField("post_replay_local_selector_found",
                post_replay_local_selector.found);
  PrintIntField("post_replay_method_cache_state_status",
                post_replay_method_cache_state_status);
  PrintUint64Field("post_replay_method_cache_entry_count",
                   post_replay_method_cache_state.cache_entry_count);
  PrintUint64Field("post_replay_method_cache_live_dispatch_count",
                   post_replay_method_cache_state.live_dispatch_count);
  PrintUint64Field("post_replay_method_cache_fallback_dispatch_count",
                   post_replay_method_cache_state.fallback_dispatch_count);
  PrintStringField("post_replay_method_cache_last_selector",
                   post_replay_method_cache_state.last_selector);
  PrintStringField("post_replay_method_cache_last_resolved_class_name",
                   post_replay_method_cache_state.last_resolved_class_name);
  PrintStringField("post_replay_method_cache_last_resolved_owner_identity",
                   post_replay_method_cache_state.last_resolved_owner_identity);
  PrintIntField("post_replay_provider_method_status",
                post_replay_provider_method_status);
  PrintIntField("post_replay_provider_method_found",
                post_replay_provider_method.found);
  PrintIntField("post_replay_provider_method_resolved",
                post_replay_provider_method.resolved);
  PrintStringField("post_replay_provider_method_owner_identity",
                   post_replay_provider_method.resolved_owner_identity);
  PrintIntField("post_replay_imported_protocol_method_status",
                post_replay_imported_protocol_method_status);
  PrintIntField("post_replay_imported_protocol_method_found",
                post_replay_imported_protocol_method.found);
  PrintIntField("post_replay_imported_protocol_method_resolved",
                post_replay_imported_protocol_method.resolved);
  PrintStringField("post_replay_imported_protocol_method_owner_identity",
                   post_replay_imported_protocol_method.resolved_owner_identity);
  PrintIntField("post_replay_local_method_status",
                post_replay_local_method_status);
  PrintIntField("post_replay_local_method_found",
                post_replay_local_method.found);
  PrintIntField("post_replay_local_method_resolved",
                post_replay_local_method.resolved);
  PrintStringField("post_replay_local_method_owner_identity",
                   post_replay_local_method.resolved_owner_identity,
                   false);
  std::printf("}\n");
  return 0;
}
