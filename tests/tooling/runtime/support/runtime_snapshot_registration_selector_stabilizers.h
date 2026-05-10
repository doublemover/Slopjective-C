#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_SUPPORT_RUNTIME_SNAPSHOT_REGISTRATION_SELECTOR_STABILIZERS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_SUPPORT_RUNTIME_SNAPSHOT_REGISTRATION_SELECTOR_STABILIZERS_H_

#include "support/runtime_snapshot_stabilizer_common.h"

namespace objc3c::runtime::probe {

inline void StabilizeRegistrationState(
    objc3_runtime_registration_state_snapshot &snapshot,
    std::string &module_storage, std::string &identity_storage) {
  StabilizeNullableCString(snapshot.last_registered_module_name, module_storage,
                           snapshot.last_registered_module_name);
  StabilizeNullableCString(snapshot.last_registered_translation_unit_identity_key,
                           identity_storage,
                           snapshot.last_registered_translation_unit_identity_key);
}

inline void StabilizeRegistrationState(
    objc3_runtime_registration_state_snapshot &snapshot,
    std::string &module_storage, std::string &identity_storage,
    std::string &rejected_module_storage,
    std::string &rejected_identity_storage) {
  StabilizeRegistrationState(snapshot, module_storage, identity_storage);
  StabilizeNullableCString(snapshot.last_rejected_module_name,
                           rejected_module_storage,
                           snapshot.last_rejected_module_name);
  StabilizeNullableCString(snapshot.last_rejected_translation_unit_identity_key,
                           rejected_identity_storage,
                           snapshot.last_rejected_translation_unit_identity_key);
}

inline void StabilizeResetReplayState(
    objc3_runtime_reset_replay_state_snapshot &snapshot,
    std::string &module_storage, std::string &identity_storage) {
  StabilizeNullableCString(snapshot.last_replayed_module_name, module_storage,
                           snapshot.last_replayed_module_name);
  StabilizeNullableCString(snapshot.last_replayed_translation_unit_identity_key,
                           identity_storage,
                           snapshot.last_replayed_translation_unit_identity_key);
}

inline void StabilizeSelectorEntry(
    objc3_runtime_selector_lookup_entry_snapshot &snapshot,
    std::string &selector_storage) {
  StabilizeNullableCString(snapshot.canonical_selector, selector_storage,
                           snapshot.canonical_selector);
}

inline void StabilizeSelectorTableState(
    objc3_runtime_selector_lookup_table_state_snapshot &snapshot,
    std::string &selector_storage) {
  StabilizeNullableCString(snapshot.last_materialized_selector,
                           selector_storage,
                           snapshot.last_materialized_selector);
}


}  // namespace objc3c::runtime::probe

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_SUPPORT_RUNTIME_SNAPSHOT_REGISTRATION_SELECTOR_STABILIZERS_H_
