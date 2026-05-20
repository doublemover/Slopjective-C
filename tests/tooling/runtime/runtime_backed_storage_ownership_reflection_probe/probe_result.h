#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_BACKED_STORAGE_OWNERSHIP_REFLECTION_PROBE_PROBE_RESULT_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_BACKED_STORAGE_OWNERSHIP_REFLECTION_PROBE_PROBE_RESULT_H_

#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <string>

namespace objc3c::runtime::probe::runtime_backed_storage_ownership_reflection {

struct RealizedClassObservation {
  objc3_runtime_realized_class_entry_snapshot snapshot{};
  std::string module_name;
  std::string translation_unit_identity_key;
  std::string class_name;
  std::string class_owner_identity;
  std::string metaclass_owner_identity;
  std::string super_class_owner_identity;
  std::string super_metaclass_owner_identity;
  std::string attached_category_owner_identity;
  std::string attached_category_name;
};

struct StorageOwnershipPropertyObservation {
  objc3_runtime_property_entry_snapshot snapshot{};
  std::string queried_class_name;
  std::string resolved_class_name;
  std::string property_name;
  std::string declaration_owner_identity;
  std::string export_owner_identity;
  std::string getter_selector;
  std::string setter_selector;
  std::string effective_getter_selector;
  std::string effective_setter_selector;
  std::string ivar_binding_symbol;
  std::string synthesized_binding_symbol;
  std::string ivar_layout_symbol;
  std::string property_attribute_profile;
  std::string property_behavior_name;
  std::string ownership_lifetime_profile;
  std::string ownership_runtime_hook_profile;
  std::string accessor_ownership_profile;
  std::string getter_owner_identity;
  std::string setter_owner_identity;
};

struct StorageAccessorImplementationObservation {
  objc3_runtime_storage_accessor_implementation_snapshot snapshot{};
  std::string property_registry_state_snapshot_symbol;
  std::string property_entry_snapshot_symbol;
  std::string current_property_read_symbol;
  std::string current_property_write_symbol;
  std::string current_property_exchange_symbol;
  std::string bind_current_property_context_symbol;
  std::string clear_current_property_context_symbol;
  std::string weak_current_property_load_symbol;
  std::string weak_current_property_store_symbol;
  std::string implementation_model;
  std::string reflection_model;
  std::string fail_closed_model;
};

struct StorageReflectionFixture {
  RealizedClassObservation box_entry;
};

struct BackedStorageOwnershipAssertions {
  StorageAccessorImplementationObservation implementation_surface;
  StorageOwnershipPropertyObservation current_value_property;
  StorageOwnershipPropertyObservation copied_value_property;
  StorageOwnershipPropertyObservation weak_value_property;
  StorageOwnershipPropertyObservation borrowed_value_property;
  StorageOwnershipPropertyObservation guarded_value_property;
};

struct ProbeResult {
  StorageReflectionFixture fixture;
  BackedStorageOwnershipAssertions assertions;
};

}  // namespace objc3c::runtime::probe::runtime_backed_storage_ownership_reflection

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_BACKED_STORAGE_OWNERSHIP_REFLECTION_PROBE_PROBE_RESULT_H_
