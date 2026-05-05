  RuntimeState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  snapshot->cache_entry_count =
      static_cast<std::uint64_t>(state.method_cache.size());
  snapshot->fast_path_seed_count = state.fast_path_seed_count;
  snapshot->fast_path_hit_count = state.fast_path_hit_count;
  snapshot->live_dispatch_count = state.live_dispatch_count;
  snapshot->fallback_dispatch_count = state.fallback_dispatch_count;
  snapshot->last_selector_stable_id = state.last_dispatch_selector_stable_id;
  snapshot->last_normalized_receiver_identity =
      state.last_dispatch_normalized_receiver_identity;
  snapshot->last_resolved_parameter_count = state.last_dispatch_parameter_count;
  snapshot->last_property_base_identity =
      state.last_dispatch_property_base_identity;
  snapshot->last_property_slot_index = state.last_dispatch_property_slot_index;
  snapshot->last_dispatch_used_cache = state.last_dispatch_used_cache ? 1 : 0;
  snapshot->last_dispatch_used_fast_path =
      state.last_dispatch_used_fast_path ? 1 : 0;
  snapshot->last_dispatch_resolved_live_method =
      state.last_dispatch_resolved_live_method ? 1 : 0;
  snapshot->last_dispatch_fell_back = state.last_dispatch_fell_back ? 1 : 0;
  snapshot->last_effective_direct_dispatch =
      state.last_dispatch_effective_direct_dispatch ? 1 : 0;
  snapshot->last_used_builtin = state.last_dispatch_used_builtin ? 1 : 0;
  snapshot->last_selector = StableCString(state.last_dispatch_selector);
  snapshot->last_fast_path_reason = StableCString(state.last_fast_path_reason);
  snapshot->last_dispatch_path = StableCString(state.last_dispatch_path);
  snapshot->last_implementation_kind =
      StableCString(state.last_dispatch_implementation_kind);
  snapshot->last_property_name =
      StableCString(state.last_dispatch_property_name);
  snapshot->last_resolved_class_name =
      StableCString(state.last_resolved_class_name);
  snapshot->last_resolved_owner_identity =
      StableCString(state.last_resolved_owner_identity);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

int objc3_runtime_copy_method_cache_entry_for_testing(
    int receiver, const char *selector,
    objc3_runtime_method_cache_entry_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  snapshot->found = 0;
  snapshot->resolved = 0;
  snapshot->dispatch_family_is_class = 0;
  snapshot->normalized_receiver_identity = 0;
  snapshot->selector_stable_id = 0;
  snapshot->parameter_count = 0;
  snapshot->category_probe_count = 0;
  snapshot->protocol_probe_count = 0;
  snapshot->fast_path_seeded = 0;
  snapshot->effective_direct_dispatch = 0;
  snapshot->objc_final_declared = 0;
  snapshot->objc_sealed_declared = 0;
  snapshot->selector = nullptr;
  snapshot->fast_path_reason = nullptr;
  snapshot->resolved_class_name = nullptr;
  snapshot->resolved_owner_identity = nullptr;

  RuntimeState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  std::uint64_t base_identity = 0;
  std::uint64_t normalized_receiver_identity = 0;
  DispatchFamily family = DispatchFamily::Invalid;
  if (!DecodeReceiverIdentity(state, receiver, base_identity, family,
                              normalized_receiver_identity)) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  }
  if (selector == nullptr || selector[0] == '\0') {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  }
  const auto selector_it = state.selector_index_by_name.find(selector);
  if (selector_it == state.selector_index_by_name.end()) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  }
  const MethodCacheKey key{normalized_receiver_identity,
                           state.selector_slots[selector_it->second]
                               .handle.stable_id};
  const auto cache_it = state.method_cache.find(key);
  if (cache_it == state.method_cache.end()) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  }
  const MethodCacheEntry &entry = cache_it->second;
  snapshot->found = 1;
  snapshot->resolved = entry.resolved ? 1 : 0;
  snapshot->dispatch_family_is_class =
      entry.dispatch_family_is_class ? 1 : 0;
  snapshot->normalized_receiver_identity = entry.normalized_receiver_identity;
  snapshot->selector_stable_id = entry.selector_stable_id;
  snapshot->parameter_count = entry.parameter_count;
  snapshot->category_probe_count = entry.category_probe_count;
  snapshot->protocol_probe_count = entry.protocol_probe_count;
  snapshot->fast_path_seeded = entry.fast_path_seeded ? 1 : 0;
  snapshot->effective_direct_dispatch =
      entry.effective_direct_dispatch ? 1 : 0;
  snapshot->objc_final_declared = entry.objc_final_declared ? 1 : 0;
  snapshot->objc_sealed_declared = entry.objc_sealed_declared ? 1 : 0;
  snapshot->selector = StableCString(entry.selector_storage);
  snapshot->fast_path_reason = StableCString(entry.fast_path_reason);
  snapshot->resolved_class_name = StableCString(entry.class_name);
  snapshot->resolved_owner_identity = StableCString(entry.owner_identity);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

