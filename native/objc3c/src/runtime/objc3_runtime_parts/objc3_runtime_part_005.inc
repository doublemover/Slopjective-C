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
extern "C" int objc3_runtime_load_weak_current_property_i32(void) {
  ++g_runtime_arc_debug_weak_current_property_load_count;
  const int result = objc3_runtime_read_current_property_i32();
  g_runtime_arc_debug_last_weak_loaded_value = result;
  return result;
}

extern "C" void objc3_runtime_store_weak_current_property_i32(int value) {
  ++g_runtime_arc_debug_weak_current_property_store_count;
  g_runtime_arc_debug_last_weak_stored_value = value;
  objc3_runtime_write_current_property_i32(value);
}

extern "C" void objc3_runtime_store_thrown_error_i32(int *slot, int value) {
  ++g_runtime_error_bridge_store_call_count;
  g_runtime_error_bridge_last_stored_error_value = value;
  if (slot != nullptr) {
    *slot = value;
  }
}

extern "C" int objc3_runtime_load_thrown_error_i32(const int *slot) {
  ++g_runtime_error_bridge_load_call_count;
  const int value = slot != nullptr ? *slot : 0;
  g_runtime_error_bridge_last_loaded_error_value = value;
  return value;
}

extern "C" int objc3_runtime_bridge_status_error_i32(int status_value,
                                                     int mapped_error_value) {
  ++g_runtime_error_bridge_status_bridge_call_count;
  g_runtime_error_bridge_last_status_bridge_status_value = status_value;
  const int bridged_error =
      mapped_error_value != 0 ? mapped_error_value : status_value;
  g_runtime_error_bridge_last_status_bridge_error_value = bridged_error;
  return bridged_error;
}

extern "C" int objc3_runtime_bridge_nserror_error_i32(int error_value) {
  ++g_runtime_error_bridge_nserror_bridge_call_count;
  g_runtime_error_bridge_last_nserror_bridge_error_value = error_value;
  return error_value;
}

extern "C" int objc3_runtime_catch_matches_error_i32(int error_value,
                                                     int catch_kind,
                                                     int catch_all) {
  ++g_runtime_error_bridge_catch_match_call_count;
  g_runtime_error_bridge_last_catch_match_error_value = error_value;
  g_runtime_error_bridge_last_catch_match_kind = catch_kind;
  g_runtime_error_bridge_last_catch_match_is_catch_all = catch_all != 0 ? 1 : 0;
  g_runtime_error_bridge_last_catch_kind_name =
      RuntimeErrorCatchKindName(catch_kind);
  const int matches =
      catch_all != 0 ? 1 : ((error_value != 0) && RuntimeErrorCatchKindMatches(catch_kind) ? 1 : 0);
  g_runtime_error_bridge_last_catch_match_result = matches;
  return matches;
}

extern "C" int objc3_runtime_allocate_async_continuation_i32(
    int resume_entry_tag, int executor_tag) {
  ++g_runtime_async_continuation_allocation_call_count;
  const int handle = g_runtime_async_continuation_next_handle++;
  g_runtime_async_continuation_last_allocated_handle = handle;
  g_runtime_async_continuation_last_allocated_resume_entry_tag =
      resume_entry_tag;
  g_runtime_async_continuation_last_allocated_executor_tag = executor_tag;
  g_runtime_async_live_continuation_handles.insert(handle);
  return handle;
}

extern "C" int objc3_runtime_handoff_async_continuation_to_executor_i32(
    int continuation_handle, int executor_tag) {
  ++g_runtime_async_continuation_handoff_call_count;
  g_runtime_async_continuation_last_handoff_handle = continuation_handle;
  g_runtime_async_continuation_last_handoff_executor_tag = executor_tag;
  if (continuation_handle == 0 ||
      g_runtime_async_live_continuation_handles.find(continuation_handle) ==
          g_runtime_async_live_continuation_handles.end()) {
    return 0;
  }
  return continuation_handle;
}

