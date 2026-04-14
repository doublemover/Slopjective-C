#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "support/json_probe_writer.h"
#include "support/runtime_snapshot_json.h"
#include "support/runtime_snapshot_stabilizers.h"

#include <cstdio>
#include <string>

namespace {

using objc3c::runtime::probe::PrintPropertyEntryStorageOwnership;
using objc3c::runtime::probe::PrintRealizedClassEntryPropertySummary;

using objc3c::runtime::probe::StabilizeNullableCString;
using objc3c::runtime::probe::StabilizePropertyEntry;
using objc3c::runtime::probe::StabilizeRealizedClassEntry;

using objc3c::runtime::probe::PrintJsonStringOrNull;

struct StablePropertyEntry {
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
  std::string ownership_lifetime_profile;
  std::string ownership_runtime_hook_profile;
  std::string accessor_ownership_profile;
  std::string getter_owner_identity;
  std::string setter_owner_identity;
};

struct StableRealizedClassEntry {
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

void PrintStorageAccessorImplementation(
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
  PrintJsonStringOrNull(snapshot.property_registry_state_snapshot_symbol);
  std::printf(",\"property_entry_snapshot_symbol\":");
  PrintJsonStringOrNull(snapshot.property_entry_snapshot_symbol);
  std::printf(",\"current_property_read_symbol\":");
  PrintJsonStringOrNull(snapshot.current_property_read_symbol);
  std::printf(",\"current_property_write_symbol\":");
  PrintJsonStringOrNull(snapshot.current_property_write_symbol);
  std::printf(",\"current_property_exchange_symbol\":");
  PrintJsonStringOrNull(snapshot.current_property_exchange_symbol);
  std::printf(",\"bind_current_property_context_symbol\":");
  PrintJsonStringOrNull(snapshot.bind_current_property_context_symbol);
  std::printf(",\"clear_current_property_context_symbol\":");
  PrintJsonStringOrNull(snapshot.clear_current_property_context_symbol);
  std::printf(",\"weak_current_property_load_symbol\":");
  PrintJsonStringOrNull(snapshot.weak_current_property_load_symbol);
  std::printf(",\"weak_current_property_store_symbol\":");
  PrintJsonStringOrNull(snapshot.weak_current_property_store_symbol);
  std::printf(",\"implementation_model\":");
  PrintJsonStringOrNull(snapshot.implementation_model);
  std::printf(",\"reflection_model\":");
  PrintJsonStringOrNull(snapshot.reflection_model);
  std::printf(",\"fail_closed_model\":");
  PrintJsonStringOrNull(snapshot.fail_closed_model);
  std::printf("}");
}

void LoadProperty(const char *class_name, const char *property_name,
                  StablePropertyEntry &entry) {
  (void)objc3_runtime_copy_property_entry_for_testing(class_name, property_name,
                                                      &entry.snapshot);
  StabilizePropertyEntry(entry);
}

} // namespace

int main() {
  StableRealizedClassEntry box_entry;
  StablePropertyEntry current_value;
  StablePropertyEntry copied_value;
  StablePropertyEntry weak_value;
  StablePropertyEntry borrowed_value;
  StablePropertyEntry guarded_value;
  objc3_runtime_storage_accessor_implementation_snapshot implementation{};

  (void)objc3_runtime_copy_realized_class_entry_for_testing(
      "Box", &box_entry.snapshot);
  StabilizeRealizedClassEntry(box_entry);
  LoadProperty("Box", "currentValue", current_value);
  LoadProperty("Box", "copiedValue", copied_value);
  LoadProperty("Box", "weakValue", weak_value);
  LoadProperty("Box", "borrowedValue", borrowed_value);
  LoadProperty("Box", "guardedValue", guarded_value);
  (void)objc3_runtime_copy_storage_accessor_implementation_snapshot_for_testing(
      &implementation);

  std::printf("{");
  std::printf("\"box_entry\":");
  PrintRealizedClassEntryPropertySummary(box_entry.snapshot);
  std::printf(",\"implementation_surface\":");
  PrintStorageAccessorImplementation(implementation);
  std::printf(",\"current_value_property\":");
  PrintPropertyEntryStorageOwnership(current_value.snapshot);
  std::printf(",\"copied_value_property\":");
  PrintPropertyEntryStorageOwnership(copied_value.snapshot);
  std::printf(",\"weak_value_property\":");
  PrintPropertyEntryStorageOwnership(weak_value.snapshot);
  std::printf(",\"borrowed_value_property\":");
  PrintPropertyEntryStorageOwnership(borrowed_value.snapshot);
  std::printf(",\"guarded_value_property\":");
  PrintPropertyEntryStorageOwnership(guarded_value.snapshot);
  std::printf("}");
  return 0;
}