int objc3_runtime_copy_realized_class_graph_state_for_testing(
    objc3_runtime_realized_class_graph_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  RuntimeState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  snapshot->realized_class_count =
      static_cast<std::uint64_t>(state.realized_class_nodes.size());
  snapshot->root_class_count = state.realized_root_class_count;
  snapshot->metaclass_edge_count = state.realized_metaclass_edge_count;
  snapshot->receiver_class_binding_count = state.receiver_class_binding_count;
  snapshot->attached_category_count = state.realized_attached_category_count;
  snapshot->protocol_conformance_edge_count =
      state.realized_protocol_conformance_edge_count;
  snapshot->live_instance_count = state.live_runtime_instance_count;
  snapshot->last_allocated_receiver_identity =
      state.last_allocated_runtime_instance_receiver;
  snapshot->last_allocated_base_identity =
      state.last_allocated_runtime_instance_base_identity;
  snapshot->last_allocated_instance_size_bytes =
      state.last_allocated_runtime_instance_size_bytes;
  snapshot->last_realized_class_name =
      StableCString(state.last_realized_class_name);
  snapshot->last_realized_class_owner_identity =
      StableCString(state.last_realized_class_owner_identity);
  snapshot->last_realized_metaclass_owner_identity =
      StableCString(state.last_realized_metaclass_owner_identity);
  snapshot->last_attached_category_owner_identity =
      StableCString(state.last_attached_category_owner_identity);
  snapshot->last_attached_category_name =
      StableCString(state.last_attached_category_name);
  snapshot->last_allocated_class_name =
      StableCString(state.last_allocated_runtime_instance_class_name);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

int objc3_runtime_copy_realized_class_entry_for_testing(
    const char *class_name, objc3_runtime_realized_class_entry_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  snapshot->found = 0;
  snapshot->base_identity = 0;
  snapshot->registration_order_ordinal = 0;
  snapshot->is_root_class = 0;
  snapshot->implementation_backed = 0;
  snapshot->attached_category_count = 0;
  snapshot->direct_protocol_count = 0;
  snapshot->attached_protocol_count = 0;
  snapshot->runtime_property_accessor_count = 0;
  snapshot->runtime_instance_size_bytes = 0;
  snapshot->module_name = nullptr;
  snapshot->translation_unit_identity_key = nullptr;
  snapshot->class_name = nullptr;
  snapshot->class_owner_identity = nullptr;
  snapshot->metaclass_owner_identity = nullptr;
  snapshot->super_class_owner_identity = nullptr;
  snapshot->super_metaclass_owner_identity = nullptr;
  snapshot->last_attached_category_owner_identity = nullptr;
  snapshot->last_attached_category_name = nullptr;

  if (class_name == nullptr || class_name[0] == '\0') {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  }

  RuntimeState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  state.last_queried_class_name = class_name != nullptr ? class_name : "";
  state.last_resolved_class_query_name.clear();
  state.last_resolved_class_query_owner_identity.clear();
  state.last_class_query_found = false;
  const auto found = state.realized_class_node_indices_by_name.find(class_name);
  if (found == state.realized_class_node_indices_by_name.end() ||
      found->second.empty()) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  }
  const std::size_t node_index = found->second.front();
  if (node_index >= state.realized_class_nodes.size()) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  }
  const RealizedClassNode &node = state.realized_class_nodes[node_index];
  state.last_class_query_found = true;
  state.last_resolved_class_query_name = node.class_name;
  state.last_resolved_class_query_owner_identity = node.class_owner_identity;
  snapshot->found = 1;
  snapshot->base_identity = node.base_identity;
  snapshot->registration_order_ordinal = node.registration_order_ordinal;
  snapshot->is_root_class = node.is_root_class ? 1 : 0;
  snapshot->implementation_backed = node.implementation_backed ? 1 : 0;
  snapshot->attached_category_count =
      static_cast<std::uint64_t>(node.attached_category_records.size());
  snapshot->runtime_property_accessor_count =
      static_cast<std::uint64_t>(node.runtime_property_accessors.size());
  snapshot->runtime_instance_size_bytes =
      static_cast<std::uint64_t>(node.runtime_instance_size_bytes);
  snapshot->direct_protocol_count =
      (node.bundle != nullptr &&
       node.bundle->class_record.adopted_protocol_refs != nullptr)
          ? node.bundle->class_record.adopted_protocol_refs->count
          : 0;
  snapshot->module_name = StableCString(node.module_name);
  snapshot->translation_unit_identity_key =
      StableCString(node.translation_unit_identity_key);
  snapshot->class_name = StableCString(node.class_name);
  snapshot->class_owner_identity = StableCString(node.class_owner_identity);
  snapshot->metaclass_owner_identity = StableCString(node.metaclass_owner_identity);
  snapshot->super_class_owner_identity =
      StableCString(node.super_class_owner_identity);
  snapshot->super_metaclass_owner_identity =
      StableCString(node.super_metaclass_owner_identity);
  std::uint64_t attached_protocol_count = 0;
  for (const EmittedCategoryRecord *category_record : node.attached_category_records) {
    if (category_record != nullptr && category_record->adopted_protocol_refs != nullptr) {
      attached_protocol_count += category_record->adopted_protocol_refs->count;
    }
  }
  snapshot->attached_protocol_count = attached_protocol_count;
  if (!node.attached_category_records.empty()) {
    const EmittedCategoryRecord *last_category =
        node.attached_category_records.back();
    snapshot->last_attached_category_owner_identity =
        last_category->category_owner_identity != nullptr
            ? last_category->category_owner_identity
            : nullptr;
    snapshot->last_attached_category_name =
        last_category->category_name != nullptr ? last_category->category_name
                                                : nullptr;
  }
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

