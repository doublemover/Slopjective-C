int objc3_runtime_copy_storage_accessor_implementation_snapshot_for_testing(
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
      "runtime-registration-realizes-property-accessor-records-from-emitted-descriptors-and-ivar-layout-without-storage-rederivation";
  snapshot->reflection_model =
      "private-property-registry-and-entry-snapshots-publish-runtime-owned-accessor-layout-and-ownership-facts";
  snapshot->fail_closed_model =
      "missing-realized-layout-or-accessor-records-produce-no-reflection-hit-and-no-storage-fallback-synthesis";
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

int objc3_runtime_copy_object_model_query_state_for_testing(
    objc3_runtime_object_model_query_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  snapshot->realized_class_count = 0;
  snapshot->reflectable_property_count = 0;
  snapshot->attached_category_count = 0;
  snapshot->protocol_conformance_edge_count = 0;
  snapshot->method_cache_entry_count = 0;
  snapshot->last_class_query_found = 0;
  snapshot->last_property_query_found = 0;
  snapshot->last_property_query_inherited = 0;
  snapshot->last_protocol_query_class_found = 0;
  snapshot->last_protocol_query_protocol_found = 0;
  snapshot->last_protocol_query_conforms = 0;
  snapshot->last_queried_class_name = nullptr;
  snapshot->last_resolved_class_name = nullptr;
  snapshot->last_resolved_class_owner_identity = nullptr;
  snapshot->last_queried_property_name = nullptr;
  snapshot->last_resolved_property_class_name = nullptr;
  snapshot->last_resolved_property_owner_identity = nullptr;
  snapshot->last_queried_protocol_class_name = nullptr;
  snapshot->last_queried_protocol_name = nullptr;
  snapshot->last_matched_protocol_owner_identity = nullptr;
  snapshot->last_matched_attachment_owner_identity = nullptr;

  RuntimeState &state = ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  snapshot->realized_class_count =
      static_cast<std::uint64_t>(state.realized_class_nodes.size());
  snapshot->protocol_conformance_edge_count =
      state.realized_protocol_conformance_edge_count;
  snapshot->method_cache_entry_count =
      static_cast<std::uint64_t>(state.method_cache.size());
  snapshot->last_class_query_found = state.last_class_query_found ? 1 : 0;
  snapshot->last_property_query_found = state.last_property_query_found ? 1 : 0;
  snapshot->last_property_query_inherited =
      state.last_property_query_inherited ? 1 : 0;
  snapshot->last_protocol_query_class_found =
      state.last_protocol_query_class_found ? 1 : 0;
  snapshot->last_protocol_query_protocol_found =
      state.last_protocol_query_protocol_found ? 1 : 0;
  snapshot->last_protocol_query_conforms =
      state.last_protocol_query_conforms ? 1 : 0;
  for (const RealizedClassNode &node : state.realized_class_nodes) {
    snapshot->attached_category_count +=
        static_cast<std::uint64_t>(node.attached_category_records.size());
    snapshot->reflectable_property_count +=
        static_cast<std::uint64_t>(node.runtime_property_accessors.size());
  }
  snapshot->last_queried_class_name =
      BorrowRuntimeCString(state.last_queried_class_name);
  snapshot->last_resolved_class_name =
      BorrowRuntimeCString(state.last_resolved_class_query_name);
  snapshot->last_resolved_class_owner_identity =
      BorrowRuntimeCString(state.last_resolved_class_query_owner_identity);
  snapshot->last_queried_property_name =
      BorrowRuntimeCString(state.last_queried_property_name);
  snapshot->last_resolved_property_class_name =
      BorrowRuntimeCString(state.last_reflected_property_class_name);
  snapshot->last_resolved_property_owner_identity =
      BorrowRuntimeCString(state.last_reflected_property_owner_identity);
  snapshot->last_queried_protocol_class_name =
      BorrowRuntimeCString(state.last_protocol_conformance_class_name);
  snapshot->last_queried_protocol_name =
      BorrowRuntimeCString(state.last_protocol_conformance_protocol_name);
  snapshot->last_matched_protocol_owner_identity =
      BorrowRuntimeCString(state.last_protocol_conformance_owner_identity);
  snapshot->last_matched_attachment_owner_identity =
      BorrowRuntimeCString(state.last_protocol_conformance_attachment_owner_identity);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

int objc3_runtime_copy_memory_management_state_for_testing(
    objc3_runtime_memory_management_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  snapshot->live_runtime_instance_count = 0;
  snapshot->weak_target_count = 0;
  snapshot->weak_slot_ref_count = 0;
  snapshot->autoreleasepool_depth = RuntimeAutoreleasePoolDepth();
  snapshot->autoreleasepool_max_depth = RuntimeAutoreleasePoolMaxDepth();
  snapshot->queued_autorelease_value_count = CountQueuedAutoreleaseValues();
  snapshot->drained_autorelease_value_count =
      RuntimeAutoreleasePoolDrainedValueCount();
  snapshot->last_autoreleased_value = RuntimeLastAutoreleasedValue();
  snapshot->last_drained_autorelease_value =
      RuntimeLastDrainedAutoreleaseValue();

  RuntimeState &state = ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  snapshot->live_runtime_instance_count = state.live_runtime_instance_count;
  snapshot->weak_target_count =
      static_cast<std::uint64_t>(state.weak_slot_refs_by_target_receiver.size());
  for (const auto &entry : state.weak_slot_refs_by_target_receiver) {
    snapshot->weak_slot_ref_count +=
        static_cast<std::uint64_t>(entry.second.size());
  }
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

int objc3_runtime_replay_registered_images_for_testing(void) {
  RuntimeState &state = ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  return ReplayRegisteredImagesForTestingUnlocked(state, RegisterImageUnlocked);
}

// optional/key-path runtime-helper freeze anchor: the live Part 3
// runtime helper boundary intentionally remains minimal here. Optional sends
// execute through objc3_runtime_lookup_selector plus checked runtime dispatch
// status handling, while validated typed key-path sites currently contribute
// retained descriptor handles and descriptor sections without claiming full
// runtime evaluation yet.

// error-runtime/bridge-helper anchor: the current runnable Part 6
// slice now routes thrown-error storage, bridge normalization, and catch-kind
// matching through this same private bootstrap-internal helper cluster instead
// of treating raw function-local slots as the runtime boundary.
// live catch/bridge/runtime integration anchor: linked Part 6
// object-code probes now execute through these helpers and consume the
// snapshot state below to prove live store/load/bridge/catch traffic.
