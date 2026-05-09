#include "runtime/reflection/property_entry_descriptor_snapshot_fields.h"

#include "runtime/metadata/runtime_emitted_records.h"
#include "runtime/metadata/runtime_realized_records.h"
#include "runtime/strings/borrowed_string.h"

namespace objc3c::runtime {

void PopulateRuntimePropertyEntryDescriptorSnapshotFields(
    const RealizedPropertyAccessor &accessor,
    const EmittedPropertyDescriptor &descriptor,
    objc3_runtime_property_entry_snapshot &snapshot) {
  snapshot.setter_available = descriptor.effective_setter_available ? 1 : 0;
  snapshot.has_runtime_getter = 1;
  snapshot.has_runtime_setter =
      !accessor.setter_owner_identity.empty() ? 1 : 0;
  snapshot.property_name = descriptor.property_name;
  snapshot.declaration_owner_identity =
      descriptor.declaration_owner_identity != nullptr
          ? descriptor.declaration_owner_identity
          : nullptr;
  snapshot.export_owner_identity =
      descriptor.export_owner_identity != nullptr
          ? descriptor.export_owner_identity
          : nullptr;
  snapshot.getter_selector =
      descriptor.getter_selector != nullptr ? descriptor.getter_selector
                                            : nullptr;
  snapshot.setter_selector =
      descriptor.setter_selector != nullptr ? descriptor.setter_selector
                                            : nullptr;
  snapshot.effective_getter_selector =
      descriptor.effective_getter_selector != nullptr
          ? descriptor.effective_getter_selector
          : nullptr;
  snapshot.effective_setter_selector =
      descriptor.effective_setter_selector != nullptr
          ? descriptor.effective_setter_selector
          : nullptr;
  snapshot.ivar_binding_symbol =
      descriptor.ivar_binding_symbol != nullptr ? descriptor.ivar_binding_symbol
                                                : nullptr;
  snapshot.synthesized_binding_symbol =
      descriptor.synthesized_binding_symbol != nullptr
          ? descriptor.synthesized_binding_symbol
          : nullptr;
  snapshot.ivar_layout_symbol =
      descriptor.ivar_layout_symbol != nullptr ? descriptor.ivar_layout_symbol
                                               : nullptr;
  snapshot.ivar_layout_replay_key =
      accessor.ivar_descriptor != nullptr &&
              accessor.ivar_descriptor->layout_replay_key != nullptr
          ? accessor.ivar_descriptor->layout_replay_key
          : descriptor.ivar_layout_replay_key;
  snapshot.property_attribute_profile =
      descriptor.property_attribute_profile != nullptr
          ? descriptor.property_attribute_profile
          : nullptr;
  snapshot.ownership_lifetime_profile =
      descriptor.ownership_lifetime_profile != nullptr
          ? descriptor.ownership_lifetime_profile
          : nullptr;
  snapshot.ownership_runtime_hook_profile =
      descriptor.ownership_runtime_hook_profile != nullptr
          ? descriptor.ownership_runtime_hook_profile
          : nullptr;
  snapshot.accessor_ownership_profile =
      descriptor.accessor_ownership_profile != nullptr
          ? descriptor.accessor_ownership_profile
          : nullptr;
  snapshot.getter_owner_identity =
      BorrowRuntimeCString(accessor.getter_owner_identity);
  snapshot.setter_owner_identity =
      BorrowRuntimeCString(accessor.setter_owner_identity);
}

}  // namespace objc3c::runtime
