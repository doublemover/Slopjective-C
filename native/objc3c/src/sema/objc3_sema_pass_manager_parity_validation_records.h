#pragma once

#include <cstddef>
#include <string>

#include "sema/objc3_sema_contract.h"
#include "sema/objc3_sema_pass_flow_core_contract.h"

struct Objc3SemaUnsafeErrorParityValidationReadinessRecord {
  std::string unsafe_error_parity_validation_readiness_owner =
      kObjc3SemaUnsafeErrorParityValidationReadinessOwner;
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string typed_semantic_handoff_owner =
      kObjc3SemaTypedSemanticHandoffOwner;
  std::string owner_model = kObjc3SemaNoRetiredRouteOwnerModel;
  bool strict_no_retired_route = true;
  bool strict_no_compatibility = true;
  std::size_t required_validation_count = 5u;
  std::size_t passed_validation_count = 0;
  std::size_t failed_validation_count = 0;
  bool unsafe_pointer_extension_ready = false;
  bool inline_asm_intrinsic_governance_ready = false;
  bool ns_error_bridging_ready = false;
  bool error_diagnostics_recovery_ready = false;
  bool result_like_lowering_ready = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3SemaUnsafeErrorParityValidationReadinessRecord(
    const Objc3SemaUnsafeErrorParityValidationReadinessRecord &record) {
  return Objc3SemaOwnerIsExplicit(
             record.unsafe_error_parity_validation_readiness_owner) &&
         Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
         Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
         record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
         record.strict_no_retired_route && record.strict_no_compatibility &&
         record.required_validation_count == 5u &&
         record.passed_validation_count == record.required_validation_count &&
         record.failed_validation_count == 0u &&
         record.unsafe_pointer_extension_ready &&
         record.inline_asm_intrinsic_governance_ready &&
         record.ns_error_bridging_ready &&
         record.error_diagnostics_recovery_ready &&
         record.result_like_lowering_ready && record.deterministic;
}

struct Objc3SemaControlBindingParityValidationReadinessRecord {
  std::string control_binding_parity_validation_readiness_owner =
      kObjc3SemaControlBindingParityValidationReadinessOwner;
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string typed_semantic_handoff_owner =
      kObjc3SemaTypedSemanticHandoffOwner;
  std::string owner_model = kObjc3SemaNoRetiredRouteOwnerModel;
  bool strict_no_retired_route = true;
  bool strict_no_compatibility = true;
  std::size_t required_validation_count = 6u;
  std::size_t passed_validation_count = 0;
  std::size_t failed_validation_count = 0;
  bool unwind_cleanup_ready = false;
  bool async_continuation_ready = false;
  bool symbol_graph_scope_resolution_ready = false;
  bool method_lookup_override_conflict_ready = false;
  bool property_synthesis_ivar_binding_ready = false;
  bool id_class_sel_object_pointer_type_checking_ready = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3SemaControlBindingParityValidationReadinessRecord(
    const Objc3SemaControlBindingParityValidationReadinessRecord &record) {
  return Objc3SemaOwnerIsExplicit(
             record.control_binding_parity_validation_readiness_owner) &&
         Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
         Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
         record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
         record.strict_no_retired_route && record.strict_no_compatibility &&
         record.required_validation_count == 6u &&
         record.passed_validation_count == record.required_validation_count &&
         record.failed_validation_count == 0u &&
         record.unwind_cleanup_ready && record.async_continuation_ready &&
         record.symbol_graph_scope_resolution_ready &&
         record.method_lookup_override_conflict_ready &&
         record.property_synthesis_ivar_binding_ready &&
         record.id_class_sel_object_pointer_type_checking_ready &&
         record.deterministic;
}

struct Objc3SemaAsyncBlockMessageParityValidationReadinessRecord {
  std::string async_block_message_parity_validation_readiness_owner =
      kObjc3SemaAsyncBlockMessageParityValidationReadinessOwner;
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string typed_semantic_handoff_owner =
      kObjc3SemaTypedSemanticHandoffOwner;
  std::string owner_model = kObjc3SemaNoRetiredRouteOwnerModel;
  bool strict_no_retired_route = true;
  bool strict_no_compatibility = true;
  std::size_t required_validation_count = 7u;
  std::size_t passed_validation_count = 0;
  std::size_t failed_validation_count = 0;
  bool await_lowering_suspension_state_lowering_ready = false;
  bool block_literal_capture_semantics_ready = false;
  bool block_abi_invoke_trampoline_ready = false;
  bool block_storage_escape_ready = false;
  bool block_copy_dispose_ready = false;
  bool block_determinism_perf_baseline_ready = false;
  bool message_send_selector_lowering_ready = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3SemaAsyncBlockMessageParityValidationReadinessRecord(
    const Objc3SemaAsyncBlockMessageParityValidationReadinessRecord &record) {
  return Objc3SemaOwnerIsExplicit(
             record.async_block_message_parity_validation_readiness_owner) &&
         Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
         Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
         record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
         record.strict_no_retired_route && record.strict_no_compatibility &&
         record.required_validation_count == 7u &&
         record.passed_validation_count == record.required_validation_count &&
         record.failed_validation_count == 0u &&
         record.await_lowering_suspension_state_lowering_ready &&
         record.block_literal_capture_semantics_ready &&
         record.block_abi_invoke_trampoline_ready &&
         record.block_storage_escape_ready && record.block_copy_dispose_ready &&
         record.block_determinism_perf_baseline_ready &&
         record.message_send_selector_lowering_ready && record.deterministic;
}

struct Objc3SemaDispatchRuntimeArcParityValidationReadinessRecord {
  std::string dispatch_runtime_arc_parity_validation_readiness_owner =
      kObjc3SemaDispatchRuntimeArcParityValidationReadinessOwner;
  std::string stage_input_owner = kObjc3SemaStageInputOwner;
  std::string typed_semantic_handoff_owner =
      kObjc3SemaTypedSemanticHandoffOwner;
  std::string owner_model = kObjc3SemaNoRetiredRouteOwnerModel;
  bool strict_no_retired_route = true;
  bool strict_no_compatibility = true;
  std::size_t required_validation_count = 8u;
  std::size_t passed_validation_count = 0;
  std::size_t failed_validation_count = 0;
  bool dispatch_abi_marshalling_ready = false;
  bool nil_receiver_semantics_foldability_ready = false;
  bool super_dispatch_method_family_ready = false;
  bool runtime_link_host_link_ready = false;
  bool retain_release_operation_ready = false;
  bool weak_unowned_semantics_ready = false;
  bool arc_diagnostics_fixit_ready = false;
  bool autoreleasepool_scope_ready = false;
  bool deterministic = false;
};

inline bool IsReadyObjc3SemaDispatchRuntimeArcParityValidationReadinessRecord(
    const Objc3SemaDispatchRuntimeArcParityValidationReadinessRecord &record) {
  return Objc3SemaOwnerIsExplicit(
             record.dispatch_runtime_arc_parity_validation_readiness_owner) &&
         Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
         Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
         record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
         record.strict_no_retired_route && record.strict_no_compatibility &&
         record.required_validation_count == 8u &&
         record.passed_validation_count == record.required_validation_count &&
         record.failed_validation_count == 0u &&
         record.dispatch_abi_marshalling_ready &&
         record.nil_receiver_semantics_foldability_ready &&
         record.super_dispatch_method_family_ready &&
         record.runtime_link_host_link_ready &&
         record.retain_release_operation_ready &&
         record.weak_unowned_semantics_ready &&
         record.arc_diagnostics_fixit_ready &&
         record.autoreleasepool_scope_ready && record.deterministic;
}
