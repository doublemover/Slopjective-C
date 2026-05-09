#if defined(_WIN32) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif
#if defined(_WIN32) && !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif
#if defined(_WIN32) && !defined(NOMINMAX)
#define NOMINMAX
#endif

#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <deque>
#include <memory>
#include <mutex>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "runtime/blocks/block_runtime_records.h"
#include "runtime/dispatch/runtime_resolution_records.h"
#include "runtime/metadata/runtime_emitted_records.h"
#include "runtime/metadata/runtime_registration_records.h"
#include "runtime/metadata/runtime_realized_records.h"
#include "runtime/storage/runtime_instance_records.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/state/runtime_thread_records.h"

#if defined(_WIN32)
#include <windows.h>
#endif

using objc3c::runtime::EmittedCategoryRecord;
using objc3c::runtime::EmittedClassBundle;
using objc3c::runtime::EmittedClassRecord;
using objc3c::runtime::EmittedIvarDescriptor;
using objc3c::runtime::EmittedIvarLayoutRecord;
using objc3c::runtime::EmittedKeyPathDescriptor;
using objc3c::runtime::EmittedMethodListEntry;
using objc3c::runtime::EmittedMethodListHeader;
using objc3c::runtime::EmittedMethodListRef;
using objc3c::runtime::EmittedPropertyDescriptor;
using objc3c::runtime::EmittedProtocolRecord;
using objc3c::runtime::HasTerminalStrictDispatchError;
using objc3c::runtime::KeyPathSlot;
using objc3c::runtime::MethodCacheEntry;
using objc3c::runtime::PromotePointerCaptureCellsIntoRuntimeOwnedStorage;
using objc3c::runtime::PropertyLookupCacheEntry;
using objc3c::runtime::PropertyLookupCacheKey;
using objc3c::runtime::PropertyLookupCacheKeyHash;
using objc3c::runtime::RealizedClassNode;
using objc3c::runtime::RealizedPropertyAccessor;
using objc3c::runtime::RegisteredImageMetadata;
using objc3c::runtime::RuntimeBlockInvokeFn;
using objc3c::runtime::RuntimeBlockRecord;
using objc3c::runtime::RuntimeInstanceRecord;
using objc3c::runtime::RuntimeState;
using objc3c::runtime::RuntimeAutoreleasePoolFrame;
using objc3c::runtime::RuntimeDispatchFrame;
using objc3c::runtime::RuntimeWeakSlotRef;
using objc3c::runtime::SelectorSlot;
using objc3c::runtime::SlowPathResolution;