extern "C" int objc3_runtime_resume_async_continuation_i32(
    int continuation_handle, int result_value) {
  ++g_runtime_async_continuation_resume_call_count;
  g_runtime_async_continuation_last_resume_handle = continuation_handle;
  g_runtime_async_continuation_last_resume_result_value = result_value;
  const auto found =
      g_runtime_async_live_continuation_handles.find(continuation_handle);
  if (continuation_handle == 0 ||
      found == g_runtime_async_live_continuation_handles.end()) {
    g_runtime_async_continuation_last_resume_return_value = 0;
    return 0;
  }
  g_runtime_async_live_continuation_handles.erase(found);
  g_runtime_async_continuation_last_resume_return_value = result_value;
  return result_value;
}

// live task runtime anchor: the supported Part 7 task slice now
// executes through this private helper cluster and publishes deterministic
// snapshot state for linked runtime probes without widening the public ABI.
// system-helper/runtime-contract anchor: Part 8 runtime/helper proof
// intentionally reuses the existing retain/release/autorelease helper cluster
// instead of claiming a separate resource or borrowed-pointer runtime ABI.
// live cleanup/runtime integration anchor: linked Part 8 probes now
// also execute through this same helper cluster while emitted cleanup/resource
// calls remain ordinary lowered function calls in the compiled module body.
extern "C" int objc3_runtime_spawn_task_i32(int task_kind, int executor_tag) {
  ++g_runtime_task_spawn_call_count;
  g_runtime_task_last_spawn_kind = task_kind;
  g_runtime_task_last_spawn_executor_tag = executor_tag;
  return 100 + (task_kind * 10) + (executor_tag != 0 ? 1 : 0);
}

extern "C" int objc3_runtime_enter_task_group_scope_i32(int executor_tag) {
  ++g_runtime_task_scope_call_count;
  g_runtime_task_last_scope_executor_tag = executor_tag;
  return 1;
}

extern "C" int objc3_runtime_add_task_group_task_i32(int executor_tag) {
  ++g_runtime_task_add_task_call_count;
  g_runtime_task_last_add_task_executor_tag = executor_tag;
  return 1;
}

extern "C" int objc3_runtime_wait_task_group_next_i32(int executor_tag) {
  ++g_runtime_task_wait_next_call_count;
  g_runtime_task_last_wait_next_executor_tag = executor_tag;
  g_runtime_task_last_wait_next_result = 23;
  return g_runtime_task_last_wait_next_result;
}

extern "C" int objc3_runtime_cancel_task_group_i32(int executor_tag) {
  ++g_runtime_task_cancel_all_call_count;
  g_runtime_task_last_cancel_all_executor_tag = executor_tag;
  g_runtime_task_last_cancel_all_result = 31;
  return g_runtime_task_last_cancel_all_result;
}

extern "C" int objc3_runtime_task_is_cancelled_i32(int executor_tag) {
  ++g_runtime_task_cancellation_poll_call_count;
  g_runtime_task_last_cancellation_poll_executor_tag = executor_tag;
  g_runtime_task_last_cancellation_poll_result = 0;
  return g_runtime_task_last_cancellation_poll_result;
}

extern "C" int objc3_runtime_task_on_cancel_i32(int executor_tag) {
  ++g_runtime_task_on_cancel_call_count;
  g_runtime_task_last_on_cancel_executor_tag = executor_tag;
  return 41;
}

extern "C" int objc3_runtime_executor_hop_i32(int value, int executor_tag) {
  ++g_runtime_task_executor_hop_call_count;
  g_runtime_task_last_executor_hop_executor_tag = executor_tag;
  g_runtime_task_last_executor_hop_value = value;
  return value;
}