int objc3_runtime_copy_property_registry_state_for_testing(
    objc3_runtime_property_registry_state_snapshot *snapshot) {
  // property-metadata-reflection anchor: the private registry-state
  // snapshot is the canonical diagnostic/testing surface for aggregate
  // reflectable property metadata counts and last-query evidence.
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  snapshot->layout_ready_class_count = 0;
  snapshot->reflectable_property_count = 0;
  snapshot->writable_property_count = 0;
  snapshot->slot_backed_property_count = 0;
  snapshot->property_lookup_cache_entry_count = 0;
  snapshot->property_lookup_cache_hit_count = 0;
  snapshot->property_lookup_cache_miss_count = 0;
  snapshot->last_query_found = 0;
  snapshot->last_query_inherited = 0;
  snapshot->last_query_used_cache = 0;
  snapshot->last_queried_class_name = nullptr;
  snapshot->last_queried_property_name = nullptr;
  snapshot->last_resolved_class_name = nullptr;
  snapshot->last_resolved_owner_identity = nullptr;

  RuntimeState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  for (const RealizedClassNode &node : state.realized_class_nodes) {
    if (!node.runtime_layout_ready) {
      continue;
    }
    snapshot->layout_ready_class_count += 1;
    snapshot->reflectable_property_count +=
        static_cast<std::uint64_t>(node.runtime_property_accessors.size());
    for (const RealizedPropertyAccessor &accessor : node.runtime_property_accessors) {
      snapshot->writable_property_count +=
          !accessor.setter_owner_identity.empty() ? 1u : 0u;
      snapshot->slot_backed_property_count +=
          accessor.ivar_descriptor != nullptr ? 1u : 0u;
    }
  }
  snapshot->property_lookup_cache_entry_count =
      static_cast<std::uint64_t>(state.property_lookup_cache.size());
  snapshot->property_lookup_cache_hit_count = state.property_lookup_cache_hit_count;
  snapshot->property_lookup_cache_miss_count =
      state.property_lookup_cache_miss_count;
  snapshot->last_query_found = state.last_property_query_found ? 1 : 0;
  snapshot->last_query_inherited = state.last_property_query_inherited ? 1 : 0;
  snapshot->last_query_used_cache =
      state.last_property_query_used_cache ? 1 : 0;
  snapshot->last_queried_class_name =
      StableCString(state.last_queried_property_class_name);
  snapshot->last_queried_property_name =
      StableCString(state.last_queried_property_name);
  snapshot->last_resolved_class_name =
      StableCString(state.last_reflected_property_class_name);
  snapshot->last_resolved_owner_identity =
      StableCString(state.last_reflected_property_owner_identity);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

int objc3_runtime_copy_property_entry_for_testing(
    const char *class_name, const char *property_name,
    objc3_runtime_property_entry_snapshot *snapshot) {
  // property-metadata-reflection anchor: the private per-property
  // snapshot exposes runtime-owned accessor/layout facts by class/property name
  // without widening the public ABI or rederiving metadata from source.
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  snapshot->found = 0;
  snapshot->inherited = 0;
  snapshot->setter_available = 0;
  snapshot->has_runtime_getter = 0;
  snapshot->has_runtime_setter = 0;
  snapshot->base_identity = 0;
  snapshot->slot_index = 0;
  snapshot->offset_bytes = 0;
  snapshot->size_bytes = 0;
  snapshot->alignment_bytes = 0;
  snapshot->padding_bytes = 0;
  snapshot->inherited_slot_count = 0;
  snapshot->inherited_size_bytes = 0;
  snapshot->owner_size_bytes = 0;
  snapshot->init_order_index = 0;
  snapshot->destroy_order_index = 0;
  snapshot->layout_valid = 0;
  snapshot->instance_size_bytes = 0;
  snapshot->queried_class_name = nullptr;
  snapshot->resolved_class_name = nullptr;
  snapshot->property_name = nullptr;
  snapshot->declaration_owner_identity = nullptr;
  snapshot->export_owner_identity = nullptr;
  snapshot->getter_selector = nullptr;
  snapshot->setter_selector = nullptr;
  snapshot->effective_getter_selector = nullptr;
  snapshot->effective_setter_selector = nullptr;
  snapshot->ivar_binding_symbol = nullptr;
  snapshot->synthesized_binding_symbol = nullptr;
  snapshot->ivar_layout_symbol = nullptr;
  snapshot->ivar_layout_replay_key = nullptr;
  snapshot->property_attribute_profile = nullptr;
  snapshot->ownership_lifetime_profile = nullptr;
  snapshot->ownership_runtime_hook_profile = nullptr;
  snapshot->accessor_ownership_profile = nullptr;
  snapshot->getter_owner_identity = nullptr;
  snapshot->setter_owner_identity = nullptr;

  RuntimeState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  state.last_queried_property_class_name =
      class_name != nullptr ? class_name : "";
  state.last_queried_property_name =
      property_name != nullptr ? property_name : "";
  state.last_reflected_property_class_name.clear();
  state.last_reflected_property_owner_identity.clear();
  state.last_property_query_found = false;
  state.last_property_query_inherited = false;

  snapshot->queried_class_name =
      StableCString(state.last_queried_property_class_name);
  if (class_name == nullptr || class_name[0] == '\0' ||
      property_name == nullptr || property_name[0] == '\0') {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  }

  const auto found = state.realized_class_node_indices_by_name.find(class_name);
  if (found == state.realized_class_node_indices_by_name.end() ||
      found->second.empty()) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  }
  const std::size_t node_index = found->second.front();
  if (node_index >= state.realized_class_nodes.size()) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  }

  const RealizedClassNode &start_node = state.realized_class_nodes[node_index];
  const RealizedClassNode *resolved_node = nullptr;
  bool inherited = false;
  bool used_cache = false;
  const RealizedPropertyAccessor *accessor =
      FindRuntimePropertyAccessorByNameUnlocked(
          state, start_node, property_name, resolved_node, inherited,
          used_cache);
  state.last_property_query_used_cache = used_cache;
  if (accessor == nullptr || resolved_node == nullptr ||
      accessor->property_descriptor == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  }

  state.last_property_query_found = true;
  state.last_property_query_inherited = inherited;
  state.last_reflected_property_class_name = resolved_node->class_name;
  state.last_reflected_property_owner_identity =
      accessor->property_descriptor->declaration_owner_identity != nullptr
          ? accessor->property_descriptor->declaration_owner_identity
          : "";

  const EmittedPropertyDescriptor &descriptor = *accessor->property_descriptor;
  snapshot->found = 1;
  snapshot->inherited = inherited ? 1 : 0;
  snapshot->setter_available = descriptor.effective_setter_available ? 1 : 0;
  snapshot->has_runtime_getter = 1;
  snapshot->has_runtime_setter =
      !accessor->setter_owner_identity.empty() ? 1 : 0;
  snapshot->base_identity = resolved_node->base_identity;
  snapshot->slot_index =
      accessor->ivar_descriptor != nullptr
          ? accessor->ivar_descriptor->slot_index
          : descriptor.ivar_layout_slot_index;
  snapshot->offset_bytes = EffectiveIvarOffset(*accessor);
  snapshot->size_bytes = EffectiveIvarSize(*accessor);
  snapshot->alignment_bytes =
      accessor->ivar_descriptor != nullptr
          ? accessor->ivar_descriptor->alignment_bytes
          : descriptor.ivar_layout_alignment_bytes;
  snapshot->padding_bytes =
      accessor->ivar_descriptor != nullptr
          ? accessor->ivar_descriptor->padding_bytes
          : descriptor.ivar_layout_padding_bytes;
  snapshot->inherited_slot_count =
      accessor->ivar_descriptor != nullptr
          ? accessor->ivar_descriptor->inherited_slot_count
          : descriptor.ivar_layout_inherited_slot_count;
  snapshot->inherited_size_bytes =
      accessor->ivar_descriptor != nullptr
          ? accessor->ivar_descriptor->inherited_size_bytes
          : descriptor.ivar_layout_inherited_size_bytes;
  snapshot->owner_size_bytes =
      accessor->ivar_descriptor != nullptr
          ? accessor->ivar_descriptor->owner_size_bytes
          : descriptor.ivar_layout_owner_size_bytes;
  snapshot->init_order_index =
      accessor->ivar_descriptor != nullptr
          ? accessor->ivar_descriptor->init_order_index
          : descriptor.ivar_init_order_index;
  snapshot->destroy_order_index =
      accessor->ivar_descriptor != nullptr
          ? accessor->ivar_descriptor->destroy_order_index
          : descriptor.ivar_destroy_order_index;
  snapshot->layout_valid =
      accessor->ivar_descriptor != nullptr
          ? (accessor->ivar_descriptor->layout_valid ? 1 : 0)
          : (descriptor.ivar_layout_valid ? 1 : 0);
  snapshot->instance_size_bytes =
      static_cast<std::uint64_t>(resolved_node->runtime_instance_size_bytes);
  snapshot->resolved_class_name =
      StableCString(state.last_reflected_property_class_name);
  snapshot->property_name = descriptor.property_name;
  snapshot->declaration_owner_identity =
      descriptor.declaration_owner_identity != nullptr
          ? descriptor.declaration_owner_identity
          : nullptr;
  snapshot->export_owner_identity =
      descriptor.export_owner_identity != nullptr
          ? descriptor.export_owner_identity
          : nullptr;
  snapshot->getter_selector =
      descriptor.getter_selector != nullptr ? descriptor.getter_selector : nullptr;
  snapshot->setter_selector =
      descriptor.setter_selector != nullptr ? descriptor.setter_selector : nullptr;
  snapshot->effective_getter_selector =
      descriptor.effective_getter_selector != nullptr
          ? descriptor.effective_getter_selector
          : nullptr;
  snapshot->effective_setter_selector =
      descriptor.effective_setter_selector != nullptr
          ? descriptor.effective_setter_selector
          : nullptr;
  snapshot->ivar_binding_symbol =
      descriptor.ivar_binding_symbol != nullptr ? descriptor.ivar_binding_symbol
                                                : nullptr;
  snapshot->synthesized_binding_symbol =
      descriptor.synthesized_binding_symbol != nullptr
          ? descriptor.synthesized_binding_symbol
          : nullptr;
  snapshot->ivar_layout_symbol =
      descriptor.ivar_layout_symbol != nullptr ? descriptor.ivar_layout_symbol
                                               : nullptr;
  snapshot->ivar_layout_replay_key =
      accessor->ivar_descriptor != nullptr &&
              accessor->ivar_descriptor->layout_replay_key != nullptr
          ? accessor->ivar_descriptor->layout_replay_key
          : descriptor.ivar_layout_replay_key;
  snapshot->property_attribute_profile =
      descriptor.property_attribute_profile != nullptr
          ? descriptor.property_attribute_profile
          : nullptr;
  snapshot->ownership_lifetime_profile =
      descriptor.ownership_lifetime_profile != nullptr
          ? descriptor.ownership_lifetime_profile
          : nullptr;
  snapshot->ownership_runtime_hook_profile =
      descriptor.ownership_runtime_hook_profile != nullptr
          ? descriptor.ownership_runtime_hook_profile
          : nullptr;
  snapshot->accessor_ownership_profile =
      descriptor.accessor_ownership_profile != nullptr
          ? descriptor.accessor_ownership_profile
          : nullptr;
  snapshot->getter_owner_identity =
      StableCString(accessor->getter_owner_identity);
  snapshot->setter_owner_identity =
      StableCString(accessor->setter_owner_identity);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

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

