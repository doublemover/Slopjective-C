    }
  }
  return true;
}

bool CollectPreferredCategoryRecordsForImage(
    const RegisteredImageMetadata &record, const std::string &class_name,
    std::vector<const EmittedCategoryRecord *> &preferred_records) {
  // class-realization-runtime freeze anchor: category attachment is
  // resolved from emitted category records only after one concrete class name
  // has been selected. Runtime prefers implementation records over interface
  // records per category name and fails closed on conflicting attachments.
  std::unordered_map<std::string, const EmittedCategoryRecord *> grouped_records;
  grouped_records.reserve(static_cast<std::size_t>(record.category_descriptor_count));
  for (std::uint64_t index = 0; index < record.category_descriptor_count; ++index) {
    const auto *category_record = static_cast<const EmittedCategoryRecord *>(
        AggregateEntry(record.category_descriptor_root, index));
    if (category_record == nullptr || category_record->class_name == nullptr ||
        category_record->category_name == nullptr ||
        category_record->record_kind == nullptr ||
        category_record->owner_identity == nullptr) {
      return false;
    }
    if (class_name != category_record->class_name) {
      continue;
    }
    const std::string key = category_record->category_name;
    const bool is_implementation =
        std::string(category_record->record_kind) == "implementation";
    auto existing = grouped_records.find(key);
    if (existing == grouped_records.end()) {
      grouped_records.emplace(key, category_record);
      continue;
    }
    const bool existing_is_implementation =
        std::string(existing->second->record_kind) == "implementation";
    if (existing_is_implementation == is_implementation &&
        std::string(existing->second->owner_identity) !=
            std::string(category_record->owner_identity)) {
      return false;
    }
    if (!existing_is_implementation && is_implementation) {
      existing->second = category_record;
    }
  }
  preferred_records.clear();
  preferred_records.reserve(grouped_records.size());
  for (const auto &entry : grouped_records) {
    preferred_records.push_back(entry.second);
  }
  std::sort(preferred_records.begin(), preferred_records.end(),
            [](const EmittedCategoryRecord *lhs, const EmittedCategoryRecord *rhs) {
              return std::make_tuple(std::string(lhs->class_name),
                                     std::string(lhs->category_name),
                                     std::string(lhs->record_kind),
                                     std::string(lhs->owner_identity)) <
                     std::make_tuple(std::string(rhs->class_name),
                                     std::string(rhs->category_name),
                                     std::string(rhs->record_kind),
                                     std::string(rhs->owner_identity));
            });
  return true;
}

bool ProbeProtocolSelectorDeclarationsFromAggregateUnlocked(
    RuntimeState &state,
    const objc3_runtime_pointer_aggregate *protocol_refs, DispatchFamily family,
    std::uint64_t selector_stable_id, const char *selector_spelling,
    std::uint64_t &protocol_probe_count, bool &ambiguous) {
  if (protocol_refs == nullptr) {
    return true;
  }
  for (std::uint64_t index = 0; index < protocol_refs->count; ++index) {
    const auto *protocol_record = static_cast<const EmittedProtocolRecord *>(
        AggregateEntry(protocol_refs, index));
    if (protocol_record == nullptr) {
      return false;
    }
    bool declared = false;
    if (!ProbeProtocolSelectorDeclarationUnlocked(
            state, protocol_record, family, selector_stable_id,
            selector_spelling, protocol_probe_count, declared, ambiguous)) {
      return false;
    }
    if (ambiguous) {
      return true;
    }
  }
  return true;
}

bool QueryProtocolConformanceFromProtocolRecordUnlocked(
    const EmittedProtocolRecord *start_record, const char *protocol_name,
    std::unordered_set<const EmittedProtocolRecord *> &visited,
    std::uint64_t &visited_protocol_count, std::string &matched_owner_identity) {
  if (start_record == nullptr || protocol_name == nullptr || protocol_name[0] == '\0') {
    return false;
  }
  std::vector<const EmittedProtocolRecord *> stack;
  stack.push_back(start_record);
  while (!stack.empty()) {
    const EmittedProtocolRecord *record = stack.back();
    stack.pop_back();
    if (record == nullptr || !visited.insert(record).second) {
      continue;
    }
    if (record->protocol_name == nullptr || record->owner_identity == nullptr) {
      return false;
    }
    ++visited_protocol_count;
    if (std::strcmp(record->protocol_name, protocol_name) == 0) {
      matched_owner_identity = record->owner_identity;
      return true;
    }
    const objc3_runtime_pointer_aggregate *inherited_refs =
        record->inherited_protocol_refs;
    if (inherited_refs == nullptr) {
      continue;
    }
    for (std::uint64_t index = 0; index < inherited_refs->count; ++index) {
      const auto *inherited_record = static_cast<const EmittedProtocolRecord *>(
          AggregateEntry(inherited_refs, index));
      if (inherited_record == nullptr) {
        return false;
      }
      stack.push_back(inherited_record);
    }
  }
  return false;
}

bool QueryProtocolConformanceFromAggregateUnlocked(
    const objc3_runtime_pointer_aggregate *protocol_refs,
    const char *protocol_name,
    std::unordered_set<const EmittedProtocolRecord *> &visited,
    std::uint64_t &visited_protocol_count, std::string &matched_owner_identity) {
  if (protocol_refs == nullptr) {
    return false;
  }
  for (std::uint64_t index = 0; index < protocol_refs->count; ++index) {
    const auto *protocol_record = static_cast<const EmittedProtocolRecord *>(
        AggregateEntry(protocol_refs, index));
    if (protocol_record == nullptr) {
      return false;
    }
    if (QueryProtocolConformanceFromProtocolRecordUnlocked(
            protocol_record, protocol_name, visited, visited_protocol_count,
            matched_owner_identity)) {
      return true;
    }
  }
  return false;
}

bool AttachRealizedCategoryRecordsUnlocked(RuntimeState &state,
                                           RealizedClassNode &node) {
  // category-attachment-protocol-conformance anchor: realized class
  // nodes now own preferred category attachments and direct protocol edges so
  // later dispatch/query paths consume the published graph instead of
  // rediscovering category/protocol relationships on each lookup.
  node.attached_category_records.clear();
  node.runtime_attachment_ready = false;
  if (node.image == nullptr) {
    return false;
  }
  std::vector<const EmittedCategoryRecord *> category_records;
  if (!CollectPreferredCategoryRecordsForImage(*node.image, node.class_name,
                                               category_records)) {
    return false;
  }
  for (const EmittedCategoryRecord *category_record : category_records) {
    if (category_record == nullptr || category_record->class_owner_identity == nullptr ||
        category_record->category_owner_identity == nullptr ||
        category_record->owner_identity == nullptr) {
      return false;
    }
    if (node.class_owner_identity != category_record->class_owner_identity) {
      return false;
    }
    node.attached_category_records.push_back(category_record);
    state.last_attached_category_owner_identity =
        category_record->category_owner_identity;
    state.last_attached_category_name = category_record->category_name != nullptr
                                            ? category_record->category_name
                                            : "";
    if (category_record->adopted_protocol_refs != nullptr) {
      state.realized_protocol_conformance_edge_count +=
          category_record->adopted_protocol_refs->count;
    }
  }
  state.realized_attached_category_count +=
      static_cast<std::uint64_t>(node.attached_category_records.size());
  if (node.bundle != nullptr &&
      node.bundle->class_record.adopted_protocol_refs != nullptr) {
    state.realized_protocol_conformance_edge_count +=
        node.bundle->class_record.adopted_protocol_refs->count;
  }
  node.runtime_attachment_ready = true;
  return true;
}

