const objc3_runtime_selector_handle *LookupSelectorUnlocked(const char *selector) {
  // selector-table anchor: metadata-backed selector pools now
  // materialize the canonical runtime selector table, while direct lookup of
  // non-emitted selectors remains a dynamic strict-error path. The later runtime step layers
  // method-cache and class/metaclass slow-path dispatch on top of the same
  // preserved public lookup/dispatch surface.
  const char *normalized_selector =
      objc3c::support::selectors::NormalizeSelectorSpelling(selector);
  if (!objc3c::runtime::RuntimeSelectorTableAcceptsDynamicSelector(
          normalized_selector)) {
    return nullptr;
  }

  RuntimeState &state = ProcessRuntimeState();
  const auto found = state.selector_index_by_name.find(normalized_selector);
  if (found != state.selector_index_by_name.end()) {
    return &state.selector_slots[found->second].handle;
  }

  SelectorSlot slot;
  slot.spelling_storage = normalized_selector;
  state.selector_slots.push_back(std::move(slot));
  SelectorSlot &stored = state.selector_slots.back();
  stored.handle.selector = stored.spelling_storage.c_str();
  stored.handle.stable_id = static_cast<std::uint64_t>(state.selector_slots.size());
  const std::size_t index = state.selector_slots.size() - 1u;
  state.selector_index_by_name.emplace(stored.spelling_storage, index);
  ++state.dynamic_selector_count;
  state.last_materialized_selector = stored.spelling_storage;
  state.last_materialized_stable_id = stored.handle.stable_id;
  state.last_materialized_registration_order_ordinal = 0;
  state.last_materialized_selector_pool_index = 0;
  state.last_materialized_from_metadata = false;
  return &stored.handle;
}

bool MaterializeSelectorLookupEntryUnlocked(RuntimeState &state,
                                            const char *selector,
                                            std::uint64_t registration_order_ordinal,
                                            std::uint64_t selector_pool_index) {
  const char *normalized_selector =
      objc3c::support::selectors::NormalizeSelectorSpelling(selector);
  if (!objc3c::runtime::RuntimeSelectorTableAcceptsMetadataSelector(
          normalized_selector)) {
    return false;
  }

  const auto found = state.selector_index_by_name.find(normalized_selector);
  if (found == state.selector_index_by_name.end()) {
    SelectorSlot slot;
    slot.spelling_storage = normalized_selector;
    slot.metadata_backed = true;
    slot.metadata_provider_count = 1;
    slot.first_registration_order_ordinal = registration_order_ordinal;
    slot.last_registration_order_ordinal = registration_order_ordinal;
    slot.first_selector_pool_index = selector_pool_index;
    slot.last_selector_pool_index = selector_pool_index;
    state.selector_slots.push_back(std::move(slot));
    SelectorSlot &stored = state.selector_slots.back();
    stored.handle.selector = stored.spelling_storage.c_str();
    stored.handle.stable_id =
        static_cast<std::uint64_t>(state.selector_slots.size());
    state.selector_index_by_name.emplace(stored.spelling_storage,
                                         state.selector_slots.size() - 1u);
    ++state.metadata_backed_selector_count;
    ++state.metadata_provider_edge_count;
    state.last_materialized_selector = stored.spelling_storage;
    state.last_materialized_stable_id = stored.handle.stable_id;
    state.last_materialized_registration_order_ordinal =
        registration_order_ordinal;
    state.last_materialized_selector_pool_index = selector_pool_index;
    state.last_materialized_from_metadata = true;
    return true;
  }

  SelectorSlot &stored = state.selector_slots[found->second];
  if (!stored.metadata_backed) {
    stored.metadata_backed = true;
    stored.first_registration_order_ordinal = registration_order_ordinal;
    stored.first_selector_pool_index = selector_pool_index;
    ++state.metadata_backed_selector_count;
    if (state.dynamic_selector_count > 0) {
      --state.dynamic_selector_count;
    }
  }
  ++stored.metadata_provider_count;
  stored.last_registration_order_ordinal = registration_order_ordinal;
  stored.last_selector_pool_index = selector_pool_index;
  ++state.metadata_provider_edge_count;
  state.last_materialized_selector = stored.spelling_storage;
  state.last_materialized_stable_id = stored.handle.stable_id;
  state.last_materialized_registration_order_ordinal =
      registration_order_ordinal;
  state.last_materialized_selector_pool_index = selector_pool_index;
  state.last_materialized_from_metadata = true;
  return true;
}

