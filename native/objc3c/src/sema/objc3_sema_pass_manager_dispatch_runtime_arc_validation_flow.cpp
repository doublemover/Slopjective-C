#include "sema/objc3_sema_pass_manager_contract_flow.h"

Objc3SemaDispatchRuntimeArcParityValidationReadinessRecord
BuildObjc3SemaDispatchRuntimeArcParityValidationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_dispatch_abi_marshalling_handoff,
    bool deterministic_nil_receiver_semantics_foldability_handoff,
    bool deterministic_super_dispatch_method_family_handoff,
    bool deterministic_runtime_link_host_link_handoff,
    bool deterministic_retain_release_operation_handoff,
    bool deterministic_weak_unowned_semantics_handoff,
    bool deterministic_arc_diagnostics_fixit_handoff,
    bool deterministic_autoreleasepool_scope_handoff) {
  Objc3SemaDispatchRuntimeArcParityValidationReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_retired_route = input.strict_no_retired_route;
  record.strict_no_compatibility = input.strict_no_compatibility;
  const auto &dispatch_summary = surface.dispatch_abi_marshalling_summary;
  record.dispatch_abi_marshalling_ready =
      deterministic_dispatch_abi_marshalling_handoff &&
      dispatch_summary.message_send_sites ==
          surface.dispatch_abi_marshalling_sites_total &&
      dispatch_summary.receiver_slots ==
          surface.dispatch_abi_marshalling_receiver_slots_total &&
      dispatch_summary.selector_symbol_slots ==
          surface.dispatch_abi_marshalling_selector_symbol_slots_total &&
      dispatch_summary.argument_slots ==
          surface.dispatch_abi_marshalling_argument_slots_total &&
      dispatch_summary.keyword_argument_slots ==
          surface.dispatch_abi_marshalling_keyword_argument_slots_total &&
      dispatch_summary.unary_argument_slots ==
          surface.dispatch_abi_marshalling_unary_argument_slots_total &&
      dispatch_summary.arity_mismatch_sites ==
          surface.dispatch_abi_marshalling_arity_mismatch_sites_total &&
      dispatch_summary.missing_selector_symbol_sites ==
          surface.dispatch_abi_marshalling_missing_selector_symbol_sites_total &&
      dispatch_summary.contract_violation_sites ==
          surface.dispatch_abi_marshalling_contract_violation_sites_total &&
      dispatch_summary.receiver_slots == dispatch_summary.message_send_sites &&
      dispatch_summary.selector_symbol_slots +
              dispatch_summary.missing_selector_symbol_sites ==
          dispatch_summary.message_send_sites &&
      dispatch_summary.keyword_argument_slots +
              dispatch_summary.unary_argument_slots ==
          dispatch_summary.argument_slots &&
      dispatch_summary.keyword_argument_slots <= dispatch_summary.argument_slots &&
      dispatch_summary.unary_argument_slots <= dispatch_summary.argument_slots &&
      dispatch_summary.selector_symbol_slots <=
          dispatch_summary.message_send_sites &&
      dispatch_summary.missing_selector_symbol_sites <=
          dispatch_summary.message_send_sites &&
      dispatch_summary.arity_mismatch_sites <=
          dispatch_summary.message_send_sites &&
      dispatch_summary.contract_violation_sites <=
          dispatch_summary.message_send_sites &&
      dispatch_summary.deterministic;
  const auto &nil_receiver_summary =
      surface.nil_receiver_semantics_foldability_summary;
  record.nil_receiver_semantics_foldability_ready =
      deterministic_nil_receiver_semantics_foldability_handoff &&
      nil_receiver_summary.message_send_sites ==
          surface.nil_receiver_semantics_foldability_sites_total &&
      nil_receiver_summary.receiver_nil_literal_sites ==
          surface
              .nil_receiver_semantics_foldability_receiver_nil_literal_sites_total &&
      nil_receiver_summary.nil_receiver_semantics_enabled_sites ==
          surface.nil_receiver_semantics_foldability_enabled_sites_total &&
      nil_receiver_summary.nil_receiver_foldable_sites ==
          surface.nil_receiver_semantics_foldability_foldable_sites_total &&
      nil_receiver_summary.nil_receiver_runtime_dispatch_required_sites ==
          surface
              .nil_receiver_semantics_foldability_runtime_dispatch_required_sites_total &&
      nil_receiver_summary.non_nil_receiver_sites ==
          surface.nil_receiver_semantics_foldability_non_nil_receiver_sites_total &&
      nil_receiver_summary.contract_violation_sites ==
          surface.nil_receiver_semantics_foldability_contract_violation_sites_total &&
      nil_receiver_summary.receiver_nil_literal_sites ==
          nil_receiver_summary.nil_receiver_semantics_enabled_sites &&
      nil_receiver_summary.nil_receiver_foldable_sites <=
          nil_receiver_summary.nil_receiver_semantics_enabled_sites &&
      nil_receiver_summary.nil_receiver_runtime_dispatch_required_sites +
              nil_receiver_summary.nil_receiver_foldable_sites ==
          nil_receiver_summary.message_send_sites &&
      nil_receiver_summary.nil_receiver_semantics_enabled_sites +
              nil_receiver_summary.non_nil_receiver_sites ==
          nil_receiver_summary.message_send_sites &&
      nil_receiver_summary.contract_violation_sites <=
          nil_receiver_summary.message_send_sites &&
      nil_receiver_summary.deterministic;
  const auto &super_dispatch_summary =
      surface.super_dispatch_method_family_summary;
  record.super_dispatch_method_family_ready =
      deterministic_super_dispatch_method_family_handoff &&
      super_dispatch_summary.message_send_sites ==
          surface.super_dispatch_method_family_sites_total &&
      super_dispatch_summary.receiver_super_identifier_sites ==
          surface
              .super_dispatch_method_family_receiver_super_identifier_sites_total &&
      super_dispatch_summary.super_dispatch_enabled_sites ==
          surface.super_dispatch_method_family_enabled_sites_total &&
      super_dispatch_summary.super_dispatch_requires_class_context_sites ==
          surface
              .super_dispatch_method_family_requires_class_context_sites_total &&
      super_dispatch_summary.method_family_init_sites ==
          surface.super_dispatch_method_family_init_sites_total &&
      super_dispatch_summary.method_family_copy_sites ==
          surface.super_dispatch_method_family_copy_sites_total &&
      super_dispatch_summary.method_family_mutable_copy_sites ==
          surface.super_dispatch_method_family_mutable_copy_sites_total &&
      super_dispatch_summary.method_family_new_sites ==
          surface.super_dispatch_method_family_new_sites_total &&
      super_dispatch_summary.method_family_none_sites ==
          surface.super_dispatch_method_family_none_sites_total &&
      super_dispatch_summary.method_family_returns_retained_result_sites ==
          surface
              .super_dispatch_method_family_returns_retained_result_sites_total &&
      super_dispatch_summary.method_family_returns_related_result_sites ==
          surface
              .super_dispatch_method_family_returns_related_result_sites_total &&
      super_dispatch_summary.contract_violation_sites ==
          surface.super_dispatch_method_family_contract_violation_sites_total &&
      super_dispatch_summary.receiver_super_identifier_sites ==
          super_dispatch_summary.super_dispatch_enabled_sites &&
      super_dispatch_summary.super_dispatch_requires_class_context_sites ==
          super_dispatch_summary.super_dispatch_enabled_sites &&
      super_dispatch_summary.method_family_init_sites +
              super_dispatch_summary.method_family_copy_sites +
              super_dispatch_summary.method_family_mutable_copy_sites +
              super_dispatch_summary.method_family_new_sites +
              super_dispatch_summary.method_family_none_sites ==
          super_dispatch_summary.message_send_sites &&
      super_dispatch_summary.method_family_returns_related_result_sites <=
          super_dispatch_summary.method_family_init_sites &&
      super_dispatch_summary.method_family_returns_retained_result_sites <=
          super_dispatch_summary.message_send_sites &&
      super_dispatch_summary.contract_violation_sites <=
          super_dispatch_summary.message_send_sites &&
      super_dispatch_summary.deterministic;
  const auto &runtime_link_summary = surface.runtime_link_host_link_summary;
  record.runtime_link_host_link_ready =
      deterministic_runtime_link_host_link_handoff &&
      runtime_link_summary.message_send_sites ==
          surface.runtime_link_host_link_message_send_sites_total &&
      runtime_link_summary.runtime_link_required_sites ==
          surface.runtime_link_host_link_required_sites_total &&
      runtime_link_summary.runtime_link_elided_sites ==
          surface.runtime_link_host_link_elided_sites_total &&
      runtime_link_summary.runtime_dispatch_arg_slots ==
          surface.runtime_link_host_link_runtime_dispatch_arg_slots_total &&
      runtime_link_summary.runtime_dispatch_declaration_parameter_count ==
          surface
              .runtime_link_host_link_runtime_dispatch_declaration_parameter_count_total &&
      runtime_link_summary.contract_violation_sites ==
          surface.runtime_link_host_link_contract_violation_sites_total &&
      runtime_link_summary.runtime_dispatch_symbol ==
          surface.runtime_link_host_link_runtime_dispatch_symbol &&
      runtime_link_summary.default_runtime_dispatch_symbol_binding ==
          surface
              .runtime_link_host_link_default_runtime_dispatch_symbol_binding &&
      runtime_link_summary.runtime_link_required_sites +
              runtime_link_summary.runtime_link_elided_sites ==
          runtime_link_summary.message_send_sites &&
      runtime_link_summary.contract_violation_sites <=
          runtime_link_summary.message_send_sites &&
      (runtime_link_summary.message_send_sites == 0 ||
       runtime_link_summary.runtime_dispatch_declaration_parameter_count ==
           runtime_link_summary.runtime_dispatch_arg_slots + 2u) &&
      (runtime_link_summary.default_runtime_dispatch_symbol_binding ==
       (runtime_link_summary.runtime_dispatch_symbol ==
        kObjc3RuntimeLinkHostLinkDefaultDispatchSymbol)) &&
      runtime_link_summary.deterministic;
  const auto &retain_release_summary = surface.retain_release_operation_summary;
  record.retain_release_operation_ready =
      deterministic_retain_release_operation_handoff &&
      retain_release_summary.ownership_qualified_sites ==
          surface.retain_release_operation_ownership_qualified_sites_total &&
      retain_release_summary.retain_insertion_sites ==
          surface.retain_release_operation_retain_insertion_sites_total &&
      retain_release_summary.release_insertion_sites ==
          surface.retain_release_operation_release_insertion_sites_total &&
      retain_release_summary.autorelease_insertion_sites ==
          surface.retain_release_operation_autorelease_insertion_sites_total &&
      retain_release_summary.contract_violation_sites ==
          surface.retain_release_operation_contract_violation_sites_total &&
      retain_release_summary.retain_insertion_sites <=
          retain_release_summary.ownership_qualified_sites +
              retain_release_summary.contract_violation_sites &&
      retain_release_summary.release_insertion_sites <=
          retain_release_summary.ownership_qualified_sites +
              retain_release_summary.contract_violation_sites &&
      retain_release_summary.autorelease_insertion_sites <=
          retain_release_summary.ownership_qualified_sites +
              retain_release_summary.contract_violation_sites &&
      retain_release_summary.deterministic;
  const auto &weak_unowned_summary = surface.weak_unowned_semantics_summary;
  record.weak_unowned_semantics_ready =
      deterministic_weak_unowned_semantics_handoff &&
      weak_unowned_summary.ownership_candidate_sites ==
          surface.weak_unowned_semantics_ownership_candidate_sites_total &&
      weak_unowned_summary.weak_reference_sites ==
          surface.weak_unowned_semantics_weak_reference_sites_total &&
      weak_unowned_summary.unowned_reference_sites ==
          surface.weak_unowned_semantics_unowned_reference_sites_total &&
      weak_unowned_summary.unowned_safe_reference_sites ==
          surface.weak_unowned_semantics_unowned_safe_reference_sites_total &&
      weak_unowned_summary.weak_unowned_conflict_sites ==
          surface.weak_unowned_semantics_conflict_sites_total &&
      weak_unowned_summary.contract_violation_sites ==
          surface.weak_unowned_semantics_contract_violation_sites_total &&
      weak_unowned_summary.unowned_safe_reference_sites <=
          weak_unowned_summary.unowned_reference_sites &&
      weak_unowned_summary.weak_unowned_conflict_sites <=
          weak_unowned_summary.ownership_candidate_sites &&
      weak_unowned_summary.contract_violation_sites <=
          weak_unowned_summary.ownership_candidate_sites +
              weak_unowned_summary.weak_unowned_conflict_sites &&
      weak_unowned_summary.deterministic;
  const auto &arc_diagnostics_summary = surface.arc_diagnostics_fixit_summary;
  record.arc_diagnostics_fixit_ready =
      deterministic_arc_diagnostics_fixit_handoff &&
      arc_diagnostics_summary.ownership_arc_diagnostic_candidate_sites ==
          surface.ownership_arc_diagnostic_candidate_sites_total &&
      arc_diagnostics_summary.ownership_arc_fixit_available_sites ==
          surface.ownership_arc_fixit_available_sites_total &&
      arc_diagnostics_summary.ownership_arc_profiled_sites ==
          surface.ownership_arc_profiled_sites_total &&
      arc_diagnostics_summary.ownership_arc_weak_unowned_conflict_diagnostic_sites ==
          surface.ownership_arc_weak_unowned_conflict_diagnostic_sites_total &&
      arc_diagnostics_summary.ownership_arc_empty_fixit_hint_sites ==
          surface.ownership_arc_empty_fixit_hint_sites_total &&
      arc_diagnostics_summary.contract_violation_sites ==
          surface.ownership_arc_contract_violation_sites_total &&
      arc_diagnostics_summary.ownership_arc_fixit_available_sites <=
          arc_diagnostics_summary.ownership_arc_diagnostic_candidate_sites +
              arc_diagnostics_summary.contract_violation_sites &&
      arc_diagnostics_summary.ownership_arc_profiled_sites <=
          arc_diagnostics_summary.ownership_arc_diagnostic_candidate_sites +
              arc_diagnostics_summary.contract_violation_sites &&
      arc_diagnostics_summary
              .ownership_arc_weak_unowned_conflict_diagnostic_sites <=
          arc_diagnostics_summary.ownership_arc_diagnostic_candidate_sites +
              arc_diagnostics_summary.contract_violation_sites &&
      arc_diagnostics_summary.ownership_arc_empty_fixit_hint_sites <=
          arc_diagnostics_summary.ownership_arc_fixit_available_sites +
              arc_diagnostics_summary.contract_violation_sites &&
      arc_diagnostics_summary.deterministic;
  const auto &autoreleasepool_summary = surface.autoreleasepool_scope_summary;
  record.autoreleasepool_scope_ready =
      deterministic_autoreleasepool_scope_handoff &&
      autoreleasepool_summary.scope_sites ==
          surface.autoreleasepool_scope_sites_total &&
      autoreleasepool_summary.scope_symbolized_sites ==
          surface.autoreleasepool_scope_symbolized_sites_total &&
      autoreleasepool_summary.contract_violation_sites ==
          surface.autoreleasepool_scope_contract_violation_sites_total &&
      autoreleasepool_summary.max_scope_depth ==
          surface.autoreleasepool_scope_max_depth_total &&
      autoreleasepool_summary.scope_symbolized_sites <=
          autoreleasepool_summary.scope_sites &&
      autoreleasepool_summary.contract_violation_sites <=
          autoreleasepool_summary.scope_sites &&
      (autoreleasepool_summary.scope_sites > 0u ||
       autoreleasepool_summary.max_scope_depth == 0u) &&
      autoreleasepool_summary.max_scope_depth <=
          static_cast<unsigned>(autoreleasepool_summary.scope_sites) &&
      autoreleasepool_summary.deterministic;
  record.passed_validation_count =
      Objc3SemaEvidenceCount(record.dispatch_abi_marshalling_ready) +
      Objc3SemaEvidenceCount(
          record.nil_receiver_semantics_foldability_ready) +
      Objc3SemaEvidenceCount(record.super_dispatch_method_family_ready) +
      Objc3SemaEvidenceCount(record.runtime_link_host_link_ready) +
      Objc3SemaEvidenceCount(record.retain_release_operation_ready) +
      Objc3SemaEvidenceCount(record.weak_unowned_semantics_ready) +
      Objc3SemaEvidenceCount(record.arc_diagnostics_fixit_ready) +
      Objc3SemaEvidenceCount(record.autoreleasepool_scope_ready);
  record.failed_validation_count =
      record.required_validation_count >= record.passed_validation_count
          ? (record.required_validation_count -
             record.passed_validation_count)
          : record.required_validation_count;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.dispatch_runtime_arc_parity_validation_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
      record.strict_no_retired_route && record.strict_no_compatibility &&
      record.required_validation_count == 8u &&
      record.passed_validation_count == record.required_validation_count &&
      record.failed_validation_count == 0u;
  return record;
}