RuntimeMethodReturnKind ClassifySynthesizedPropertyReturnType(
    const EmittedPropertyDescriptor &descriptor) {
  const std::string type_name =
      descriptor.type_name != nullptr ? descriptor.type_name : "";
  if (type_name == "bool" || type_name == "BOOL") {
    return RuntimeMethodReturnKind::Bool;
  }
  if (!type_name.empty()) {
    return RuntimeMethodReturnKind::I32Like;
  }
  return RuntimeMethodReturnKind::Unsupported;
}

bool AttachRealizedPropertyLayoutRecordsUnlocked(RuntimeState &state,
                                                 RealizedClassNode &node) {
  // instance-allocation-layout-runtime anchor: realized classes now
  // eagerly consume emitted property and ivar metadata into a runtime-owned
  // layout/accessor view so alloc/new and synthesized accessors can execute
  // against per-instance storage instead of lane-C globals.
  (void)state;
  node.runtime_property_accessors.clear();
  node.runtime_layout_ready = false;
  node.runtime_instance_size_bytes = 0;
  if (node.image == nullptr || node.image->property_descriptor_root == nullptr ||
      node.image->ivar_descriptor_root == nullptr ||
      node.bundle_owner_identity.empty()) {
    return false;
  }
  const std::string ivar_owner_identity =
      node.interface_owner_identity.empty() ? node.bundle_owner_identity
                                            : node.interface_owner_identity;
  std::unordered_map<std::string, const EmittedIvarDescriptor *> ivar_by_binding;
  std::unordered_map<std::string, const EmittedIvarDescriptor *> ivar_by_property;
  std::size_t max_end = 0u;
  std::size_t max_alignment = 1u;
  std::size_t published_owner_size = 0u;
  for (std::uint64_t index = 0; index < node.image->ivar_descriptor_count; ++index) {
    const auto *descriptor = static_cast<const EmittedIvarDescriptor *>(
        AggregateEntry(node.image->ivar_descriptor_root, index));
    if (descriptor == nullptr || descriptor->declaration_owner_identity == nullptr ||
        descriptor->property_name == nullptr || descriptor->ivar_binding_symbol == nullptr) {
      return false;
    }
    if (ivar_owner_identity != descriptor->declaration_owner_identity) {
      continue;
    }
    if (descriptor->layout_record == nullptr || !descriptor->layout_valid ||
        !descriptor->layout_record->layout_valid ||
        descriptor->layout_record->layout_replay_key == nullptr ||
        descriptor->layout_record->layout_replay_key[0] == '\0' ||
        descriptor->layout_replay_key == nullptr ||
        descriptor->layout_replay_key[0] == '\0') {
      return false;
    }
    const std::size_t offset = descriptor->offset_global != nullptr
                                   ? static_cast<std::size_t>(*descriptor->offset_global)
                                   : static_cast<std::size_t>(descriptor->offset_bytes);
    const std::size_t size = static_cast<std::size_t>(descriptor->size_bytes);
    const std::size_t alignment =
        std::max<std::size_t>(static_cast<std::size_t>(descriptor->alignment_bytes),
                              1u);
    if (descriptor->layout_record->offset_bytes != descriptor->offset_bytes ||
        descriptor->layout_record->size_bytes != descriptor->size_bytes ||
        descriptor->layout_record->alignment_bytes !=
            descriptor->alignment_bytes ||
        descriptor->layout_record->padding_bytes != descriptor->padding_bytes ||
        descriptor->layout_record->inherited_slot_count !=
            descriptor->inherited_slot_count ||
        descriptor->layout_record->inherited_size_bytes !=
            descriptor->inherited_size_bytes ||
        descriptor->layout_record->owner_size_bytes !=
            descriptor->owner_size_bytes ||
        descriptor->layout_record->init_order_index !=
            descriptor->init_order_index ||
        descriptor->layout_record->destroy_order_index !=
            descriptor->destroy_order_index ||
        offset != static_cast<std::size_t>(descriptor->offset_bytes) ||
        (alignment != 0u && offset % alignment != 0u)) {
      return false;
    }
    if (size != 0u) {
      max_end = std::max(max_end, offset + size);
      max_alignment = std::max(max_alignment, alignment);
    }
    published_owner_size =
        std::max<std::size_t>(published_owner_size,
                              static_cast<std::size_t>(
                                  descriptor->owner_size_bytes));
    ivar_by_binding.emplace(descriptor->ivar_binding_symbol, descriptor);
    ivar_by_property.emplace(descriptor->property_name, descriptor);
  }
  node.runtime_instance_size_bytes =
      std::max<std::size_t>(published_owner_size, AlignTo(max_end, max_alignment));

  for (std::uint64_t index = 0; index < node.image->property_descriptor_count; ++index) {
    const auto *descriptor = static_cast<const EmittedPropertyDescriptor *>(
        AggregateEntry(node.image->property_descriptor_root, index));
    if (descriptor == nullptr || descriptor->declaration_owner_identity == nullptr ||
        descriptor->property_name == nullptr ||
        descriptor->effective_getter_selector == nullptr) {
      return false;
    }
    if (node.bundle_owner_identity != descriptor->declaration_owner_identity) {
      continue;
    }
    if (descriptor->synthesized_binding_symbol == nullptr ||
        descriptor->synthesized_binding_symbol[0] == '\0') {
      continue;
    }
    const EmittedIvarDescriptor *ivar_descriptor = nullptr;
    if (descriptor->ivar_binding_symbol != nullptr &&
        descriptor->ivar_binding_symbol[0] != '\0') {
      const auto ivar_by_binding_it =
          ivar_by_binding.find(descriptor->ivar_binding_symbol);
      if (ivar_by_binding_it != ivar_by_binding.end()) {
        ivar_descriptor = ivar_by_binding_it->second;
      }
    }
    if (ivar_descriptor == nullptr) {
      const auto ivar_by_property_it =
          ivar_by_property.find(descriptor->property_name);
      if (ivar_by_property_it != ivar_by_property.end()) {
        ivar_descriptor = ivar_by_property_it->second;
      }
    }
    if (ivar_descriptor == nullptr) {
      continue;
    }
    RealizedPropertyAccessor accessor;
    accessor.property_descriptor = descriptor;
    accessor.ivar_descriptor = ivar_descriptor;
    accessor.getter_return_kind =
        ClassifySynthesizedPropertyReturnType(*descriptor);
    accessor.getter_owner_identity = BuildSynthesizedInstanceMethodOwnerIdentity(
        descriptor->declaration_owner_identity,
        descriptor->effective_getter_selector);
    if (descriptor->effective_setter_available &&
        descriptor->effective_setter_selector != nullptr &&
        descriptor->effective_setter_selector[0] != '\0') {
      accessor.setter_owner_identity = BuildSynthesizedInstanceMethodOwnerIdentity(
          descriptor->declaration_owner_identity,
          descriptor->effective_setter_selector);
    }
    node.runtime_property_accessors.push_back(std::move(accessor));
  }

  std::sort(node.runtime_property_accessors.begin(),
            node.runtime_property_accessors.end(),
            [](const RealizedPropertyAccessor &lhs,
               const RealizedPropertyAccessor &rhs) {
              const std::string lhs_owner =
                  lhs.property_descriptor != nullptr &&
                          lhs.property_descriptor->declaration_owner_identity != nullptr
                      ? lhs.property_descriptor->declaration_owner_identity
                      : "";
              const std::string rhs_owner =
                  rhs.property_descriptor != nullptr &&
                          rhs.property_descriptor->declaration_owner_identity != nullptr
                      ? rhs.property_descriptor->declaration_owner_identity
                      : "";
              const std::string lhs_name =
                  lhs.property_descriptor != nullptr &&
                          lhs.property_descriptor->property_name != nullptr
                      ? lhs.property_descriptor->property_name
                      : "";
              const std::string rhs_name =
                  rhs.property_descriptor != nullptr &&
                          rhs.property_descriptor->property_name != nullptr
                      ? rhs.property_descriptor->property_name
                      : "";
              return std::tie(lhs_owner, lhs_name, lhs.getter_owner_identity,
                              lhs.setter_owner_identity) <
                     std::tie(rhs_owner, rhs_name, rhs.getter_owner_identity,
                              rhs.setter_owner_identity);
            });
  node.runtime_layout_ready = !node.runtime_property_accessors.empty() ||
                              node.runtime_instance_size_bytes != 0u;
  return true;
}