std::uint64_t CountKeyPathComponents(const char *component_path) {
  if (component_path == nullptr || component_path[0] == '\0') {
    return 0;
  }
  std::uint64_t count = 1;
  for (const unsigned char *cursor =
           reinterpret_cast<const unsigned char *>(component_path);
       *cursor != 0U; ++cursor) {
    if (*cursor == static_cast<unsigned char>('.')) {
      ++count;
    }
  }
  return count;
}

bool KeyPathDescriptorsCompatible(const KeyPathSlot &slot,
                                  const EmittedKeyPathDescriptor &descriptor) {
  const char *generic_metadata_replay_key =
      descriptor.generic_metadata_replay_key == nullptr
          ? ""
          : descriptor.generic_metadata_replay_key;
  return slot.root_name_storage == descriptor.root_name &&
         slot.component_path_storage == descriptor.component_path &&
         slot.profile_storage == descriptor.profile &&
         slot.generic_metadata_replay_key_storage ==
             generic_metadata_replay_key &&
         slot.root_is_self == descriptor.root_is_self;
}

bool MaterializeKeyPathDescriptorUnlocked(
    RuntimeState &state, const EmittedKeyPathDescriptor &descriptor,
    std::uint64_t registration_order_ordinal) {
  if (!objc3c::runtime::RuntimeKeyPathHandleIsValid(descriptor.stable_id) ||
      !objc3c::runtime::RuntimeKeyPathDescriptorIsMaterializable(
          descriptor.root_name, descriptor.component_path) ||
      descriptor.profile == nullptr ||
      descriptor.profile[0] == '\0') {
    return false;
  }

  const auto found = state.keypath_slots.find(descriptor.stable_id);
  if (found == state.keypath_slots.end()) {
    KeyPathSlot slot;
    slot.stable_id = descriptor.stable_id;
    slot.root_name_storage = descriptor.root_name;
    slot.component_path_storage = descriptor.component_path;
    slot.profile_storage = descriptor.profile;
    slot.generic_metadata_replay_key_storage =
        descriptor.generic_metadata_replay_key == nullptr
            ? ""
            : descriptor.generic_metadata_replay_key;
    slot.root_is_self = descriptor.root_is_self;
    slot.component_count = CountKeyPathComponents(descriptor.component_path);
    slot.metadata_provider_count = 1;
    slot.first_registration_order_ordinal = registration_order_ordinal;
    slot.last_registration_order_ordinal = registration_order_ordinal;
    if (slot.component_count == 0) {
      return false;
    }
    state.keypath_slots.emplace(descriptor.stable_id, std::move(slot));
    ++state.image_backed_keypath_count;
    state.last_materialized_keypath_handle = descriptor.stable_id;
    state.last_materialized_keypath_registration_order_ordinal =
        registration_order_ordinal;
    state.last_materialized_keypath_profile = descriptor.profile;
    return true;
  }

  KeyPathSlot &slot = found->second;
  if (!KeyPathDescriptorsCompatible(slot, descriptor)) {
    if (!slot.ambiguous) {
      slot.ambiguous = true;
      ++state.ambiguous_keypath_handle_count;
    }
  }
  ++slot.metadata_provider_count;
  slot.last_registration_order_ordinal = registration_order_ordinal;
  state.last_materialized_keypath_handle = descriptor.stable_id;
  state.last_materialized_keypath_registration_order_ordinal =
      registration_order_ordinal;
  state.last_materialized_keypath_profile = slot.profile_storage;
  return true;
}

std::uint64_t DescriptorTotal(
    const objc3_runtime_image_descriptor *image) {
  return objc3c::runtime::RuntimeDescriptorTotal(image);
}

std::uint64_t AggregateCount(const objc3_runtime_pointer_aggregate *aggregate) {
  return objc3c::runtime::RuntimeAggregateCount(aggregate);
}

const void *AggregateEntry(const objc3_runtime_pointer_aggregate *aggregate,
                           std::uint64_t index) {
  return objc3c::runtime::RuntimeAggregateEntry(aggregate, index);
}

