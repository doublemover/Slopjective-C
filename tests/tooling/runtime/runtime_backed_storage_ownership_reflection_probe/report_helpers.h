#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_BACKED_STORAGE_OWNERSHIP_REFLECTION_PROBE_REPORT_HELPERS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_BACKED_STORAGE_OWNERSHIP_REFLECTION_PROBE_REPORT_HELPERS_H_

#include "probe_result.h"
#include "support/json_probe_writer.h"
#include "support/runtime_snapshot_json.h"

#include <cstdio>

namespace objc3c::runtime::probe::runtime_backed_storage_ownership_reflection {

inline void PrintStorageAccessorImplementation(
    const objc3_runtime_storage_accessor_implementation_snapshot &snapshot) {
  std::printf("{");
  std::printf(
      "\"property_registry_ready\":%llu,",
      static_cast<unsigned long long>(snapshot.property_registry_ready));
  std::printf("\"runtime_accessor_dispatch_ready\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.runtime_accessor_dispatch_ready));
  std::printf("\"runtime_layout_ready\":%llu,",
              static_cast<unsigned long long>(snapshot.runtime_layout_ready));
  std::printf("\"reflection_query_ready\":%llu,",
              static_cast<unsigned long long>(snapshot.reflection_query_ready));
  std::printf("\"deterministic\":%llu,",
              static_cast<unsigned long long>(snapshot.deterministic));
  std::printf("\"property_registry_state_snapshot_symbol\":");
  ::objc3c::runtime::probe::PrintJsonStringOrNull(
      snapshot.property_registry_state_snapshot_symbol);
  std::printf(",\"property_entry_snapshot_symbol\":");
  ::objc3c::runtime::probe::PrintJsonStringOrNull(
      snapshot.property_entry_snapshot_symbol);
  std::printf(",\"current_property_read_symbol\":");
  ::objc3c::runtime::probe::PrintJsonStringOrNull(
      snapshot.current_property_read_symbol);
  std::printf(",\"current_property_write_symbol\":");
  ::objc3c::runtime::probe::PrintJsonStringOrNull(
      snapshot.current_property_write_symbol);
  std::printf(",\"current_property_exchange_symbol\":");
  ::objc3c::runtime::probe::PrintJsonStringOrNull(
      snapshot.current_property_exchange_symbol);
  std::printf(",\"bind_current_property_context_symbol\":");
  ::objc3c::runtime::probe::PrintJsonStringOrNull(
      snapshot.bind_current_property_context_symbol);
  std::printf(",\"clear_current_property_context_symbol\":");
  ::objc3c::runtime::probe::PrintJsonStringOrNull(
      snapshot.clear_current_property_context_symbol);
  std::printf(",\"weak_current_property_load_symbol\":");
  ::objc3c::runtime::probe::PrintJsonStringOrNull(
      snapshot.weak_current_property_load_symbol);
  std::printf(",\"weak_current_property_store_symbol\":");
  ::objc3c::runtime::probe::PrintJsonStringOrNull(
      snapshot.weak_current_property_store_symbol);
  std::printf(",\"implementation_model\":");
  ::objc3c::runtime::probe::PrintJsonStringOrNull(
      snapshot.implementation_model);
  std::printf(",\"reflection_model\":");
  ::objc3c::runtime::probe::PrintJsonStringOrNull(snapshot.reflection_model);
  std::printf(",\"fail_closed_model\":");
  ::objc3c::runtime::probe::PrintJsonStringOrNull(snapshot.fail_closed_model);
  std::printf("}");
}

inline void PrintProbeReport(const ProbeResult &result) {
  const StorageReflectionFixture &fixture = result.fixture;
  const BackedStorageOwnershipAssertions &assertions = result.assertions;

  std::printf("{");
  std::printf("\"box_entry\":");
  ::objc3c::runtime::probe::PrintRealizedClassEntryPropertySummary(
      fixture.box_entry.snapshot);
  std::printf(",\"implementation_surface\":");
  PrintStorageAccessorImplementation(assertions.implementation_surface.snapshot);
  std::printf(",\"current_value_property\":");
  ::objc3c::runtime::probe::PrintPropertyEntryStorageOwnership(
      assertions.current_value_property.snapshot);
  std::printf(",\"copied_value_property\":");
  ::objc3c::runtime::probe::PrintPropertyEntryStorageOwnership(
      assertions.copied_value_property.snapshot);
  std::printf(",\"weak_value_property\":");
  ::objc3c::runtime::probe::PrintPropertyEntryStorageOwnership(
      assertions.weak_value_property.snapshot);
  std::printf(",\"borrowed_value_property\":");
  ::objc3c::runtime::probe::PrintPropertyEntryStorageOwnership(
      assertions.borrowed_value_property.snapshot);
  std::printf(",\"guarded_value_property\":");
  ::objc3c::runtime::probe::PrintPropertyEntryStorageOwnership(
      assertions.guarded_value_property.snapshot);
  std::printf("}");
}

}  // namespace objc3c::runtime::probe::runtime_backed_storage_ownership_reflection

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_BACKED_STORAGE_OWNERSHIP_REFLECTION_PROBE_REPORT_HELPERS_H_