bool TryResolveRuntimeManagedPropertyAccessorUnlocked(
    const RuntimeState &state, const RealizedClassNode &start_node,
    std::uint64_t normalized_receiver_identity, std::uint64_t selector_stable_id,
    const char *selector_spelling, SlowPathResolution &resolution) {
  if (selector_spelling == nullptr || selector_spelling[0] == '\0') {
    return false;
  }
  std::unordered_set<const RealizedClassNode *> visited;
  const RealizedClassNode *node = &start_node;
  while (node != nullptr && visited.insert(node).second) {
    if (node->runtime_layout_ready) {
      for (const RealizedPropertyAccessor &accessor :
           node->runtime_property_accessors) {
        if (accessor.property_descriptor == nullptr ||
            accessor.ivar_descriptor == nullptr ||
            accessor.getter_return_kind == RuntimeMethodReturnKind::Unsupported) {
          continue;
        }
        if (accessor.property_descriptor->effective_getter_selector != nullptr &&
            std::strcmp(accessor.property_descriptor->effective_getter_selector,
                        selector_spelling) == 0) {
          resolution.resolved = true;
          resolution.dispatch_family_is_class = false;
          resolution.selector_storage = selector_spelling;
          resolution.class_name = node->class_name;
          resolution.owner_identity = accessor.getter_owner_identity;
          resolution.normalized_receiver_identity = normalized_receiver_identity;
          resolution.selector_stable_id = selector_stable_id;
          resolution.parameter_count = 0;
          resolution.return_kind = accessor.getter_return_kind;
          resolution.implementation = nullptr;
          resolution.builtin_kind = RuntimeBuiltinKind::PropertyGetter;
          resolution.runtime_property_accessor = &accessor;
          return true;
        }
        if (accessor.property_descriptor->effective_setter_available &&
            accessor.property_descriptor->effective_setter_selector != nullptr &&
            std::strcmp(accessor.property_descriptor->effective_setter_selector,
                        selector_spelling) == 0) {
          resolution.resolved = true;
          resolution.dispatch_family_is_class = false;
          resolution.selector_storage = selector_spelling;
          resolution.class_name = node->class_name;
          resolution.owner_identity = accessor.setter_owner_identity;
          resolution.normalized_receiver_identity = normalized_receiver_identity;
          resolution.selector_stable_id = selector_stable_id;
          resolution.parameter_count = 1;
          resolution.return_kind = RuntimeMethodReturnKind::Void;
          resolution.implementation = nullptr;
          resolution.builtin_kind = RuntimeBuiltinKind::PropertySetter;
          resolution.runtime_property_accessor = &accessor;
          return true;
        }
      }
    }
    node = node->has_super_node ? &state.realized_class_nodes[node->super_node_index]
                                : nullptr;
  }
  return false;
}

const RealizedPropertyAccessor *FindRuntimePropertyAccessorByNameUnlocked(
    RuntimeState &state, const RealizedClassNode &start_node,
    const char *property_name, const RealizedClassNode *&resolved_node,
    bool &inherited, bool &used_cache) {
  // property-metadata-reflection anchor: private reflective helper
  // lookup walks the realized runtime-owned property graph by class/property
  // name instead of rediscovering accessors or layout from source.
  if (property_name == nullptr || property_name[0] == '\0') {
    return nullptr;
  }
  PropertyLookupCacheKey cache_key;
  cache_key.start_base_identity = start_node.base_identity;
  cache_key.property_name = property_name;
  const auto cache_it = state.property_lookup_cache.find(cache_key);
  if (cache_it != state.property_lookup_cache.end()) {
    used_cache = true;
    ++state.property_lookup_cache_hit_count;
    if (!cache_it->second.found) {
      inherited = false;
      resolved_node = nullptr;
      return nullptr;
    }
    if (cache_it->second.resolved_node_index < state.realized_class_nodes.size()) {
      const RealizedClassNode &cached_node =
          state.realized_class_nodes[cache_it->second.resolved_node_index];
      if (cache_it->second.accessor_index <
          cached_node.runtime_property_accessors.size()) {
        resolved_node = &cached_node;
        inherited = cache_it->second.inherited;
        return &cached_node.runtime_property_accessors[cache_it->second.accessor_index];
      }
    }
    state.property_lookup_cache.erase(cache_it);
  }
  used_cache = false;
  ++state.property_lookup_cache_miss_count;
  std::unordered_set<const RealizedClassNode *> visited;
  const RealizedClassNode *node = &start_node;
  inherited = false;
  while (node != nullptr && visited.insert(node).second) {
    if (node->runtime_layout_ready) {
      for (const RealizedPropertyAccessor &accessor :
           node->runtime_property_accessors) {
        if (accessor.property_descriptor == nullptr ||
            accessor.property_descriptor->property_name == nullptr) {
          continue;
        }
        if (std::strcmp(accessor.property_descriptor->property_name,
                        property_name) == 0) {
          resolved_node = node;
          inherited = node != &start_node;
          const std::size_t resolved_node_index = static_cast<std::size_t>(
              node - state.realized_class_nodes.data());
          const std::size_t accessor_index = static_cast<std::size_t>(
              &accessor - node->runtime_property_accessors.data());
          state.property_lookup_cache.emplace(
              std::move(cache_key),
              PropertyLookupCacheEntry{true, inherited, resolved_node_index,
                                       accessor_index});
          return &accessor;
        }
      }
    }
    node = node->has_super_node ? &state.realized_class_nodes[node->super_node_index]
                                : nullptr;
  }
  state.property_lookup_cache.emplace(std::move(cache_key),
                                      PropertyLookupCacheEntry{});
  return nullptr;
}

