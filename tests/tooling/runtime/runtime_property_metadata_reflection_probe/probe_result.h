#pragma once

#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <string>

namespace objc3c::runtime::probe::runtime_property_metadata_reflection {

struct PropertyRegistryObservation {
  objc3_runtime_property_registry_state_snapshot state{};
  std::string queried_class;
  std::string queried_property;
  std::string resolved_class;
  std::string resolved_owner;
};

struct PropertyEntryObservation {
  objc3_runtime_property_entry_snapshot entry{};
  std::string queried_class;
  std::string resolved_class;
  std::string property_name;
  std::string declaration_owner;
  std::string export_owner;
  std::string getter_selector;
  std::string setter_selector;
  std::string effective_getter_selector;
  std::string effective_setter_selector;
  std::string ivar_binding;
  std::string synthesized_binding;
  std::string layout_symbol;
  std::string getter_owner;
  std::string setter_owner;
};

struct RealizedClassObservation {
  objc3_runtime_realized_class_entry_snapshot entry{};
  std::string module;
  std::string identity;
  std::string class_name;
  std::string class_owner;
  std::string metaclass_owner;
  std::string super_class;
  std::string super_metaclass;
  std::string category_owner;
  std::string category_name;
};

struct RuntimeReflectionFixture {
  RealizedClassObservation widget_entry;
};

struct ReflectionAssertions {
  PropertyRegistryObservation registry_state_before;
  PropertyEntryObservation token_property;
  PropertyEntryObservation value_property;
  PropertyEntryObservation count_property;
  PropertyRegistryObservation registry_state_after_count;
  PropertyEntryObservation missing_property;
  PropertyEntryObservation missing_class_property;
  PropertyRegistryObservation registry_state_after_missing;
};

struct ProbeResult {
  RuntimeReflectionFixture fixture;
  ReflectionAssertions assertions;
};

}  // namespace objc3c::runtime::probe::runtime_property_metadata_reflection