namespace {

using objc3c::runtime::MethodCacheKey;
using objc3c::runtime::MethodCacheKeyHash;
using objc3c::runtime::BuildReceiverBaseIdentity;
using objc3c::runtime::ClassifyRuntimeReturnType;
using objc3c::runtime::DescribeResolvedImplementationKind;
using objc3c::runtime::DispatchFamily;
using objc3c::runtime::IsRuntimeReceiverBaseIdentity;
using objc3c::runtime::RuntimeBuiltinKind;
using objc3c::runtime::RuntimeMethodReturnKind;
using objc3c::runtime::RuntimeMethodReturnKindName;
using objc3c::runtime::RuntimeMethodShapeStatus;
using objc3c::runtime::RuntimeReceiverIdentityBase;
using objc3c::runtime::RuntimeReceiverIdentityStride;
using objc3c::runtime::RuntimeTypedDispatchFailure;
using objc3c::runtime::RuntimeTypedDispatchResult;
using objc3c::runtime::RuntimeTypedDispatchSuccess;
using objc3c::runtime::BorrowRuntimeCString;
constexpr const char *kObjc3DefaultMetaprogrammingHostCacheRoot =
    "tmp/artifacts/objc3c-native/cache/metaprogramming";
[[maybe_unused]] constexpr const char *kObjc3ConformancePublicationContractId =
    "objc3c.driver.conformance.report.publication.v1";
[[maybe_unused]] constexpr const char *kObjc3ConformanceClaimOperationsContractId =
    "objc3c.toolchain.conformance.claim.operations.v1";
[[maybe_unused]] constexpr const char *kObjc3ConformanceClaimSelectionModel =
    "canonical-language-profile-drives-current-publication-surface";
[[maybe_unused]] constexpr const char *kObjc3ConformanceClaimFailureModel =
    "strictness-and-strict-concurrency-remain-fail-closed-and-unclaimed";
[[maybe_unused]] constexpr const char
    *kObjc3AdvancedFeatureOpsContractId =
        "objc3c.advanced.feature.ci.runbook.dashboard.contract.v1";
[[maybe_unused]] constexpr const char
    *kObjc3AdvancedFeatureReportingContractId =
        "objc3c.tooling.feature.aware.conformance.report.emission.v1";
[[maybe_unused]] constexpr const char
    *kObjc3AdvancedFeatureReleaseEvidenceContractId =
        "objc3c.tooling.corpus.sharding.release.evidence.packaging.v1";
[[maybe_unused]] constexpr const char
    *kObjc3AdvancedFeatureEvidenceGateScriptPath =
    "scripts/check_release_evidence.py";

const char *MetaprogrammingHostCacheRootForTesting() {
#if defined(_WIN32)
  char buffer[32768] = {};
  const DWORD length = GetEnvironmentVariableA(
      "OBJC3C_METAPROGRAMMING_CACHE_ROOT", buffer,
      static_cast<DWORD>(sizeof(buffer)));
  if (length == 0 || length >= sizeof(buffer)) {
    return kObjc3DefaultMetaprogrammingHostCacheRoot;
  }
  static std::string cached_override;
  cached_override.assign(buffer, static_cast<std::size_t>(length));
  return cached_override.c_str();
#else
  const char *override_value = std::getenv("OBJC3C_METAPROGRAMMING_CACHE_ROOT");
  if (override_value == nullptr || override_value[0] == '\0') {
    return kObjc3DefaultMetaprogrammingHostCacheRoot;
  }
  static std::string cached_override;
  cached_override = override_value;
  return cached_override.c_str();
#endif
}
[[maybe_unused]] constexpr const char
    *kObjc3AdvancedFeatureEvidenceRunbookPath =
        "spec/conformance/release_evidence_gate_maintenance.md";
[[maybe_unused]] constexpr const char
    *kObjc3AdvancedFeatureDashboardSchemaPath =
        "schemas/objc3-conformance-dashboard-status-v1.schema.json";
[[maybe_unused]] constexpr const char
    *kObjc3AdvancedFeatureReleaseEvidenceOperationContractId =
        "objc3c.tooling.release.evidence.toolchain.operations.v1";
[[maybe_unused]] constexpr const char
    *kObjc3AdvancedFeatureDashboardStatusPublicationContractId =
        "objc3c.tooling.dashboard.status.publication.v1";

RuntimeState &State() {
  static RuntimeState state;
  return state;
}

const objc3_runtime_selector_handle *LookupSelectorUnlocked(const char *selector);

thread_local std::vector<RuntimeDispatchFrame> g_runtime_dispatch_frames;
thread_local RuntimeDispatchFrame g_runtime_testing_dispatch_frame;
thread_local bool g_runtime_has_testing_dispatch_frame = false;
thread_local std::vector<RuntimeAutoreleasePoolFrame>
    g_runtime_autoreleasepool_frames;
thread_local std::uint64_t g_runtime_autoreleasepool_max_depth = 0;
thread_local std::uint64_t g_runtime_autoreleasepool_drained_value_count = 0;
thread_local std::uint64_t g_runtime_arc_debug_retain_call_count = 0;
thread_local std::uint64_t g_runtime_arc_debug_release_call_count = 0;
thread_local std::uint64_t g_runtime_arc_debug_autorelease_call_count = 0;
thread_local std::uint64_t g_runtime_arc_debug_autoreleasepool_push_count = 0;
thread_local std::uint64_t g_runtime_arc_debug_autoreleasepool_pop_count = 0;
thread_local std::uint64_t g_runtime_arc_debug_current_property_read_count = 0;
thread_local std::uint64_t g_runtime_arc_debug_current_property_write_count = 0;
thread_local std::uint64_t g_runtime_arc_debug_current_property_exchange_count = 0;
thread_local std::uint64_t g_runtime_arc_debug_weak_current_property_load_count = 0;
thread_local std::uint64_t g_runtime_arc_debug_weak_current_property_store_count = 0;
thread_local int g_runtime_last_autoreleased_value = 0;
thread_local int g_runtime_last_drained_autorelease_value = 0;
thread_local int g_runtime_arc_debug_last_retain_value = 0;
thread_local int g_runtime_arc_debug_last_release_value = 0;
thread_local int g_runtime_arc_debug_last_autorelease_value = 0;
thread_local int g_runtime_arc_debug_last_property_read_value = 0;
thread_local int g_runtime_arc_debug_last_property_written_value = 0;
thread_local int g_runtime_arc_debug_last_property_exchange_previous_value = 0;
thread_local int g_runtime_arc_debug_last_property_exchange_new_value = 0;
thread_local int g_runtime_arc_debug_last_weak_loaded_value = 0;
thread_local int g_runtime_arc_debug_last_weak_stored_value = 0;
thread_local int g_runtime_arc_debug_last_property_receiver = 0;
thread_local std::string g_runtime_arc_debug_last_property_name;
thread_local std::string g_runtime_arc_debug_last_property_owner_identity;
thread_local std::uint64_t g_runtime_block_promote_call_count = 0;
thread_local std::uint64_t g_runtime_block_invoke_call_count = 0;
thread_local int g_runtime_last_promoted_block_handle = 0;
thread_local int g_runtime_last_promote_has_pointer_capture_storage = 0;
thread_local int g_runtime_last_invoked_block_handle = 0;
thread_local int g_runtime_last_block_invoke_result = 0;
thread_local std::uint64_t g_runtime_error_bridge_store_call_count = 0;
thread_local std::uint64_t g_runtime_error_bridge_load_call_count = 0;
thread_local std::uint64_t g_runtime_error_bridge_status_bridge_call_count = 0;
thread_local std::uint64_t g_runtime_error_bridge_nserror_bridge_call_count = 0;
thread_local std::uint64_t g_runtime_error_bridge_catch_match_call_count = 0;
thread_local int g_runtime_error_bridge_last_stored_error_value = 0;
thread_local int g_runtime_error_bridge_last_loaded_error_value = 0;
thread_local int g_runtime_error_bridge_last_status_bridge_status_value = 0;
thread_local int g_runtime_error_bridge_last_status_bridge_error_value = 0;
thread_local int g_runtime_error_bridge_last_nserror_bridge_error_value = 0;
thread_local int g_runtime_error_bridge_last_catch_match_error_value = 0;
thread_local int g_runtime_error_bridge_last_catch_match_kind = 0;
thread_local int g_runtime_error_bridge_last_catch_match_is_catch_all = 0;
thread_local int g_runtime_error_bridge_last_catch_match_result = 0;
thread_local std::string g_runtime_error_bridge_last_catch_kind_name;
thread_local std::uint64_t g_runtime_async_continuation_allocation_call_count = 0;
thread_local std::uint64_t g_runtime_async_continuation_handoff_call_count = 0;
thread_local std::uint64_t g_runtime_async_continuation_resume_call_count = 0;
thread_local int g_runtime_async_continuation_next_handle = 1;
thread_local int g_runtime_async_continuation_last_allocated_handle = 0;
thread_local int g_runtime_async_continuation_last_allocated_resume_entry_tag = 0;
thread_local int g_runtime_async_continuation_last_allocated_executor_tag = 0;
thread_local int g_runtime_async_continuation_last_handoff_handle = 0;
thread_local int g_runtime_async_continuation_last_handoff_executor_tag = 0;
thread_local int g_runtime_async_continuation_last_resume_handle = 0;
thread_local int g_runtime_async_continuation_last_resume_result_value = 0;
thread_local int g_runtime_async_continuation_last_resume_return_value = 0;
thread_local std::unordered_set<int> g_runtime_async_live_continuation_handles;
thread_local std::uint64_t g_runtime_task_spawn_call_count = 0;
thread_local std::uint64_t g_runtime_task_scope_call_count = 0;
thread_local std::uint64_t g_runtime_task_add_task_call_count = 0;
thread_local std::uint64_t g_runtime_task_wait_next_call_count = 0;
thread_local std::uint64_t g_runtime_task_cancel_all_call_count = 0;
thread_local std::uint64_t g_runtime_task_cancellation_poll_call_count = 0;
thread_local std::uint64_t g_runtime_task_on_cancel_call_count = 0;
thread_local std::uint64_t g_runtime_task_executor_hop_call_count = 0;
thread_local std::uint64_t g_runtime_actor_isolation_thunk_call_count = 0;
thread_local std::uint64_t g_runtime_actor_nonisolated_entry_call_count = 0;
thread_local std::uint64_t g_runtime_actor_hop_to_executor_call_count = 0;
thread_local std::uint64_t g_runtime_actor_replay_proof_call_count = 0;
thread_local std::uint64_t g_runtime_actor_race_guard_call_count = 0;
thread_local std::uint64_t g_runtime_actor_bind_executor_call_count = 0;
thread_local std::uint64_t g_runtime_actor_mailbox_enqueue_call_count = 0;
thread_local std::uint64_t g_runtime_actor_mailbox_drain_call_count = 0;
thread_local int g_runtime_task_last_spawn_kind = 0;
thread_local int g_runtime_task_last_spawn_executor_tag = 0;
thread_local int g_runtime_task_last_scope_executor_tag = 0;
thread_local int g_runtime_task_last_add_task_executor_tag = 0;
thread_local int g_runtime_task_last_wait_next_executor_tag = 0;
thread_local int g_runtime_task_last_cancel_all_executor_tag = 0;
thread_local int g_runtime_task_last_cancellation_poll_executor_tag = 0;
thread_local int g_runtime_task_last_on_cancel_executor_tag = 0;
thread_local int g_runtime_task_last_executor_hop_executor_tag = 0;
thread_local int g_runtime_task_last_executor_hop_value = 0;
thread_local int g_runtime_task_last_wait_next_result = 0;
thread_local int g_runtime_task_last_cancel_all_result = 0;
thread_local int g_runtime_task_last_cancellation_poll_result = 0;
thread_local int g_runtime_actor_last_isolation_executor_tag = 0;
thread_local int g_runtime_actor_last_nonisolated_value = 0;
thread_local int g_runtime_actor_last_nonisolated_executor_tag = 0;
thread_local int g_runtime_actor_last_hop_value = 0;
thread_local int g_runtime_actor_last_hop_executor_tag = 0;
thread_local int g_runtime_actor_last_hop_result = 0;
thread_local int g_runtime_actor_last_replay_proof_executor_tag = 0;
thread_local int g_runtime_actor_last_race_guard_executor_tag = 0;
thread_local int g_runtime_actor_last_bound_actor_handle = 0;
thread_local int g_runtime_actor_last_bound_executor_tag = 0;
thread_local int g_runtime_actor_last_mailbox_actor_handle = 0;
thread_local int g_runtime_actor_last_mailbox_enqueued_value = 0;
thread_local int g_runtime_actor_last_mailbox_executor_tag = 0;
thread_local int g_runtime_actor_last_mailbox_depth = 0;
thread_local int g_runtime_actor_last_mailbox_drained_value = 0;
thread_local std::unordered_map<int, std::deque<int>> g_runtime_actor_mailboxes;

const void *AggregateEntry(const objc3_runtime_pointer_aggregate *aggregate,
                           std::uint64_t index);
std::vector<const EmittedClassBundle *> CollectPreferredClassBundlesForImage(
    const RegisteredImageMetadata &record, const std::string &class_name);

void ClearMethodCacheStateUnlocked(RuntimeState &state) {
  state.method_cache.clear();
  state.method_cache_hit_count = 0;
  state.method_cache_miss_count = 0;
  state.slow_path_lookup_count = 0;
  state.live_dispatch_count = 0;
  state.strict_dispatch_error_count = 0;
  state.fast_path_seed_count = 0;
  state.fast_path_hit_count = 0;
  state.last_dispatch_selector.clear();
  state.last_dispatch_selector_stable_id = 0;
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
}

void ClearRealizedClassGraphUnlocked(RuntimeState &state) {
  state.realized_class_name_by_base_identity.clear();
  state.ambiguous_realized_base_identities.clear();
  state.realized_class_node_indices_by_name.clear();
  state.realized_class_nodes.clear();
  state.property_lookup_cache.clear();
  state.property_lookup_cache_hit_count = 0;
  state.property_lookup_cache_miss_count = 0;
  state.realized_root_class_count = 0;
  state.realized_metaclass_edge_count = 0;
  state.receiver_class_binding_count = 0;
  state.realized_attached_category_count = 0;
  state.realized_protocol_conformance_edge_count = 0;
  state.last_realized_class_name.clear();
  state.last_realized_class_owner_identity.clear();
  state.last_realized_metaclass_owner_identity.clear();
  state.last_attached_category_owner_identity.clear();
  state.last_attached_category_name.clear();
  state.last_queried_class_name.clear();
  state.last_resolved_class_query_name.clear();
  state.last_resolved_class_query_owner_identity.clear();
  state.last_class_query_found = false;
  state.last_protocol_conformance_class_name.clear();
  state.last_protocol_conformance_protocol_name.clear();
  state.last_protocol_conformance_owner_identity.clear();
  state.last_protocol_conformance_attachment_owner_identity.clear();
  state.last_protocol_query_class_found = false;
  state.last_protocol_query_protocol_found = false;
  state.last_protocol_query_conforms = false;
  state.last_queried_property_class_name.clear();
  state.last_queried_property_name.clear();
  state.last_reflected_property_class_name.clear();
  state.last_reflected_property_owner_identity.clear();
  state.last_property_query_found = false;
  state.last_property_query_inherited = false;
  state.last_property_query_used_cache = false;
  state.last_property_query_used_cache = false;
}

void ClearRuntimeInstanceStateUnlocked(RuntimeState &state) {
  state.runtime_instances_by_receiver.clear();
  state.runtime_blocks_by_handle.clear();
  state.weak_slot_refs_by_target_receiver.clear();
  state.next_runtime_instance_receiver = 0x100000;
  state.next_runtime_block_handle = 0x200000;
  state.live_runtime_instance_count = 0;
  state.last_allocated_runtime_instance_receiver = 0;
  state.last_allocated_runtime_instance_base_identity = 0;
  state.last_allocated_runtime_instance_size_bytes = 0;
  state.last_allocated_runtime_instance_class_name.clear();
}

std::size_t AlignTo(std::size_t value, std::size_t alignment) {
  const std::size_t effective_alignment = std::max<std::size_t>(alignment, 1u);
  const std::size_t remainder = value % effective_alignment;
  return remainder == 0u ? value : value + (effective_alignment - remainder);
}

RuntimeDispatchFrame *CurrentRuntimeDispatchFrame() {
  if (!g_runtime_dispatch_frames.empty()) {
    return &g_runtime_dispatch_frames.back();
  }
  return g_runtime_has_testing_dispatch_frame ? &g_runtime_testing_dispatch_frame
                                              : nullptr;
}

void PushRuntimeDispatchFrame(int receiver, std::uint64_t base_identity,
                              const RealizedPropertyAccessor *accessor) {
  RuntimeDispatchFrame frame;
  frame.receiver = receiver;
  frame.base_identity = base_identity;
  frame.runtime_property_accessor = accessor;
  g_runtime_dispatch_frames.push_back(std::move(frame));
}

std::vector<int> PopRuntimeDispatchFrameAutoreleaseValues() {
  if (g_runtime_dispatch_frames.empty()) {
    return {};
  }
  RuntimeDispatchFrame frame = std::move(g_runtime_dispatch_frames.back());
  g_runtime_dispatch_frames.pop_back();
  return std::move(frame.autorelease_values);
}

void ResetRuntimeAutoreleasepoolStateForTesting() {
  g_runtime_dispatch_frames.clear();
  g_runtime_testing_dispatch_frame = RuntimeDispatchFrame{};
  g_runtime_has_testing_dispatch_frame = false;
  g_runtime_autoreleasepool_frames.clear();
  g_runtime_autoreleasepool_max_depth = 0;
  g_runtime_autoreleasepool_drained_value_count = 0;
  g_runtime_last_autoreleased_value = 0;
  g_runtime_last_drained_autorelease_value = 0;
  g_runtime_arc_debug_retain_call_count = 0;
  g_runtime_arc_debug_release_call_count = 0;
  g_runtime_arc_debug_autorelease_call_count = 0;
  g_runtime_arc_debug_autoreleasepool_push_count = 0;
  g_runtime_arc_debug_autoreleasepool_pop_count = 0;
  g_runtime_arc_debug_current_property_read_count = 0;
  g_runtime_arc_debug_current_property_write_count = 0;
  g_runtime_arc_debug_current_property_exchange_count = 0;
  g_runtime_arc_debug_weak_current_property_load_count = 0;
  g_runtime_arc_debug_weak_current_property_store_count = 0;
  g_runtime_arc_debug_last_retain_value = 0;
  g_runtime_arc_debug_last_release_value = 0;
  g_runtime_arc_debug_last_autorelease_value = 0;
  g_runtime_arc_debug_last_property_read_value = 0;
  g_runtime_arc_debug_last_property_written_value = 0;
  g_runtime_arc_debug_last_property_exchange_previous_value = 0;
  g_runtime_arc_debug_last_property_exchange_new_value = 0;
  g_runtime_arc_debug_last_weak_loaded_value = 0;
  g_runtime_arc_debug_last_weak_stored_value = 0;
  g_runtime_arc_debug_last_property_receiver = 0;
  g_runtime_arc_debug_last_property_name.clear();
  g_runtime_arc_debug_last_property_owner_identity.clear();
  g_runtime_block_promote_call_count = 0;
  g_runtime_block_invoke_call_count = 0;
  g_runtime_last_promoted_block_handle = 0;
  g_runtime_last_promote_has_pointer_capture_storage = 0;
  g_runtime_last_invoked_block_handle = 0;
  g_runtime_last_block_invoke_result = 0;
  g_runtime_error_bridge_store_call_count = 0;
  g_runtime_error_bridge_load_call_count = 0;
  g_runtime_error_bridge_status_bridge_call_count = 0;
  g_runtime_error_bridge_nserror_bridge_call_count = 0;
  g_runtime_error_bridge_catch_match_call_count = 0;
  g_runtime_error_bridge_last_stored_error_value = 0;
  g_runtime_error_bridge_last_loaded_error_value = 0;
  g_runtime_error_bridge_last_status_bridge_status_value = 0;
  g_runtime_error_bridge_last_status_bridge_error_value = 0;
  g_runtime_error_bridge_last_nserror_bridge_error_value = 0;
  g_runtime_error_bridge_last_catch_match_error_value = 0;
  g_runtime_error_bridge_last_catch_match_kind = 0;
  g_runtime_error_bridge_last_catch_match_is_catch_all = 0;
  g_runtime_error_bridge_last_catch_match_result = 0;
  g_runtime_error_bridge_last_catch_kind_name.clear();
  g_runtime_async_continuation_allocation_call_count = 0;
  g_runtime_async_continuation_handoff_call_count = 0;
  g_runtime_async_continuation_resume_call_count = 0;
  g_runtime_async_continuation_next_handle = 1;
  g_runtime_async_continuation_last_allocated_handle = 0;
  g_runtime_async_continuation_last_allocated_resume_entry_tag = 0;
  g_runtime_async_continuation_last_allocated_executor_tag = 0;
  g_runtime_async_continuation_last_handoff_handle = 0;
  g_runtime_async_continuation_last_handoff_executor_tag = 0;
  g_runtime_async_continuation_last_resume_handle = 0;
  g_runtime_async_continuation_last_resume_result_value = 0;
  g_runtime_async_continuation_last_resume_return_value = 0;
  g_runtime_async_live_continuation_handles.clear();
  g_runtime_task_spawn_call_count = 0;
  g_runtime_task_scope_call_count = 0;
  g_runtime_task_add_task_call_count = 0;
  g_runtime_task_wait_next_call_count = 0;
  g_runtime_task_cancel_all_call_count = 0;
  g_runtime_task_cancellation_poll_call_count = 0;
  g_runtime_task_on_cancel_call_count = 0;
  g_runtime_task_executor_hop_call_count = 0;
  g_runtime_actor_isolation_thunk_call_count = 0;
  g_runtime_actor_nonisolated_entry_call_count = 0;
  g_runtime_actor_hop_to_executor_call_count = 0;
  g_runtime_actor_replay_proof_call_count = 0;
  g_runtime_actor_race_guard_call_count = 0;
  g_runtime_actor_bind_executor_call_count = 0;
  g_runtime_actor_mailbox_enqueue_call_count = 0;
  g_runtime_actor_mailbox_drain_call_count = 0;
  g_runtime_task_last_spawn_kind = 0;
  g_runtime_task_last_spawn_executor_tag = 0;
  g_runtime_task_last_scope_executor_tag = 0;
  g_runtime_task_last_add_task_executor_tag = 0;
  g_runtime_task_last_wait_next_executor_tag = 0;
  g_runtime_task_last_cancel_all_executor_tag = 0;
  g_runtime_task_last_cancellation_poll_executor_tag = 0;
  g_runtime_task_last_on_cancel_executor_tag = 0;
  g_runtime_task_last_executor_hop_executor_tag = 0;
  g_runtime_task_last_executor_hop_value = 0;
  g_runtime_task_last_wait_next_result = 0;
  g_runtime_task_last_cancel_all_result = 0;
  g_runtime_task_last_cancellation_poll_result = 0;
  g_runtime_actor_last_isolation_executor_tag = 0;
  g_runtime_actor_last_nonisolated_value = 0;
  g_runtime_actor_last_nonisolated_executor_tag = 0;
  g_runtime_actor_last_hop_value = 0;
  g_runtime_actor_last_hop_executor_tag = 0;
  g_runtime_actor_last_hop_result = 0;
  g_runtime_actor_last_replay_proof_executor_tag = 0;
  g_runtime_actor_last_race_guard_executor_tag = 0;
  g_runtime_actor_last_bound_actor_handle = 0;
  g_runtime_actor_last_bound_executor_tag = 0;
  g_runtime_actor_last_mailbox_actor_handle = 0;
  g_runtime_actor_last_mailbox_enqueued_value = 0;
  g_runtime_actor_last_mailbox_executor_tag = 0;
  g_runtime_actor_last_mailbox_depth = 0;
  g_runtime_actor_last_mailbox_drained_value = 0;
  g_runtime_actor_mailboxes.clear();
}

void RecordArcDebugPropertyContext(const RuntimeDispatchFrame *frame) {
  g_runtime_arc_debug_last_property_receiver =
      frame != nullptr ? frame->receiver : 0;
  g_runtime_arc_debug_last_property_name.clear();
  g_runtime_arc_debug_last_property_owner_identity.clear();
  if (frame == nullptr || frame->runtime_property_accessor == nullptr ||
      frame->runtime_property_accessor->property_descriptor == nullptr) {
    return;
  }
  const EmittedPropertyDescriptor &descriptor =
      *frame->runtime_property_accessor->property_descriptor;
  if (descriptor.property_name != nullptr) {
    g_runtime_arc_debug_last_property_name = descriptor.property_name;
  }
  if (descriptor.declaration_owner_identity != nullptr) {
    g_runtime_arc_debug_last_property_owner_identity =
        descriptor.declaration_owner_identity;
  } else if (descriptor.export_owner_identity != nullptr) {
    g_runtime_arc_debug_last_property_owner_identity =
        descriptor.export_owner_identity;
  } else if (descriptor.owner_identity != nullptr) {
    g_runtime_arc_debug_last_property_owner_identity = descriptor.owner_identity;
  }
}

const char *RuntimeErrorCatchKindName(int catch_kind) {
  switch (catch_kind) {
  case 1:
    return "nserror";
  case 2:
    return "id<error>";
  default:
    return "unknown";
  }
}

void PushRuntimeAutoreleasePoolFrame() {
  g_runtime_autoreleasepool_frames.push_back({});
  ++g_runtime_arc_debug_autoreleasepool_push_count;
  g_runtime_autoreleasepool_max_depth = std::max<std::uint64_t>(
      g_runtime_autoreleasepool_max_depth,
      static_cast<std::uint64_t>(g_runtime_autoreleasepool_frames.size()));
}

std::vector<int> PopRuntimeAutoreleasePoolFrameValues() {
  if (g_runtime_autoreleasepool_frames.empty()) {
    return {};
  }
  RuntimeAutoreleasePoolFrame frame =
      std::move(g_runtime_autoreleasepool_frames.back());
  g_runtime_autoreleasepool_frames.pop_back();
  return std::move(frame.values);
}

std::uint64_t CountQueuedAutoreleaseValues() {
  std::uint64_t total = 0;
  for (const RuntimeAutoreleasePoolFrame &frame : g_runtime_autoreleasepool_frames) {
    total += static_cast<std::uint64_t>(frame.values.size());
  }
  return total;
}

void EnqueueAutoreleaseValue(int value) {
  if (!objc3c::runtime::RuntimeAutoreleasePoolCanEnqueue(value)) {
    return;
  }
  g_runtime_last_autoreleased_value = value;
  if (!g_runtime_autoreleasepool_frames.empty()) {
    g_runtime_autoreleasepool_frames.back().values.push_back(value);
    return;
  }
  if (g_runtime_dispatch_frames.size() >= 2u) {
    g_runtime_dispatch_frames[g_runtime_dispatch_frames.size() - 2u]
        .autorelease_values.push_back(value);
    return;
  }
  if (RuntimeDispatchFrame *frame = CurrentRuntimeDispatchFrame()) {
    frame->autorelease_values.push_back(value);
  }
}

bool IsRuntimeManagedReceiverValueUnlocked(const RuntimeState &state, int value) {
  return objc3c::runtime::RuntimeInstanceReceiverIsManaged(value) &&
         state.runtime_instances_by_receiver.find(value) !=
             state.runtime_instances_by_receiver.end();
}

void RemoveWeakSlotRefUnlocked(RuntimeState &state, int target_receiver,
                               int owner_receiver, std::size_t offset,
                               std::size_t size) {
  if (!objc3c::runtime::RuntimeWeakSlotTargetIsTrackable(target_receiver)) {
    return;
  }
  const auto found = state.weak_slot_refs_by_target_receiver.find(target_receiver);
  if (found == state.weak_slot_refs_by_target_receiver.end()) {
    return;
  }
  auto &refs = found->second;
  refs.erase(
      std::remove_if(
          refs.begin(), refs.end(),
          [&](const RuntimeWeakSlotRef &ref) {
            return ref.owner_receiver == owner_receiver && ref.offset == offset &&
                   ref.size == size;
          }),
      refs.end());
  if (refs.empty()) {
    state.weak_slot_refs_by_target_receiver.erase(found);
  }
}

void RegisterWeakSlotRefUnlocked(RuntimeState &state, int target_receiver,
                                 int owner_receiver, std::size_t offset,
                                 std::size_t size) {
  if (!objc3c::runtime::RuntimeWeakSlotTargetIsTrackable(target_receiver)) {
    return;
  }
  std::vector<RuntimeWeakSlotRef> &refs =
      state.weak_slot_refs_by_target_receiver[target_receiver];
  const auto duplicate =
      std::find_if(refs.begin(), refs.end(), [&](const RuntimeWeakSlotRef &ref) {
        return ref.owner_receiver == owner_receiver && ref.offset == offset &&
               ref.size == size;
      });
  if (duplicate == refs.end()) {
    refs.push_back(RuntimeWeakSlotRef{owner_receiver, offset, size});
  }
}

void RemoveWeakSlotRefsOwnedByReceiverUnlocked(RuntimeState &state,
                                               int owner_receiver) {
  for (auto it = state.weak_slot_refs_by_target_receiver.begin();
       it != state.weak_slot_refs_by_target_receiver.end();) {
    auto &refs = it->second;
    refs.erase(std::remove_if(refs.begin(), refs.end(),
                              [&](const RuntimeWeakSlotRef &ref) {
                                return ref.owner_receiver == owner_receiver;
                              }),
               refs.end());
    if (refs.empty()) {
      it = state.weak_slot_refs_by_target_receiver.erase(it);
    } else {
      ++it;
    }
  }
}

void ZeroWeakSlotRefsForTargetUnlocked(RuntimeState &state, int target_receiver) {
  if (!objc3c::runtime::RuntimeWeakSlotTargetIsTrackable(target_receiver)) {
    return;
  }
  const auto found = state.weak_slot_refs_by_target_receiver.find(target_receiver);
  if (found == state.weak_slot_refs_by_target_receiver.end()) {
    return;
  }
  for (const RuntimeWeakSlotRef &ref : found->second) {
    const auto owner_it = state.runtime_instances_by_receiver.find(ref.owner_receiver);
    if (owner_it == state.runtime_instances_by_receiver.end()) {
      continue;
    }
    RuntimeInstanceRecord &owner = owner_it->second;
    if (ref.size == 0u || ref.offset + ref.size > owner.storage_bytes.size()) {
      continue;
    }
    std::fill(owner.storage_bytes.begin() + static_cast<std::ptrdiff_t>(ref.offset),
              owner.storage_bytes.begin() +
                  static_cast<std::ptrdiff_t>(ref.offset + ref.size),
              0);
  }
  state.weak_slot_refs_by_target_receiver.erase(found);
}

std::string BuildSynthesizedInstanceMethodOwnerIdentity(
    const char *declaration_owner_identity, const char *selector) {
  const std::string owner =
      declaration_owner_identity != nullptr ? declaration_owner_identity : "";
  const std::string selector_text = selector != nullptr ? selector : "";
  return owner + "::instance_method:" + selector_text;
}

const EmittedMethodListEntry *MethodListEntries(
    const EmittedMethodListHeader *header) {
  return header == nullptr
             ? nullptr
             : reinterpret_cast<const EmittedMethodListEntry *>(header + 1);
}

const EmittedMethodListRef *SelectMethodListRef(const EmittedProtocolRecord &record,
                                                DispatchFamily family) {
  return family == DispatchFamily::Class ? record.class_method_list_ref
                                         : record.instance_method_list_ref;
}

const EmittedMethodListRef *SelectMethodListRef(const EmittedCategoryRecord &record,
                                                DispatchFamily family) {
  return family == DispatchFamily::Class ? record.class_method_list_ref
                                         : record.instance_method_list_ref;
}

bool SelectorMatchesMethodEntryUnlocked(RuntimeState &,
                                        const char *entry_selector,
                                        std::uint64_t selector_stable_id,
                                        const char *selector_spelling) {
  if (entry_selector == nullptr) {
    return false;
  }
  if (selector_stable_id == 0 && selector_spelling != nullptr &&
      selector_spelling[0] != '\0') {
    const objc3_runtime_selector_handle *lookup_handle =
        LookupSelectorUnlocked(selector_spelling);
    selector_stable_id = lookup_handle != nullptr ? lookup_handle->stable_id : 0;
  }
  if (selector_stable_id == 0) {
    return false;
  }
  const objc3_runtime_selector_handle *entry_handle =
      LookupSelectorUnlocked(entry_selector);
  return entry_handle != nullptr &&
         entry_handle->stable_id == selector_stable_id;
}

bool TryResolveMethodFromMethodListRefUnlocked(
    RuntimeState &state, const EmittedMethodListRef *method_list_ref,
    const char *resolved_class_name, DispatchFamily family,
    std::uint64_t normalized_receiver_identity, std::uint64_t selector_stable_id,
    const char *selector_spelling, SlowPathResolution &resolution,
    bool &ambiguous) {
  if (method_list_ref == nullptr || method_list_ref->count == 0 ||
      method_list_ref->method_list == nullptr) {
    return true;
  }
  const auto *header =
      static_cast<const EmittedMethodListHeader *>(method_list_ref->method_list);
  if (header == nullptr || header->count != method_list_ref->count) {
    resolution.strict_error_status =
        OBJC3_RUNTIME_DISPATCH_STATUS_MALFORMED_METADATA;
    return false;
  }
  const EmittedMethodListEntry *entries = MethodListEntries(header);
  for (std::uint64_t index = 0; index < header->count; ++index) {
    const EmittedMethodListEntry &entry = entries[index];
    if (entry.selector == nullptr || entry.owner_identity == nullptr ||
        entry.return_type_name == nullptr) {
      resolution.strict_error_status =
          OBJC3_RUNTIME_DISPATCH_STATUS_MALFORMED_METADATA;
      return false;
    }
    if (!SelectorMatchesMethodEntryUnlocked(state, entry.selector,
                                            selector_stable_id,
                                            selector_spelling)) {
      continue;
    }
    if (entry.has_body == 0 || entry.implementation == nullptr) {
      continue;
    }
    const objc3_runtime_dispatch_status_code shape_status =
        RuntimeMethodShapeStatus(entry.return_type_name, entry.parameter_count);
    if (shape_status != OBJC3_RUNTIME_DISPATCH_STATUS_OK) {
      resolution.strict_error_status = shape_status;
      return true;
    }
    const RuntimeMethodReturnKind entry_return_kind =
        ClassifyRuntimeReturnType(entry.return_type_name);
    if (resolution.resolved &&
        (resolution.implementation != entry.implementation ||
         resolution.parameter_count != entry.parameter_count ||
         resolution.return_kind != entry_return_kind ||
         resolution.owner_identity != entry.owner_identity ||
         resolution.class_name !=
             (resolved_class_name != nullptr ? resolved_class_name : ""))) {
      ambiguous = true;
      return true;
    }
    resolution.resolved = true;
    resolution.dispatch_family_is_class = family == DispatchFamily::Class;
    resolution.selector_storage = selector_spelling != nullptr ? selector_spelling : "";
    resolution.class_name =
        resolved_class_name != nullptr ? resolved_class_name : "";
    resolution.owner_identity = entry.owner_identity;
    resolution.normalized_receiver_identity = normalized_receiver_identity;
    resolution.selector_stable_id = selector_stable_id;
    resolution.parameter_count = entry.parameter_count;
    resolution.return_kind = entry_return_kind;
    resolution.implementation = entry.implementation;
    resolution.effective_direct_dispatch = entry.effective_direct_dispatch;
    resolution.objc_final_declared = entry.objc_final_declared;
  }
  return true;
}

bool ProbeProtocolSelectorDeclarationUnlocked(
    RuntimeState &state, const EmittedProtocolRecord *start_record,
    DispatchFamily family, std::uint64_t selector_stable_id,
    const char *selector_spelling, std::uint64_t &protocol_probe_count,
    bool &declared, bool &ambiguous) {
  // protocol/category-aware resolution anchor: adopted/inherited
  // protocol method lists now provide declaration-aware negative resolution
  // while remaining non-callable, so unresolved selectors publish strict
  // dispatch errors with explicit protocol probe evidence.
  (void)ambiguous;
  if (start_record == nullptr) {
    return true;
  }
  std::vector<const EmittedProtocolRecord *> stack;
  stack.push_back(start_record);
  std::unordered_set<const EmittedProtocolRecord *> visited;
  while (!stack.empty()) {
    const EmittedProtocolRecord *record = stack.back();
    stack.pop_back();
    if (record == nullptr || !visited.insert(record).second) {
      continue;
    }
    if (record->protocol_name == nullptr || record->owner_identity == nullptr ||
        !objc3c::runtime::RuntimeProtocolConformanceEdgeIsMaterializable(
            "protocol-adopter", record->protocol_name)) {
      return false;
    }
    ++protocol_probe_count;
    const EmittedMethodListRef *method_list_ref = SelectMethodListRef(*record, family);
    if (method_list_ref != nullptr && method_list_ref->count != 0 &&
        method_list_ref->method_list != nullptr) {
      const auto *header =
          static_cast<const EmittedMethodListHeader *>(method_list_ref->method_list);
      if (header == nullptr || header->count != method_list_ref->count) {
        return false;
      }
      const EmittedMethodListEntry *entries = MethodListEntries(header);
      for (std::uint64_t index = 0; index < header->count; ++index) {
        const EmittedMethodListEntry &entry = entries[index];
        if (entry.selector == nullptr || entry.owner_identity == nullptr ||
            entry.return_type_name == nullptr) {
          return false;
        }
        if (!SelectorMatchesMethodEntryUnlocked(state, entry.selector,
                                                selector_stable_id,
                                                selector_spelling)) {
          continue;
        }
        declared = true;
      }
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