bool TryResolveMethodFromAttachedCategoriesUnlocked(
    RuntimeState &state, const RealizedClassNode &node, DispatchFamily family,
    std::uint64_t normalized_receiver_identity, std::uint64_t selector_stable_id,
    const char *selector_spelling, SlowPathResolution &resolution,
    bool &ambiguous, std::uint64_t &category_probe_count,
    std::uint64_t &protocol_probe_count) {
  // protocol/category-aware resolution anchor: once class bodies miss,
  // preferred category implementation records become the next live method tier
  // and adopted/inherited protocol records provide declaration-aware negative
  // lookup evidence for unsupported selectors.
  if (node.class_name.empty() || !node.runtime_attachment_ready) {
    return false;
  }
  for (const EmittedCategoryRecord *category_record :
       node.attached_category_records) {
    ++category_probe_count;
    if (!TryResolveMethodFromMethodListRefUnlocked(
            state, SelectMethodListRef(*category_record, family),
            node.class_name.c_str(),
            family, normalized_receiver_identity, selector_stable_id,
            selector_spelling, resolution, ambiguous)) {
      return false;
    }
    if (ambiguous || resolution.resolved) {
      return true;
    }
    if (!ProbeProtocolSelectorDeclarationsFromAggregateUnlocked(
            state, category_record->adopted_protocol_refs, family,
            selector_stable_id, selector_spelling, protocol_probe_count,
            ambiguous)) {
      return false;
    }
    if (ambiguous) {
      return true;
    }
  }
  return true;
}

bool IsImplementationOwnerIdentity(const char *owner_identity) {
  if (owner_identity == nullptr) {
    return false;
  }
  static constexpr char kImplementationPrefix[] = "implementation:";
  return std::string(owner_identity).rfind(kImplementationPrefix, 0) == 0;
}

RuntimeMethodReturnKind ClassifyRuntimeReturnType(const char *return_type_name) {
  if (return_type_name == nullptr) {
    return RuntimeMethodReturnKind::Unsupported;
  }
  const std::string type_name = return_type_name;
  if (type_name == "void") {
    return RuntimeMethodReturnKind::Void;
  }
  if (type_name == "bool") {
    return RuntimeMethodReturnKind::Bool;
  }
  if (type_name.empty()) {
    return RuntimeMethodReturnKind::Unsupported;
  }
  return RuntimeMethodReturnKind::I32Like;
}

bool TryResolveRuntimeBuiltinObjectSampleMethod(
    const std::string &class_name, DispatchFamily family,
    const char *selector_spelling, SlowPathResolution &resolution) {
  if (class_name.empty() || selector_spelling == nullptr ||
      selector_spelling[0] == '\0') {
    return false;
  }
  const std::string selector = selector_spelling;
  // runtime property/layout consumption freeze anchor: builtin
  // alloc/new and synthesized property access remain on the runtime-owned
  // lookup/dispatch surface instead of rederiving storage from source.
  // instance-allocation-layout-runtime anchor: builtin alloc/new now
  // materialize distinct runtime instance identities backed by realized class
  // layout, and synthesized property accessors execute against per-instance
  // storage instead of lane-C globals.
  if (family == DispatchFamily::Class &&
      (selector == "alloc" || selector == "new")) {
    resolution.resolved = true;
    resolution.dispatch_family_is_class = true;
    resolution.class_name = class_name;
    resolution.selector_storage = selector;
    resolution.owner_identity =
        "runtime-builtin:" + class_name + "::class_method:" + selector;
    resolution.parameter_count = 0;
    resolution.implementation = nullptr;
    resolution.builtin_kind =
        selector == "alloc" ? RuntimeBuiltinKind::Alloc : RuntimeBuiltinKind::New;
    return true;
  }
  if (family == DispatchFamily::Instance && selector == "init") {
    resolution.resolved = true;
    resolution.dispatch_family_is_class = false;
    resolution.class_name = class_name;
    resolution.selector_storage = selector;
    resolution.owner_identity =
        "runtime-builtin:" + class_name + "::instance_method:init";
    resolution.parameter_count = 0;
    resolution.implementation = nullptr;
    resolution.builtin_kind = RuntimeBuiltinKind::Init;
    return true;
  }
  return false;
}

bool DecodeReceiverIdentity(const RuntimeState &state, int receiver,
                            std::uint64_t &base_identity,
                            DispatchFamily &family,
                            std::uint64_t &normalized_receiver_identity) {
  if (receiver <= 0) {
    return false;
  }
  const auto runtime_instance_it = state.runtime_instances_by_receiver.find(receiver);
  if (runtime_instance_it != state.runtime_instances_by_receiver.end()) {
    base_identity = runtime_instance_it->second.base_identity;
    family = DispatchFamily::Instance;
    normalized_receiver_identity = base_identity + 1u;
    return true;
  }
  const std::int64_t signed_receiver = receiver;
  if (signed_receiver < static_cast<std::int64_t>(kReceiverIdentityBase)) {
    return false;
  }
  const std::int64_t delta =
      signed_receiver - static_cast<std::int64_t>(kReceiverIdentityBase);
  const std::int64_t ordinal =
      delta / static_cast<std::int64_t>(kReceiverIdentityStride);
  const std::int64_t salt =
      delta % static_cast<std::int64_t>(kReceiverIdentityStride);
  if (ordinal < 0) {
    return false;
  }
  base_identity = static_cast<std::uint64_t>(
      static_cast<std::int64_t>(kReceiverIdentityBase) +
      ordinal * static_cast<std::int64_t>(kReceiverIdentityStride));
  switch (salt) {
    case 0:
    case 2:
      family = DispatchFamily::Class;
      normalized_receiver_identity = base_identity + 2u;
      return true;
    case 1:
      family = DispatchFamily::Instance;
      normalized_receiver_identity = base_identity + 1u;
      return true;
    default:
      return false;
  }
}

std::vector<const RegisteredImageMetadata *> OrderedRegisteredImages(
    const RuntimeState &state) {
  std::vector<const RegisteredImageMetadata *> ordered;
  ordered.reserve(state.registered_image_metadata_by_identity_key.size());
  for (const auto &entry : state.registered_image_metadata_by_identity_key) {
    ordered.push_back(&entry.second);
  }
  std::sort(
      ordered.begin(), ordered.end(),
      [](const RegisteredImageMetadata *lhs, const RegisteredImageMetadata *rhs) {
        if (lhs->registration_order_ordinal != rhs->registration_order_ordinal) {
          return lhs->registration_order_ordinal < rhs->registration_order_ordinal;
        }
        return lhs->translation_unit_identity_key < rhs->translation_unit_identity_key;
      });
  return ordered;
}

