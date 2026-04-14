#include "support/runtime_snapshot_stabilizers.h"
#include "support/output_expectations.h"

#include <string>

namespace {

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

}  // namespace

int main() {
  using objc3c::runtime::probe::ExpectTextEqual;
  using objc3c::runtime::probe::ExpectTrue;
  using objc3c::runtime::probe::StabilizeNullableCString;
  using objc3c::runtime::probe::StabilizePropertyEntry;
  using objc3c::runtime::probe::StabilizeRegistrationState;

  std::string source = "module-before-reset";
  std::string storage;
  const char *field = source.c_str();
  StabilizeNullableCString(field, storage, field);
  source.assign("module-after-reset");
  if (ExpectTextEqual(storage, "module-before-reset",
                      "stabilized nullable CString storage", 1) != 0 ||
      ExpectTextEqual(field, "module-before-reset",
                      "stabilized nullable CString field", 1) != 0) {
    return 1;
  }

  field = "not-null";
  StabilizeNullableCString(nullptr, storage, field);
  if (ExpectTextEqual(storage, "", "null source clears storage", 2) != 0 ||
      ExpectTrue(field == nullptr, "null source clears field", 2) != 0) {
    return 2;
  }

  objc3_runtime_registration_state_snapshot registration{};
  std::string registered_module = "registered-module";
  std::string registered_identity = "registered-identity";
  std::string rejected_module = "rejected-module";
  std::string rejected_identity = "rejected-identity";
  registration.last_registered_module_name = registered_module.c_str();
  registration.last_registered_translation_unit_identity_key =
      registered_identity.c_str();
  registration.last_rejected_module_name = rejected_module.c_str();
  registration.last_rejected_translation_unit_identity_key =
      rejected_identity.c_str();

  std::string registered_module_storage;
  std::string registered_identity_storage;
  std::string rejected_module_storage;
  std::string rejected_identity_storage;
  StabilizeRegistrationState(registration, registered_module_storage,
                             registered_identity_storage,
                             rejected_module_storage,
                             rejected_identity_storage);
  registered_module.assign("mutated");
  registered_identity.assign("mutated");
  rejected_module.assign("mutated");
  rejected_identity.assign("mutated");
  if (ExpectTextEqual(registration.last_registered_module_name,
                      "registered-module", "registered module stable copy",
                      3) != 0 ||
      ExpectTextEqual(registration.last_registered_translation_unit_identity_key,
                      "registered-identity", "registered identity stable copy",
                      3) != 0 ||
      ExpectTextEqual(registration.last_rejected_module_name, "rejected-module",
                      "rejected module stable copy", 3) != 0 ||
      ExpectTextEqual(registration.last_rejected_translation_unit_identity_key,
                      "rejected-identity", "rejected identity stable copy",
                      3) != 0) {
    return 3;
  }

  StablePropertyEntryForTest entry;
  std::string property_name = "count";
  std::string owner = "Widget";
  std::string lifetime = "strong";
  entry.snapshot.property_name = property_name.c_str();
  entry.snapshot.declaration_owner_identity = owner.c_str();
  entry.snapshot.ownership_lifetime_profile = lifetime.c_str();
  StabilizePropertyEntry(entry);
  property_name.assign("mutated");
  owner.assign("mutated");
  lifetime.assign("mutated");
  if (ExpectTextEqual(entry.snapshot.property_name, "count",
                      "property entry name stable copy", 4) != 0 ||
      ExpectTextEqual(entry.snapshot.declaration_owner_identity, "Widget",
                      "property entry owner stable copy", 4) != 0 ||
      ExpectTextEqual(entry.snapshot.ownership_lifetime_profile, "strong",
                      "property entry lifetime stable copy", 4) != 0) {
    return 4;
  }

  return 0;
}