int objc3_runtime_copy_protocol_conformance_query_for_testing(
    const char *class_name, const char *protocol_name,
    objc3_runtime_protocol_conformance_query_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  snapshot->class_found = 0;
  snapshot->protocol_found = 0;
  snapshot->conforms = 0;
  snapshot->visited_protocol_count = 0;
  snapshot->attached_category_count = 0;
  snapshot->class_name = nullptr;
  snapshot->protocol_name = nullptr;
  snapshot->matched_protocol_owner_identity = nullptr;
  snapshot->matched_attachment_owner_identity = nullptr;

  if (class_name == nullptr || class_name[0] == '\0' ||
      protocol_name == nullptr || protocol_name[0] == '\0') {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  }

  RuntimeState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  state.last_protocol_conformance_class_name = class_name;
  state.last_protocol_conformance_protocol_name = protocol_name;
  state.last_protocol_conformance_owner_identity.clear();
  state.last_protocol_conformance_attachment_owner_identity.clear();
  state.last_protocol_query_class_found = false;
  state.last_protocol_query_protocol_found = false;
  state.last_protocol_query_conforms = false;
  snapshot->class_name = StableCString(state.last_protocol_conformance_class_name);
  snapshot->protocol_name =
      StableCString(state.last_protocol_conformance_protocol_name);
  snapshot->protocol_found = ProtocolExistsByNameUnlocked(state, protocol_name) ? 1 : 0;
  state.last_protocol_query_protocol_found = snapshot->protocol_found != 0;

  const auto found = state.realized_class_node_indices_by_name.find(class_name);
  if (found == state.realized_class_node_indices_by_name.end() ||
      found->second.empty()) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  }
  const std::size_t node_index = found->second.front();
  if (node_index >= state.realized_class_nodes.size()) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  }

  const RealizedClassNode &node = state.realized_class_nodes[node_index];
  snapshot->class_found = 1;
  state.last_protocol_query_class_found = true;
  snapshot->attached_category_count =
      static_cast<std::uint64_t>(node.attached_category_records.size());

  std::string matched_protocol_owner_identity;
  std::string matched_attachment_owner_identity;
  if (QueryRealizedClassProtocolConformanceUnlocked(
          state, &node, protocol_name, snapshot->visited_protocol_count,
          matched_protocol_owner_identity, matched_attachment_owner_identity)) {
    snapshot->conforms = 1;
    state.last_protocol_query_conforms = true;
    state.last_protocol_conformance_owner_identity = matched_protocol_owner_identity;
    state.last_protocol_conformance_attachment_owner_identity =
        matched_attachment_owner_identity;
    snapshot->matched_protocol_owner_identity =
        StableCString(state.last_protocol_conformance_owner_identity);
    snapshot->matched_attachment_owner_identity =
        StableCString(state.last_protocol_conformance_attachment_owner_identity);
  }
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

  RuntimeState &state = State();
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
      StableCString(state.last_queried_class_name);
  snapshot->last_resolved_class_name =
      StableCString(state.last_resolved_class_query_name);
  snapshot->last_resolved_class_owner_identity =
      StableCString(state.last_resolved_class_query_owner_identity);
  snapshot->last_queried_property_name =
      StableCString(state.last_queried_property_name);
  snapshot->last_resolved_property_class_name =
      StableCString(state.last_reflected_property_class_name);
  snapshot->last_resolved_property_owner_identity =
      StableCString(state.last_reflected_property_owner_identity);
  snapshot->last_queried_protocol_class_name =
      StableCString(state.last_protocol_conformance_class_name);
  snapshot->last_queried_protocol_name =
      StableCString(state.last_protocol_conformance_protocol_name);
  snapshot->last_matched_protocol_owner_identity =
      StableCString(state.last_protocol_conformance_owner_identity);
  snapshot->last_matched_attachment_owner_identity =
      StableCString(state.last_protocol_conformance_attachment_owner_identity);
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
  snapshot->autoreleasepool_depth =
      static_cast<std::uint64_t>(g_runtime_autoreleasepool_frames.size());
  snapshot->autoreleasepool_max_depth = g_runtime_autoreleasepool_max_depth;
  snapshot->queued_autorelease_value_count = CountQueuedAutoreleaseValues();
  snapshot->drained_autorelease_value_count =
      g_runtime_autoreleasepool_drained_value_count;
  snapshot->last_autoreleased_value = g_runtime_last_autoreleased_value;
  snapshot->last_drained_autorelease_value =
      g_runtime_last_drained_autorelease_value;

  RuntimeState &state = State();
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