bool CollectSortedImageClassNames(const RegisteredImageMetadata &record,
                                  std::vector<std::string> &class_names) {
  std::unordered_set<std::string> seen;
  seen.reserve(static_cast<std::size_t>(record.class_descriptor_count));
  for (std::uint64_t index = 0; index < record.class_descriptor_count; ++index) {
    const auto *bundle = static_cast<const EmittedClassBundle *>(
        AggregateEntry(record.class_descriptor_root, index));
    if (bundle == nullptr || bundle->class_record.class_name == nullptr ||
        bundle->class_record.class_name[0] == '\0') {
      return false;
    }
    seen.insert(bundle->class_record.class_name);
  }
  class_names.assign(seen.begin(), seen.end());
  std::sort(class_names.begin(), class_names.end());
  return true;
}

void RebuildRealizedClassGraphUnlocked(RuntimeState &state) {
  // metaclass-graph-root-class anchor: runtime now republishes a
  // realized class/metaclass graph keyed by stable receiver base identities,
  // preserving root classes as explicit graph nodes rather than rediscovering
  // the class family from emitted bundles on every dispatch.
  ClearRealizedClassGraphUnlocked(state);

  std::vector<const RegisteredImageMetadata *> ordered_images =
      OrderedRegisteredImages(state);
  std::size_t estimated_realized_node_count = 0;
  std::unordered_set<std::string> global_class_name_set;
  for (const RegisteredImageMetadata *record : ordered_images) {
    if (record == nullptr) {
      continue;
    }
    estimated_realized_node_count +=
        static_cast<std::size_t>(record->class_descriptor_count);
  }
  for (const RegisteredImageMetadata *record : ordered_images) {
    std::vector<std::string> class_names;
    if (!CollectSortedImageClassNames(*record, class_names)) {
      continue;
    }
    global_class_name_set.reserve(global_class_name_set.size() +
                                  class_names.size());
    for (const std::string &class_name : class_names) {
      global_class_name_set.insert(class_name);
    }
  }

  std::vector<std::string> global_class_names(global_class_name_set.begin(),
                                             global_class_name_set.end());
  std::sort(global_class_names.begin(), global_class_names.end());
  std::unordered_map<std::string, std::size_t> global_ordinal_by_class_name;
  global_ordinal_by_class_name.reserve(global_class_names.size());
  state.realized_class_name_by_base_identity.reserve(global_class_names.size());
  for (std::size_t ordinal = 0; ordinal < global_class_names.size(); ++ordinal) {
    const std::string &class_name = global_class_names[ordinal];
    global_ordinal_by_class_name.emplace(class_name, ordinal);
    const std::uint64_t base_identity = BuildReceiverBaseIdentity(ordinal);
    state.realized_class_name_by_base_identity.emplace(base_identity,
                                                       class_name);
  }
  state.receiver_class_binding_count =
      static_cast<std::uint64_t>(state.realized_class_name_by_base_identity.size());

  std::unordered_map<const EmittedClassBundle *, std::size_t> node_index_by_bundle;
  node_index_by_bundle.reserve(estimated_realized_node_count);
  state.realized_class_nodes.reserve(estimated_realized_node_count);
  state.realized_class_node_indices_by_name.reserve(global_class_names.size());
  for (const RegisteredImageMetadata *record : ordered_images) {
    std::vector<std::string> class_names;
    if (!CollectSortedImageClassNames(*record, class_names)) {
      continue;
    }
    for (const std::string &class_name : class_names) {
      const auto ordinal_it = global_ordinal_by_class_name.find(class_name);
      if (ordinal_it == global_ordinal_by_class_name.end()) {
        continue;
      }
      const auto bundles = CollectPreferredClassBundlesForImage(*record, class_name);
      for (const EmittedClassBundle *bundle : bundles) {
        if (bundle == nullptr) {
          continue;
        }
        RealizedClassNode node;
        node.module_name = record->module_name;
        node.translation_unit_identity_key = record->translation_unit_identity_key;
        node.class_name = class_name;
        node.bundle_owner_identity =
            bundle->class_record.bundle_owner_identity != nullptr
                ? bundle->class_record.bundle_owner_identity
                : "";
        node.interface_owner_identity = node.bundle_owner_identity;
        for (std::uint64_t candidate_index = 0;
             candidate_index < record->class_descriptor_count; ++candidate_index) {
          const auto *candidate = static_cast<const EmittedClassBundle *>(
              AggregateEntry(record->class_descriptor_root, candidate_index));
          if (candidate == nullptr || candidate->class_record.class_name == nullptr ||
              candidate->class_record.bundle_owner_identity == nullptr) {
            continue;
          }
          if (class_name != candidate->class_record.class_name) {
            continue;
          }
          if (std::string(candidate->class_record.bundle_owner_identity).rfind(
                  "interface:", 0) == 0) {
            node.interface_owner_identity =
                candidate->class_record.bundle_owner_identity;
            break;
          }
        }
        node.class_owner_identity =
            bundle->class_record.object_owner_identity != nullptr
                ? bundle->class_record.object_owner_identity
                : "";
        node.metaclass_owner_identity =
            bundle->metaclass_record.object_owner_identity != nullptr
                ? bundle->metaclass_record.object_owner_identity
                : "";
        node.super_class_owner_identity =
            bundle->class_record.super_owner_identity != nullptr
                ? bundle->class_record.super_owner_identity
                : "";
        node.super_metaclass_owner_identity =
            bundle->metaclass_record.super_owner_identity != nullptr
                ? bundle->metaclass_record.super_owner_identity
                : "";
        node.registration_order_ordinal = record->registration_order_ordinal;
        node.base_identity = BuildReceiverBaseIdentity(ordinal_it->second);
        node.is_root_class = bundle->class_record.super_bundle == nullptr;
        node.implementation_backed =
            (bundle->class_record.method_list_ref != nullptr &&
             IsImplementationOwnerIdentity(
                 bundle->class_record.method_list_ref->owner_identity)) ||
            (bundle->metaclass_record.method_list_ref != nullptr &&
             IsImplementationOwnerIdentity(
                 bundle->metaclass_record.method_list_ref->owner_identity));
        node.objc_final_declared = bundle->class_record.objc_final_declared;
        node.objc_sealed_declared = bundle->class_record.objc_sealed_declared;
        node.image = record;
        node.bundle = bundle;
        const std::size_t node_index = state.realized_class_nodes.size();
        state.realized_class_nodes.push_back(std::move(node));
        node_index_by_bundle.emplace(bundle, node_index);
        state.realized_class_node_indices_by_name[class_name].push_back(node_index);
      }
    }
  }

  for (std::size_t index = 0; index < state.realized_class_nodes.size(); ++index) {
    RealizedClassNode &node = state.realized_class_nodes[index];
    if (node.bundle != nullptr && node.bundle->class_record.super_bundle != nullptr) {
      const auto super_it = node_index_by_bundle.find(
          static_cast<const EmittedClassBundle *>(node.bundle->class_record.super_bundle));
      if (super_it != node_index_by_bundle.end()) {
        node.super_node_index = super_it->second;
        node.has_super_node = true;
        ++state.realized_metaclass_edge_count;
      }
    }
    if (node.is_root_class) {
      ++state.realized_root_class_count;
    }
    (void)AttachRealizedCategoryRecordsUnlocked(state, node);
    (void)AttachRealizedPropertyLayoutRecordsUnlocked(state, node);
  }

  if (!state.realized_class_nodes.empty()) {
    const RealizedClassNode &last_node = state.realized_class_nodes.back();
    state.last_realized_class_name = last_node.class_name;
    state.last_realized_class_owner_identity = last_node.class_owner_identity;
    state.last_realized_metaclass_owner_identity =
        last_node.metaclass_owner_identity;
  }
}

