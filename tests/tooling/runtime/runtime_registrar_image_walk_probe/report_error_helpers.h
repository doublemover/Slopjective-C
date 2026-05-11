#pragma once

#include "probe_result.h"
#include "registrar_image_fixture_setup.h"
#include "../support/json_probe_writer.h"

#include <cstdio>

namespace objc3c::runtime::probe::runtime_registrar_image_walk {

inline unsigned long long JsonUInt64(std::uint64_t value) {
  return static_cast<unsigned long long>(value);
}

inline void PrintRegistrationReportFields(
    const RegistrationObservation &registration) {
  const objc3_runtime_registration_state_snapshot &snapshot =
      registration.snapshot;

  ::objc3c::runtime::probe::PrintUint64Field(
      "registered_image_count", JsonUInt64(snapshot.registered_image_count));
  ::objc3c::runtime::probe::PrintUint64Field(
      "registered_descriptor_total",
      JsonUInt64(snapshot.registered_descriptor_total));
  ::objc3c::runtime::probe::PrintIntField("last_registration_status",
                                          snapshot.last_registration_status);
  ::objc3c::runtime::probe::PrintStringField(
      "last_registered_module_name",
      ReportString(registration.last_registered_module_name));
  ::objc3c::runtime::probe::PrintStringField(
      "last_registered_translation_unit_identity_key",
      ReportString(
          registration.last_registered_translation_unit_identity_key));
}

inline void PrintImageWalkReportFields(const ImageWalkObservation &walk) {
  const objc3_runtime_image_walk_state_snapshot &snapshot = walk.snapshot;

  ::objc3c::runtime::probe::PrintUint64Field(
      "walked_image_count", JsonUInt64(snapshot.walked_image_count));
  ::objc3c::runtime::probe::PrintUint64Field(
      "last_discovery_root_entry_count",
      JsonUInt64(snapshot.last_discovery_root_entry_count));
  ::objc3c::runtime::probe::PrintUint64Field(
      "last_walked_class_descriptor_count",
      JsonUInt64(snapshot.last_walked_class_descriptor_count));
  ::objc3c::runtime::probe::PrintUint64Field(
      "last_walked_protocol_descriptor_count",
      JsonUInt64(snapshot.last_walked_protocol_descriptor_count));
  ::objc3c::runtime::probe::PrintUint64Field(
      "last_walked_category_descriptor_count",
      JsonUInt64(snapshot.last_walked_category_descriptor_count));
  ::objc3c::runtime::probe::PrintUint64Field(
      "last_walked_property_descriptor_count",
      JsonUInt64(snapshot.last_walked_property_descriptor_count));
  ::objc3c::runtime::probe::PrintUint64Field(
      "last_walked_ivar_descriptor_count",
      JsonUInt64(snapshot.last_walked_ivar_descriptor_count));
  ::objc3c::runtime::probe::PrintUint64Field(
      "last_walked_selector_pool_count",
      JsonUInt64(snapshot.last_walked_selector_pool_count));
  ::objc3c::runtime::probe::PrintUint64Field(
      "last_walked_string_pool_count",
      JsonUInt64(snapshot.last_walked_string_pool_count));
  ::objc3c::runtime::probe::PrintIntField(
      "last_linker_anchor_matches_discovery_root",
      snapshot.last_linker_anchor_matches_discovery_root);
  ::objc3c::runtime::probe::PrintIntField(
      "last_registration_used_staged_table",
      snapshot.last_registration_used_staged_table);
  ::objc3c::runtime::probe::PrintStringField(
      "last_walked_module_name", ReportString(walk.last_walked_module_name));
  ::objc3c::runtime::probe::PrintStringField(
      "last_walked_translation_unit_identity_key",
      ReportString(walk.last_walked_translation_unit_identity_key));
}

inline void PrintSelectorInvariantReportFields(
    const SelectorInvariantObservation &selectors) {
  ::objc3c::runtime::probe::PrintUint64Field(
      "known_selector_stable_id",
      JsonUInt64(selectors.known_selector_stable_id));
  ::objc3c::runtime::probe::PrintUint64Field(
      "unknown_selector_stable_id",
      JsonUInt64(selectors.unknown_selector_stable_id), false);
}

inline void PrintCopyStatusReportFields(const ProbeResult &result) {
  ::objc3c::runtime::probe::PrintIntField(
      "registration_copy_status", result.registration.copy_status);
  ::objc3c::runtime::probe::PrintIntField("image_walk_copy_status",
                                          result.image_walk.copy_status);
}

inline void PrintRuntimeRegistrarImageWalkReport(const ProbeResult &result) {
  std::printf("{");
  PrintCopyStatusReportFields(result);
  PrintRegistrationReportFields(result.registration);
  PrintImageWalkReportFields(result.image_walk);
  PrintSelectorInvariantReportFields(result.selectors);
  std::printf("}\n");
}

}  // namespace objc3c::runtime::probe::runtime_registrar_image_walk
