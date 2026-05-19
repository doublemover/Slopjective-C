#pragma once

#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <string>

namespace objc3c::runtime::probe::runtime_snapshot_stabilizers_support {

struct MutableCStringStabilizerFixture {
  std::string source = "module-before-reset";
  std::string storage;
  const char *field = source.c_str();

  void MutateSource() { source.assign("module-after-reset"); }
};

struct RegistrationStateStabilizerFixture {
  objc3_runtime_registration_state_snapshot snapshot{};
  std::string registered_module = "registered-module";
  std::string registered_identity = "registered-identity";
  std::string rejected_module = "rejected-module";
  std::string rejected_identity = "rejected-identity";
  std::string registered_module_storage;
  std::string registered_identity_storage;
  std::string rejected_module_storage;
  std::string rejected_identity_storage;

  RegistrationStateStabilizerFixture() {
    snapshot.last_registered_module_name = registered_module.c_str();
    snapshot.last_registered_translation_unit_identity_key =
        registered_identity.c_str();
    snapshot.last_rejected_module_name = rejected_module.c_str();
    snapshot.last_rejected_translation_unit_identity_key =
        rejected_identity.c_str();
  }

  void MutateBackingStrings() {
    registered_module.assign("mutated");
    registered_identity.assign("mutated");
    rejected_module.assign("mutated");
    rejected_identity.assign("mutated");
  }
};

struct StablePropertyEntryForTest {
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

struct PropertyEntryStabilizerFixture {
  StablePropertyEntryForTest entry;
  std::string property_name = "count";
  std::string owner = "Widget";
  std::string lifetime = "strong";

  PropertyEntryStabilizerFixture() {
    entry.snapshot.property_name = property_name.c_str();
    entry.snapshot.declaration_owner_identity = owner.c_str();
    entry.snapshot.ownership_lifetime_profile = lifetime.c_str();
  }

  void MutateBackingStrings() {
    property_name.assign("mutated");
    owner.assign("mutated");
    lifetime.assign("mutated");
  }
};

}  // namespace objc3c::runtime::probe::runtime_snapshot_stabilizers_support