bool ResolveReceiverClassNameUnlocked(const RuntimeState &state,
                                      std::uint64_t base_identity,
                                      std::string &class_name,
                                      bool &ambiguous) {
  ambiguous = false;
  if (base_identity < kReceiverIdentityBase ||
      ((base_identity - kReceiverIdentityBase) % kReceiverIdentityStride) != 0) {
    return false;
  }
  if (state.ambiguous_realized_base_identities.find(base_identity) !=
      state.ambiguous_realized_base_identities.end()) {
    ambiguous = true;
    return false;
  }
  const auto found =
      state.realized_class_name_by_base_identity.find(base_identity);
  if (found == state.realized_class_name_by_base_identity.end()) {
    return false;
  }
  class_name = found->second;
  return true;
}

std::vector<const EmittedClassBundle *> CollectPreferredClassBundlesForImage(
    const RegisteredImageMetadata &record, const std::string &class_name) {
  std::vector<const EmittedClassBundle *> implementation_bundles;
  std::vector<const EmittedClassBundle *> candidate_bundles;
  implementation_bundles.reserve(static_cast<std::size_t>(record.class_descriptor_count));
  candidate_bundles.reserve(static_cast<std::size_t>(record.class_descriptor_count));
  for (std::uint64_t index = 0; index < record.class_descriptor_count; ++index) {
    const auto *bundle = static_cast<const EmittedClassBundle *>(
        AggregateEntry(record.class_descriptor_root, index));
    if (bundle == nullptr || bundle->class_record.class_name == nullptr ||
        class_name != bundle->class_record.class_name) {
      continue;
    }
    const bool implementation_backed =
        bundle->class_record.method_list_ref != nullptr &&
        IsImplementationOwnerIdentity(bundle->class_record.method_list_ref->owner_identity);
    if (implementation_backed) {
      implementation_bundles.push_back(bundle);
    } else {
      candidate_bundles.push_back(bundle);
    }
  }
  return implementation_bundles.empty() ? candidate_bundles
                                        : implementation_bundles;
}

bool TryResolveMethodFromRealizedClassChainUnlocked(
    RuntimeState &state, const RealizedClassNode *start_node,
    DispatchFamily family, std::uint64_t normalized_receiver_identity,
    std::uint64_t selector_stable_id, const char *selector_spelling,
    SlowPathResolution &resolution, bool &ambiguous,
    std::uint64_t &category_probe_count,
    std::uint64_t &protocol_probe_count) {
  // class-realization-runtime freeze anchor: runtime walks the
  // emitted class/metaclass chain directly, consults attached categories at
  // each realized class node, and preserves protocol checks as
  // declaration-aware negative evidence only.
  std::unordered_set<const RealizedClassNode *> visited;
  const RealizedClassNode *node = start_node;
  while (node != nullptr && visited.insert(node).second) {
    if (node->bundle == nullptr || node->image == nullptr) {
      return false;
    }
    const EmittedClassBundle *bundle = node->bundle;
    const EmittedClassRecord &record =
        family == DispatchFamily::Class ? bundle->metaclass_record
                                        : bundle->class_record;
    if (!TryResolveMethodFromMethodListRefUnlocked(
            state, record.method_list_ref, record.class_name, family,
            normalized_receiver_identity, selector_stable_id, selector_spelling,
            resolution, ambiguous)) {
      return false;
    }
    if (ambiguous || resolution.resolved) {
      return true;
    }
    if (!TryResolveMethodFromAttachedCategoriesUnlocked(
            state, *node, family,
            normalized_receiver_identity, selector_stable_id, selector_spelling,
            resolution, ambiguous, category_probe_count,
            protocol_probe_count)) {
      return false;
    }
    if (ambiguous || resolution.resolved) {
      return true;
    }
    if (!ProbeProtocolSelectorDeclarationsFromAggregateUnlocked(
            state, record.adopted_protocol_refs, family, selector_stable_id,
            selector_spelling, protocol_probe_count, ambiguous)) {
      return false;
    }
    if (ambiguous) {
      return true;
    }
    node = node->has_super_node ? &state.realized_class_nodes[node->super_node_index]
                                : nullptr;
  }
  return true;
}

bool HasSelectorInMethodListRefUnlocked(const EmittedMethodListRef *method_list_ref,
                                        const char *selector) {
  if (method_list_ref == nullptr || method_list_ref->count == 0 ||
      method_list_ref->method_list == nullptr || selector == nullptr ||
      selector[0] == '\0') {
    return false;
  }
  const auto *header =
      static_cast<const EmittedMethodListHeader *>(method_list_ref->method_list);
  if (header == nullptr || header->count != method_list_ref->count) {
    return false;
  }
  const EmittedMethodListEntry *entries = MethodListEntries(header);
  for (std::uint64_t index = 0; index < header->count; ++index) {
    const EmittedMethodListEntry &entry = entries[index];
    if (entry.selector != nullptr && std::strcmp(entry.selector, selector) == 0) {
      return true;
    }
  }
  return false;
}

bool HasAttachedCategorySelectorConflictUnlocked(const RealizedClassNode &node,
                                                 DispatchFamily family,
                                                 const char *selector) {
  for (const EmittedCategoryRecord *category_record : node.attached_category_records) {
    if (category_record == nullptr) {
      continue;
    }
    const EmittedMethodListRef *method_list_ref =
        SelectMethodListRef(*category_record, family);
    if (HasSelectorInMethodListRefUnlocked(method_list_ref, selector)) {
      return true;
    }
  }
  return false;
}

std::string BuildDispatchIntentFastPathReason(const RealizedClassNode &node,
                                              const EmittedClassRecord &record,
                                              const EmittedMethodListEntry &entry) {
  if (entry.effective_direct_dispatch) {
    return "direct";
  }
  if (entry.objc_final_declared) {
    return "method-final";
  }
  if (record.objc_final_declared || node.objc_final_declared) {
    return "class-final";
  }
  if (record.objc_sealed_declared || node.objc_sealed_declared) {
    return "class-sealed";
  }
  return {};
}

const char *DescribeResolvedImplementationKind(
    RuntimeBuiltinKind builtin_kind, const void *implementation) {
  switch (builtin_kind) {
    case RuntimeBuiltinKind::Alloc:
      return "builtin-alloc";
    case RuntimeBuiltinKind::Init:
      return "builtin-init";
    case RuntimeBuiltinKind::New:
      return "builtin-new";
    case RuntimeBuiltinKind::PropertyGetter:
      return "builtin-property-getter";
    case RuntimeBuiltinKind::PropertySetter:
      return "builtin-property-setter";
    case RuntimeBuiltinKind::None:
      break;
  }
  if (implementation != nullptr) {
    return "emitted-method-body";
  }
  return "";
}