extern "C" int objc3_runtime_actor_enter_isolation_thunk_i32(int executor_tag) {
  // actor lowering/runtime anchor: helper-backed actor lowering now
  // reaches the runtime through this private thunk/hop/nonisolated cluster.
  ++g_runtime_actor_isolation_thunk_call_count;
  g_runtime_actor_last_isolation_executor_tag = executor_tag;
  return executor_tag;
}

extern "C" int objc3_runtime_actor_enter_nonisolated_i32(int value,
                                                         int executor_tag) {
  ++g_runtime_actor_nonisolated_entry_call_count;
  g_runtime_actor_last_nonisolated_value = value;
  g_runtime_actor_last_nonisolated_executor_tag = executor_tag;
  return value;
}

extern "C" int objc3_runtime_actor_hop_to_executor_i32(int value,
                                                       int executor_tag) {
  ++g_runtime_actor_hop_to_executor_call_count;
  g_runtime_actor_last_hop_value = value;
  g_runtime_actor_last_hop_executor_tag = executor_tag;
  g_runtime_actor_last_hop_result = value;
  return value;
}

extern "C" int objc3_runtime_actor_record_replay_proof_i32(int executor_tag) {
  ++g_runtime_actor_replay_proof_call_count;
  g_runtime_actor_last_replay_proof_executor_tag = executor_tag;
  return executor_tag;
}

extern "C" int objc3_runtime_actor_record_race_guard_i32(int executor_tag) {
  ++g_runtime_actor_race_guard_call_count;
  g_runtime_actor_last_race_guard_executor_tag = executor_tag;
  return executor_tag;
}

extern "C" int objc3_runtime_actor_bind_executor_i32(int actor_handle,
                                                     int executor_tag) {
  // actor-mailbox/isolation-runtime anchor: actor runtime proof now
  // includes private mailbox binding state and deterministic enqueue/drain
  // helpers without widening the public runtime ABI.
  ++g_runtime_actor_bind_executor_call_count;
  g_runtime_actor_last_bound_actor_handle = actor_handle;
  g_runtime_actor_last_bound_executor_tag = executor_tag;
  g_runtime_actor_last_mailbox_actor_handle = actor_handle;
  g_runtime_actor_last_mailbox_executor_tag = executor_tag;
  g_runtime_actor_last_mailbox_depth =
      static_cast<int>(g_runtime_actor_mailboxes[actor_handle].size());
  return executor_tag;
}

extern "C" int objc3_runtime_actor_mailbox_enqueue_i32(int actor_handle,
                                                       int value,
                                                       int executor_tag) {
  ++g_runtime_actor_mailbox_enqueue_call_count;
  g_runtime_actor_last_mailbox_actor_handle = actor_handle;
  g_runtime_actor_last_mailbox_enqueued_value = value;
  g_runtime_actor_last_mailbox_executor_tag = executor_tag;
  std::deque<int> &mailbox = g_runtime_actor_mailboxes[actor_handle];
  mailbox.push_back(value);
  g_runtime_actor_last_mailbox_depth = static_cast<int>(mailbox.size());
  return value;
}

extern "C" int objc3_runtime_actor_mailbox_drain_next_i32(int actor_handle,
                                                          int executor_tag) {
  ++g_runtime_actor_mailbox_drain_call_count;
  g_runtime_actor_last_mailbox_actor_handle = actor_handle;
  g_runtime_actor_last_mailbox_executor_tag = executor_tag;
  std::deque<int> &mailbox = g_runtime_actor_mailboxes[actor_handle];
  if (mailbox.empty()) {
    g_runtime_actor_last_mailbox_depth = 0;
    g_runtime_actor_last_mailbox_drained_value = 0;
    return 0;
  }
  const int value = mailbox.front();
  mailbox.pop_front();
  g_runtime_actor_last_mailbox_depth = static_cast<int>(mailbox.size());
  g_runtime_actor_last_mailbox_drained_value = value;
  return value;
}

