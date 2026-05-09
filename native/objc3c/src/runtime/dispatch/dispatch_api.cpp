#include "runtime/dispatch/dispatch_api.h"

#include "runtime/classes/class_graph.h"
#include "runtime/dispatch/builtin_methods.h"
#include "runtime/dispatch/dispatch_family.h"
#include "runtime/dispatch/dispatch_result_state.h"
#include "runtime/dispatch/method_cache.h"
#include "runtime/dispatch/method_invocation.h"
#include "runtime/dispatch/runtime_resolution_records.h"
#include "runtime/dispatch/typed_dispatch_result.h"
#include "runtime/memory/arc_value_lifetime.h"
#include "runtime/memory/dispatch_frame_state.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "runtime/public/objc3_runtime_api.h"
#include "runtime/public/objc3_runtime_result_contract.h"
#include "runtime/selectors/selector_table.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/state/runtime_state_store.h"
#include "runtime/strings/borrowed_string.h"

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <mutex>
#include <utility>
#include <vector>

namespace objc3c::runtime {

objc3_runtime_dispatch_status_code RuntimeStrictDispatchStatus(
    bool resolved, bool ambiguous,
    objc3_runtime_dispatch_status_code unresolved_status) {
  if (resolved) {
    return OBJC3_RUNTIME_DISPATCH_STATUS_OK;
  }
  if (ambiguous) {
    return OBJC3_RUNTIME_DISPATCH_STATUS_CATEGORY_CONFLICT;
  }
  return unresolved_status;
}

bool RuntimeDispatchStatusIsSuccess(
    objc3_runtime_dispatch_status_code status_code) {
  return status_code == OBJC3_RUNTIME_DISPATCH_STATUS_OK;
}

}  // namespace objc3c::runtime

extern "C" objc3_runtime_dispatch_i32_result objc3_runtime_dispatch_i32_checked(
    int receiver, const char *selector, int a0, int a1, int a2, int a3) {
  using objc3c::runtime::DecodeReceiverIdentity;
  using objc3c::runtime::DescribeResolvedImplementationKind;
  using objc3c::runtime::DispatchFamily;
  using objc3c::runtime::InvokeRuntimeBuiltinMethod;
  using objc3c::runtime::InvokeRuntimeMethodImplementation;
  using objc3c::runtime::LookupSelectorUnlocked;
  using objc3c::runtime::MethodCacheEntry;
  using objc3c::runtime::MethodCacheKey;
  using objc3c::runtime::PopRuntimeDispatchFrameAutoreleaseValues;
  using objc3c::runtime::ProcessRuntimeState;
  using objc3c::runtime::PushRuntimeDispatchFrame;
  using objc3c::runtime::RealizedPropertyAccessor;
  using objc3c::runtime::RecordPostResolutionStrictDispatchFailure;
  using objc3c::runtime::RecordTypedDispatchSuccess;
  using objc3c::runtime::ReleaseRuntimeValueUnlocked;
  using objc3c::runtime::ResolveMethodSlowPathUnlocked;
  using objc3c::runtime::RuntimeBuiltinKind;
  using objc3c::runtime::RuntimeDispatchStatusIsSuccess;
  using objc3c::runtime::RuntimeMethodReturnKind;
  using objc3c::runtime::RuntimeState;
  using objc3c::runtime::RuntimeStrictDispatchStatus;
  using objc3c::runtime::RuntimeTypedDispatchResult;
  using objc3c::runtime::SlowPathResolution;
  using objc3c::runtime::StoreDispatchResultContractUnlocked;

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
            state.last_dispatch_property_base_identity =
                receiver_base_identity;
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
            resolved_runtime_property_accessor =
                entry.runtime_property_accessor;
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
              RuntimeStrictDispatchStatus(resolution.resolved,
                                          resolution.ambiguous,
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
            state.last_dispatch_property_base_identity =
                receiver_base_identity;
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
    const RuntimeTypedDispatchResult result = InvokeRuntimeMethodImplementation(
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
    if (!RuntimeDispatchStatusIsSuccess(result.status_code)) {
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
  if (resolved_live_method &&
      resolved_builtin_kind != RuntimeBuiltinKind::None) {
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
    if (!RuntimeDispatchStatusIsSuccess(result.status_code)) {
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

extern "C" int objc3_runtime_copy_dispatch_state_for_testing(
    objc3_runtime_dispatch_state_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  objc3c::runtime::RuntimeState &state = objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  snapshot->cache_entry_count =
      static_cast<std::uint64_t>(state.method_cache.size());
  snapshot->fast_path_seed_count = state.fast_path_seed_count;
  snapshot->fast_path_hit_count = state.fast_path_hit_count;
  snapshot->live_dispatch_count = state.live_dispatch_count;
  snapshot->strict_dispatch_error_count = state.strict_dispatch_error_count;
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
  snapshot->last_dispatch_strict_error =
      state.last_dispatch_strict_error ? 1 : 0;
  snapshot->last_effective_direct_dispatch =
      state.last_dispatch_effective_direct_dispatch ? 1 : 0;
  snapshot->last_used_builtin = state.last_dispatch_used_builtin ? 1 : 0;
  snapshot->last_dispatch_status_code = state.last_dispatch_status_code;
  snapshot->last_selector =
      objc3c::runtime::BorrowRuntimeCString(state.last_dispatch_selector);
  snapshot->last_fast_path_reason =
      objc3c::runtime::BorrowRuntimeCString(state.last_fast_path_reason);
  snapshot->last_dispatch_path =
      objc3c::runtime::BorrowRuntimeCString(state.last_dispatch_path);
  snapshot->last_implementation_kind =
      objc3c::runtime::BorrowRuntimeCString(
          state.last_dispatch_implementation_kind);
  snapshot->last_return_kind =
      objc3c::runtime::BorrowRuntimeCString(state.last_dispatch_return_kind);
  snapshot->last_diagnostic_code =
      objc3c::runtime::BorrowRuntimeCString(state.last_dispatch_diagnostic_code);
  snapshot->last_diagnostic_message =
      objc3c::runtime::BorrowRuntimeCString(
          state.last_dispatch_diagnostic_message);
  snapshot->last_result_contract =
      objc3c::runtime::BorrowRuntimeCString(state.last_dispatch_result_contract);
  snapshot->last_property_name =
      objc3c::runtime::BorrowRuntimeCString(state.last_dispatch_property_name);
  snapshot->last_resolved_class_name =
      objc3c::runtime::BorrowRuntimeCString(state.last_resolved_class_name);
  snapshot->last_resolved_owner_identity =
      objc3c::runtime::BorrowRuntimeCString(state.last_resolved_owner_identity);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

extern "C" int objc3_runtime_dispatch_i32(int receiver, const char *selector,
                                          int a0, int a1, int a2, int a3) {
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
