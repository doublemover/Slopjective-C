#include "sema/objc3_sema_dispatch_ownership_equivalence.h"

bool IsEquivalentMessageSendSelectorLoweringSummary(
    const Objc3MessageSendSelectorLoweringSummary &lhs,
    const Objc3MessageSendSelectorLoweringSummary &rhs) {
  return lhs.message_send_sites == rhs.message_send_sites &&
         lhs.unary_form_sites == rhs.unary_form_sites &&
         lhs.keyword_form_sites == rhs.keyword_form_sites &&
         lhs.selector_lowering_symbol_sites == rhs.selector_lowering_symbol_sites &&
         lhs.selector_lowering_piece_entries == rhs.selector_lowering_piece_entries &&
         lhs.selector_lowering_argument_piece_entries == rhs.selector_lowering_argument_piece_entries &&
         lhs.selector_lowering_normalized_sites == rhs.selector_lowering_normalized_sites &&
         lhs.selector_lowering_form_mismatch_sites == rhs.selector_lowering_form_mismatch_sites &&
         lhs.selector_lowering_arity_mismatch_sites == rhs.selector_lowering_arity_mismatch_sites &&
         lhs.selector_lowering_symbol_mismatch_sites == rhs.selector_lowering_symbol_mismatch_sites &&
         lhs.selector_lowering_missing_symbol_sites == rhs.selector_lowering_missing_symbol_sites &&
         lhs.selector_lowering_contract_violation_sites == rhs.selector_lowering_contract_violation_sites;
}

bool IsEquivalentDispatchAbiMarshallingSummary(
    const Objc3DispatchAbiMarshallingSummary &lhs,
    const Objc3DispatchAbiMarshallingSummary &rhs) {
  return lhs.message_send_sites == rhs.message_send_sites &&
         lhs.receiver_slots == rhs.receiver_slots &&
         lhs.selector_symbol_slots == rhs.selector_symbol_slots &&
         lhs.argument_slots == rhs.argument_slots &&
         lhs.keyword_argument_slots == rhs.keyword_argument_slots &&
         lhs.unary_argument_slots == rhs.unary_argument_slots &&
         lhs.arity_mismatch_sites == rhs.arity_mismatch_sites &&
         lhs.missing_selector_symbol_sites == rhs.missing_selector_symbol_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentNilReceiverSemanticsFoldabilitySummary(
    const Objc3NilReceiverSemanticsFoldabilitySummary &lhs,
    const Objc3NilReceiverSemanticsFoldabilitySummary &rhs) {
  return lhs.message_send_sites == rhs.message_send_sites &&
         lhs.receiver_nil_literal_sites == rhs.receiver_nil_literal_sites &&
         lhs.nil_receiver_semantics_enabled_sites == rhs.nil_receiver_semantics_enabled_sites &&
         lhs.nil_receiver_foldable_sites == rhs.nil_receiver_foldable_sites &&
         lhs.nil_receiver_runtime_dispatch_required_sites == rhs.nil_receiver_runtime_dispatch_required_sites &&
         lhs.non_nil_receiver_sites == rhs.non_nil_receiver_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentSuperDispatchMethodFamilySummary(
    const Objc3SuperDispatchMethodFamilySummary &lhs,
    const Objc3SuperDispatchMethodFamilySummary &rhs) {
  return lhs.message_send_sites == rhs.message_send_sites &&
         lhs.receiver_super_identifier_sites == rhs.receiver_super_identifier_sites &&
         lhs.super_dispatch_enabled_sites == rhs.super_dispatch_enabled_sites &&
         lhs.super_dispatch_requires_class_context_sites == rhs.super_dispatch_requires_class_context_sites &&
         lhs.method_family_init_sites == rhs.method_family_init_sites &&
         lhs.method_family_copy_sites == rhs.method_family_copy_sites &&
         lhs.method_family_mutable_copy_sites == rhs.method_family_mutable_copy_sites &&
         lhs.method_family_new_sites == rhs.method_family_new_sites &&
         lhs.method_family_none_sites == rhs.method_family_none_sites &&
         lhs.method_family_returns_retained_result_sites == rhs.method_family_returns_retained_result_sites &&
         lhs.method_family_returns_related_result_sites == rhs.method_family_returns_related_result_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentRuntimeLinkHostLinkSummary(
    const Objc3RuntimeLinkHostLinkSummary &lhs,
    const Objc3RuntimeLinkHostLinkSummary &rhs) {
  return lhs.message_send_sites == rhs.message_send_sites &&
         lhs.runtime_link_required_sites == rhs.runtime_link_required_sites &&
         lhs.runtime_link_elided_sites == rhs.runtime_link_elided_sites &&
         lhs.runtime_dispatch_arg_slots == rhs.runtime_dispatch_arg_slots &&
         lhs.runtime_dispatch_declaration_parameter_count == rhs.runtime_dispatch_declaration_parameter_count &&
         lhs.contract_violation_sites == rhs.contract_violation_sites &&
         lhs.runtime_dispatch_symbol == rhs.runtime_dispatch_symbol &&
         lhs.default_runtime_dispatch_symbol_binding == rhs.default_runtime_dispatch_symbol_binding;
}

bool IsEquivalentRetainReleaseOperationSummary(
    const Objc3RetainReleaseOperationSummary &lhs,
    const Objc3RetainReleaseOperationSummary &rhs) {
  return lhs.ownership_qualified_sites == rhs.ownership_qualified_sites &&
         lhs.retain_insertion_sites == rhs.retain_insertion_sites &&
         lhs.release_insertion_sites == rhs.release_insertion_sites &&
         lhs.autorelease_insertion_sites == rhs.autorelease_insertion_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentWeakUnownedSemanticsSummary(
    const Objc3WeakUnownedSemanticsSummary &lhs,
    const Objc3WeakUnownedSemanticsSummary &rhs) {
  return lhs.ownership_candidate_sites == rhs.ownership_candidate_sites &&
         lhs.weak_reference_sites == rhs.weak_reference_sites &&
         lhs.unowned_reference_sites == rhs.unowned_reference_sites &&
         lhs.unowned_safe_reference_sites == rhs.unowned_safe_reference_sites &&
         lhs.weak_unowned_conflict_sites == rhs.weak_unowned_conflict_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentArcDiagnosticsFixitSummary(
    const Objc3ArcDiagnosticsFixitSummary &lhs,
    const Objc3ArcDiagnosticsFixitSummary &rhs) {
  return lhs.ownership_arc_diagnostic_candidate_sites == rhs.ownership_arc_diagnostic_candidate_sites &&
         lhs.ownership_arc_fixit_available_sites == rhs.ownership_arc_fixit_available_sites &&
         lhs.ownership_arc_profiled_sites == rhs.ownership_arc_profiled_sites &&
         lhs.ownership_arc_weak_unowned_conflict_diagnostic_sites ==
             rhs.ownership_arc_weak_unowned_conflict_diagnostic_sites &&
         lhs.ownership_arc_empty_fixit_hint_sites == rhs.ownership_arc_empty_fixit_hint_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentAutoreleasePoolScopeSummary(
    const Objc3AutoreleasePoolScopeSummary &lhs,
    const Objc3AutoreleasePoolScopeSummary &rhs) {
  return lhs.scope_sites == rhs.scope_sites &&
         lhs.scope_symbolized_sites == rhs.scope_symbolized_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites &&
         lhs.max_scope_depth == rhs.max_scope_depth;
}