extern "C" int objc3_runtime_retain_i32(int value) {
  ++g_runtime_arc_debug_retain_call_count;
  g_runtime_arc_debug_last_retain_value = value;
  RuntimeState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  RetainRuntimeValueUnlocked(state, value);
  return value;
}

extern "C" int objc3_runtime_release_i32(int value) {
  ++g_runtime_arc_debug_release_call_count;
  g_runtime_arc_debug_last_release_value = value;
  RuntimeState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  ReleaseRuntimeValueUnlocked(state, value);
  return value;
}

extern "C" int objc3_runtime_autorelease_i32(int value) {
  ++g_runtime_arc_debug_autorelease_call_count;
  g_runtime_arc_debug_last_autorelease_value = value;
  EnqueueAutoreleaseValue(value);
  return value;
}

extern "C" int objc3_runtime_promote_block_i32(
    const void *storage, std::uint64_t storage_size_bytes,
    int has_pointer_capture_storage) {
  // block-runtime allocation/copy-dispose/invoke anchor: promotion
  // now admits pointer-capture block storage, preserves helper pointers, and
  // executes copy-helper setup before publishing the runtime-owned block
  // handle.
  // byref-forwarding/heap-promotion/ownership-interop anchor:
  // pointer-capture promotion also rewrites capture slots onto runtime-owned
  // heap cells before helper execution so escaping blocks do not retain stack
  // addresses after the source frame returns.
  if (storage == nullptr || storage_size_bytes == 0u) {
    return 0;
  }
  ++g_runtime_block_promote_call_count;
  g_runtime_last_promote_has_pointer_capture_storage =
      has_pointer_capture_storage != 0 ? 1 : 0;
  RuntimeState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  int block_handle = state.next_runtime_block_handle;
  while (block_handle <= 0 ||
         state.runtime_instances_by_receiver.find(block_handle) !=
             state.runtime_instances_by_receiver.end() ||
         state.runtime_blocks_by_handle.find(block_handle) !=
             state.runtime_blocks_by_handle.end()) {
    ++block_handle;
  }
  state.next_runtime_block_handle = block_handle + 1;
  RuntimeBlockRecord record;
  record.block_handle = block_handle;
  record.has_pointer_capture_storage = has_pointer_capture_storage != 0;
  record.storage_size_bytes =
      static_cast<std::size_t>(std::max<std::uint64_t>(storage_size_bytes, 1u));
  const std::size_t storage_word_count =
      (record.storage_size_bytes + sizeof(std::uint64_t) - 1u) /
      sizeof(std::uint64_t);
  record.storage_words.assign(storage_word_count, 0u);
  std::memcpy(record.storage_words.data(), storage, record.storage_size_bytes);
  std::memcpy(&record.invoke, record.storage_words.data(), sizeof(record.invoke));
  if (record.has_pointer_capture_storage) {
    if (record.storage_size_bytes < sizeof(void *) * 3u) {
      return 0;
    }
    std::memcpy(&record.copy_helper,
                reinterpret_cast<const unsigned char *>(record.storage_words.data()) +
                    sizeof(void *),
                sizeof(record.copy_helper));
    std::memcpy(&record.dispose_helper,
                reinterpret_cast<const unsigned char *>(record.storage_words.data()) +
                    sizeof(void *) * 2u,
                sizeof(record.dispose_helper));
    if (!PromotePointerCaptureCellsIntoRuntimeOwnedStorage(record)) {
      return 0;
    }
    if (record.copy_helper != nullptr) {
      record.copy_helper(record.storage_words.data());
    }
  }
  if (record.invoke == nullptr) {
    return 0;
  }
  state.runtime_blocks_by_handle.emplace(block_handle, std::move(record));
  g_runtime_last_promoted_block_handle = block_handle;
  return block_handle;
}

