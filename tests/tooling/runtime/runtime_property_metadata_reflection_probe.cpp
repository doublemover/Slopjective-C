#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "support/json_probe_writer.h"
#include "support/runtime_snapshot_json.h"
#include "support/runtime_snapshot_stabilizers.h"

#include <cstdio>
#include <string>

namespace {

using objc3c::runtime::probe::PrintPropertyEntryFull;
using objc3c::runtime::probe::PrintPropertyRegistryStateFull;
using objc3c::runtime::probe::PrintRealizedClassEntryPropertySummary;

using objc3c::runtime::probe::StabilizeNullableCString;
using objc3c::runtime::probe::StabilizePropertyEntry;
using objc3c::runtime::probe::StabilizePropertyRegistryState;
using objc3c::runtime::probe::StabilizeRealizedClassEntry;

using objc3c::runtime::probe::PrintJsonStringOrNull;

} // namespace

int main() {
  objc3_runtime_property_registry_state_snapshot registry_before{};
  objc3_runtime_property_registry_state_snapshot registry_after_count{};
  objc3_runtime_property_registry_state_snapshot registry_after_missing{};
  objc3_runtime_property_entry_snapshot token_entry{};
  objc3_runtime_property_entry_snapshot value_entry{};
  objc3_runtime_property_entry_snapshot count_entry{};
  objc3_runtime_property_entry_snapshot missing_entry{};
  objc3_runtime_property_entry_snapshot missing_class_entry{};
  objc3_runtime_realized_class_entry_snapshot widget_entry{};

  std::string registry_before_queried_class;
  std::string registry_before_queried_property;
  std::string registry_before_resolved_class;
  std::string registry_before_resolved_owner;
  std::string registry_after_count_queried_class;
  std::string registry_after_count_queried_property;
  std::string registry_after_count_resolved_class;
  std::string registry_after_count_resolved_owner;
  std::string registry_after_missing_queried_class;
  std::string registry_after_missing_queried_property;
  std::string registry_after_missing_resolved_class;
  std::string registry_after_missing_resolved_owner;

  std::string token_queried_class;
  std::string token_resolved_class;
  std::string token_property_name;
  std::string token_declaration_owner;
  std::string token_export_owner;
  std::string token_getter_selector;
  std::string token_setter_selector;
  std::string token_effective_getter_selector;
  std::string token_effective_setter_selector;
  std::string token_ivar_binding;
  std::string token_synthesized_binding;
  std::string token_layout_symbol;
  std::string token_getter_owner;
  std::string token_setter_owner;

  std::string value_queried_class;
  std::string value_resolved_class;
  std::string value_property_name;
  std::string value_declaration_owner;
  std::string value_export_owner;
  std::string value_getter_selector;
  std::string value_setter_selector;
  std::string value_effective_getter_selector;
  std::string value_effective_setter_selector;
  std::string value_ivar_binding;
  std::string value_synthesized_binding;
  std::string value_layout_symbol;
  std::string value_getter_owner;
  std::string value_setter_owner;

  std::string count_queried_class;
  std::string count_resolved_class;
  std::string count_property_name;
  std::string count_declaration_owner;
  std::string count_export_owner;
  std::string count_getter_selector;
  std::string count_setter_selector;
  std::string count_effective_getter_selector;
  std::string count_effective_setter_selector;
  std::string count_ivar_binding;
  std::string count_synthesized_binding;
  std::string count_layout_symbol;
  std::string count_getter_owner;
  std::string count_setter_owner;

  std::string missing_queried_class;
  std::string missing_resolved_class;
  std::string missing_property_name;
  std::string missing_declaration_owner;
  std::string missing_export_owner;
  std::string missing_getter_selector;
  std::string missing_setter_selector;
  std::string missing_effective_getter_selector;
  std::string missing_effective_setter_selector;
  std::string missing_ivar_binding;
  std::string missing_synthesized_binding;
  std::string missing_layout_symbol;
  std::string missing_getter_owner;
  std::string missing_setter_owner;

  std::string missing_class_queried_class;
  std::string missing_class_resolved_class;
  std::string missing_class_property_name;
  std::string missing_class_declaration_owner;
  std::string missing_class_export_owner;
  std::string missing_class_getter_selector;
  std::string missing_class_setter_selector;
  std::string missing_class_effective_getter_selector;
  std::string missing_class_effective_setter_selector;
  std::string missing_class_ivar_binding;
  std::string missing_class_synthesized_binding;
  std::string missing_class_layout_symbol;
  std::string missing_class_getter_owner;
  std::string missing_class_setter_owner;

  std::string widget_module_storage;
  std::string widget_identity_storage;
  std::string widget_class_storage;
  std::string widget_class_owner_storage;
  std::string widget_metaclass_owner_storage;
  std::string widget_super_class_storage;
  std::string widget_super_metaclass_storage;
  std::string widget_category_owner_storage;
  std::string widget_category_name_storage;

  (void)objc3_runtime_copy_property_registry_state_for_testing(
      &registry_before);
  StabilizePropertyRegistryState(registry_before, registry_before_queried_class,
                                 registry_before_queried_property,
                                 registry_before_resolved_class,
                                 registry_before_resolved_owner);

  (void)objc3_runtime_copy_realized_class_entry_for_testing("Widget",
                                                            &widget_entry);
  StabilizeRealizedClassEntry(
      widget_entry, widget_module_storage, widget_identity_storage,
      widget_class_storage, widget_class_owner_storage,
      widget_metaclass_owner_storage, widget_super_class_storage,
      widget_super_metaclass_storage, widget_category_owner_storage,
      widget_category_name_storage);

  (void)objc3_runtime_copy_property_entry_for_testing("Widget", "token",
                                                      &token_entry);
  StabilizePropertyEntry(token_entry, token_queried_class, token_resolved_class,
                         token_property_name, token_declaration_owner,
                         token_export_owner, token_getter_selector,
                         token_setter_selector, token_effective_getter_selector,
                         token_effective_setter_selector, token_ivar_binding,
                         token_synthesized_binding, token_layout_symbol,
                         token_getter_owner, token_setter_owner);

  (void)objc3_runtime_copy_property_entry_for_testing("Widget", "value",
                                                      &value_entry);
  StabilizePropertyEntry(value_entry, value_queried_class, value_resolved_class,
                         value_property_name, value_declaration_owner,
                         value_export_owner, value_getter_selector,
                         value_setter_selector, value_effective_getter_selector,
                         value_effective_setter_selector, value_ivar_binding,
                         value_synthesized_binding, value_layout_symbol,
                         value_getter_owner, value_setter_owner);

  (void)objc3_runtime_copy_property_entry_for_testing("Widget", "count",
                                                      &count_entry);
  StabilizePropertyEntry(count_entry, count_queried_class, count_resolved_class,
                         count_property_name, count_declaration_owner,
                         count_export_owner, count_getter_selector,
                         count_setter_selector, count_effective_getter_selector,
                         count_effective_setter_selector, count_ivar_binding,
                         count_synthesized_binding, count_layout_symbol,
                         count_getter_owner, count_setter_owner);

  (void)objc3_runtime_copy_property_registry_state_for_testing(
      &registry_after_count);
  StabilizePropertyRegistryState(
      registry_after_count, registry_after_count_queried_class,
      registry_after_count_queried_property,
      registry_after_count_resolved_class, registry_after_count_resolved_owner);

  (void)objc3_runtime_copy_property_entry_for_testing("Widget", "missing",
                                                      &missing_entry);
  StabilizePropertyEntry(
      missing_entry, missing_queried_class, missing_resolved_class,
      missing_property_name, missing_declaration_owner, missing_export_owner,
      missing_getter_selector, missing_setter_selector,
      missing_effective_getter_selector, missing_effective_setter_selector,
      missing_ivar_binding, missing_synthesized_binding, missing_layout_symbol,
      missing_getter_owner, missing_setter_owner);

  (void)objc3_runtime_copy_property_entry_for_testing("MissingWidget", "count",
                                                      &missing_class_entry);
  StabilizePropertyEntry(
      missing_class_entry, missing_class_queried_class,
      missing_class_resolved_class, missing_class_property_name,
      missing_class_declaration_owner, missing_class_export_owner,
      missing_class_getter_selector, missing_class_setter_selector,
      missing_class_effective_getter_selector,
      missing_class_effective_setter_selector, missing_class_ivar_binding,
      missing_class_synthesized_binding, missing_class_layout_symbol,
      missing_class_getter_owner, missing_class_setter_owner);

  (void)objc3_runtime_copy_property_registry_state_for_testing(
      &registry_after_missing);
  StabilizePropertyRegistryState(registry_after_missing,
                                 registry_after_missing_queried_class,
                                 registry_after_missing_queried_property,
                                 registry_after_missing_resolved_class,
                                 registry_after_missing_resolved_owner);

  std::printf("{");
  std::printf("\"registry_state_before\":");
  PrintPropertyRegistryStateFull(registry_before);
  std::printf(",\"widget_entry\":");
  PrintRealizedClassEntryPropertySummary(widget_entry);
  std::printf(",\"token_property\":");
  PrintPropertyEntryFull(token_entry);
  std::printf(",\"value_property\":");
  PrintPropertyEntryFull(value_entry);
  std::printf(",\"count_property\":");
  PrintPropertyEntryFull(count_entry);
  std::printf(",\"registry_state_after_count\":");
  PrintPropertyRegistryStateFull(registry_after_count);
  std::printf(",\"missing_property\":");
  PrintPropertyEntryFull(missing_entry);
  std::printf(",\"missing_class_property\":");
  PrintPropertyEntryFull(missing_class_entry);
  std::printf(",\"registry_state_after_missing\":");
  PrintPropertyRegistryStateFull(registry_after_missing);
  std::printf("}");
  return 0;
}
