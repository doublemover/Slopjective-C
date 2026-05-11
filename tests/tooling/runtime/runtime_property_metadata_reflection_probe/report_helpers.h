#pragma once

#include "probe_result.h"
#include "support/runtime_snapshot_json.h"

#include <cstdio>

namespace objc3c::runtime::probe::runtime_property_metadata_reflection {

inline void PrintRuntimePropertyMetadataReflectionReport(
    const ProbeResult &result) {
  const RuntimeReflectionFixture &fixture = result.fixture;
  const ReflectionAssertions &assertions = result.assertions;

  std::printf("{");
  std::printf("\"registry_state_before\":");
  ::objc3c::runtime::probe::PrintPropertyRegistryStateFull(
      assertions.registry_state_before.state);
  std::printf(",\"widget_entry\":");
  ::objc3c::runtime::probe::PrintRealizedClassEntryPropertySummary(
      fixture.widget_entry.entry);
  std::printf(",\"token_property\":");
  ::objc3c::runtime::probe::PrintPropertyEntryFull(
      assertions.token_property.entry);
  std::printf(",\"value_property\":");
  ::objc3c::runtime::probe::PrintPropertyEntryFull(
      assertions.value_property.entry);
  std::printf(",\"count_property\":");
  ::objc3c::runtime::probe::PrintPropertyEntryFull(
      assertions.count_property.entry);
  std::printf(",\"registry_state_after_count\":");
  ::objc3c::runtime::probe::PrintPropertyRegistryStateFull(
      assertions.registry_state_after_count.state);
  std::printf(",\"missing_property\":");
  ::objc3c::runtime::probe::PrintPropertyEntryFull(
      assertions.missing_property.entry);
  std::printf(",\"missing_class_property\":");
  ::objc3c::runtime::probe::PrintPropertyEntryFull(
      assertions.missing_class_property.entry);
  std::printf(",\"registry_state_after_missing\":");
  ::objc3c::runtime::probe::PrintPropertyRegistryStateFull(
      assertions.registry_state_after_missing.state);
  std::printf("}");
}

}  // namespace objc3c::runtime::probe::runtime_property_metadata_reflection