extern "C" int objc3_runtime_invoke_block_i32(int block_handle, int a0, int a1,
                                              int a2, int a3) {
  // block-runtime allocation/copy-dispose/invoke anchor: invoke now
  // works for any promoted runtime block record with a concrete invoke thunk,
  // including pointer-capture storage promoted through the private helper
  // surface.
  ++g_runtime_block_invoke_call_count;
  g_runtime_last_invoked_block_handle = block_handle;
  RuntimeBlockInvokeFn invoke = nullptr;
  std::vector<std::uint64_t> storage_words;
  {
    RuntimeState &state = State();
    std::lock_guard<std::mutex> lock(state.mutex);
    const auto block_it = state.runtime_blocks_by_handle.find(block_handle);
    if (block_it == state.runtime_blocks_by_handle.end() ||
        block_it->second.invoke == nullptr) {
      return 0;
    }
    invoke = block_it->second.invoke;
    storage_words = block_it->second.storage_words;
  }
  if (invoke == nullptr || storage_words.empty()) {
    return 0;
  }
  const int result = invoke(storage_words.data(), a0, a1, a2, a3);
  g_runtime_last_block_invoke_result = result;
  return result;
}

// runtime-memory-management implementation anchor: autoreleasepool
// scopes are now private runtime-owned stack frames that retain queued values
// until pop-time LIFO drain, while public ABI widening remains deferred.
// cleanup-unwind integration anchor: current Part 5 cleanup/unwind
// execution proofs reuse this private pool-frame drain path and the paired
// memory-management snapshots as the runtime-owned cleanup carrier.
// runtime cleanup/unwind implementation anchor: the broader runnable
// cleanup execution matrix still proves cleanup through this same private
// helper cluster while public runtime headers remain unchanged.
// system-helper/runtime-contract anchor: Part 8 cleanup/resource
// proof still routes through these same private autoreleasepool frames and
// memory-management snapshots, with no new public cleanup or invalidation ABI.
// live cleanup/runtime integration anchor: the supported Part 8
// fixture path now proves autoreleasepool traffic and cleanup-driven resource
// invalidation through this same private pool-frame implementation.
extern "C" void objc3_runtime_push_autoreleasepool_scope(void) {
  PushRuntimeAutoreleasePoolFrame();
}

// hardening anchor: task-runtime probes now rely on these existing
// autoreleasepool helpers plus reset-stable snapshot state to prove
// cancellation/autorelease determinism without widening the public ABI.
extern "C" void objc3_runtime_pop_autoreleasepool_scope(void) {
  ++g_runtime_arc_debug_autoreleasepool_pop_count;
  const std::vector<int> values = PopRuntimeAutoreleasePoolFrameValues();
  if (values.empty()) {
    return;
  }
  RuntimeState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  for (auto it = values.rbegin(); it != values.rend(); ++it) {
    g_runtime_last_drained_autorelease_value = *it;
    ++g_runtime_autoreleasepool_drained_value_count;
    ReleaseRuntimeValueUnlocked(state, *it);
  }
}

// runtime-fast-path-integration anchor: Part 9 freezes the current
// mixed dispatch boundary exactly as implemented here. IR-lowered direct sites
// bypass this entrypoint, while objc_dynamic opt-out and unresolved sends still
// flow through the canonical method-cache/slow-path/strict-error surface below.
// live-dispatch-fast-path anchor: cache hits can now come from
// registration-time seeded direct/final/sealed entries, and that seeded path
// remains explicit in the runtime-owned proof counters and last-fast-path
// reason snapshot state.
const char *DispatchDiagnosticCode(
    objc3_runtime_dispatch_status_code status_code) {
  switch (status_code) {
    case OBJC3_RUNTIME_DISPATCH_STATUS_OK:
    case OBJC3_RUNTIME_DISPATCH_STATUS_NIL_RECEIVER:
      return "";
    case OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_SELECTOR:
      return "O3RT001";
    case OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_RECEIVER_CLASS:
      return "O3RT002";
    case OBJC3_RUNTIME_DISPATCH_STATUS_MISSING_CLASS_GRAPH:
      return "O3RT003";
    case OBJC3_RUNTIME_DISPATCH_STATUS_MALFORMED_METADATA:
      return "O3RT004";
    case OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_RETURN_TYPE:
      return "O3RT005";
    case OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_ARGUMENT_LAYOUT:
      return "O3RT006";
    case OBJC3_RUNTIME_DISPATCH_STATUS_CATEGORY_CONFLICT:
      return "O3RT007";
  }
  return "O3RT000";
}

