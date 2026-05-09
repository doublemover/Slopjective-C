    }
  }
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
    if (ambiguous || resolution.resolved ||
        HasTerminalStrictDispatchError(resolution)) {
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
    resolution.return_kind = RuntimeMethodReturnKind::ObjectReference;
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
    resolution.return_kind = RuntimeMethodReturnKind::ObjectReference;
    resolution.implementation = nullptr;
    resolution.builtin_kind = RuntimeBuiltinKind::Init;
    return true;
  }
  return false;
}

bool ResolveReceiverClassNameUnlocked(const RuntimeState &state,
                                      std::uint64_t base_identity,
                                      std::string &class_name,
                                      bool &ambiguous) {
  ambiguous = false;
  if (!IsRuntimeReceiverBaseIdentity(base_identity)) {
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
    if (ambiguous || resolution.resolved ||
        HasTerminalStrictDispatchError(resolution)) {
      return true;
    }
    if (!TryResolveMethodFromAttachedCategoriesUnlocked(
            state, *node, family,
            normalized_receiver_identity, selector_stable_id, selector_spelling,
            resolution, ambiguous, category_probe_count,
            protocol_probe_count)) {
      return false;
    }
    if (ambiguous || resolution.resolved ||
        HasTerminalStrictDispatchError(resolution)) {
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
    resolution.strict_error_status =
        receiver_ambiguous ? OBJC3_RUNTIME_DISPATCH_STATUS_CATEGORY_CONFLICT
                           : OBJC3_RUNTIME_DISPATCH_STATUS_MISSING_CLASS_GRAPH;
    return resolution;
  }

  const auto realized_nodes_it =
      state.realized_class_node_indices_by_name.find(resolved_class_name);
  if (realized_nodes_it == state.realized_class_node_indices_by_name.end()) {
    resolution.strict_error_status =
        OBJC3_RUNTIME_DISPATCH_STATUS_MISSING_CLASS_GRAPH;
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
      if (image_resolution.strict_error_status !=
          OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_SELECTOR) {
        return image_resolution;
      }
      SlowPathResolution malformed_resolution;
      malformed_resolution.selector_storage =
          selector_spelling != nullptr ? selector_spelling : "";
      malformed_resolution.normalized_receiver_identity =
          normalized_receiver_identity;
      malformed_resolution.selector_stable_id = selector_stable_id;
      malformed_resolution.strict_error_status =
          OBJC3_RUNTIME_DISPATCH_STATUS_MALFORMED_METADATA;
      return malformed_resolution;
    }
    if (method_ambiguous) {
      resolution = SlowPathResolution{};
      resolution.ambiguous = true;
      resolution.strict_error_status =
          OBJC3_RUNTIME_DISPATCH_STATUS_CATEGORY_CONFLICT;
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
    if (HasTerminalStrictDispatchError(image_resolution)) {
      image_resolution.category_probe_count =
          category_probe_count + image_category_probe_count;
      image_resolution.protocol_probe_count =
          protocol_probe_count + image_protocol_probe_count;
      return image_resolution;
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
    if (HasTerminalStrictDispatchError(image_resolution)) {
      return image_resolution;
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
        resolution.strict_error_status =
            OBJC3_RUNTIME_DISPATCH_STATUS_CATEGORY_CONFLICT;
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