std::size_t EstimateSeedableMethodEntriesForMethodList(
    const EmittedMethodListRef *method_list_ref) {
  if (method_list_ref == nullptr || method_list_ref->count == 0 ||
      method_list_ref->method_list == nullptr) {
    return 0;
  }
  const auto *header =
      static_cast<const EmittedMethodListHeader *>(method_list_ref->method_list);
  if (header == nullptr || header->count != method_list_ref->count) {
    return 0;
  }
  const EmittedMethodListEntry *entries = MethodListEntries(header);
  std::size_t count = 0;
  for (std::uint64_t index = 0; index < header->count; ++index) {
    const EmittedMethodListEntry &entry = entries[index];
    if (entry.selector == nullptr || entry.owner_identity == nullptr ||
        entry.return_type_name == nullptr || entry.has_body == 0 ||
        entry.implementation == nullptr || entry.parameter_count > 4) {
      continue;
    }
    if (ClassifyRuntimeReturnType(entry.return_type_name) ==
        RuntimeMethodReturnKind::Unsupported) {
      continue;
    }
    ++count;
  }
  return count;
}

void ReserveMethodCacheForFastPathSeedUnlocked(RuntimeState &state) {
  std::size_t reserve_count = state.method_cache.size();
  for (const RealizedClassNode &node : state.realized_class_nodes) {
    if (node.bundle == nullptr) {
      continue;
    }
    reserve_count += EstimateSeedableMethodEntriesForMethodList(
        node.bundle->class_record.method_list_ref);
    reserve_count += EstimateSeedableMethodEntriesForMethodList(
        node.bundle->metaclass_record.method_list_ref);
  }
  state.method_cache.reserve(reserve_count);
}

void SeedDispatchIntentFastPathCacheForMethodListUnlocked(
    RuntimeState &state, const RealizedClassNode &node,
    const EmittedClassRecord &record, DispatchFamily family,
    std::uint64_t normalized_receiver_identity) {
  const EmittedMethodListRef *method_list_ref = record.method_list_ref;
  if (method_list_ref == nullptr || method_list_ref->count == 0 ||
      method_list_ref->method_list == nullptr) {
    return;
  }
  const auto *header =
      static_cast<const EmittedMethodListHeader *>(method_list_ref->method_list);
  if (header == nullptr || header->count != method_list_ref->count) {
    return;
  }
  const EmittedMethodListEntry *entries = MethodListEntries(header);
  for (std::uint64_t index = 0; index < header->count; ++index) {
    const EmittedMethodListEntry &entry = entries[index];
    if (entry.selector == nullptr || entry.owner_identity == nullptr ||
        entry.return_type_name == nullptr || entry.has_body == 0 ||
        entry.implementation == nullptr || entry.parameter_count > 4) {
      continue;
    }
    const RuntimeMethodReturnKind return_kind =
        ClassifyRuntimeReturnType(entry.return_type_name);
    if (return_kind == RuntimeMethodReturnKind::Unsupported) {
      continue;
    }
    const std::string fast_path_reason =
        BuildDispatchIntentFastPathReason(node, record, entry);
    if (fast_path_reason.empty()) {
      continue;
    }
    if (HasAttachedCategorySelectorConflictUnlocked(node, family, entry.selector)) {
      continue;
    }
    const auto selector_it = state.selector_index_by_name.find(entry.selector);
    if (selector_it == state.selector_index_by_name.end()) {
      continue;
    }
    const std::uint64_t selector_stable_id =
        state.selector_slots[selector_it->second].handle.stable_id;
    if (selector_stable_id == 0) {
      continue;
    }
    const MethodCacheKey cache_key{normalized_receiver_identity, selector_stable_id};
    if (state.method_cache.find(cache_key) != state.method_cache.end()) {
      continue;
    }
    MethodCacheEntry cache_entry;
    cache_entry.resolved = true;
    cache_entry.dispatch_family_is_class = family == DispatchFamily::Class;
    cache_entry.fast_path_seeded = true;
    cache_entry.effective_direct_dispatch = entry.effective_direct_dispatch;
    cache_entry.objc_final_declared =
        entry.objc_final_declared || record.objc_final_declared ||
        node.objc_final_declared;
    cache_entry.objc_sealed_declared =
        record.objc_sealed_declared || node.objc_sealed_declared;
    cache_entry.selector_storage = entry.selector;
    cache_entry.fast_path_reason = fast_path_reason;
    cache_entry.class_name = node.class_name;
    cache_entry.owner_identity = entry.owner_identity;
    cache_entry.normalized_receiver_identity = normalized_receiver_identity;
    cache_entry.selector_stable_id = selector_stable_id;
    cache_entry.parameter_count = entry.parameter_count;
    cache_entry.return_kind = return_kind;
    cache_entry.implementation = entry.implementation;
    if (state.method_cache.emplace(cache_key, std::move(cache_entry)).second) {
      ++state.fast_path_seed_count;
    }
  }
}

void SeedDispatchIntentFastPathCacheUnlocked(RuntimeState &state) {
  // live-dispatch-fast-path anchor: registration-time class-graph
  // rebuild now pre-seeds deterministic cache entries for safe implementation-
  // backed direct/final/sealed methods so the first live dispatch can hit the
  // runtime cache without paying a slow-path lookup.
  ReserveMethodCacheForFastPathSeedUnlocked(state);
  for (const RealizedClassNode &node : state.realized_class_nodes) {
    if (node.bundle == nullptr) {
      continue;
    }
    SeedDispatchIntentFastPathCacheForMethodListUnlocked(
        state, node, node.bundle->class_record, DispatchFamily::Instance,
        node.base_identity + 1u);
    SeedDispatchIntentFastPathCacheForMethodListUnlocked(
        state, node, node.bundle->metaclass_record, DispatchFamily::Class,
        node.base_identity + 2u);
  }
}