const char *DispatchDiagnosticMessage(
    objc3_runtime_dispatch_status_code status_code) {
  switch (status_code) {
    case OBJC3_RUNTIME_DISPATCH_STATUS_OK:
      return "";
    case OBJC3_RUNTIME_DISPATCH_STATUS_NIL_RECEIVER:
      return "";
    case OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_SELECTOR:
      return "runtime dispatch failed: unknown selector";
    case OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_RECEIVER_CLASS:
      return "runtime dispatch failed: unknown receiver class";
    case OBJC3_RUNTIME_DISPATCH_STATUS_MISSING_CLASS_GRAPH:
      return "runtime dispatch failed: missing class graph";
    case OBJC3_RUNTIME_DISPATCH_STATUS_MALFORMED_METADATA:
      return "runtime dispatch failed: malformed metadata";
    case OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_RETURN_TYPE:
      return "runtime dispatch failed: unsupported return type";
    case OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_ARGUMENT_LAYOUT:
      return "runtime dispatch failed: unsupported argument layout";
    case OBJC3_RUNTIME_DISPATCH_STATUS_CATEGORY_CONFLICT:
      return "runtime dispatch failed: category conflict";
  }
  return "runtime dispatch failed: internal dispatch status";
}

objc3_runtime_dispatch_i32_result MakeDispatchI32Result(
    objc3_runtime_dispatch_status_code status_code, int value) {
  objc3_runtime_dispatch_i32_result result{};
  result.status_code = status_code;
  result.value = value;
  result.diagnostic_code = DispatchDiagnosticCode(status_code);
  result.diagnostic_message = DispatchDiagnosticMessage(status_code);
  return result;
}

objc3_runtime_dispatch_i32_result objc3_runtime_dispatch_i32_checked(
    int receiver, const char *selector, int a0, int a1, int a2, int a3) {
  RuntimeState &state = State();
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
    state.last_fast_path_reason.clear();
    state.last_dispatch_path.clear();
    state.last_dispatch_implementation_kind.clear();
    state.last_dispatch_property_name.clear();
    state.last_resolved_class_name.clear();
    state.last_resolved_owner_identity.clear();
    state.last_dispatch_parameter_count = 0;
    state.last_dispatch_property_base_identity = 0;
    state.last_dispatch_property_slot_index = 0;

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
            ++state.live_dispatch_count;
            if (entry.fast_path_seeded) {
              ++state.fast_path_hit_count;
            }
          } else {
            state.last_dispatch_path = "cache-hit-error";
            state.last_dispatch_implementation_kind = "strict-dispatch-error";
            dispatch_status =
                OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_SELECTOR;
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
            ++state.live_dispatch_count;
          } else {
            state.last_dispatch_path = "slow-path-error";
            state.last_dispatch_implementation_kind = "strict-dispatch-error";
            dispatch_status =
                resolution.ambiguous
                    ? OBJC3_RUNTIME_DISPATCH_STATUS_CATEGORY_CONFLICT
                    : OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_SELECTOR;
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
      }
    }
  }
  // runtime call ABI generation anchor: canonical runtime dispatch
  // owns nil-receiver semantics for lowered instance/class/super surfaces, so
  // a zero receiver returns zero without requiring lowering-side elision.
  if (receiver == 0) {
    std::lock_guard<std::mutex> lock(state.mutex);
    state.last_dispatch_path = "nil-short-circuit";
    state.last_dispatch_implementation_kind = "nil-short-circuit";
    return MakeDispatchI32Result(OBJC3_RUNTIME_DISPATCH_STATUS_NIL_RECEIVER,
                                 0);
  }
  if (resolved_live_method && resolved_implementation != nullptr) {
    PushRuntimeDispatchFrame(receiver, receiver_base_identity,
                             resolved_runtime_property_accessor);
    const int result =
        InvokeResolvedMethod(resolved_implementation, resolved_return_kind,
                            resolved_parameter_count, a0, a1, a2, a3);
    const std::vector<int> autorelease_values =
        PopRuntimeDispatchFrameAutoreleaseValues();
    if (!autorelease_values.empty()) {
      std::lock_guard<std::mutex> lock(state.mutex);
      for (int value : autorelease_values) {
        ReleaseRuntimeValueUnlocked(state, value);
      }
    }
    return MakeDispatchI32Result(OBJC3_RUNTIME_DISPATCH_STATUS_OK, result);
  }
  if (resolved_live_method && resolved_builtin_kind != RuntimeBuiltinKind::None) {
    PushRuntimeDispatchFrame(receiver, receiver_base_identity,
                             resolved_runtime_property_accessor);
    const int result = InvokeRuntimeBuiltinMethod(
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
    return MakeDispatchI32Result(OBJC3_RUNTIME_DISPATCH_STATUS_OK, result);
  }
  return MakeDispatchI32Result(dispatch_status, 0);
}

