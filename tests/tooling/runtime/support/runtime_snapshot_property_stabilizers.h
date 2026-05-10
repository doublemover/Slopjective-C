#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_SUPPORT_RUNTIME_SNAPSHOT_PROPERTY_STABILIZERS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_SUPPORT_RUNTIME_SNAPSHOT_PROPERTY_STABILIZERS_H_

#include "support/runtime_snapshot_stabilizer_common.h"

namespace objc3c::runtime::probe {

inline void StabilizePropertyEntry(
    objc3_runtime_property_entry_snapshot &snapshot,
    std::string &queried_class_storage, std::string &resolved_class_storage,
    std::string &property_name_storage,
    std::string &declaration_owner_storage,
    std::string &export_owner_storage, std::string &getter_selector_storage,
    std::string &setter_selector_storage,
    std::string &effective_getter_selector_storage,
    std::string &effective_setter_selector_storage,
    std::string &ivar_binding_storage,
    std::string &synthesized_binding_storage,
    std::string &layout_symbol_storage, std::string &getter_owner_storage,
    std::string &setter_owner_storage) {
  StabilizeNullableCString(snapshot.queried_class_name, queried_class_storage,
                           snapshot.queried_class_name);
  StabilizeNullableCString(snapshot.resolved_class_name, resolved_class_storage,
                           snapshot.resolved_class_name);
  StabilizeNullableCString(snapshot.property_name, property_name_storage,
                           snapshot.property_name);
  StabilizeNullableCString(snapshot.declaration_owner_identity,
                           declaration_owner_storage,
                           snapshot.declaration_owner_identity);
  StabilizeNullableCString(snapshot.export_owner_identity, export_owner_storage,
                           snapshot.export_owner_identity);
  StabilizeNullableCString(snapshot.getter_selector, getter_selector_storage,
                           snapshot.getter_selector);
  StabilizeNullableCString(snapshot.setter_selector, setter_selector_storage,
                           snapshot.setter_selector);
  StabilizeNullableCString(snapshot.effective_getter_selector,
                           effective_getter_selector_storage,
                           snapshot.effective_getter_selector);
  StabilizeNullableCString(snapshot.effective_setter_selector,
                           effective_setter_selector_storage,
                           snapshot.effective_setter_selector);
  StabilizeNullableCString(snapshot.ivar_binding_symbol, ivar_binding_storage,
                           snapshot.ivar_binding_symbol);
  StabilizeNullableCString(snapshot.synthesized_binding_symbol,
                           synthesized_binding_storage,
                           snapshot.synthesized_binding_symbol);
  StabilizeNullableCString(snapshot.ivar_layout_symbol, layout_symbol_storage,
                           snapshot.ivar_layout_symbol);
  StabilizeNullableCString(snapshot.getter_owner_identity,
                           getter_owner_storage,
                           snapshot.getter_owner_identity);
  StabilizeNullableCString(snapshot.setter_owner_identity,
                           setter_owner_storage,
                           snapshot.setter_owner_identity);
}

inline void StabilizePropertyEntry(
    objc3_runtime_property_entry_snapshot &snapshot,
    std::string &queried_class_storage, std::string &resolved_class_storage,
    std::string &property_name_storage,
    std::string &declaration_owner_storage,
    std::string &getter_owner_storage, std::string &setter_owner_storage,
    std::string &lifetime_storage, std::string &hook_storage,
    std::string &accessor_storage) {
  StabilizeNullableCString(snapshot.queried_class_name, queried_class_storage,
                           snapshot.queried_class_name);
  StabilizeNullableCString(snapshot.resolved_class_name, resolved_class_storage,
                           snapshot.resolved_class_name);
  StabilizeNullableCString(snapshot.property_name, property_name_storage,
                           snapshot.property_name);
  StabilizeNullableCString(snapshot.declaration_owner_identity,
                           declaration_owner_storage,
                           snapshot.declaration_owner_identity);
  StabilizeNullableCString(snapshot.getter_owner_identity, getter_owner_storage,
                           snapshot.getter_owner_identity);
  StabilizeNullableCString(snapshot.setter_owner_identity, setter_owner_storage,
                           snapshot.setter_owner_identity);
  StabilizeNullableCString(snapshot.ownership_lifetime_profile,
                           lifetime_storage,
                           snapshot.ownership_lifetime_profile);
  StabilizeNullableCString(snapshot.ownership_runtime_hook_profile,
                           hook_storage,
                           snapshot.ownership_runtime_hook_profile);
  StabilizeNullableCString(snapshot.accessor_ownership_profile,
                           accessor_storage,
                           snapshot.accessor_ownership_profile);
}

inline void StabilizePropertyEntry(
    objc3_runtime_property_entry_snapshot &snapshot,
    std::string &queried_class_storage, std::string &resolved_class_storage,
    std::string &property_name_storage, std::string &owner_storage,
    std::string &lifetime_storage, std::string &hook_storage,
    std::string &accessor_storage) {
  StabilizeNullableCString(snapshot.queried_class_name, queried_class_storage,
                           snapshot.queried_class_name);
  StabilizeNullableCString(snapshot.resolved_class_name, resolved_class_storage,
                           snapshot.resolved_class_name);
  StabilizeNullableCString(snapshot.property_name, property_name_storage,
                           snapshot.property_name);
  StabilizeNullableCString(snapshot.declaration_owner_identity, owner_storage,
                           snapshot.declaration_owner_identity);
  StabilizeNullableCString(snapshot.ownership_lifetime_profile,
                           lifetime_storage,
                           snapshot.ownership_lifetime_profile);
  StabilizeNullableCString(snapshot.ownership_runtime_hook_profile,
                           hook_storage,
                           snapshot.ownership_runtime_hook_profile);
  StabilizeNullableCString(snapshot.accessor_ownership_profile,
                           accessor_storage,
                           snapshot.accessor_ownership_profile);
}

template <typename StablePropertyEntry>
inline void StabilizePropertyEntry(StablePropertyEntry &entry) {
  StabilizeNullableCString(entry.snapshot.queried_class_name,
                           entry.queried_class_name,
                           entry.snapshot.queried_class_name);
  StabilizeNullableCString(entry.snapshot.resolved_class_name,
                           entry.resolved_class_name,
                           entry.snapshot.resolved_class_name);
  StabilizeNullableCString(entry.snapshot.property_name, entry.property_name,
                           entry.snapshot.property_name);
  StabilizeNullableCString(entry.snapshot.declaration_owner_identity,
                           entry.declaration_owner_identity,
                           entry.snapshot.declaration_owner_identity);
  StabilizeNullableCString(entry.snapshot.export_owner_identity,
                           entry.export_owner_identity,
                           entry.snapshot.export_owner_identity);
  StabilizeNullableCString(entry.snapshot.getter_selector,
                           entry.getter_selector,
                           entry.snapshot.getter_selector);
  StabilizeNullableCString(entry.snapshot.setter_selector,
                           entry.setter_selector,
                           entry.snapshot.setter_selector);
  StabilizeNullableCString(entry.snapshot.effective_getter_selector,
                           entry.effective_getter_selector,
                           entry.snapshot.effective_getter_selector);
  StabilizeNullableCString(entry.snapshot.effective_setter_selector,
                           entry.effective_setter_selector,
                           entry.snapshot.effective_setter_selector);
  StabilizeNullableCString(entry.snapshot.ivar_binding_symbol,
                           entry.ivar_binding_symbol,
                           entry.snapshot.ivar_binding_symbol);
  StabilizeNullableCString(entry.snapshot.synthesized_binding_symbol,
                           entry.synthesized_binding_symbol,
                           entry.snapshot.synthesized_binding_symbol);
  StabilizeNullableCString(entry.snapshot.ivar_layout_symbol,
                           entry.ivar_layout_symbol,
                           entry.snapshot.ivar_layout_symbol);
  StabilizeNullableCString(entry.snapshot.property_attribute_profile,
                           entry.property_attribute_profile,
                           entry.snapshot.property_attribute_profile);
  StabilizeNullableCString(entry.snapshot.ownership_lifetime_profile,
                           entry.ownership_lifetime_profile,
                           entry.snapshot.ownership_lifetime_profile);
  StabilizeNullableCString(entry.snapshot.ownership_runtime_hook_profile,
                           entry.ownership_runtime_hook_profile,
                           entry.snapshot.ownership_runtime_hook_profile);
  StabilizeNullableCString(entry.snapshot.accessor_ownership_profile,
                           entry.accessor_ownership_profile,
                           entry.snapshot.accessor_ownership_profile);
  StabilizeNullableCString(entry.snapshot.getter_owner_identity,
                           entry.getter_owner_identity,
                           entry.snapshot.getter_owner_identity);
  StabilizeNullableCString(entry.snapshot.setter_owner_identity,
                           entry.setter_owner_identity,
                           entry.snapshot.setter_owner_identity);
}

inline void StabilizePropertyRegistryState(
    objc3_runtime_property_registry_state_snapshot &snapshot,
    std::string &queried_class_storage, std::string &queried_property_storage,
    std::string &resolved_class_storage, std::string &resolved_owner_storage) {
  StabilizeNullableCString(snapshot.last_queried_class_name,
                           queried_class_storage,
                           snapshot.last_queried_class_name);
  StabilizeNullableCString(snapshot.last_queried_property_name,
                           queried_property_storage,
                           snapshot.last_queried_property_name);
  StabilizeNullableCString(snapshot.last_resolved_class_name,
                           resolved_class_storage,
                           snapshot.last_resolved_class_name);
  StabilizeNullableCString(snapshot.last_resolved_owner_identity,
                           resolved_owner_storage,
                           snapshot.last_resolved_owner_identity);
}


}  // namespace objc3c::runtime::probe

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_SUPPORT_RUNTIME_SNAPSHOT_PROPERTY_STABILIZERS_H_
