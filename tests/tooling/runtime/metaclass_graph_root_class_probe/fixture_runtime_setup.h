#pragma once

#include "probe_state.h"
#include "support/runtime_snapshot_stabilizers.h"

namespace objc3c {
namespace runtime {
namespace probe {
namespace metaclass_graph_root_class {

inline void StabilizeMetaclassGraphRootClassEntry(
    objc3_runtime_realized_class_entry_snapshot &snapshot,
    RealizedEntryReportStorage &storage) {
  ::objc3c::runtime::probe::StabilizeRealizedEntry(
      snapshot, storage.module_name, storage.translation_unit_identity_key,
      storage.class_name, storage.class_owner_identity,
      storage.metaclass_owner_identity, storage.super_class_owner_identity,
      storage.super_metaclass_owner_identity);
  ::objc3c::runtime::probe::StabilizeNullableCString(
      snapshot.instance_isa_owner_identity, storage.instance_isa_owner_identity,
      snapshot.instance_isa_owner_identity);
  ::objc3c::runtime::probe::StabilizeNullableCString(
      snapshot.class_object_isa_owner_identity,
      storage.class_object_isa_owner_identity,
      snapshot.class_object_isa_owner_identity);
  ::objc3c::runtime::probe::StabilizeNullableCString(
      snapshot.metaclass_object_isa_owner_identity,
      storage.metaclass_object_isa_owner_identity,
      snapshot.metaclass_object_isa_owner_identity);
  ::objc3c::runtime::probe::StabilizeNullableCString(
      snapshot.root_class_owner_identity, storage.root_class_owner_identity,
      snapshot.root_class_owner_identity);
  ::objc3c::runtime::probe::StabilizeNullableCString(
      snapshot.root_metaclass_owner_identity,
      storage.root_metaclass_owner_identity,
      snapshot.root_metaclass_owner_identity);
}

inline void CaptureRuntimeBootstrapFixture(RuntimeBootstrapFixture &fixture) {
  (void)objc3_runtime_copy_registration_state_for_testing(
      &fixture.registration_state);
  (void)objc3_runtime_copy_realized_class_graph_state_for_testing(
      &fixture.graph_state);
  (void)objc3_runtime_copy_realized_class_entry_for_testing(
      kRootClassName, &fixture.root_entry);
  (void)objc3_runtime_copy_realized_class_entry_for_testing(
      kWidgetClassName, &fixture.widget_entry);
  ::objc3c::runtime::probe::StabilizeRegistrationState(
      fixture.registration_state, fixture.registration_state_storage.module_name,
      fixture.registration_state_storage.translation_unit_identity_key,
      fixture.registration_state_storage.rejected_module_name,
      fixture.registration_state_storage
          .rejected_translation_unit_identity_key);
  ::objc3c::runtime::probe::StabilizeRealizedGraphState(
      fixture.graph_state, fixture.graph_state_storage.class_name,
      fixture.graph_state_storage.class_owner_identity,
      fixture.graph_state_storage.metaclass_owner_identity);
  StabilizeMetaclassGraphRootClassEntry(fixture.root_entry,
                                        fixture.root_entry_storage);
  StabilizeMetaclassGraphRootClassEntry(fixture.widget_entry,
                                        fixture.widget_entry_storage);
}

}  // namespace metaclass_graph_root_class
}  // namespace probe
}  // namespace runtime
}  // namespace objc3c
