#pragma once

#include "stabilizer_fixture_data.h"
#include "../support/output_expectations.h"
#include "../support/runtime_snapshot_stabilizers.h"

#include <string>

namespace objc3c::runtime::probe::runtime_snapshot_stabilizers_support {

inline int VerifyNullableCStringStabilizerPreservesStorage() {
  MutableCStringStabilizerFixture fixture;
  ::objc3c::runtime::probe::StabilizeNullableCString(
      fixture.field, fixture.storage, fixture.field);
  fixture.MutateSource();

  if (::objc3c::runtime::probe::ExpectTextEqual(
          fixture.storage, "module-before-reset",
          "stabilized nullable CString storage", 1) != 0 ||
      ::objc3c::runtime::probe::ExpectTextEqual(
          fixture.field, "module-before-reset",
          "stabilized nullable CString field", 1) != 0) {
    return 1;
  }
  return 0;
}

inline int VerifyNullableCStringStabilizerClearsNullSource() {
  std::string storage;
  const char *field = "not-null";
  ::objc3c::runtime::probe::StabilizeNullableCString(nullptr, storage, field);

  if (::objc3c::runtime::probe::ExpectTextEqual(
          storage, "", "null source clears storage", 2) != 0 ||
      ::objc3c::runtime::probe::ExpectTrue(field == nullptr,
                                           "null source clears field", 2) != 0) {
    return 2;
  }
  return 0;
}

inline int VerifyRegistrationStateStabilizerCopiesRuntimeStrings() {
  RegistrationStateStabilizerFixture fixture;
  ::objc3c::runtime::probe::StabilizeRegistrationState(
      fixture.snapshot, fixture.registered_module_storage,
      fixture.registered_identity_storage, fixture.rejected_module_storage,
      fixture.rejected_identity_storage);
  fixture.MutateBackingStrings();

  if (::objc3c::runtime::probe::ExpectTextEqual(
          fixture.snapshot.last_registered_module_name, "registered-module",
          "registered module stable copy", 3) != 0 ||
      ::objc3c::runtime::probe::ExpectTextEqual(
          fixture.snapshot.last_registered_translation_unit_identity_key,
          "registered-identity", "registered identity stable copy", 3) != 0 ||
      ::objc3c::runtime::probe::ExpectTextEqual(
          fixture.snapshot.last_rejected_module_name, "rejected-module",
          "rejected module stable copy", 3) != 0 ||
      ::objc3c::runtime::probe::ExpectTextEqual(
          fixture.snapshot.last_rejected_translation_unit_identity_key,
          "rejected-identity", "rejected identity stable copy", 3) != 0) {
    return 3;
  }
  return 0;
}

inline int VerifyPropertyEntryStabilizerCopiesRuntimeStrings() {
  PropertyEntryStabilizerFixture fixture;
  ::objc3c::runtime::probe::StabilizePropertyEntry(fixture.entry);
  fixture.MutateBackingStrings();

  if (::objc3c::runtime::probe::ExpectTextEqual(
          fixture.entry.snapshot.property_name, "count",
          "property entry name stable copy", 4) != 0 ||
      ::objc3c::runtime::probe::ExpectTextEqual(
          fixture.entry.snapshot.declaration_owner_identity, "Widget",
          "property entry owner stable copy", 4) != 0 ||
      ::objc3c::runtime::probe::ExpectTextEqual(
          fixture.entry.snapshot.ownership_lifetime_profile, "strong",
          "property entry lifetime stable copy", 4) != 0) {
    return 4;
  }
  return 0;
}

}  // namespace objc3c::runtime::probe::runtime_snapshot_stabilizers_support