bool AggregateContainsPointer(const objc3_runtime_pointer_aggregate *aggregate,
                              const void *target) {
  return objc3c::runtime::RuntimeAggregateContainsPointer(aggregate, target);
}

void MarkRejectedRegistrationUnlocked(
    RuntimeState &state, const objc3_runtime_image_descriptor *image,
    int status) {
  state.last_registration_status = status;
  state.last_rejected_module_name =
      image != nullptr && image->module_name != nullptr ? image->module_name : "";
  state.last_rejected_translation_unit_identity_key =
      image != nullptr && image->translation_unit_identity_key != nullptr
          ? image->translation_unit_identity_key
          : "";
  state.last_rejected_registration_order_ordinal =
      image != nullptr ? image->registration_order_ordinal : 0;
}

void ClearRejectedRegistrationUnlocked(RuntimeState &state) {
  state.last_rejected_module_name.clear();
  state.last_rejected_translation_unit_identity_key.clear();
  state.last_rejected_registration_order_ordinal = 0;
}

void ApplyImageWalkRecordUnlocked(RuntimeState &state,
                                  const RegisteredImageMetadata &record) {
  state.walked_image_count = static_cast<std::uint64_t>(
      state.registered_image_metadata_by_identity_key.size());
  state.last_discovery_root_entry_count = record.discovery_root_entry_count;
  state.last_walked_class_descriptor_count = record.class_descriptor_count;
  state.last_walked_protocol_descriptor_count = record.protocol_descriptor_count;
  state.last_walked_category_descriptor_count = record.category_descriptor_count;
  state.last_walked_property_descriptor_count = record.property_descriptor_count;
  state.last_walked_ivar_descriptor_count = record.ivar_descriptor_count;
  state.last_walked_selector_pool_count = record.selector_pool_count;
  state.last_walked_string_pool_count = record.string_pool_count;
  state.last_walked_keypath_descriptor_count = record.keypath_descriptor_count;
  state.last_linker_anchor_matches_discovery_root =
      record.linker_anchor_matches_discovery_root;
  state.last_registration_used_staged_table =
      record.used_staged_registration_table;
  state.last_walked_module_name = record.module_name;
  state.last_walked_translation_unit_identity_key =
      record.translation_unit_identity_key;
}

void RetainBootstrapRecordUnlocked(RuntimeState &state,
                                   const RegisteredImageMetadata &record) {
  const auto found = state.retained_bootstrap_metadata_by_identity_key.find(
      record.translation_unit_identity_key);
  if (found == state.retained_bootstrap_metadata_by_identity_key.end()) {
    state.retained_bootstrap_identity_order.push_back(
        record.translation_unit_identity_key);
  }
  state.retained_bootstrap_metadata_by_identity_key
      [record.translation_unit_identity_key] = record;
}

