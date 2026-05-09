// cross-module preservation anchor: the runtime helper behavior stays
// image-local, while the driver/link-plan layer now fail-closes imported Part 6
// replay drift before mixed-module executables can claim runnable compatibility.
// continuation/runtime-helper anchor: the first Part 7 runtime
// helper cluster now exposes deterministic logical continuation-handle
// allocation, executor handoff, and resume behavior for probes, while the
// current async lowering slice remains the non-suspending direct-call model.
// live continuation/runtime integration anchor: that same direct-call
// slice now emits live helper traffic on supported await sites, so linked
// native async fixtures execute through allocation, handoff, and resume before
// returning their final value.
// runtime-fast-path-integration anchor: Part 9 freezes the current
// mixed dispatch boundary exactly as implemented here. IR-lowered direct sites
// bypass this entrypoint, while objc_dynamic opt-out and unresolved sends still
// flow through the canonical method-cache/slow-path/strict-error surface below.
// live-dispatch-fast-path anchor: cache hits can now come from
// registration-time seeded direct/final/sealed entries, and that seeded path
// remains explicit in the runtime-owned proof counters and last-fast-path
// reason snapshot state.
objc3_runtime_dispatch_i32_result objc3_runtime_dispatch_i32_checked(
    int receiver, const char *selector, int a0, int a1, int a2, int a3) {
  RuntimeState &state = ProcessRuntimeState();
  const void *resolved_implementation = nullptr;
  const RealizedPropertyAccessor *resolved_runtime_property_accessor = nullptr;
  std::uint64_t resolved_parameter_count = 0;
  RuntimeMethodReturnKind resolved_return_kind =
      RuntimeMethodReturnKind::Unsupported;
  RuntimeBuiltinKind resolved_builtin_kind = RuntimeBuiltinKind::None;
  bool resolved_live_method = false;
  std::uint64_t receiver_base_identity = 0;
  objc3_runtime_dispatch_status_code dispatch_status =
      OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_SELECTOR;
  {
    std::lock_guard<std::mutex> lock(state.mutex);
    const objc3_runtime_selector_handle *selector_handle =
        LookupSelectorUnlocked(selector);
    state.last_dispatch_selector = selector != nullptr ? selector : "";
    state.last_dispatch_selector_stable_id =
        selector_handle != nullptr ? selector_handle->stable_id : 0;
    state.last_dispatch_normalized_receiver_identity = 0;
    state.last_category_probe_count = 0;
    state.last_protocol_probe_count = 0;
    state.last_dispatch_used_cache = false;
    state.last_dispatch_used_fast_path = false;
    state.last_dispatch_resolved_live_method = false;
    state.last_dispatch_strict_error = false;
    state.last_dispatch_effective_direct_dispatch = false;
    state.last_dispatch_used_builtin = false;
    state.last_dispatch_status_code =
        OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_SELECTOR;
    state.last_fast_path_reason.clear();
    state.last_dispatch_path.clear();
    state.last_dispatch_implementation_kind.clear();
    state.last_dispatch_return_kind.clear();
    state.last_dispatch_diagnostic_code.clear();
    state.last_dispatch_diagnostic_message.clear();
    state.last_dispatch_result_contract.clear();
    state.last_dispatch_property_name.clear();
    state.last_resolved_class_name.clear();
    state.last_resolved_owner_identity.clear();
    state.last_dispatch_parameter_count = 0;
    state.last_dispatch_property_base_identity = 0;
    state.last_dispatch_property_slot_index = 0;
    StoreDispatchResultContractUnlocked(
        state, dispatch_status, RuntimeMethodReturnKind::Unsupported);

    if (receiver != 0 && selector_handle != nullptr) {
      std::uint64_t base_identity = 0;
      std::uint64_t normalized_receiver_identity = 0;
      DispatchFamily family = DispatchFamily::Invalid;
      if (DecodeReceiverIdentity(state, receiver, base_identity, family,
                                 normalized_receiver_identity)) {
        receiver_base_identity = base_identity;
        state.last_dispatch_normalized_receiver_identity =
            normalized_receiver_identity;
        const MethodCacheKey cache_key{normalized_receiver_identity,
                                       selector_handle->stable_id};
        const auto cache_it = state.method_cache.find(cache_key);
        if (cache_it != state.method_cache.end()) {
          const MethodCacheEntry &entry = cache_it->second;
          ++state.method_cache_hit_count;
          state.last_dispatch_used_cache = true;
          state.last_dispatch_used_fast_path = entry.fast_path_seeded;
          state.last_dispatch_resolved_live_method = entry.resolved;
          state.last_dispatch_strict_error = !entry.resolved;
          state.last_dispatch_effective_direct_dispatch =
              entry.effective_direct_dispatch;
          state.last_category_probe_count = entry.category_probe_count;
          state.last_protocol_probe_count = entry.protocol_probe_count;
          state.last_fast_path_reason = entry.fast_path_reason;
          state.last_resolved_class_name = entry.class_name;
          state.last_resolved_owner_identity = entry.owner_identity;
          state.last_dispatch_parameter_count = entry.parameter_count;
          if (entry.runtime_property_accessor != nullptr &&
              entry.runtime_property_accessor->property_descriptor != nullptr) {
            state.last_dispatch_property_name =
                entry.runtime_property_accessor->property_descriptor
                        ->property_name != nullptr
                    ? entry.runtime_property_accessor->property_descriptor
                          ->property_name
                    : "";
            state.last_dispatch_property_base_identity = receiver_base_identity;
            state.last_dispatch_property_slot_index =
                entry.runtime_property_accessor->ivar_descriptor != nullptr
                    ? entry.runtime_property_accessor->ivar_descriptor->slot_index
                    : entry.runtime_property_accessor->property_descriptor
                          ->ivar_layout_slot_index;
          }
          if (entry.resolved) {
            resolved_live_method = true;
            resolved_builtin_kind = entry.builtin_kind;
            resolved_implementation = entry.implementation;
            resolved_runtime_property_accessor = entry.runtime_property_accessor;
            resolved_parameter_count = entry.parameter_count;
            resolved_return_kind = entry.return_kind;
            state.last_dispatch_used_builtin =
                entry.builtin_kind != RuntimeBuiltinKind::None;
            state.last_dispatch_path =
                entry.fast_path_seeded ? "cache-hit-fast-path"
                                       : "cache-hit-live";
            state.last_dispatch_implementation_kind =
                DescribeResolvedImplementationKind(entry.builtin_kind,
                                                   entry.implementation);
            if (entry.fast_path_seeded) {
              ++state.fast_path_hit_count;
            }
          } else {
            state.last_dispatch_path = "cache-hit-error";
            state.last_dispatch_implementation_kind = "strict-dispatch-error";
            dispatch_status = entry.strict_error_status;
            StoreDispatchResultContractUnlocked(
                state, dispatch_status, RuntimeMethodReturnKind::Unsupported);
            ++state.strict_dispatch_error_count;
          }
        } else {
          ++state.method_cache_miss_count;
          ++state.slow_path_lookup_count;
          SlowPathResolution resolution = ResolveMethodSlowPathUnlocked(
              state, base_identity, normalized_receiver_identity, family,
              selector_handle->stable_id, selector_handle->selector);
          MethodCacheEntry cache_entry;
          cache_entry.resolved = resolution.resolved;
          cache_entry.dispatch_family_is_class =
              resolution.dispatch_family_is_class;
          cache_entry.effective_direct_dispatch =
              resolution.effective_direct_dispatch;
          cache_entry.objc_final_declared = resolution.objc_final_declared;
          cache_entry.objc_sealed_declared = resolution.objc_sealed_declared;
          cache_entry.selector_storage = resolution.selector_storage;
          cache_entry.fast_path_reason = resolution.fast_path_reason;
          cache_entry.class_name = resolution.class_name;
          cache_entry.owner_identity = resolution.owner_identity;
          cache_entry.normalized_receiver_identity =
              normalized_receiver_identity;
          cache_entry.selector_stable_id = selector_handle->stable_id;
          cache_entry.parameter_count = resolution.parameter_count;
          cache_entry.return_kind = resolution.return_kind;
          cache_entry.category_probe_count = resolution.category_probe_count;
          cache_entry.protocol_probe_count = resolution.protocol_probe_count;
          cache_entry.strict_error_status =
              objc3c::runtime::RuntimeStrictDispatchStatus(
                  resolution.resolved, resolution.ambiguous,
                  resolution.strict_error_status);
          cache_entry.implementation = resolution.implementation;
          cache_entry.builtin_kind = resolution.builtin_kind;
          cache_entry.runtime_property_accessor =
              resolution.runtime_property_accessor;
          state.method_cache.emplace(cache_key, std::move(cache_entry));
          state.last_dispatch_resolved_live_method = resolution.resolved;
          state.last_dispatch_strict_error = !resolution.resolved;
          state.last_dispatch_effective_direct_dispatch =
              resolution.effective_direct_dispatch;
          state.last_category_probe_count = resolution.category_probe_count;
          state.last_protocol_probe_count = resolution.protocol_probe_count;
          state.last_fast_path_reason = resolution.fast_path_reason;
          state.last_resolved_class_name = resolution.class_name;
          state.last_resolved_owner_identity = resolution.owner_identity;
          state.last_dispatch_parameter_count = resolution.parameter_count;
          if (resolution.runtime_property_accessor != nullptr &&
              resolution.runtime_property_accessor->property_descriptor !=
                  nullptr) {
            state.last_dispatch_property_name =
                resolution.runtime_property_accessor->property_descriptor
                        ->property_name != nullptr
                    ? resolution.runtime_property_accessor->property_descriptor
                          ->property_name
                    : "";
            state.last_dispatch_property_base_identity = receiver_base_identity;
            state.last_dispatch_property_slot_index =
                resolution.runtime_property_accessor->ivar_descriptor != nullptr
                    ? resolution.runtime_property_accessor->ivar_descriptor
                          ->slot_index
                    : resolution.runtime_property_accessor->property_descriptor
                          ->ivar_layout_slot_index;
          }
          if (resolution.resolved) {
            resolved_live_method = true;
            resolved_builtin_kind = resolution.builtin_kind;
            resolved_implementation = resolution.implementation;
            resolved_runtime_property_accessor =
                resolution.runtime_property_accessor;
            resolved_parameter_count = resolution.parameter_count;
            resolved_return_kind = resolution.return_kind;
            state.last_dispatch_used_builtin =
                resolution.builtin_kind != RuntimeBuiltinKind::None;
            state.last_dispatch_path = "slow-path-live";
            state.last_dispatch_implementation_kind =
                DescribeResolvedImplementationKind(resolution.builtin_kind,
                                                   resolution.implementation);
          } else {
            state.last_dispatch_path = "slow-path-error";
            state.last_dispatch_implementation_kind = "strict-dispatch-error";
            dispatch_status = cache_entry.strict_error_status;
            StoreDispatchResultContractUnlocked(
                state, dispatch_status, RuntimeMethodReturnKind::Unsupported);
            ++state.strict_dispatch_error_count;
          }
        }
      } else {
        ++state.strict_dispatch_error_count;
        state.last_dispatch_strict_error = true;
        state.last_dispatch_path = "invalid-receiver-error";
        state.last_dispatch_implementation_kind = "strict-dispatch-error";
        dispatch_status =
            OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_RECEIVER_CLASS;
        StoreDispatchResultContractUnlocked(
            state, dispatch_status, RuntimeMethodReturnKind::Unsupported);
      }
    } else if (receiver != 0) {
      ++state.strict_dispatch_error_count;
      state.last_dispatch_strict_error = true;
      state.last_dispatch_path = "unknown-selector-error";
      state.last_dispatch_implementation_kind = "strict-dispatch-error";
      dispatch_status = OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_SELECTOR;
      StoreDispatchResultContractUnlocked(
          state, dispatch_status, RuntimeMethodReturnKind::Unsupported);
    }
  }
  // runtime call ABI generation anchor: nil receiver is an explicit checked
  // dispatch status, not a value-success path for the i32 entrypoint.
  if (receiver == 0) {
    std::lock_guard<std::mutex> lock(state.mutex);
    ++state.strict_dispatch_error_count;
    state.last_dispatch_strict_error = true;
    state.last_dispatch_resolved_live_method = false;
    state.last_dispatch_path = "nil-receiver-error";
    state.last_dispatch_implementation_kind = "strict-dispatch-error";
    StoreDispatchResultContractUnlocked(
        state, OBJC3_RUNTIME_DISPATCH_STATUS_NIL_RECEIVER,
        RuntimeMethodReturnKind::Unsupported);
    return objc3c::runtime::MakeRuntimeDispatchI32Result(
        OBJC3_RUNTIME_DISPATCH_STATUS_NIL_RECEIVER, 0);
  }
  if (resolved_live_method && resolved_implementation != nullptr) {
    PushRuntimeDispatchFrame(receiver, receiver_base_identity,
                             resolved_runtime_property_accessor);
    const RuntimeTypedDispatchResult result =
        objc3c::runtime::InvokeRuntimeMethodImplementation(
            resolved_implementation, resolved_return_kind,
            resolved_parameter_count, a0, a1, a2, a3);
    const std::vector<int> autorelease_values =
        PopRuntimeDispatchFrameAutoreleaseValues();
    if (!autorelease_values.empty()) {
      std::lock_guard<std::mutex> lock(state.mutex);
      for (int value : autorelease_values) {
        ReleaseRuntimeValueUnlocked(state, value);
      }
    }
    if (!objc3c::runtime::RuntimeDispatchStatusIsSuccess(result.status_code)) {
      RecordPostResolutionStrictDispatchFailure(
          state, result.status_code, result.return_kind,
          "resolved-method-invocation-error");
      return objc3c::runtime::MakeRuntimeDispatchI32Result(result.status_code,
                                                          0);
    }
    RecordTypedDispatchSuccess(state, result.return_kind);
    return objc3c::runtime::MakeRuntimeDispatchI32Result(
        OBJC3_RUNTIME_DISPATCH_STATUS_OK, result.value);
  }
  if (resolved_live_method && resolved_builtin_kind != RuntimeBuiltinKind::None) {
    PushRuntimeDispatchFrame(receiver, receiver_base_identity,
                             resolved_runtime_property_accessor);
    const RuntimeTypedDispatchResult result = InvokeRuntimeBuiltinMethod(
        state, resolved_builtin_kind, receiver, receiver_base_identity,
        resolved_runtime_property_accessor, a0, a1, a2, a3);
    const std::vector<int> autorelease_values =
        PopRuntimeDispatchFrameAutoreleaseValues();
    if (!autorelease_values.empty()) {
      std::lock_guard<std::mutex> lock(state.mutex);
      for (int value : autorelease_values) {
        ReleaseRuntimeValueUnlocked(state, value);
      }
    }
    if (!objc3c::runtime::RuntimeDispatchStatusIsSuccess(result.status_code)) {
      RecordPostResolutionStrictDispatchFailure(
          state, result.status_code, result.return_kind,
          "runtime-builtin-invocation-error");
      return objc3c::runtime::MakeRuntimeDispatchI32Result(result.status_code,
                                                          0);
    }
    RecordTypedDispatchSuccess(state, result.return_kind);
    return objc3c::runtime::MakeRuntimeDispatchI32Result(
        OBJC3_RUNTIME_DISPATCH_STATUS_OK, result.value);
  }
  if (resolved_live_method) {
    RecordPostResolutionStrictDispatchFailure(
        state, OBJC3_RUNTIME_DISPATCH_STATUS_MALFORMED_METADATA,
        resolved_return_kind, "resolved-method-missing-callable-error");
    return objc3c::runtime::MakeRuntimeDispatchI32Result(
        OBJC3_RUNTIME_DISPATCH_STATUS_MALFORMED_METADATA, 0);
  }
  {
    std::lock_guard<std::mutex> lock(state.mutex);
    StoreDispatchResultContractUnlocked(
        state, dispatch_status, RuntimeMethodReturnKind::Unsupported);
  }
  return objc3c::runtime::MakeRuntimeDispatchI32Result(dispatch_status, 0);
}