int objc3_runtime_dispatch_i32(int receiver, const char *selector, int a0,
                               int a1, int a2, int a3) {
  const objc3_runtime_dispatch_i32_result result =
      objc3_runtime_dispatch_i32_checked(receiver, selector, a0, a1, a2, a3);
  return result.value;
}

int objc3_runtime_copy_registration_state_for_testing(
    objc3_runtime_registration_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  RuntimeState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  snapshot->registered_image_count = state.registered_image_count;
  snapshot->registered_descriptor_total = state.registered_descriptor_total;
  snapshot->next_expected_registration_order_ordinal =
      state.next_expected_registration_order_ordinal;
  snapshot->last_successful_registration_order_ordinal =
      state.last_successful_registration_order_ordinal;
  snapshot->last_registration_status = state.last_registration_status;
  snapshot->last_registered_module_name =
      StableCString(state.last_registered_module_name);
  snapshot->last_registered_translation_unit_identity_key =
      StableCString(state.last_registered_translation_unit_identity_key);
  snapshot->last_rejected_module_name =
      StableCString(state.last_rejected_module_name);
  snapshot->last_rejected_translation_unit_identity_key =
      StableCString(state.last_rejected_translation_unit_identity_key);
  snapshot->last_rejected_registration_order_ordinal =
      state.last_rejected_registration_order_ordinal;
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

extern "C" int objc3_msgsend_i32(int receiver, const char *selector, int a0,
                                 int a1, int a2, int a3) {
  return objc3_runtime_dispatch_i32(receiver, selector, a0, a1, a2, a3);
}

void objc3_runtime_reset_for_testing(void) {
  RuntimeState &state = State();
  std::lock_guard<std::mutex> lock(state.mutex);
  // live-restart-hardening anchor: teardown clears only live runtime
  // state, preserves the retained bootstrap catalog for restart, zeroes image-
  // local init cells, and advances reset-generation evidence for repeated
  // restart-cycle probes.
  ClearLiveRegistrationStateUnlocked(state);
  state.last_reset_cleared_image_local_init_state_count =
      ZeroRetainedBootstrapImageLocalInitStatesUnlocked(state);
  ++state.reset_generation;
  state.last_replay_status = OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  state.last_replayed_image_count = 0;
  state.last_replayed_module_name.clear();
  state.last_replayed_translation_unit_identity_key.clear();
  ResetRuntimeAutoreleasepoolStateForTesting();
}