bool TryWalkRegistrationTableUnlocked(
    RuntimeState &state,
    const objc3_runtime_registration_table *registration_table,
    const objc3_runtime_image_descriptor *image,
    RegisteredImageMetadata &record) {
  // runtime-bootstrap-table-consumption anchor: staged registration
  // tables must match the image descriptor exactly, discovery-root membership
  // must close over every descriptor family before image state is published,
  // and the walked image snapshot is derived from this validated table rather
  // than from sidecar manifests or replay-only bookkeeping.
  if (!objc3c::runtime::RuntimeRegistrationTableShapeIsSupported(
          registration_table, image)) {
    return false;
  }

  const void *const linker_anchor_target =
      *reinterpret_cast<const void *const *>(registration_table->linker_anchor);
  const std::uint64_t discovery_root_entry_count =
      AggregateCount(registration_table->discovery_root);
  if (discovery_root_entry_count < 6) {
    return false;
  }

  const std::uint64_t class_descriptor_count =
      AggregateCount(registration_table->class_descriptor_root);
  const std::uint64_t protocol_descriptor_count =
      AggregateCount(registration_table->protocol_descriptor_root);
  const std::uint64_t category_descriptor_count =
      AggregateCount(registration_table->category_descriptor_root);
  const std::uint64_t property_descriptor_count =
      AggregateCount(registration_table->property_descriptor_root);
  const std::uint64_t ivar_descriptor_count =
      AggregateCount(registration_table->ivar_descriptor_root);
  const std::uint64_t selector_pool_count =
      AggregateCount(registration_table->selector_pool_root);
  const std::uint64_t string_pool_count =
      AggregateCount(registration_table->string_pool_root);
  const std::uint64_t keypath_descriptor_count =
      AggregateCount(registration_table->keypath_descriptor_root);
  state.selector_index_by_name.reserve(
      state.selector_index_by_name.size() +
      static_cast<std::size_t>(selector_pool_count));
  state.keypath_slots.reserve(
      state.keypath_slots.size() +
      static_cast<std::size_t>(keypath_descriptor_count));
  const bool linker_anchor_matches_discovery_root =
      linker_anchor_target == registration_table->discovery_root;

  if (class_descriptor_count != image->class_descriptor_count ||
      protocol_descriptor_count != image->protocol_descriptor_count ||
      category_descriptor_count != image->category_descriptor_count ||
      property_descriptor_count != image->property_descriptor_count ||
      ivar_descriptor_count != image->ivar_descriptor_count ||
      !linker_anchor_matches_discovery_root ||
      !AggregateContainsPointer(registration_table->discovery_root,
                                registration_table->class_descriptor_root) ||
      !AggregateContainsPointer(registration_table->discovery_root,
                                registration_table->protocol_descriptor_root) ||
      !AggregateContainsPointer(registration_table->discovery_root,
                                registration_table->category_descriptor_root) ||
      !AggregateContainsPointer(registration_table->discovery_root,
                                registration_table->property_descriptor_root) ||
      !AggregateContainsPointer(registration_table->discovery_root,
                                registration_table->ivar_descriptor_root)) {
    return false;
  }

  if (registration_table->selector_pool_root != nullptr &&
      !AggregateContainsPointer(registration_table->discovery_root,
                                registration_table->selector_pool_root)) {
    return false;
  }
  if (registration_table->string_pool_root != nullptr &&
      !AggregateContainsPointer(registration_table->discovery_root,
                                registration_table->string_pool_root)) {
    return false;
  }
  if (registration_table->keypath_descriptor_root != nullptr &&
      !AggregateContainsPointer(registration_table->discovery_root,
                                registration_table->keypath_descriptor_root)) {
    return false;
  }

  std::unordered_set<std::string> selector_pool_spelling_set;
  selector_pool_spelling_set.reserve(
      static_cast<std::size_t>(selector_pool_count));
  std::vector<std::string> selector_pool_spellings;
  selector_pool_spellings.reserve(static_cast<std::size_t>(selector_pool_count));
  for (std::uint64_t index = 0; index < selector_pool_count; ++index) {
    const char *selector = reinterpret_cast<const char *>(
        AggregateEntry(registration_table->selector_pool_root, index));
    if (selector == nullptr || selector[0] == '\0') {
      return false;
    }
    const auto inserted = selector_pool_spelling_set.emplace(selector);
    if (!inserted.second) {
      return false;
    }
    selector_pool_spellings.emplace_back(selector);
  }
  for (std::uint64_t index = 0; index < string_pool_count; ++index) {
    const char *value = reinterpret_cast<const char *>(
        AggregateEntry(registration_table->string_pool_root, index));
    if (value == nullptr) {
      return false;
    }
  }
  for (std::uint64_t index = 0; index < keypath_descriptor_count; ++index) {
    const auto *descriptor =
        reinterpret_cast<const EmittedKeyPathDescriptor *>(
            AggregateEntry(registration_table->keypath_descriptor_root, index));
    if (descriptor == nullptr ||
        !MaterializeKeyPathDescriptorUnlocked(
            state, *descriptor, image->registration_order_ordinal)) {
      return false;
    }
  }

  record.module_name = image->module_name;
  record.translation_unit_identity_key = image->translation_unit_identity_key;
  record.registration_order_ordinal = image->registration_order_ordinal;
  record.registration_table = registration_table;
  record.discovery_root = registration_table->discovery_root;
  record.class_descriptor_root = registration_table->class_descriptor_root;
  record.protocol_descriptor_root = registration_table->protocol_descriptor_root;
  record.category_descriptor_root = registration_table->category_descriptor_root;
  record.property_descriptor_root = registration_table->property_descriptor_root;
  record.ivar_descriptor_root = registration_table->ivar_descriptor_root;
  record.selector_pool_root = registration_table->selector_pool_root;
  record.string_pool_root = registration_table->string_pool_root;
  record.keypath_descriptor_root = registration_table->keypath_descriptor_root;
  record.discovery_root_entry_count = discovery_root_entry_count;
  record.class_descriptor_count = class_descriptor_count;
  record.protocol_descriptor_count = protocol_descriptor_count;
  record.category_descriptor_count = category_descriptor_count;
  record.property_descriptor_count = property_descriptor_count;
  record.ivar_descriptor_count = ivar_descriptor_count;
  record.selector_pool_count = selector_pool_count;
  record.string_pool_count = string_pool_count;
  record.keypath_descriptor_count = keypath_descriptor_count;
  record.linker_anchor_matches_discovery_root =
      linker_anchor_matches_discovery_root;
  record.used_staged_registration_table = true;

  for (std::size_t index = 0; index < selector_pool_spellings.size(); ++index) {
    if (!MaterializeSelectorLookupEntryUnlocked(
            state, selector_pool_spellings[index].c_str(),
            image->registration_order_ordinal,
            static_cast<std::uint64_t>(index + 1u))) {
      return false;
    }
  }
  return true;
}

