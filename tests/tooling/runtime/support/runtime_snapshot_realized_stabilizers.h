#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_SUPPORT_RUNTIME_SNAPSHOT_REALIZED_STABILIZERS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_SUPPORT_RUNTIME_SNAPSHOT_REALIZED_STABILIZERS_H_

#include "support/runtime_snapshot_stabilizer_common.h"

namespace objc3c::runtime::probe {

inline void StabilizeRealizedClassEntry(
    objc3_runtime_realized_class_entry_snapshot &snapshot,
    std::string &module_storage, std::string &identity_storage,
    std::string &class_storage, std::string &class_owner_storage,
    std::string &metaclass_owner_storage, std::string &super_class_storage,
    std::string &super_metaclass_storage, std::string &category_owner_storage,
    std::string &category_name_storage) {
  StabilizeNullableCString(snapshot.module_name, module_storage,
                           snapshot.module_name);
  StabilizeNullableCString(snapshot.translation_unit_identity_key,
                           identity_storage,
                           snapshot.translation_unit_identity_key);
  StabilizeNullableCString(snapshot.class_name, class_storage,
                           snapshot.class_name);
  StabilizeNullableCString(snapshot.class_owner_identity, class_owner_storage,
                           snapshot.class_owner_identity);
  StabilizeNullableCString(snapshot.metaclass_owner_identity,
                           metaclass_owner_storage,
                           snapshot.metaclass_owner_identity);
  StabilizeNullableCString(snapshot.super_class_owner_identity,
                           super_class_storage,
                           snapshot.super_class_owner_identity);
  StabilizeNullableCString(snapshot.super_metaclass_owner_identity,
                           super_metaclass_storage,
                           snapshot.super_metaclass_owner_identity);
  StabilizeNullableCString(snapshot.last_attached_category_owner_identity,
                           category_owner_storage,
                           snapshot.last_attached_category_owner_identity);
  StabilizeNullableCString(snapshot.last_attached_category_name,
                           category_name_storage,
                           snapshot.last_attached_category_name);
}

template <typename StableRealizedClassEntry>
inline void StabilizeRealizedClassEntry(StableRealizedClassEntry &entry) {
  StabilizeNullableCString(entry.snapshot.module_name, entry.module_name,
                           entry.snapshot.module_name);
  StabilizeNullableCString(entry.snapshot.translation_unit_identity_key,
                           entry.translation_unit_identity_key,
                           entry.snapshot.translation_unit_identity_key);
  StabilizeNullableCString(entry.snapshot.class_name, entry.class_name,
                           entry.snapshot.class_name);
  StabilizeNullableCString(entry.snapshot.class_owner_identity,
                           entry.class_owner_identity,
                           entry.snapshot.class_owner_identity);
  StabilizeNullableCString(entry.snapshot.metaclass_owner_identity,
                           entry.metaclass_owner_identity,
                           entry.snapshot.metaclass_owner_identity);
  StabilizeNullableCString(entry.snapshot.super_class_owner_identity,
                           entry.super_class_owner_identity,
                           entry.snapshot.super_class_owner_identity);
  StabilizeNullableCString(entry.snapshot.super_metaclass_owner_identity,
                           entry.super_metaclass_owner_identity,
                           entry.snapshot.super_metaclass_owner_identity);
  StabilizeNullableCString(entry.snapshot.last_attached_category_owner_identity,
                           entry.attached_category_owner_identity,
                           entry.snapshot.last_attached_category_owner_identity);
  StabilizeNullableCString(entry.snapshot.last_attached_category_name,
                           entry.attached_category_name,
                           entry.snapshot.last_attached_category_name);
}

inline void StabilizeRealizedClassGraph(
    objc3_runtime_realized_class_graph_state_snapshot &snapshot,
    std::string &class_storage) {
  StabilizeNullableCString(snapshot.last_allocated_class_name, class_storage,
                           snapshot.last_allocated_class_name);
}

inline void StabilizeRealizedEntry(
    objc3_runtime_realized_class_entry_snapshot &snapshot,
    std::string &module_storage, std::string &identity_storage,
    std::string &class_storage, std::string &class_owner_storage,
    std::string &metaclass_owner_storage,
    std::string &super_class_owner_storage,
    std::string &super_metaclass_owner_storage) {
  StabilizeNullableCString(snapshot.module_name, module_storage,
                           snapshot.module_name);
  StabilizeNullableCString(snapshot.translation_unit_identity_key,
                           identity_storage,
                           snapshot.translation_unit_identity_key);
  StabilizeNullableCString(snapshot.class_name, class_storage,
                           snapshot.class_name);
  StabilizeNullableCString(snapshot.class_owner_identity, class_owner_storage,
                           snapshot.class_owner_identity);
  StabilizeNullableCString(snapshot.metaclass_owner_identity,
                           metaclass_owner_storage,
                           snapshot.metaclass_owner_identity);
  StabilizeNullableCString(snapshot.super_class_owner_identity,
                           super_class_owner_storage,
                           snapshot.super_class_owner_identity);
  StabilizeNullableCString(snapshot.super_metaclass_owner_identity,
                           super_metaclass_owner_storage,
                           snapshot.super_metaclass_owner_identity);
}

inline void StabilizeRealizedEntry(
    objc3_runtime_realized_class_entry_snapshot &snapshot,
    std::string &module_storage, std::string &identity_storage,
    std::string &class_storage, std::string &class_owner_storage,
    std::string &metaclass_owner_storage,
    std::string &super_class_owner_storage,
    std::string &super_metaclass_owner_storage,
    std::string &category_owner_storage, std::string &category_name_storage) {
  StabilizeRealizedEntry(snapshot, module_storage, identity_storage,
                         class_storage, class_owner_storage,
                         metaclass_owner_storage, super_class_owner_storage,
                         super_metaclass_owner_storage);
  StabilizeNullableCString(snapshot.last_attached_category_owner_identity,
                           category_owner_storage,
                           snapshot.last_attached_category_owner_identity);
  StabilizeNullableCString(snapshot.last_attached_category_name,
                           category_name_storage,
                           snapshot.last_attached_category_name);
}

inline void StabilizeRealizedGraphState(
    objc3_runtime_realized_class_graph_state_snapshot &snapshot,
    std::string &class_storage, std::string &class_owner_storage,
    std::string &metaclass_owner_storage) {
  StabilizeNullableCString(snapshot.last_realized_class_name, class_storage,
                           snapshot.last_realized_class_name);
  StabilizeNullableCString(snapshot.last_realized_class_owner_identity,
                           class_owner_storage,
                           snapshot.last_realized_class_owner_identity);
  StabilizeNullableCString(snapshot.last_realized_metaclass_owner_identity,
                           metaclass_owner_storage,
                           snapshot.last_realized_metaclass_owner_identity);
}

inline void StabilizeRealizedGraphState(
    objc3_runtime_realized_class_graph_state_snapshot &snapshot,
    std::string &class_storage, std::string &owner_storage,
    std::string &metaclass_storage, std::string &category_owner_storage,
    std::string &category_name_storage, std::string &allocated_class_storage,
    std::string &lifecycle_failure_storage) {
  StabilizeRealizedGraphState(snapshot, class_storage, owner_storage,
                              metaclass_storage);
  StabilizeNullableCString(snapshot.last_attached_category_owner_identity,
                           category_owner_storage,
                           snapshot.last_attached_category_owner_identity);
  StabilizeNullableCString(snapshot.last_attached_category_name,
                           category_name_storage,
                           snapshot.last_attached_category_name);
  StabilizeNullableCString(snapshot.last_allocated_class_name,
                           allocated_class_storage,
                           snapshot.last_allocated_class_name);
  StabilizeNullableCString(snapshot.last_instance_lifecycle_failure_reason,
                           lifecycle_failure_storage,
                           snapshot.last_instance_lifecycle_failure_reason);
}


}  // namespace objc3c::runtime::probe

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_SUPPORT_RUNTIME_SNAPSHOT_REALIZED_STABILIZERS_H_
