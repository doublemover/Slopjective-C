#pragma once

#include "probe_state.h"
#include "support/runtime_snapshot_stabilizers.h"

namespace objc3c {
namespace runtime {
namespace probe {
namespace metaclass_graph_root_class {

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
  ::objc3c::runtime::probe::StabilizeRealizedEntry(
      fixture.root_entry, fixture.root_entry_storage.module_name,
      fixture.root_entry_storage.translation_unit_identity_key,
      fixture.root_entry_storage.class_name,
      fixture.root_entry_storage.class_owner_identity,
      fixture.root_entry_storage.metaclass_owner_identity,
      fixture.root_entry_storage.super_class_owner_identity,
      fixture.root_entry_storage.super_metaclass_owner_identity);
  ::objc3c::runtime::probe::StabilizeRealizedEntry(
      fixture.widget_entry, fixture.widget_entry_storage.module_name,
      fixture.widget_entry_storage.translation_unit_identity_key,
      fixture.widget_entry_storage.class_name,
      fixture.widget_entry_storage.class_owner_identity,
      fixture.widget_entry_storage.metaclass_owner_identity,
      fixture.widget_entry_storage.super_class_owner_identity,
      fixture.widget_entry_storage.super_metaclass_owner_identity);
}

}  // namespace metaclass_graph_root_class
}  // namespace probe
}  // namespace runtime
}  // namespace objc3c
