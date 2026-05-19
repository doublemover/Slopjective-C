#include "runtime/images/registration.h"

#include "runtime/classes/class_graph.h"
#include "runtime/dispatch/method_fast_path_seed.h"
#include "runtime/images/image_descriptor.h"
#include "runtime/images/registration_snapshots.h"
#include "runtime/images/registration_state_publish.h"
#include "runtime/images/registration_table_walk.h"
#include "runtime/metadata/runtime_ownership_contracts.h"
#include "runtime/metadata/runtime_registration_records.h"
#include "runtime/state/runtime_state_clear.h"
#include "runtime/state/runtime_state_records.h"

#include <cstdint>

namespace objc3c::runtime {

int RegisterImageUnlocked(
    RuntimeState &state, const objc3_runtime_image_descriptor *image,
    const objc3_runtime_registration_table *staged_registration_table,
    bool retain_bootstrap_record, bool mark_image_local_init_state) {
  // runtime-bootstrap-table-consumption anchor: duplicate identity rejection
  // and out-of-order rejection happen before live counters advance, while
  // successful staged-table consumption is the only path allowed to publish
  // bootstrap-visible image-walk state.
  (void)RuntimeImageDescriptorOwnershipModel();
  state.runtime_owner_split_explicit = RuntimeOwnerSplitContractIsReady();
  state.retired_route_path_allowed = RuntimeRetiredRoutePathsAreAllowed();
  if (!RuntimeImageDescriptorHasRequiredIdentity(image)) {
    MarkRejectedRegistrationUnlocked(
        state, image, OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR);
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  if (state.registration_order_by_identity_key.find(
          image->translation_unit_identity_key) !=
      state.registration_order_by_identity_key.end()) {
    MarkRejectedRegistrationUnlocked(
        state, image,
        OBJC3_RUNTIME_REGISTRATION_STATUS_DUPLICATE_TRANSLATION_UNIT_IDENTITY_KEY);
    return OBJC3_RUNTIME_REGISTRATION_STATUS_DUPLICATE_TRANSLATION_UNIT_IDENTITY_KEY;
  }

  if (image->registration_order_ordinal !=
      state.next_expected_registration_order_ordinal) {
    MarkRejectedRegistrationUnlocked(
        state, image,
        OBJC3_RUNTIME_REGISTRATION_STATUS_OUT_OF_ORDER_REGISTRATION);
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OUT_OF_ORDER_REGISTRATION;
  }

  std::uint64_t descriptor_total = RuntimeDescriptorTotal(image);
  if (staged_registration_table != nullptr) {
    state.registration_order_by_identity_key.reserve(
        state.registration_order_by_identity_key.size() + 1u);
    state.registered_image_metadata_by_identity_key.reserve(
        state.registered_image_metadata_by_identity_key.size() + 1u);
    RegisteredImageMetadata record;
    if (!TryWalkRegistrationTableUnlocked(state, staged_registration_table,
                                          image, record)) {
      ClearImageWalkSnapshotUnlocked(state);
      MarkRejectedRegistrationUnlocked(
          state, image,
          OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_REGISTRATION_ROOTS);
      return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_REGISTRATION_ROOTS;
    }
    descriptor_total = record.class_descriptor_count +
                       record.protocol_descriptor_count +
                       record.category_descriptor_count +
                       record.property_descriptor_count +
                       record.ivar_descriptor_count;
    if (retain_bootstrap_record) {
      RetainBootstrapRecordUnlocked(state, record);
    }
    state.registered_image_metadata_by_identity_key
        [record.translation_unit_identity_key] = record;
    ApplyImageWalkRecordUnlocked(
        state,
        state.registered_image_metadata_by_identity_key.at(
            record.translation_unit_identity_key));
    if (mark_image_local_init_state &&
        staged_registration_table->image_local_init_state != nullptr) {
      *staged_registration_table->image_local_init_state = 1;
    }
  } else {
    ClearImageWalkSnapshotUnlocked(state);
  }

  ++state.registered_image_count;
  state.registered_descriptor_total += descriptor_total;
  state.next_expected_registration_order_ordinal =
      image->registration_order_ordinal + 1;
  state.last_successful_registration_order_ordinal =
      image->registration_order_ordinal;
  state.last_registration_status = OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  state.last_malformed_class_graph_reason.clear();
  state.last_registered_module_name = image->module_name;
  state.last_registered_translation_unit_identity_key =
      image->translation_unit_identity_key;
  state.registration_order_by_identity_key.emplace(
      image->translation_unit_identity_key, image->registration_order_ordinal);
  RebuildRealizedClassGraphUnlocked(state);
  ClearRejectedRegistrationUnlocked(state);
  ClearMethodCacheStateUnlocked(state);
  SeedDispatchIntentFastPathCacheUnlocked(state);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

}  // namespace objc3c::runtime