int objc3_runtime_copy_arc_debug_state_for_testing(
    objc3_runtime_arc_debug_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  snapshot->retain_call_count = g_runtime_arc_debug_retain_call_count;
  snapshot->release_call_count = g_runtime_arc_debug_release_call_count;
  snapshot->autorelease_call_count = g_runtime_arc_debug_autorelease_call_count;
  snapshot->autoreleasepool_push_count =
      g_runtime_arc_debug_autoreleasepool_push_count;
  snapshot->autoreleasepool_pop_count =
      g_runtime_arc_debug_autoreleasepool_pop_count;
  snapshot->current_property_read_count =
      g_runtime_arc_debug_current_property_read_count;
  snapshot->current_property_write_count =
      g_runtime_arc_debug_current_property_write_count;
  snapshot->current_property_exchange_count =
      g_runtime_arc_debug_current_property_exchange_count;
  snapshot->weak_current_property_load_count =
      g_runtime_arc_debug_weak_current_property_load_count;
  snapshot->weak_current_property_store_count =
      g_runtime_arc_debug_weak_current_property_store_count;
  snapshot->last_retain_value = g_runtime_arc_debug_last_retain_value;
  snapshot->last_release_value = g_runtime_arc_debug_last_release_value;
  snapshot->last_autorelease_value = g_runtime_arc_debug_last_autorelease_value;
  snapshot->last_property_read_value =
      g_runtime_arc_debug_last_property_read_value;
  snapshot->last_property_written_value =
      g_runtime_arc_debug_last_property_written_value;
  snapshot->last_property_exchange_previous_value =
      g_runtime_arc_debug_last_property_exchange_previous_value;
  snapshot->last_property_exchange_new_value =
      g_runtime_arc_debug_last_property_exchange_new_value;
  snapshot->last_weak_loaded_value = g_runtime_arc_debug_last_weak_loaded_value;
  snapshot->last_weak_stored_value = g_runtime_arc_debug_last_weak_stored_value;
  snapshot->last_property_receiver = g_runtime_arc_debug_last_property_receiver;
  snapshot->last_property_name =
      g_runtime_arc_debug_last_property_name.empty()
          ? nullptr
          : g_runtime_arc_debug_last_property_name.c_str();
  snapshot->last_property_owner_identity =
      g_runtime_arc_debug_last_property_owner_identity.empty()
          ? nullptr
          : g_runtime_arc_debug_last_property_owner_identity.c_str();
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

int objc3_runtime_copy_block_arc_runtime_abi_snapshot_for_testing(
    objc3_runtime_block_arc_runtime_abi_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  snapshot->private_runtime_abi_ready = 1;
  snapshot->public_runtime_header_unchanged = 1;
  snapshot->deterministic = 1;
  snapshot->live_runtime_block_handle_count = 0;
  snapshot->block_promote_call_count = g_runtime_block_promote_call_count;
  snapshot->block_invoke_call_count = g_runtime_block_invoke_call_count;
  snapshot->retain_call_count = g_runtime_arc_debug_retain_call_count;
  snapshot->release_call_count = g_runtime_arc_debug_release_call_count;
  snapshot->autorelease_call_count = g_runtime_arc_debug_autorelease_call_count;
  snapshot->autoreleasepool_push_count =
      g_runtime_arc_debug_autoreleasepool_push_count;
  snapshot->autoreleasepool_pop_count =
      g_runtime_arc_debug_autoreleasepool_pop_count;
  snapshot->current_property_read_count =
      g_runtime_arc_debug_current_property_read_count;
  snapshot->current_property_write_count =
      g_runtime_arc_debug_current_property_write_count;
  snapshot->current_property_exchange_count =
      g_runtime_arc_debug_current_property_exchange_count;
  snapshot->weak_current_property_load_count =
      g_runtime_arc_debug_weak_current_property_load_count;
  snapshot->weak_current_property_store_count =
      g_runtime_arc_debug_weak_current_property_store_count;
  snapshot->last_promoted_block_handle = g_runtime_last_promoted_block_handle;
  snapshot->last_promote_has_pointer_capture_storage =
      g_runtime_last_promote_has_pointer_capture_storage;
  snapshot->last_invoked_block_handle = g_runtime_last_invoked_block_handle;
  snapshot->last_block_invoke_result = g_runtime_last_block_invoke_result;
  snapshot->last_retain_value = g_runtime_arc_debug_last_retain_value;
  snapshot->last_release_value = g_runtime_arc_debug_last_release_value;
  snapshot->last_autorelease_value = g_runtime_arc_debug_last_autorelease_value;
  snapshot->block_promote_symbol = "objc3_runtime_promote_block_i32";
  snapshot->block_invoke_symbol = "objc3_runtime_invoke_block_i32";
  snapshot->retain_symbol = "objc3_runtime_retain_i32";
  snapshot->release_symbol = "objc3_runtime_release_i32";
  snapshot->autorelease_symbol = "objc3_runtime_autorelease_i32";
  snapshot->autoreleasepool_push_symbol =
      "objc3_runtime_push_autoreleasepool_scope";
  snapshot->autoreleasepool_pop_symbol =
      "objc3_runtime_pop_autoreleasepool_scope";
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
  snapshot->arc_debug_state_snapshot_symbol =
      "objc3_runtime_copy_arc_debug_state_for_testing";
  snapshot->runtime_abi_boundary_model =
      "private-block-and-arc-helper-entrypoints-plus-testing-snapshots-define-the-live-runtime-abi-without-widening-the-public-runtime-header";
  snapshot->block_runtime_model =
      "promote-invoke-and-handle-lifetime-for-supported-block-records-stay-on-bootstrap-internal-runtime-entrypoints";
  snapshot->arc_runtime_model =
      "retain-release-autorelease-autoreleasepool-and-current-property-weak-helper-traffic-stays-on-bootstrap-internal-runtime-entrypoints";
  snapshot->fail_closed_model =
      "public-runtime-header-remains-registration-lookup-dispatch-only-until-deliberate-runtime-abi-widening";

  RuntimeState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  snapshot->live_runtime_block_handle_count =
      static_cast<std::uint64_t>(state.runtime_blocks_by_handle.size());
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

int objc3_runtime_copy_error_bridge_state_for_testing(
    objc3_runtime_error_bridge_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  snapshot->store_call_count = g_runtime_error_bridge_store_call_count;
  snapshot->load_call_count = g_runtime_error_bridge_load_call_count;
  snapshot->status_bridge_call_count =
      g_runtime_error_bridge_status_bridge_call_count;
  snapshot->nserror_bridge_call_count =
      g_runtime_error_bridge_nserror_bridge_call_count;
  snapshot->catch_match_call_count =
      g_runtime_error_bridge_catch_match_call_count;
  snapshot->last_stored_error_value =
      g_runtime_error_bridge_last_stored_error_value;
  snapshot->last_loaded_error_value =
      g_runtime_error_bridge_last_loaded_error_value;
  snapshot->last_status_bridge_status_value =
      g_runtime_error_bridge_last_status_bridge_status_value;
  snapshot->last_status_bridge_error_value =
      g_runtime_error_bridge_last_status_bridge_error_value;
  snapshot->last_nserror_bridge_error_value =
      g_runtime_error_bridge_last_nserror_bridge_error_value;
  snapshot->last_catch_match_error_value =
      g_runtime_error_bridge_last_catch_match_error_value;
  snapshot->last_catch_match_kind = g_runtime_error_bridge_last_catch_match_kind;
  snapshot->last_catch_match_is_catch_all =
      g_runtime_error_bridge_last_catch_match_is_catch_all;
  snapshot->last_catch_match_result =
      g_runtime_error_bridge_last_catch_match_result;
  snapshot->last_catch_kind_name =
      g_runtime_error_bridge_last_catch_kind_name.empty()
          ? nullptr
          : g_runtime_error_bridge_last_catch_kind_name.c_str();
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

int objc3_runtime_copy_async_continuation_state_for_testing(
    objc3_runtime_async_continuation_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  snapshot->allocation_call_count =
      g_runtime_async_continuation_allocation_call_count;
  snapshot->handoff_call_count =
      g_runtime_async_continuation_handoff_call_count;
  snapshot->resume_call_count = g_runtime_async_continuation_resume_call_count;
  snapshot->live_continuation_handle_count =
      g_runtime_async_live_continuation_handles.size();
  snapshot->last_allocated_continuation_handle =
      g_runtime_async_continuation_last_allocated_handle;
  snapshot->last_allocated_resume_entry_tag =
      g_runtime_async_continuation_last_allocated_resume_entry_tag;
  snapshot->last_allocated_executor_tag =
      g_runtime_async_continuation_last_allocated_executor_tag;
  snapshot->last_handoff_continuation_handle =
      g_runtime_async_continuation_last_handoff_handle;
  snapshot->last_handoff_executor_tag =
      g_runtime_async_continuation_last_handoff_executor_tag;
  snapshot->last_resume_continuation_handle =
      g_runtime_async_continuation_last_resume_handle;
  snapshot->last_resume_result_value =
      g_runtime_async_continuation_last_resume_result_value;
  snapshot->last_resume_return_value =
      g_runtime_async_continuation_last_resume_return_value;
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

int objc3_runtime_copy_task_runtime_state_for_testing(
    objc3_runtime_task_runtime_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  snapshot->spawn_call_count = g_runtime_task_spawn_call_count;
  snapshot->scope_call_count = g_runtime_task_scope_call_count;
  snapshot->add_task_call_count = g_runtime_task_add_task_call_count;
  snapshot->wait_next_call_count = g_runtime_task_wait_next_call_count;
  snapshot->cancel_all_call_count = g_runtime_task_cancel_all_call_count;
  snapshot->cancellation_poll_call_count =
      g_runtime_task_cancellation_poll_call_count;
  snapshot->on_cancel_call_count = g_runtime_task_on_cancel_call_count;
  snapshot->executor_hop_call_count = g_runtime_task_executor_hop_call_count;
  snapshot->last_spawn_kind = g_runtime_task_last_spawn_kind;
  snapshot->last_spawn_executor_tag = g_runtime_task_last_spawn_executor_tag;
  snapshot->last_scope_executor_tag = g_runtime_task_last_scope_executor_tag;
  snapshot->last_add_task_executor_tag =
      g_runtime_task_last_add_task_executor_tag;
  snapshot->last_wait_next_executor_tag =
      g_runtime_task_last_wait_next_executor_tag;
  snapshot->last_cancel_all_executor_tag =
      g_runtime_task_last_cancel_all_executor_tag;
  snapshot->last_cancellation_poll_executor_tag =
      g_runtime_task_last_cancellation_poll_executor_tag;
  snapshot->last_on_cancel_executor_tag =
      g_runtime_task_last_on_cancel_executor_tag;
  snapshot->last_executor_hop_executor_tag =
      g_runtime_task_last_executor_hop_executor_tag;
  snapshot->last_executor_hop_value = g_runtime_task_last_executor_hop_value;
  snapshot->last_wait_next_result = g_runtime_task_last_wait_next_result;
  snapshot->last_cancel_all_result = g_runtime_task_last_cancel_all_result;
  snapshot->last_cancellation_poll_result =
      g_runtime_task_last_cancellation_poll_result;
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

int objc3_runtime_copy_actor_runtime_state_for_testing(
    objc3_runtime_actor_runtime_state_snapshot *snapshot) {
  // actor-runtime/executor-binding anchor: lane-D freezes the
  // private actor runtime proof surface on this snapshot rather than claiming
  // a wider public mailbox runtime ABI.
  // cross-module isolation-metadata hardening anchor: mixed-module
  // import/link-plan validation now preserves replay facts for this same
  // private snapshot-backed actor runtime slice without widening the public
  // runtime header.
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  snapshot->isolation_thunk_call_count =
      g_runtime_actor_isolation_thunk_call_count;
  snapshot->nonisolated_entry_call_count =
      g_runtime_actor_nonisolated_entry_call_count;
  snapshot->hop_to_executor_call_count =
      g_runtime_actor_hop_to_executor_call_count;
  snapshot->replay_proof_call_count = g_runtime_actor_replay_proof_call_count;
  snapshot->race_guard_call_count = g_runtime_actor_race_guard_call_count;
  snapshot->bind_executor_call_count = g_runtime_actor_bind_executor_call_count;
  snapshot->mailbox_enqueue_call_count =
      g_runtime_actor_mailbox_enqueue_call_count;
  snapshot->mailbox_drain_call_count = g_runtime_actor_mailbox_drain_call_count;
  snapshot->last_isolation_executor_tag =
      g_runtime_actor_last_isolation_executor_tag;
  snapshot->last_nonisolated_value = g_runtime_actor_last_nonisolated_value;
  snapshot->last_nonisolated_executor_tag =
      g_runtime_actor_last_nonisolated_executor_tag;
  snapshot->last_hop_value = g_runtime_actor_last_hop_value;
  snapshot->last_hop_executor_tag = g_runtime_actor_last_hop_executor_tag;
  snapshot->last_hop_result = g_runtime_actor_last_hop_result;
  snapshot->last_replay_proof_executor_tag =
      g_runtime_actor_last_replay_proof_executor_tag;
  snapshot->last_race_guard_executor_tag =
      g_runtime_actor_last_race_guard_executor_tag;
  snapshot->last_bound_actor_handle = g_runtime_actor_last_bound_actor_handle;
  snapshot->last_bound_executor_tag = g_runtime_actor_last_bound_executor_tag;
  snapshot->last_mailbox_actor_handle = g_runtime_actor_last_mailbox_actor_handle;
  snapshot->last_mailbox_enqueued_value =
      g_runtime_actor_last_mailbox_enqueued_value;
  snapshot->last_mailbox_executor_tag =
      g_runtime_actor_last_mailbox_executor_tag;
  snapshot->last_mailbox_depth = g_runtime_actor_last_mailbox_depth;
  snapshot->last_mailbox_drained_value =
      g_runtime_actor_last_mailbox_drained_value;
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

int objc3_runtime_copy_metaprogramming_expansion_host_boundary_snapshot_for_testing(
    objc3_runtime_metaprogramming_expansion_host_boundary_snapshot *snapshot) {
  // expansion-host/runtime-boundary anchor: lane-D freezes the
  // truthful metaprogramming host/cache boundary on one private snapshot backed by the existing
  // packaged runtime archive and property-accessor helper slice without
  // claiming live macro host execution or runtime package loading.
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  snapshot->property_runtime_ready = 1;
  snapshot->macro_host_execution_ready = 0;
  snapshot->macro_host_process_launch_ready = 0;
  snapshot->runtime_package_loader_ready = 0;
  snapshot->deterministic = 1;
  snapshot->runtime_support_library_archive_relative_path =
      "artifacts/lib/objc3_runtime.lib";
  snapshot->property_behavior_runtime_model =
      "supported-property-behavior-lowering-reuses-existing-private-runtime-property-accessor-layout-and-current-property-hooks";
  snapshot->macro_expansion_host_model =
      "macro-host-execution-process-launch-and-runtime-package-loading-remain-disabled-and-fail-closed";
  snapshot->packaging_model =
      "native-driver-packaging-still-hands-off-metaprogramming-runtime-support-through-artifacts-lib-objc3_runtime-lib-and-runtime-registration-manifests";
  snapshot->fail_closed_model =
      "no-live-macro-expansion-host-or-runtime-package-loader-is-claimed-yet";
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

int objc3_runtime_copy_metaprogramming_macro_host_process_cache_integration_snapshot_for_testing(
    objc3_runtime_metaprogramming_macro_host_process_cache_integration_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  snapshot->property_runtime_ready = 1;
  snapshot->macro_host_execution_ready = 1;
  snapshot->macro_host_process_launch_ready = 1;
  snapshot->runtime_package_loader_ready = 0;
  snapshot->deterministic = 1;
  snapshot->host_executable_relative_path =
      "artifacts/bin/objc3c-frontend-c-api-runner.exe";
  snapshot->cache_root_relative_path =
      MetaprogrammingHostCacheRootForTesting();
  snapshot->host_model =
      "native-driver-launches-objc3c-frontend-c-api-runner-for-supported-metaprogramming-expansion-cache-materialization";
  snapshot->toolchain_model =
      "frontend-runner-executes-with-manifest-enabled-and-ir-object-emission-disabled-for-deterministic-cache-materialization";
  snapshot->cache_model =
      "cache-entry-path-is-derived-from-a-stable-fnv1a64-key-over-the-metaprogramming-replay-surface-and-reused-on-subsequent-runs";
  snapshot->fail_closed_model =
      "missing-runner-corrupt-cache-or-import-surface-drift-disables-metaprogramming-host-process-cache-claims";
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

int objc3_runtime_copy_interop_bridge_packaging_toolchain_snapshot_for_testing(
    objc3_runtime_interop_bridge_packaging_toolchain_snapshot *snapshot) {
  // bridge-packaging/toolchain anchor: lane-D freezes the truthful
  // Part 11 packaging/runtime boundary on one private snapshot backed by the
  // existing runtime archive path and cross-module link-plan topology without
  // claiming live header/module/bridge generation yet.
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  snapshot->packaging_topology_ready = 1;
  snapshot->operator_visible_evidence_ready = 1;
  snapshot->header_generation_ready = 0;
  snapshot->module_generation_ready = 0;
  snapshot->bridge_generation_ready = 0;
  snapshot->deterministic = 1;
  snapshot->runtime_support_library_archive_relative_path =
      "artifacts/lib/objc3_runtime.lib";
  snapshot->registration_manifest_model =
      "runtime-registration-manifest-publishes-runtime-archive-path-owned-payloads-and-driver-link-wiring";
  snapshot->cross_module_link_plan_model =
      "runtime-import-surfaces-plus-registration-manifest-peer-artifacts-feed-one-fail-closed-cross-module-link-plan-and-linker-response-sidecar";
  snapshot->operator_visible_evidence_model =
      "operator-visible-interop-evidence-is-the-packaged-runtime-archive-registration-manifest-cross-module-link-plan-and-ir-summary";
  snapshot->fail_closed_model =
      "header-module-and-bridge-generation-remain-unclaimed-until-next-runtime-phase";
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

int objc3_runtime_copy_release_candidate_claim_snapshot_for_testing(
    objc3_runtime_release_candidate_claim_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  snapshot->claim_bundle_ready = 1;
  snapshot->deterministic = 1;
  snapshot->selected_profile = "core";
  snapshot->claimed_profile_ids_csv =
      "core,strict,strict-concurrency,strict-system";
  snapshot->targeted_profile_ids_csv =
      "strict,strict-concurrency,strict-system";
  snapshot->conformance_publication_contract_id =
      kObjc3ConformancePublicationContractId;
  snapshot->conformance_claim_operations_contract_id =
      kObjc3ConformanceClaimOperationsContractId;
  snapshot->selection_model = kObjc3ConformanceClaimSelectionModel;
  snapshot->failure_model = kObjc3ConformanceClaimFailureModel;
  snapshot->advanced_feature_ops_contract_id =
      kObjc3AdvancedFeatureOpsContractId;
  snapshot->advanced_feature_reporting_contract_id =
      kObjc3AdvancedFeatureReportingContractId;
  snapshot->advanced_feature_release_evidence_contract_id =
      kObjc3AdvancedFeatureReleaseEvidenceContractId;
  snapshot->release_evidence_operation_contract_id =
      kObjc3AdvancedFeatureReleaseEvidenceOperationContractId;
  snapshot->dashboard_status_publication_contract_id =
      kObjc3AdvancedFeatureDashboardStatusPublicationContractId;
  snapshot->release_candidate_matrix_contract_id =
      "objc3c.tooling.release.candidate.execution.matrix.v1";
  snapshot->dashboard_schema_path = kObjc3AdvancedFeatureDashboardSchemaPath;
  snapshot->gate_script_path = kObjc3AdvancedFeatureEvidenceGateScriptPath;
  snapshot->runbook_reference_path = kObjc3AdvancedFeatureEvidenceRunbookPath;
  snapshot->release_bundle_model =
      "final-claim-publication-bundle-is-the-conformance-publication-validation-release-evidence-dashboard-advanced-feature-gate-and-release-candidate-matrix-artifact-set";
  snapshot->deprecated_path_shutdown_model =
      "deprecated-claim-matrix-dashboard-ready-summary-and-toolchain-runtime-ga-scaffold-paths-remain-retired-and-fail-closed";
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

int objc3_runtime_copy_release_candidate_evidence_state_for_testing(
    objc3_runtime_release_candidate_evidence_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  snapshot->validation_artifact_ready = 1;
  snapshot->release_evidence_operation_ready = 1;
  snapshot->dashboard_status_ready = 1;
  snapshot->advanced_feature_gate_ready = 1;
  snapshot->release_candidate_matrix_ready = 1;
  snapshot->deprecated_paths_shutdown = 1;
  snapshot->deterministic = 1;
  snapshot->validation_artifact_name = "module.objc3-conformance-validation.json";
  snapshot->release_evidence_operation_artifact_name =
      "module.objc3-release-evidence-operation.json";
  snapshot->dashboard_status_artifact_name = "module.objc3-dashboard-status.json";
  snapshot->advanced_feature_gate_artifact_name =
      "module.objc3-advanced-feature-gate.json";
  snapshot->release_candidate_matrix_artifact_name =
      "module.objc3-release-candidate-matrix.json";
  snapshot->validation_model =
      "driver-validates-versioned-conformance-report-and-publication-sidecars-before-toolchain-consumption";
  snapshot->release_bundle_model =
      "validation-completes-the-final-claim-publication-bundle-over-the-live-validation-release-evidence-dashboard-gate-and-release-candidate-artifacts";
  snapshot->deprecated_path_shutdown_model =
      "deprecated-claim-matrix-dashboard-ready-summary-and-toolchain-runtime-ga-scaffold-paths-remain-retired-and-fail-closed";
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

int objc3_runtime_copy_interop_bridge_generation_snapshot_for_testing(
    objc3_runtime_interop_bridge_generation_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  snapshot->runtime_generation_ready = 1;
  snapshot->cross_module_packaging_ready = 1;
  snapshot->header_generation_ready = 1;
  snapshot->module_generation_ready = 1;
  snapshot->bridge_generation_ready = 1;
  snapshot->deterministic = 1;
  snapshot->header_artifact_relative_path = "module.interop-bridge.h";
  snapshot->module_artifact_relative_path = "module.interop-bridge.modulemap";
  snapshot->bridge_artifact_relative_path = "module.interop-bridge.json";
  snapshot->generation_model =
      "compiler-emits-deterministic-interop-bridge-header-modulemap-and-bridge-json-artifacts-for-supported-foreign-callable-surfaces";
  snapshot->packaging_model =
      "runtime-import-surfaces-and-cross-module-link-plans-preserve-interop-bridge-artifact-paths-for-mixed-module-consumption";
  snapshot->fail_closed_model =
      "missing-generated-artifacts-or-drifted-import-surface-paths-disable-live-interop-bridge-generation-claims";
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

int objc3_runtime_replay_registered_images_for_testing(void) {
  RuntimeState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  // live-registration-discovery-replay anchor: replay consumes the
  // retained bootstrap catalog only when live registration state is empty,
  // republishes discovery/image-walk state through the same staged-table path,
  // and records last-replayed identity plus replay-generation evidence.
  // live-restart-hardening anchor: replay fails closed when live
  // runtime state is still populated, repeated restart cycles must remain
  // deterministic, and replay-generation evidence must advance monotonically.
  state.last_replay_status = OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  state.last_replayed_image_count = 0;
  state.last_replayed_module_name.clear();
  state.last_replayed_translation_unit_identity_key.clear();

  if (state.registered_image_count != 0 ||
      !state.registration_order_by_identity_key.empty() ||
      state.next_expected_registration_order_ordinal != 1 ||
      state.staged_registration_table != nullptr) {
    state.last_replay_status = OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
    return state.last_replay_status;
  }

  for (const std::string &identity_key : state.retained_bootstrap_identity_order) {
    const auto found =
        state.retained_bootstrap_metadata_by_identity_key.find(identity_key);
    if (found == state.retained_bootstrap_metadata_by_identity_key.end() ||
        found->second.registration_table == nullptr ||
        found->second.registration_table->image_descriptor == nullptr) {
      ClearLiveRegistrationStateUnlocked(state);
      state.last_reset_cleared_image_local_init_state_count =
          ZeroRetainedBootstrapImageLocalInitStatesUnlocked(state);
      state.last_replay_status = OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
      return state.last_replay_status;
    }

    const RegisteredImageMetadata &record = found->second;
    const int status = RegisterImageUnlocked(state,
                                             record.registration_table->image_descriptor,
                                             record.registration_table, false,
                                             true);
    if (status != OBJC3_RUNTIME_REGISTRATION_STATUS_OK) {
      ClearLiveRegistrationStateUnlocked(state);
      state.last_reset_cleared_image_local_init_state_count =
          ZeroRetainedBootstrapImageLocalInitStatesUnlocked(state);
      state.last_replay_status = status;
      state.last_replayed_image_count = 0;
      state.last_replayed_module_name.clear();
      state.last_replayed_translation_unit_identity_key.clear();
      return status;
    }

    ++state.last_replayed_image_count;
    state.last_replayed_module_name = record.module_name;
    state.last_replayed_translation_unit_identity_key =
        record.translation_unit_identity_key;
  }

  ++state.replay_generation;
  state.last_replay_status = OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

const objc3_runtime_selector_handle *objc3_runtime_lookup_selector(
    const char *selector) {
  RuntimeState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  return LookupSelectorUnlocked(selector);
}

// optional/key-path runtime-helper freeze anchor: the live Part 3
// runtime helper boundary intentionally remains minimal here. Optional sends
// already execute through objc3_runtime_lookup_selector plus
// objc3_runtime_dispatch_i32 with lowering-owned nil short-circuit semantics,
// while validated typed key-path sites currently contribute retained descriptor
// handles and descriptor sections without claiming full runtime evaluation yet.

// runtime memory-management API freeze anchor: lowered ownership
// helpers remain private runtime entrypoints reached through IR and runtime
// probes, while the stable public runtime header stays limited to
// registration/lookup/dispatch plus snapshot helpers.
// runtime ARC helper API surface anchor: ARC-specific lowering now
// also depends on this same private helper cluster, including autoreleasepool
// scope hooks, but the ABI remains bootstrap-internal rather than public.
// runtime ARC helper implementation anchor: ARC-specific lowering
// now also depends on this same private helper cluster as a live linked
// runtime capability for the supported property/weak/autorelease-return slice,
// while the ABI remains bootstrap-internal rather than public.
extern "C" int objc3_runtime_read_current_property_i32(void) {
  RuntimeDispatchFrame *frame = CurrentRuntimeDispatchFrame();
  ++g_runtime_arc_debug_current_property_read_count;
  RecordArcDebugPropertyContext(frame);
  if (frame == nullptr || frame->runtime_property_accessor == nullptr ||
      frame->receiver == 0) {
    g_runtime_arc_debug_last_property_read_value = 0;
    return 0;
  }
  RuntimeState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  const auto instance_it = state.runtime_instances_by_receiver.find(frame->receiver);
  if (instance_it == state.runtime_instances_by_receiver.end()) {
    g_runtime_arc_debug_last_property_read_value = 0;
    return 0;
  }
  int value = 0;
  const int result = ReadRuntimeManagedPropertyValueUnlocked(
             state, instance_it->second, *frame->runtime_property_accessor, value)
                         ? value
                         : 0;
  g_runtime_arc_debug_last_property_read_value = result;
  return result;
}

extern "C" void objc3_runtime_write_current_property_i32(int value) {
  RuntimeDispatchFrame *frame = CurrentRuntimeDispatchFrame();
  ++g_runtime_arc_debug_current_property_write_count;
  g_runtime_arc_debug_last_property_written_value = value;
  RecordArcDebugPropertyContext(frame);
  if (frame == nullptr || frame->runtime_property_accessor == nullptr ||
      frame->receiver == 0) {
    return;
  }
  RuntimeState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  const auto instance_it = state.runtime_instances_by_receiver.find(frame->receiver);
  if (instance_it == state.runtime_instances_by_receiver.end()) {
    return;
  }
  (void)WriteRuntimeManagedPropertyValueUnlocked(
      state, instance_it->second, *frame->runtime_property_accessor, value);
}

extern "C" int objc3_runtime_exchange_current_property_i32(int value) {
  RuntimeDispatchFrame *frame = CurrentRuntimeDispatchFrame();
  ++g_runtime_arc_debug_current_property_exchange_count;
  g_runtime_arc_debug_last_property_exchange_new_value = value;
  RecordArcDebugPropertyContext(frame);
  if (frame == nullptr || frame->runtime_property_accessor == nullptr ||
      frame->receiver == 0) {
    g_runtime_arc_debug_last_property_exchange_previous_value = 0;
    return 0;
  }
  RuntimeState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  const auto instance_it = state.runtime_instances_by_receiver.find(frame->receiver);
  if (instance_it == state.runtime_instances_by_receiver.end()) {
    g_runtime_arc_debug_last_property_exchange_previous_value = 0;
    return 0;
  }
  int previous_value = 0;
  const int result = ExchangeRuntimeManagedPropertyValueUnlocked(
             state, instance_it->second, *frame->runtime_property_accessor, value,
             previous_value)
                         ? previous_value
                         : 0;
  g_runtime_arc_debug_last_property_exchange_previous_value = result;
  return result;
}

extern "C" int objc3_runtime_bind_current_property_context_for_testing(
    int receiver, const char *class_name, const char *property_name) {
  if (receiver == 0 || class_name == nullptr || class_name[0] == '\0' ||
      property_name == nullptr || property_name[0] == '\0') {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  RuntimeState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  const auto instance_it = state.runtime_instances_by_receiver.find(receiver);
  const auto found = state.realized_class_node_indices_by_name.find(class_name);
  if (instance_it == state.runtime_instances_by_receiver.end() ||
      found == state.realized_class_node_indices_by_name.end() ||
      found->second.empty()) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }
  const std::size_t node_index = found->second.front();
  if (node_index >= state.realized_class_nodes.size()) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  const RealizedClassNode &start_node = state.realized_class_nodes[node_index];
  const RealizedClassNode *resolved_node = nullptr;
  bool inherited = false;
  bool used_cache = false;
  const RealizedPropertyAccessor *accessor =
      FindRuntimePropertyAccessorByNameUnlocked(
          state, start_node, property_name, resolved_node, inherited,
          used_cache);
  state.last_property_query_used_cache = used_cache;
  if (accessor == nullptr || resolved_node == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  g_runtime_testing_dispatch_frame = RuntimeDispatchFrame{};
  g_runtime_testing_dispatch_frame.receiver = receiver;
  g_runtime_testing_dispatch_frame.base_identity =
      instance_it->second.base_identity;
  g_runtime_testing_dispatch_frame.runtime_property_accessor = accessor;
  g_runtime_has_testing_dispatch_frame = true;
  RecordArcDebugPropertyContext(&g_runtime_testing_dispatch_frame);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

extern "C" void objc3_runtime_clear_current_property_context_for_testing(void) {
  g_runtime_testing_dispatch_frame = RuntimeDispatchFrame{};
  g_runtime_has_testing_dispatch_frame = false;
}

// error-runtime/bridge-helper anchor: the current runnable Part 6
// slice now routes thrown-error storage, bridge normalization, and catch-kind
// matching through this same private bootstrap-internal helper cluster instead
// of treating raw function-local slots as the runtime boundary.
// live catch/bridge/runtime integration anchor: linked Part 6
// object-code probes now execute through these helpers and consume the
// snapshot state below to prove live store/load/bridge/catch traffic.
