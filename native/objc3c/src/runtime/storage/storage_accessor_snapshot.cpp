#include "runtime/objc3_runtime_bootstrap_internal.h"

extern "C" int objc3_runtime_copy_storage_accessor_implementation_snapshot_for_testing(
    objc3_runtime_storage_accessor_implementation_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  snapshot->property_registry_ready = 1;
  snapshot->runtime_accessor_dispatch_ready = 1;
  snapshot->runtime_layout_ready = 1;
  snapshot->reflection_query_ready = 1;
  snapshot->deterministic = 1;
  snapshot->property_registry_state_snapshot_symbol =
      "objc3_runtime_copy_property_registry_state_for_testing";
  snapshot->property_entry_snapshot_symbol =
      "objc3_runtime_copy_property_entry_for_testing";
  snapshot->current_property_read_symbol =
      "objc3_runtime_read_current_property_i32";
  snapshot->current_property_write_symbol =
      "objc3_runtime_write_current_property_i32";
  snapshot->current_property_exchange_symbol =
      "objc3_runtime_exchange_current_property_i32";
  snapshot->bind_current_property_context_symbol =
      "objc3_runtime_bind_current_property_context_for_testing";
  snapshot->clear_current_property_context_symbol =
      "objc3_runtime_clear_current_property_context_for_testing";
  snapshot->weak_current_property_load_symbol =
      "objc3_runtime_load_weak_current_property_i32";
  snapshot->weak_current_property_store_symbol =
      "objc3_runtime_store_weak_current_property_i32";
  snapshot->implementation_model =
      "runtime-registration-realizes-property-accessor-records-from-emitted-descriptors-and-ivar-layout-through-runtime-storage-owners";
  snapshot->reflection_model =
      "private-property-registry-and-entry-snapshots-publish-runtime-owned-accessor-layout-and-ownership-facts";
  snapshot->fail_closed_model =
      "missing-realized-layout-or-accessor-records-produce-no-reflection-hit-and-no-synthetic-storage-path";
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}