int objc3_runtime_dispatch_i32(int receiver, const char *selector, int a0,
                               int a1, int a2, int a3) {
  const objc3_runtime_dispatch_i32_result result =
      objc3_runtime_dispatch_i32_checked(receiver, selector, a0, a1, a2, a3);
  if (!objc3c::runtime::RuntimeDispatchStatusIsSuccess(result.status_code)) {
    std::fprintf(
        stderr,
        "%s [%s]\n",
        result.diagnostic_message != nullptr ? result.diagnostic_message
                                             : "runtime dispatch failed",
        result.diagnostic_code != nullptr ? result.diagnostic_code
                                          : "O3RT000");
    std::abort();
  }
  return result.value;
}

int objc3_runtime_copy_registration_state_for_testing(
    objc3_runtime_registration_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  RuntimeState &state = ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  snapshot->registered_image_count = state.registered_image_count;
  snapshot->registered_descriptor_total = state.registered_descriptor_total;
  snapshot->next_expected_registration_order_ordinal =
      state.next_expected_registration_order_ordinal;
  snapshot->last_successful_registration_order_ordinal =
      state.last_successful_registration_order_ordinal;
  snapshot->last_registration_status = state.last_registration_status;
  (void)objc3c::runtime::RuntimeCStringSnapshotOwnershipModel();
  snapshot->last_registered_module_name =
      BorrowRuntimeCString(state.last_registered_module_name);
  snapshot->last_registered_translation_unit_identity_key =
      BorrowRuntimeCString(state.last_registered_translation_unit_identity_key);
  snapshot->last_rejected_module_name =
      BorrowRuntimeCString(state.last_rejected_module_name);
  snapshot->last_rejected_translation_unit_identity_key =
      BorrowRuntimeCString(state.last_rejected_translation_unit_identity_key);
  snapshot->last_rejected_registration_order_ordinal =
      state.last_rejected_registration_order_ordinal;
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

void objc3_runtime_reset_for_testing(void) {
  RuntimeState &state = ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  (void)objc3c::runtime::RuntimeStateLockDiscipline();
  (void)objc3c::runtime::RuntimeStateOwnershipModel();
  // live-restart-hardening anchor: teardown clears only live runtime
  // state, preserves the retained bootstrap catalog for restart, zeroes image-
  // local init cells, and advances reset-generation evidence for repeated
  // restart-cycle probes.
  if (objc3c::runtime::RuntimeResetClearsLiveExecutionState()) {
    ClearLiveRegistrationStateUnlocked(state);
  }
  state.last_reset_cleared_image_local_init_state_count =
      objc3c::runtime::RuntimeResetPreservesBootstrapCatalog()
          ? ZeroRetainedBootstrapImageLocalInitStatesUnlocked(state)
          : 0;
  ++state.reset_generation;
  state.last_replay_status = OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  state.last_replayed_image_count = 0;
  state.last_replayed_module_name.clear();
  state.last_replayed_translation_unit_identity_key.clear();
  ResetRuntimeAutoreleasepoolStateForTesting();
}
