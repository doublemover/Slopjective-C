#pragma once

#include "bootstrap_record_definitions.h"
#include "../support/json_probe_writer.h"

#include <cstdint>
#include <cstdio>

namespace objc3c::runtime::probe::runtime_bootstrap_table_consumption {

inline unsigned long long JsonU64(std::uint64_t value) {
  return static_cast<unsigned long long>(value);
}

inline void PrintStartupReportFields(const StartupTableState &startup) {
  using objc3c::runtime::probe::PrintIntField;
  using objc3c::runtime::probe::PrintStringField;
  using objc3c::runtime::probe::PrintUint64Field;

  const objc3_runtime_registration_state_snapshot &registration =
      startup.registration.snapshot;
  const objc3_runtime_image_walk_state_snapshot &walk = startup.walk.snapshot;

  PrintIntField("startup_registration_copy_status",
                startup.registration.copy_status);
  PrintIntField("startup_image_walk_copy_status", startup.walk.copy_status);
  PrintUint64Field("startup_registered_image_count",
                   JsonU64(registration.registered_image_count));
  PrintUint64Field("startup_registered_descriptor_total",
                   JsonU64(registration.registered_descriptor_total));
  PrintUint64Field(
      "startup_next_expected_registration_order_ordinal",
      JsonU64(registration.next_expected_registration_order_ordinal));
  PrintUint64Field(
      "startup_last_successful_registration_order_ordinal",
      JsonU64(registration.last_successful_registration_order_ordinal));
  PrintIntField("startup_last_registration_status",
                registration.last_registration_status);
  PrintStringField(
      "startup_last_registered_module_name",
      startup.registration.registered_module_name.c_str_or_null());
  PrintStringField(
      "startup_last_registered_translation_unit_identity_key",
      startup.registration.registered_translation_unit_identity_key
          .c_str_or_null());
  PrintUint64Field("startup_walked_image_count",
                   JsonU64(walk.walked_image_count));
  PrintUint64Field("startup_last_discovery_root_entry_count",
                   JsonU64(walk.last_discovery_root_entry_count));
  PrintUint64Field("startup_last_walked_class_descriptor_count",
                   JsonU64(walk.last_walked_class_descriptor_count));
  PrintUint64Field("startup_last_walked_protocol_descriptor_count",
                   JsonU64(walk.last_walked_protocol_descriptor_count));
  PrintUint64Field("startup_last_walked_category_descriptor_count",
                   JsonU64(walk.last_walked_category_descriptor_count));
  PrintUint64Field("startup_last_walked_property_descriptor_count",
                   JsonU64(walk.last_walked_property_descriptor_count));
  PrintUint64Field("startup_last_walked_ivar_descriptor_count",
                   JsonU64(walk.last_walked_ivar_descriptor_count));
  PrintIntField("startup_last_linker_anchor_matches_discovery_root",
                walk.last_linker_anchor_matches_discovery_root);
  PrintIntField("startup_last_registration_used_staged_table",
                walk.last_registration_used_staged_table);
}

inline void PrintDuplicateReportFields(
    const DuplicateConsumptionAttempt &duplicate) {
  using objc3c::runtime::probe::PrintIntField;
  using objc3c::runtime::probe::PrintStringField;
  using objc3c::runtime::probe::PrintUint64Field;

  const objc3_runtime_registration_state_snapshot &registration =
      duplicate.registration.snapshot;
  const objc3_runtime_image_walk_state_snapshot &walk = duplicate.walk.snapshot;

  PrintIntField("duplicate_registration_status",
                duplicate.registration_status);
  PrintIntField("after_duplicate_registration_copy_status",
                duplicate.registration.copy_status);
  PrintIntField("after_duplicate_image_walk_copy_status",
                duplicate.walk.copy_status);
  PrintUint64Field("after_duplicate_registered_image_count",
                   JsonU64(registration.registered_image_count));
  PrintUint64Field("after_duplicate_registered_descriptor_total",
                   JsonU64(registration.registered_descriptor_total));
  PrintUint64Field(
      "after_duplicate_next_expected_registration_order_ordinal",
      JsonU64(registration.next_expected_registration_order_ordinal));
  PrintUint64Field(
      "after_duplicate_last_successful_registration_order_ordinal",
      JsonU64(registration.last_successful_registration_order_ordinal));
  PrintIntField("after_duplicate_last_registration_status",
                registration.last_registration_status);
  PrintStringField(
      "after_duplicate_last_rejected_module_name",
      duplicate.registration.rejected_module_name.c_str_or_null());
  PrintStringField(
      "after_duplicate_last_rejected_translation_unit_identity_key",
      duplicate.registration.rejected_translation_unit_identity_key
          .c_str_or_null());
  PrintUint64Field(
      "after_duplicate_last_rejected_registration_order_ordinal",
      JsonU64(registration.last_rejected_registration_order_ordinal));
  PrintUint64Field("after_duplicate_walked_image_count",
                   JsonU64(walk.walked_image_count));
  PrintUint64Field("after_duplicate_last_walked_class_descriptor_count",
                   JsonU64(walk.last_walked_class_descriptor_count));
  PrintUint64Field("after_duplicate_last_walked_protocol_descriptor_count",
                   JsonU64(walk.last_walked_protocol_descriptor_count));
  PrintUint64Field("after_duplicate_last_walked_category_descriptor_count",
                   JsonU64(walk.last_walked_category_descriptor_count));
  PrintUint64Field("after_duplicate_last_walked_property_descriptor_count",
                   JsonU64(walk.last_walked_property_descriptor_count));
  PrintUint64Field("after_duplicate_last_walked_ivar_descriptor_count",
                   JsonU64(walk.last_walked_ivar_descriptor_count));
  PrintIntField("after_duplicate_last_linker_anchor_matches_discovery_root",
                walk.last_linker_anchor_matches_discovery_root);
  PrintIntField("after_duplicate_last_registration_used_staged_table",
                walk.last_registration_used_staged_table, false);
}

inline void PrintProbeReport(
    const BootstrapTableConsumptionProbeResult &result) {
  std::printf("{");
  PrintStartupReportFields(result.startup);
  PrintDuplicateReportFields(result.duplicate);
  std::printf("}\n");
}

}  // namespace objc3c::runtime::probe::runtime_bootstrap_table_consumption