int RegisterImageUnlocked(
    RuntimeState &state, const objc3_runtime_image_descriptor *image,
    const objc3_runtime_registration_table *staged_registration_table,
    bool retain_bootstrap_record, bool mark_image_local_init_state) {
  // runtime-bootstrap-table-consumption anchor: duplicate identity
  // rejection and out-of-order rejection happen before live counters advance,
  // while successful staged-table consumption is the only path allowed to
  // publish bootstrap-visible image-walk state.
  (void)objc3c::runtime::RuntimeImageDescriptorOwnershipModel();
  if (!objc3c::runtime::RuntimeImageDescriptorHasRequiredIdentity(image)) {
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

  std::uint64_t descriptor_total = DescriptorTotal(image);
  if (staged_registration_table != nullptr) {
    state.registration_order_by_identity_key.reserve(
        state.registration_order_by_identity_key.size() + 1u);
    state.registered_image_metadata_by_identity_key.reserve(
        state.registered_image_metadata_by_identity_key.size() + 1u);
    RegisteredImageMetadata record;
    if (!TryWalkRegistrationTableUnlocked(state, staged_registration_table, image,
                                          record)) {
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
        state.registered_image_metadata_by_identity_key
            .at(record.translation_unit_identity_key));
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
  state.last_registered_module_name = image->module_name;
  state.last_registered_translation_unit_identity_key =
      image->translation_unit_identity_key;
  state.registration_order_by_identity_key.emplace(
      image->translation_unit_identity_key, image->registration_order_ordinal);
  // metaclass-graph-root-class anchor: successful registration now
  // republishes a runtime-owned realized class/metaclass graph and root-class
  // baseline before dispatch can consume the image.
  RebuildRealizedClassGraphUnlocked(state);
  ClearRejectedRegistrationUnlocked(state);
  ClearMethodCacheStateUnlocked(state);
  SeedDispatchIntentFastPathCacheUnlocked(state);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

bool ProtocolExistsByNameUnlocked(const RuntimeState &state,
                                  const char *protocol_name) {
  if (protocol_name == nullptr || protocol_name[0] == '\0') {
    return false;
  }
  for (const RegisteredImageMetadata *record : OrderedRegisteredImages(state)) {
    for (std::uint64_t index = 0; index < record->protocol_descriptor_count;
         ++index) {
      const auto *protocol_record = static_cast<const EmittedProtocolRecord *>(
          AggregateEntry(record->protocol_descriptor_root, index));
      if (protocol_record == nullptr || protocol_record->protocol_name == nullptr) {
        continue;
      }
      if (std::strcmp(protocol_record->protocol_name, protocol_name) == 0) {
        return true;
      }
    }
  }
  return false;
}

bool QueryRealizedClassProtocolConformanceUnlocked(
    RuntimeState &state, const RealizedClassNode *start_node,
    const char *protocol_name, std::uint64_t &visited_protocol_count,
    std::string &matched_protocol_owner_identity,
    std::string &matched_attachment_owner_identity) {
  // category-attachment-protocol-conformance anchor: runtime-facing
  // protocol conformance queries walk realized class nodes, attached category
  // protocol refs, and inherited protocol closures without widening the public
  // dispatch ABI.
  if (start_node == nullptr || protocol_name == nullptr || protocol_name[0] == '\0') {
    return false;
  }
  std::unordered_set<const EmittedProtocolRecord *> visited_protocols;
  std::unordered_set<const RealizedClassNode *> visited_nodes;
  const RealizedClassNode *node = start_node;
  while (node != nullptr && visited_nodes.insert(node).second) {
    if (node->bundle == nullptr) {
      return false;
    }
    matched_attachment_owner_identity.clear();
    const EmittedClassRecord &record = node->bundle->class_record;
    if (QueryProtocolConformanceFromAggregateUnlocked(
            record.adopted_protocol_refs, protocol_name, visited_protocols,
            visited_protocol_count, matched_protocol_owner_identity)) {
      return true;
    }
    for (const EmittedCategoryRecord *category_record :
         node->attached_category_records) {
      if (QueryProtocolConformanceFromAggregateUnlocked(
              category_record->adopted_protocol_refs, protocol_name,
              visited_protocols, visited_protocol_count,
              matched_protocol_owner_identity)) {
        matched_attachment_owner_identity =
            category_record->category_owner_identity != nullptr
                ? category_record->category_owner_identity
                : "";
        return true;
      }
    }
    node = node->has_super_node ? &state.realized_class_nodes[node->super_node_index]
                                : nullptr;
  }
  return false;
}

}  // namespace

extern "C" void objc3_runtime_stage_registration_table_for_bootstrap(
    const objc3_runtime_registration_table *registration_table) {
  RuntimeState &state = ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  state.staged_registration_table = registration_table;
}

// runtime-bootstrap-api anchor: registration, selector lookup,
// dispatch, snapshot, and reset remain the frozen bootstrap runtime boundary.
// D002/D003 may extend image walk and reset behavior, but they must preserve
// this surface and its fail-closed status/result contract.
int objc3_runtime_register_image(const objc3_runtime_image_descriptor *image) {
  RuntimeState &state = ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  const objc3_runtime_registration_table *const staged_registration_table =
      state.staged_registration_table;
  // runtime-bootstrap-table-consumption anchor: staging is one-shot
  // and is consumed by the next public registration call only.
  state.staged_registration_table = nullptr;
  return RegisterImageUnlocked(state, image, staged_registration_table, true,
                               false);
}

int objc3_runtime_copy_selector_lookup_table_state_for_testing(
    objc3_runtime_selector_lookup_table_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  RuntimeState &state = ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  snapshot->selector_table_entry_count =
      static_cast<std::uint64_t>(state.selector_slots.size());
  snapshot->metadata_backed_selector_count = state.metadata_backed_selector_count;
  snapshot->dynamic_selector_count = state.dynamic_selector_count;
  snapshot->metadata_provider_edge_count = state.metadata_provider_edge_count;
  snapshot->last_materialized_selector =
      BorrowRuntimeCString(state.last_materialized_selector);
  snapshot->last_materialized_stable_id = state.last_materialized_stable_id;
  snapshot->last_materialized_registration_order_ordinal =
      state.last_materialized_registration_order_ordinal;
  snapshot->last_materialized_selector_pool_index =
      state.last_materialized_selector_pool_index;
  snapshot->last_materialized_from_metadata =
      state.last_materialized_from_metadata ? 1 : 0;
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

int objc3_runtime_copy_selector_lookup_entry_for_testing(
    const char *selector,
    objc3_runtime_selector_lookup_entry_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  snapshot->found = 0;
  snapshot->metadata_backed = 0;
  snapshot->stable_id = 0;
  snapshot->metadata_provider_count = 0;
  snapshot->first_registration_order_ordinal = 0;
  snapshot->last_registration_order_ordinal = 0;
  snapshot->first_selector_pool_index = 0;
  snapshot->last_selector_pool_index = 0;
  snapshot->canonical_selector = nullptr;

  if (selector == nullptr || selector[0] == '\0') {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  }

  RuntimeState &state = ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  const auto found = state.selector_index_by_name.find(selector);
  if (found == state.selector_index_by_name.end()) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  }

  const SelectorSlot &slot = state.selector_slots[found->second];
  snapshot->found = 1;
  snapshot->metadata_backed = slot.metadata_backed ? 1 : 0;
  snapshot->stable_id = slot.handle.stable_id;
  snapshot->metadata_provider_count = slot.metadata_provider_count;
  snapshot->first_registration_order_ordinal =
      slot.first_registration_order_ordinal;
  snapshot->last_registration_order_ordinal =
      slot.last_registration_order_ordinal;
  snapshot->first_selector_pool_index = slot.first_selector_pool_index;
  snapshot->last_selector_pool_index = slot.last_selector_pool_index;
  snapshot->canonical_selector = slot.handle.selector;
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

// live-optional-send-and-keypath-runtime-support anchor: typed
// key-path runtime support currently materializes emitted single-component
// descriptor handles into a private runtime registry plus narrow probe helpers.
// Optional sends stay on public lookup/dispatch; full multi-component key-path
// evaluation remains deferred.
// cross-module type-surface preservation anchor: when multiple images
// register through imported runtime surfaces, the same registry path remains
// authoritative so imported typed key-path descriptors survive startup
// registration without changing handle semantics.
int objc3_runtime_copy_keypath_registry_state_for_testing(
    objc3_runtime_keypath_registry_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  RuntimeState &state = ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  snapshot->keypath_table_entry_count =
      static_cast<std::uint64_t>(state.keypath_slots.size());
  snapshot->image_backed_keypath_count = state.image_backed_keypath_count;
  snapshot->ambiguous_keypath_handle_count =
      state.ambiguous_keypath_handle_count;
  snapshot->last_materialized_handle = state.last_materialized_keypath_handle;
  snapshot->last_materialized_registration_order_ordinal =
      state.last_materialized_keypath_registration_order_ordinal;
  snapshot->last_queried_handle = state.last_queried_keypath_handle;
  snapshot->last_query_found = state.last_keypath_query_found ? 1 : 0;
  snapshot->last_query_ambiguous = state.last_keypath_query_ambiguous ? 1 : 0;
  snapshot->last_materialized_profile =
      BorrowRuntimeCString(state.last_materialized_keypath_profile);
  snapshot->last_resolved_profile =
      BorrowRuntimeCString(state.last_resolved_keypath_profile);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

int objc3_runtime_copy_keypath_entry_for_testing(
    std::uint64_t stable_id, objc3_runtime_keypath_entry_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  snapshot->found = 0;
  snapshot->ambiguous = 0;
  snapshot->root_is_self = 0;
  snapshot->stable_id = stable_id;
  snapshot->component_count = 0;
  snapshot->metadata_provider_count = 0;
  snapshot->first_registration_order_ordinal = 0;
  snapshot->last_registration_order_ordinal = 0;
  snapshot->root_name = nullptr;
  snapshot->component_path = nullptr;
  snapshot->profile = nullptr;
  snapshot->generic_metadata_replay_key = nullptr;

  RuntimeState &state = ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  state.last_queried_keypath_handle = stable_id;
  state.last_keypath_query_found = false;
  state.last_keypath_query_ambiguous = false;
  state.last_resolved_keypath_profile.clear();
  const auto found = state.keypath_slots.find(stable_id);
  if (found == state.keypath_slots.end()) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  }

  const KeyPathSlot &slot = found->second;
  state.last_keypath_query_found = true;
  state.last_keypath_query_ambiguous = slot.ambiguous;
  state.last_resolved_keypath_profile = slot.profile_storage;
  snapshot->found = 1;
  snapshot->ambiguous = slot.ambiguous ? 1 : 0;
  snapshot->root_is_self = slot.root_is_self ? 1 : 0;
  snapshot->component_count = slot.component_count;
  snapshot->metadata_provider_count = slot.metadata_provider_count;
  snapshot->first_registration_order_ordinal =
      slot.first_registration_order_ordinal;
  snapshot->last_registration_order_ordinal =
      slot.last_registration_order_ordinal;
  snapshot->root_name = BorrowRuntimeCString(slot.root_name_storage);
  snapshot->component_path = BorrowRuntimeCString(slot.component_path_storage);
  snapshot->profile = BorrowRuntimeCString(slot.profile_storage);
  snapshot->generic_metadata_replay_key =
      slot.generic_metadata_replay_key_storage.empty()
          ? nullptr
          : BorrowRuntimeCString(slot.generic_metadata_replay_key_storage);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

int objc3_runtime_keypath_component_count_for_testing(int keypath_handle) {
  if (keypath_handle <= 0 ||
      !objc3c::runtime::RuntimeKeyPathHandleIsValid(
          static_cast<std::uint64_t>(keypath_handle))) {
    return 0;
  }

  RuntimeState &state = ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  const auto found =
      state.keypath_slots.find(static_cast<std::uint64_t>(keypath_handle));
  state.last_queried_keypath_handle =
      static_cast<std::uint64_t>(keypath_handle);
  state.last_keypath_query_found = found != state.keypath_slots.end();
  state.last_keypath_query_ambiguous =
      found != state.keypath_slots.end() && found->second.ambiguous;
  state.last_resolved_keypath_profile =
      found != state.keypath_slots.end() ? found->second.profile_storage : "";
  if (found == state.keypath_slots.end() || found->second.ambiguous) {
    return 0;
  }
  return static_cast<int>(found->second.component_count);
}

int objc3_runtime_keypath_root_is_self_for_testing(int keypath_handle) {
  if (keypath_handle <= 0 ||
      !objc3c::runtime::RuntimeKeyPathHandleIsValid(
          static_cast<std::uint64_t>(keypath_handle))) {
    return 0;
  }

  RuntimeState &state = ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  const auto found =
      state.keypath_slots.find(static_cast<std::uint64_t>(keypath_handle));
  state.last_queried_keypath_handle =
      static_cast<std::uint64_t>(keypath_handle);
  state.last_keypath_query_found = found != state.keypath_slots.end();
  state.last_keypath_query_ambiguous =
      found != state.keypath_slots.end() && found->second.ambiguous;
  state.last_resolved_keypath_profile =
      found != state.keypath_slots.end() ? found->second.profile_storage : "";
  if (found == state.keypath_slots.end() || found->second.ambiguous) {
    return 0;
  }
  return found->second.root_is_self ? 1 : 0;
}

int objc3_runtime_copy_method_cache_state_for_testing(
    objc3_runtime_method_cache_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  RuntimeState &state = ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  snapshot->cache_entry_count =
      static_cast<std::uint64_t>(state.method_cache.size());
  snapshot->cache_hit_count = state.method_cache_hit_count;
  snapshot->cache_miss_count = state.method_cache_miss_count;
  snapshot->slow_path_lookup_count = state.slow_path_lookup_count;
  snapshot->live_dispatch_count = state.live_dispatch_count;
  snapshot->strict_dispatch_error_count = state.strict_dispatch_error_count;
  snapshot->fast_path_seed_count = state.fast_path_seed_count;
  snapshot->fast_path_hit_count = state.fast_path_hit_count;
  snapshot->last_selector_stable_id = state.last_dispatch_selector_stable_id;
  snapshot->last_normalized_receiver_identity =
      state.last_dispatch_normalized_receiver_identity;
  snapshot->last_category_probe_count = state.last_category_probe_count;
  snapshot->last_protocol_probe_count = state.last_protocol_probe_count;
  snapshot->last_dispatch_used_cache = state.last_dispatch_used_cache ? 1 : 0;
  snapshot->last_dispatch_used_fast_path =
      state.last_dispatch_used_fast_path ? 1 : 0;
  snapshot->last_dispatch_resolved_live_method =
      state.last_dispatch_resolved_live_method ? 1 : 0;
  snapshot->last_dispatch_strict_error = state.last_dispatch_strict_error ? 1 : 0;
  snapshot->last_selector = BorrowRuntimeCString(state.last_dispatch_selector);
  snapshot->last_fast_path_reason = BorrowRuntimeCString(state.last_fast_path_reason);
  snapshot->last_resolved_class_name =
      BorrowRuntimeCString(state.last_resolved_class_name);
  snapshot->last_resolved_owner_identity =
      BorrowRuntimeCString(state.last_resolved_owner_identity);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

int objc3_runtime_copy_dispatch_state_for_testing(
    objc3_runtime_dispatch_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