SlowPathResolution ResolveMethodSlowPathUnlocked(
    RuntimeState &state, std::uint64_t base_identity,
    std::uint64_t normalized_receiver_identity, DispatchFamily family,
    std::uint64_t selector_stable_id, const char *selector_spelling) {
  // live lookup walks the emitted class/metaclass graph that came
  // through startup registration, resolves one deterministic class family for
  // the receiver identity, and then records the outcome in the method cache.
  SlowPathResolution resolution;
  resolution.selector_storage = selector_spelling != nullptr ? selector_spelling : "";
  resolution.normalized_receiver_identity = normalized_receiver_identity;
  resolution.selector_stable_id = selector_stable_id;
  std::uint64_t category_probe_count = 0;
  std::uint64_t protocol_probe_count = 0;

  bool receiver_ambiguous = false;
  std::string resolved_class_name;
  if (!ResolveReceiverClassNameUnlocked(state, base_identity, resolved_class_name,
                                        receiver_ambiguous)) {
    resolution.ambiguous = receiver_ambiguous;
    return resolution;
  }

  const auto realized_nodes_it =
      state.realized_class_node_indices_by_name.find(resolved_class_name);
  if (realized_nodes_it == state.realized_class_node_indices_by_name.end()) {
    return resolution;
  }

  for (const std::size_t node_index : realized_nodes_it->second) {
    if (node_index >= state.realized_class_nodes.size()) {
      continue;
    }
    const RealizedClassNode &node = state.realized_class_nodes[node_index];
    SlowPathResolution image_resolution;
    image_resolution.selector_storage =
        selector_spelling != nullptr ? selector_spelling : "";
    image_resolution.normalized_receiver_identity = normalized_receiver_identity;
    image_resolution.selector_stable_id = selector_stable_id;
    std::uint64_t image_category_probe_count = 0;
    std::uint64_t image_protocol_probe_count = 0;
    bool method_ambiguous = false;
    if (!TryResolveMethodFromRealizedClassChainUnlocked(
            state, &node, family, normalized_receiver_identity,
            selector_stable_id, selector_spelling, image_resolution,
            method_ambiguous, image_category_probe_count,
            image_protocol_probe_count)) {
      return SlowPathResolution{};
    }
    if (method_ambiguous) {
      resolution = SlowPathResolution{};
      resolution.ambiguous = true;
      resolution.selector_storage =
          selector_spelling != nullptr ? selector_spelling : "";
      resolution.normalized_receiver_identity = normalized_receiver_identity;
      resolution.selector_stable_id = selector_stable_id;
      resolution.category_probe_count =
          category_probe_count + image_category_probe_count;
      resolution.protocol_probe_count =
          protocol_probe_count + image_protocol_probe_count;
      return resolution;
    }
    category_probe_count += image_category_probe_count;
    protocol_probe_count += image_protocol_probe_count;
    if (family == DispatchFamily::Instance &&
        TryResolveRuntimeManagedPropertyAccessorUnlocked(
            state, node, normalized_receiver_identity, selector_stable_id,
            selector_spelling, image_resolution)) {
      image_resolution.category_probe_count =
          category_probe_count + image_category_probe_count;
      image_resolution.protocol_probe_count =
          protocol_probe_count + image_protocol_probe_count;
    }
    if (image_resolution.resolved) {
      image_resolution.objc_final_declared =
          image_resolution.objc_final_declared || node.objc_final_declared;
      image_resolution.objc_sealed_declared =
          image_resolution.objc_sealed_declared || node.objc_sealed_declared;
      if (image_resolution.fast_path_reason.empty()) {
        if (image_resolution.objc_final_declared) {
          image_resolution.fast_path_reason = "class-final";
        } else if (image_resolution.objc_sealed_declared) {
          image_resolution.fast_path_reason = "class-sealed";
        }
      }
      if (resolution.resolved &&
          (resolution.implementation != image_resolution.implementation ||
           resolution.parameter_count != image_resolution.parameter_count ||
           resolution.owner_identity != image_resolution.owner_identity ||
           resolution.class_name != image_resolution.class_name)) {
        resolution = SlowPathResolution{};
        resolution.ambiguous = true;
        resolution.selector_storage =
            selector_spelling != nullptr ? selector_spelling : "";
        resolution.normalized_receiver_identity = normalized_receiver_identity;
        resolution.selector_stable_id = selector_stable_id;
        resolution.category_probe_count = category_probe_count;
        resolution.protocol_probe_count = protocol_probe_count;
        return resolution;
      }
      if (!resolution.resolved) {
        resolution = image_resolution;
      }
    }
  }
  resolution.category_probe_count = category_probe_count;
  resolution.protocol_probe_count = protocol_probe_count;
  if (!resolution.resolved && !resolution.ambiguous &&
      TryResolveRuntimeBuiltinObjectSampleMethod(resolved_class_name, family,
                                                selector_spelling, resolution)) {
    resolution.normalized_receiver_identity = normalized_receiver_identity;
    resolution.selector_stable_id = selector_stable_id;
    resolution.category_probe_count = category_probe_count;
    resolution.protocol_probe_count = protocol_probe_count;
  }
  return resolution;
}

int InvokeResolvedMethod(const void *implementation,
                         RuntimeMethodReturnKind return_kind,
                         std::uint64_t parameter_count, int a0, int a1, int a2,
                         int a3) {
  switch (return_kind) {
    case RuntimeMethodReturnKind::I32Like:
      switch (parameter_count) {
        case 0:
          return reinterpret_cast<int (*)()>(
              const_cast<void *>(implementation))();
        case 1:
          return reinterpret_cast<int (*)(int)>(
              const_cast<void *>(implementation))(a0);
        case 2:
          return reinterpret_cast<int (*)(int, int)>(
              const_cast<void *>(implementation))(a0, a1);
        case 3:
          return reinterpret_cast<int (*)(int, int, int)>(
              const_cast<void *>(implementation))(a0, a1, a2);
        case 4:
          return reinterpret_cast<int (*)(int, int, int, int)>(
              const_cast<void *>(implementation))(a0, a1, a2, a3);
        default:
          return 0;
      }
    case RuntimeMethodReturnKind::Bool:
      switch (parameter_count) {
        case 0:
          return reinterpret_cast<bool (*)()>(
                     const_cast<void *>(implementation))()
                     ? 1
                     : 0;
        case 1:
          return reinterpret_cast<bool (*)(int)>(
                     const_cast<void *>(implementation))(a0)
                     ? 1
                     : 0;
        case 2:
          return reinterpret_cast<bool (*)(int, int)>(
                     const_cast<void *>(implementation))(a0, a1)
                     ? 1
                     : 0;
        case 3:
          return reinterpret_cast<bool (*)(int, int, int)>(
                     const_cast<void *>(implementation))(a0, a1, a2)
                     ? 1
                     : 0;
        case 4:
          return reinterpret_cast<bool (*)(int, int, int, int)>(
                     const_cast<void *>(implementation))(a0, a1, a2, a3)
                     ? 1
                     : 0;
        default:
          return 0;
      }
    case RuntimeMethodReturnKind::Void:
      switch (parameter_count) {
        case 0:
          reinterpret_cast<void (*)()>(const_cast<void *>(implementation))();
          return 0;
        case 1:
          reinterpret_cast<void (*)(int)>(
              const_cast<void *>(implementation))(a0);
          return 0;
        case 2:
          reinterpret_cast<void (*)(int, int)>(
              const_cast<void *>(implementation))(a0, a1);
          return 0;
        case 3:
          reinterpret_cast<void (*)(int, int, int)>(
              const_cast<void *>(implementation))(a0, a1, a2);
          return 0;
        case 4:
          reinterpret_cast<void (*)(int, int, int, int)>(
              const_cast<void *>(implementation))(a0, a1, a2, a3);
          return 0;
        default:
          return 0;
      }
    case RuntimeMethodReturnKind::Unsupported:
      return 0;
  }
  return 0;
}

const RealizedClassNode *FindRealizedClassNodeByBaseIdentityUnlocked(
    const RuntimeState &state, std::uint64_t base_identity) {
  const auto class_name_it = state.realized_class_name_by_base_identity.find(base_identity);
  if (class_name_it == state.realized_class_name_by_base_identity.end()) {
