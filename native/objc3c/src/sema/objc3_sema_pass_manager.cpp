#include "sema/objc3_sema_pass_manager.h"

#include <algorithm>
#include <sstream>
#include <vector>

#include "diag/objc3_diag_utils.h"
#include "sema/objc3_semantic_passes.h"

namespace {

bool IsDiagnosticLess(const std::string &lhs, const std::string &rhs) {
  const DiagSortKey lhs_key = ParseDiagSortKey(lhs);
  const DiagSortKey rhs_key = ParseDiagSortKey(rhs);
  if (lhs_key.line != rhs_key.line) {
    return lhs_key.line < rhs_key.line;
  }
  if (lhs_key.column != rhs_key.column) {
    return lhs_key.column < rhs_key.column;
  }
  if (lhs_key.severity_rank != rhs_key.severity_rank) {
    return lhs_key.severity_rank < rhs_key.severity_rank;
  }
  if (lhs_key.code != rhs_key.code) {
    return lhs_key.code < rhs_key.code;
  }
  if (lhs_key.message != rhs_key.message) {
    return lhs_key.message < rhs_key.message;
  }
  return lhs_key.raw < rhs_key.raw;
}

std::string MakeDiag(unsigned line, unsigned column, const std::string &code, const std::string &message) {
  std::ostringstream out;
  out << "error:" << line << ":" << column << ": " << message << " [" << code << "]";
  return out.str();
}

void AppendMigrationAssistDiagnostics(const Objc3SemaPassManagerInput &input, std::vector<std::string> &diagnostics) {
  if (!input.migration_assist || input.compatibility_mode != Objc3SemaCompatibilityMode::Canonical) {
    return;
  }

  const auto append_for_literal = [&diagnostics](std::size_t count,
                                                 unsigned column,
                                                 const char *legacy_literal,
                                                 const char *canonical_literal) {
    if (count == 0) {
      return;
    }
    diagnostics.push_back(
        MakeDiag(1u,
                 column,
                 "O3S216",
                 "migration assist requires canonical literal '" + std::string(canonical_literal) +
                     "' instead of legacy '" + legacy_literal + "' (" + std::to_string(count) +
                     " occurrence(s))"));
  };

  append_for_literal(input.migration_hints.legacy_yes_count, 1u, "YES", "true");
  append_for_literal(input.migration_hints.legacy_no_count, 2u, "NO", "false");
  append_for_literal(input.migration_hints.legacy_null_count, 3u, "NULL", "nil");
}

void CanonicalizePassDiagnostics(std::vector<std::string> &diagnostics) {
  std::stable_sort(diagnostics.begin(), diagnostics.end(), IsDiagnosticLess);
}

bool IsCanonicalPassDiagnostics(const std::vector<std::string> &diagnostics) {
  return std::is_sorted(diagnostics.begin(), diagnostics.end(), IsDiagnosticLess);
}

bool IsEquivalentSelectorNormalizationSummary(const Objc3SelectorNormalizationSummary &lhs,
                                             const Objc3SelectorNormalizationSummary &rhs) {
  return lhs.methods_total == rhs.methods_total &&
         lhs.normalized_methods == rhs.normalized_methods &&
         lhs.selector_piece_entries == rhs.selector_piece_entries &&
         lhs.selector_parameter_piece_entries == rhs.selector_parameter_piece_entries &&
         lhs.selector_pieceless_methods == rhs.selector_pieceless_methods &&
         lhs.selector_spelling_mismatches == rhs.selector_spelling_mismatches &&
         lhs.selector_arity_mismatches == rhs.selector_arity_mismatches &&
         lhs.selector_parameter_linkage_mismatches == rhs.selector_parameter_linkage_mismatches &&
         lhs.selector_normalization_flag_mismatches == rhs.selector_normalization_flag_mismatches &&
         lhs.selector_missing_keyword_pieces == rhs.selector_missing_keyword_pieces;
}

bool IsEquivalentPropertyAttributeSummary(const Objc3PropertyAttributeSummary &lhs,
                                          const Objc3PropertyAttributeSummary &rhs) {
  return lhs.properties_total == rhs.properties_total &&
         lhs.attribute_entries == rhs.attribute_entries &&
         lhs.readonly_modifiers == rhs.readonly_modifiers &&
         lhs.readwrite_modifiers == rhs.readwrite_modifiers &&
         lhs.atomic_modifiers == rhs.atomic_modifiers &&
         lhs.nonatomic_modifiers == rhs.nonatomic_modifiers &&
         lhs.copy_modifiers == rhs.copy_modifiers &&
         lhs.strong_modifiers == rhs.strong_modifiers &&
         lhs.weak_modifiers == rhs.weak_modifiers &&
         lhs.assign_modifiers == rhs.assign_modifiers &&
         lhs.getter_modifiers == rhs.getter_modifiers &&
         lhs.setter_modifiers == rhs.setter_modifiers &&
         lhs.invalid_attribute_entries == rhs.invalid_attribute_entries &&
         lhs.property_contract_violations == rhs.property_contract_violations;
}

bool IsEquivalentTypeAnnotationSurfaceSummary(const Objc3TypeAnnotationSurfaceSummary &lhs,
                                              const Objc3TypeAnnotationSurfaceSummary &rhs) {
  return lhs.generic_suffix_sites == rhs.generic_suffix_sites &&
         lhs.pointer_declarator_sites == rhs.pointer_declarator_sites &&
         lhs.nullability_suffix_sites == rhs.nullability_suffix_sites &&
         lhs.ownership_qualifier_sites == rhs.ownership_qualifier_sites &&
         lhs.object_pointer_type_sites == rhs.object_pointer_type_sites &&
         lhs.invalid_generic_suffix_sites == rhs.invalid_generic_suffix_sites &&
         lhs.invalid_pointer_declarator_sites == rhs.invalid_pointer_declarator_sites &&
         lhs.invalid_nullability_suffix_sites == rhs.invalid_nullability_suffix_sites &&
         lhs.invalid_ownership_qualifier_sites == rhs.invalid_ownership_qualifier_sites;
}

bool IsEquivalentSymbolGraphScopeResolutionSummary(const Objc3SymbolGraphScopeResolutionSummary &lhs,
                                                   const Objc3SymbolGraphScopeResolutionSummary &rhs) {
  return lhs.global_symbol_nodes == rhs.global_symbol_nodes &&
         lhs.function_symbol_nodes == rhs.function_symbol_nodes &&
         lhs.interface_symbol_nodes == rhs.interface_symbol_nodes &&
         lhs.implementation_symbol_nodes == rhs.implementation_symbol_nodes &&
         lhs.interface_property_symbol_nodes == rhs.interface_property_symbol_nodes &&
         lhs.implementation_property_symbol_nodes == rhs.implementation_property_symbol_nodes &&
         lhs.interface_method_symbol_nodes == rhs.interface_method_symbol_nodes &&
         lhs.implementation_method_symbol_nodes == rhs.implementation_method_symbol_nodes &&
         lhs.top_level_scope_symbols == rhs.top_level_scope_symbols &&
         lhs.nested_scope_symbols == rhs.nested_scope_symbols &&
         lhs.scope_frames_total == rhs.scope_frames_total &&
         lhs.implementation_interface_resolution_sites == rhs.implementation_interface_resolution_sites &&
         lhs.implementation_interface_resolution_hits == rhs.implementation_interface_resolution_hits &&
         lhs.implementation_interface_resolution_misses == rhs.implementation_interface_resolution_misses &&
         lhs.method_resolution_sites == rhs.method_resolution_sites &&
         lhs.method_resolution_hits == rhs.method_resolution_hits &&
         lhs.method_resolution_misses == rhs.method_resolution_misses;
}

bool IsEquivalentClassProtocolCategoryLinkingSummary(const Objc3ClassProtocolCategoryLinkingSummary &lhs,
                                                     const Objc3ClassProtocolCategoryLinkingSummary &rhs) {
  return lhs.declared_interfaces == rhs.declared_interfaces &&
         lhs.resolved_interfaces == rhs.resolved_interfaces &&
         lhs.declared_implementations == rhs.declared_implementations &&
         lhs.resolved_implementations == rhs.resolved_implementations &&
         lhs.interface_method_symbols == rhs.interface_method_symbols &&
         lhs.implementation_method_symbols == rhs.implementation_method_symbols &&
         lhs.linked_implementation_symbols == rhs.linked_implementation_symbols &&
         lhs.protocol_composition_sites == rhs.protocol_composition_sites &&
         lhs.protocol_composition_symbols == rhs.protocol_composition_symbols &&
         lhs.category_composition_sites == rhs.category_composition_sites &&
         lhs.category_composition_symbols == rhs.category_composition_symbols &&
         lhs.invalid_protocol_composition_sites == rhs.invalid_protocol_composition_sites;
}

bool IsEquivalentMethodLookupOverrideConflictSummary(const Objc3MethodLookupOverrideConflictSummary &lhs,
                                                     const Objc3MethodLookupOverrideConflictSummary &rhs) {
  return lhs.method_lookup_sites == rhs.method_lookup_sites &&
         lhs.method_lookup_hits == rhs.method_lookup_hits &&
         lhs.method_lookup_misses == rhs.method_lookup_misses &&
         lhs.override_lookup_sites == rhs.override_lookup_sites &&
         lhs.override_lookup_hits == rhs.override_lookup_hits &&
         lhs.override_lookup_misses == rhs.override_lookup_misses &&
         lhs.override_conflicts == rhs.override_conflicts &&
         lhs.unresolved_base_interfaces == rhs.unresolved_base_interfaces;
}

bool IsEquivalentPropertySynthesisIvarBindingSummary(const Objc3PropertySynthesisIvarBindingSummary &lhs,
                                                     const Objc3PropertySynthesisIvarBindingSummary &rhs) {
  return lhs.property_synthesis_sites == rhs.property_synthesis_sites &&
         lhs.property_synthesis_explicit_ivar_bindings == rhs.property_synthesis_explicit_ivar_bindings &&
         lhs.property_synthesis_default_ivar_bindings == rhs.property_synthesis_default_ivar_bindings &&
         lhs.ivar_binding_sites == rhs.ivar_binding_sites &&
         lhs.ivar_binding_resolved == rhs.ivar_binding_resolved &&
         lhs.ivar_binding_missing == rhs.ivar_binding_missing &&
         lhs.ivar_binding_conflicts == rhs.ivar_binding_conflicts;
}

bool IsEquivalentIdClassSelObjectPointerTypeCheckingSummary(
    const Objc3IdClassSelObjectPointerTypeCheckingSummary &lhs,
    const Objc3IdClassSelObjectPointerTypeCheckingSummary &rhs) {
  return lhs.param_type_sites == rhs.param_type_sites &&
         lhs.param_id_spelling_sites == rhs.param_id_spelling_sites &&
         lhs.param_class_spelling_sites == rhs.param_class_spelling_sites &&
         lhs.param_sel_spelling_sites == rhs.param_sel_spelling_sites &&
         lhs.param_instancetype_spelling_sites == rhs.param_instancetype_spelling_sites &&
         lhs.param_object_pointer_type_sites == rhs.param_object_pointer_type_sites &&
         lhs.return_type_sites == rhs.return_type_sites &&
         lhs.return_id_spelling_sites == rhs.return_id_spelling_sites &&
         lhs.return_class_spelling_sites == rhs.return_class_spelling_sites &&
         lhs.return_sel_spelling_sites == rhs.return_sel_spelling_sites &&
         lhs.return_instancetype_spelling_sites == rhs.return_instancetype_spelling_sites &&
         lhs.return_object_pointer_type_sites == rhs.return_object_pointer_type_sites &&
         lhs.property_type_sites == rhs.property_type_sites &&
         lhs.property_id_spelling_sites == rhs.property_id_spelling_sites &&
         lhs.property_class_spelling_sites == rhs.property_class_spelling_sites &&
         lhs.property_sel_spelling_sites == rhs.property_sel_spelling_sites &&
         lhs.property_instancetype_spelling_sites == rhs.property_instancetype_spelling_sites &&
         lhs.property_object_pointer_type_sites == rhs.property_object_pointer_type_sites;
}

bool IsEquivalentBlockLiteralCaptureSemanticsSummary(const Objc3BlockLiteralCaptureSemanticsSummary &lhs,
                                                     const Objc3BlockLiteralCaptureSemanticsSummary &rhs) {
  return lhs.block_literal_sites == rhs.block_literal_sites &&
         lhs.block_parameter_entries == rhs.block_parameter_entries &&
         lhs.block_capture_entries == rhs.block_capture_entries &&
         lhs.block_body_statement_entries == rhs.block_body_statement_entries &&
         lhs.block_empty_capture_sites == rhs.block_empty_capture_sites &&
         lhs.block_nondeterministic_capture_sites == rhs.block_nondeterministic_capture_sites &&
         lhs.block_non_normalized_sites == rhs.block_non_normalized_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentBlockAbiInvokeTrampolineSemanticsSummary(
    const Objc3BlockAbiInvokeTrampolineSemanticsSummary &lhs,
    const Objc3BlockAbiInvokeTrampolineSemanticsSummary &rhs) {
  return lhs.block_literal_sites == rhs.block_literal_sites &&
         lhs.invoke_argument_slots_total == rhs.invoke_argument_slots_total &&
         lhs.capture_word_count_total == rhs.capture_word_count_total &&
         lhs.parameter_entries_total == rhs.parameter_entries_total &&
         lhs.capture_entries_total == rhs.capture_entries_total &&
         lhs.body_statement_entries_total == rhs.body_statement_entries_total &&
         lhs.descriptor_symbolized_sites == rhs.descriptor_symbolized_sites &&
         lhs.invoke_trampoline_symbolized_sites == rhs.invoke_trampoline_symbolized_sites &&
         lhs.missing_invoke_trampoline_sites == rhs.missing_invoke_trampoline_sites &&
         lhs.non_normalized_layout_sites == rhs.non_normalized_layout_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentBlockStorageEscapeSemanticsSummary(
    const Objc3BlockStorageEscapeSemanticsSummary &lhs,
    const Objc3BlockStorageEscapeSemanticsSummary &rhs) {
  return lhs.block_literal_sites == rhs.block_literal_sites &&
         lhs.mutable_capture_count_total == rhs.mutable_capture_count_total &&
         lhs.byref_slot_count_total == rhs.byref_slot_count_total &&
         lhs.parameter_entries_total == rhs.parameter_entries_total &&
         lhs.capture_entries_total == rhs.capture_entries_total &&
         lhs.body_statement_entries_total == rhs.body_statement_entries_total &&
         lhs.requires_byref_cells_sites == rhs.requires_byref_cells_sites &&
         lhs.escape_analysis_enabled_sites == rhs.escape_analysis_enabled_sites &&
         lhs.escape_to_heap_sites == rhs.escape_to_heap_sites &&
         lhs.escape_profile_normalized_sites == rhs.escape_profile_normalized_sites &&
         lhs.byref_layout_symbolized_sites == rhs.byref_layout_symbolized_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentBlockCopyDisposeSemanticsSummary(
    const Objc3BlockCopyDisposeSemanticsSummary &lhs,
    const Objc3BlockCopyDisposeSemanticsSummary &rhs) {
  return lhs.block_literal_sites == rhs.block_literal_sites &&
         lhs.mutable_capture_count_total == rhs.mutable_capture_count_total &&
         lhs.byref_slot_count_total == rhs.byref_slot_count_total &&
         lhs.parameter_entries_total == rhs.parameter_entries_total &&
         lhs.capture_entries_total == rhs.capture_entries_total &&
         lhs.body_statement_entries_total == rhs.body_statement_entries_total &&
         lhs.copy_helper_required_sites == rhs.copy_helper_required_sites &&
         lhs.dispose_helper_required_sites == rhs.dispose_helper_required_sites &&
         lhs.profile_normalized_sites == rhs.profile_normalized_sites &&
         lhs.copy_helper_symbolized_sites == rhs.copy_helper_symbolized_sites &&
         lhs.dispose_helper_symbolized_sites == rhs.dispose_helper_symbolized_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentMessageSendSelectorLoweringSummary(const Objc3MessageSendSelectorLoweringSummary &lhs,
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

bool IsEquivalentDispatchAbiMarshallingSummary(const Objc3DispatchAbiMarshallingSummary &lhs,
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

bool IsEquivalentSuperDispatchMethodFamilySummary(const Objc3SuperDispatchMethodFamilySummary &lhs,
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

bool IsEquivalentRuntimeShimHostLinkSummary(const Objc3RuntimeShimHostLinkSummary &lhs,
                                            const Objc3RuntimeShimHostLinkSummary &rhs) {
  return lhs.message_send_sites == rhs.message_send_sites &&
         lhs.runtime_shim_required_sites == rhs.runtime_shim_required_sites &&
         lhs.runtime_shim_elided_sites == rhs.runtime_shim_elided_sites &&
         lhs.runtime_dispatch_arg_slots == rhs.runtime_dispatch_arg_slots &&
         lhs.runtime_dispatch_declaration_parameter_count == rhs.runtime_dispatch_declaration_parameter_count &&
         lhs.contract_violation_sites == rhs.contract_violation_sites &&
         lhs.runtime_dispatch_symbol == rhs.runtime_dispatch_symbol &&
         lhs.default_runtime_dispatch_symbol_binding == rhs.default_runtime_dispatch_symbol_binding;
}

bool IsEquivalentRetainReleaseOperationSummary(const Objc3RetainReleaseOperationSummary &lhs,
                                               const Objc3RetainReleaseOperationSummary &rhs) {
  return lhs.ownership_qualified_sites == rhs.ownership_qualified_sites &&
         lhs.retain_insertion_sites == rhs.retain_insertion_sites &&
         lhs.release_insertion_sites == rhs.release_insertion_sites &&
         lhs.autorelease_insertion_sites == rhs.autorelease_insertion_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentWeakUnownedSemanticsSummary(const Objc3WeakUnownedSemanticsSummary &lhs,
                                             const Objc3WeakUnownedSemanticsSummary &rhs) {
  return lhs.ownership_candidate_sites == rhs.ownership_candidate_sites &&
         lhs.weak_reference_sites == rhs.weak_reference_sites &&
         lhs.unowned_reference_sites == rhs.unowned_reference_sites &&
         lhs.unowned_safe_reference_sites == rhs.unowned_safe_reference_sites &&
         lhs.weak_unowned_conflict_sites == rhs.weak_unowned_conflict_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentArcDiagnosticsFixitSummary(const Objc3ArcDiagnosticsFixitSummary &lhs,
                                            const Objc3ArcDiagnosticsFixitSummary &rhs) {
  return lhs.ownership_arc_diagnostic_candidate_sites == rhs.ownership_arc_diagnostic_candidate_sites &&
         lhs.ownership_arc_fixit_available_sites == rhs.ownership_arc_fixit_available_sites &&
         lhs.ownership_arc_profiled_sites == rhs.ownership_arc_profiled_sites &&
         lhs.ownership_arc_weak_unowned_conflict_diagnostic_sites ==
             rhs.ownership_arc_weak_unowned_conflict_diagnostic_sites &&
         lhs.ownership_arc_empty_fixit_hint_sites == rhs.ownership_arc_empty_fixit_hint_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites;
}

bool IsEquivalentAutoreleasePoolScopeSummary(const Objc3AutoreleasePoolScopeSummary &lhs,
                                             const Objc3AutoreleasePoolScopeSummary &rhs) {
  return lhs.scope_sites == rhs.scope_sites &&
         lhs.scope_symbolized_sites == rhs.scope_symbolized_sites &&
         lhs.contract_violation_sites == rhs.contract_violation_sites &&
         lhs.max_scope_depth == rhs.max_scope_depth;
}

}  // namespace

Objc3SemaPassManagerResult RunObjc3SemaPassManager(const Objc3SemaPassManagerInput &input) {
  Objc3SemaPassManagerResult result;
  if (input.program == nullptr) {
    return result;
  }

  result.executed = true;
  bool deterministic_semantic_diagnostics = true;
  for (const Objc3SemaPassId pass : kObjc3SemaPassOrder) {
    std::vector<std::string> pass_diagnostics;
    if (pass == Objc3SemaPassId::BuildIntegrationSurface) {
      result.integration_surface = BuildSemanticIntegrationSurface(*input.program, pass_diagnostics);
    } else if (pass == Objc3SemaPassId::ValidateBodies) {
      ValidateSemanticBodies(*input.program, result.integration_surface, input.validation_options, pass_diagnostics);
    } else {
      ValidatePureContractSemanticDiagnostics(*input.program, result.integration_surface.functions, pass_diagnostics);
      AppendMigrationAssistDiagnostics(input, pass_diagnostics);
    }
    CanonicalizePassDiagnostics(pass_diagnostics);
    deterministic_semantic_diagnostics = deterministic_semantic_diagnostics && IsCanonicalPassDiagnostics(pass_diagnostics);

    result.diagnostics.insert(result.diagnostics.end(), pass_diagnostics.begin(), pass_diagnostics.end());
    input.diagnostics_bus.PublishBatch(pass_diagnostics);
    result.diagnostics_after_pass[static_cast<std::size_t>(pass)] = result.diagnostics.size();
    result.diagnostics_emitted_by_pass[static_cast<std::size_t>(pass)] = pass_diagnostics.size();
  }
  result.deterministic_semantic_diagnostics = deterministic_semantic_diagnostics;
  result.type_metadata_handoff = BuildSemanticTypeMetadataHandoff(result.integration_surface);
  result.deterministic_type_metadata_handoff =
      IsDeterministicSemanticTypeMetadataHandoff(result.type_metadata_handoff);
  result.deterministic_interface_implementation_handoff =
      result.type_metadata_handoff.interface_implementation_summary.deterministic &&
      result.integration_surface.interface_implementation_summary.deterministic &&
      result.integration_surface.interface_implementation_summary.declared_interfaces ==
          result.type_metadata_handoff.interface_implementation_summary.declared_interfaces &&
      result.integration_surface.interface_implementation_summary.declared_implementations ==
          result.type_metadata_handoff.interface_implementation_summary.declared_implementations &&
      result.integration_surface.interface_implementation_summary.resolved_interfaces ==
          result.type_metadata_handoff.interface_implementation_summary.resolved_interfaces &&
      result.integration_surface.interface_implementation_summary.resolved_implementations ==
          result.type_metadata_handoff.interface_implementation_summary.resolved_implementations &&
      result.integration_surface.interface_implementation_summary.interface_method_symbols ==
          result.type_metadata_handoff.interface_implementation_summary.interface_method_symbols &&
      result.integration_surface.interface_implementation_summary.implementation_method_symbols ==
          result.type_metadata_handoff.interface_implementation_summary.implementation_method_symbols &&
      result.integration_surface.interface_implementation_summary.linked_implementation_symbols ==
          result.type_metadata_handoff.interface_implementation_summary.linked_implementation_symbols;
  result.deterministic_protocol_category_composition_handoff =
      result.type_metadata_handoff.protocol_category_composition_summary.deterministic &&
      result.integration_surface.protocol_category_composition_summary.deterministic &&
      result.integration_surface.protocol_category_composition_summary.protocol_composition_sites ==
          result.type_metadata_handoff.protocol_category_composition_summary.protocol_composition_sites &&
      result.integration_surface.protocol_category_composition_summary.protocol_composition_symbols ==
          result.type_metadata_handoff.protocol_category_composition_summary.protocol_composition_symbols &&
      result.integration_surface.protocol_category_composition_summary.category_composition_sites ==
          result.type_metadata_handoff.protocol_category_composition_summary.category_composition_sites &&
      result.integration_surface.protocol_category_composition_summary.category_composition_symbols ==
          result.type_metadata_handoff.protocol_category_composition_summary.category_composition_symbols &&
      result.integration_surface.protocol_category_composition_summary.invalid_protocol_composition_sites ==
          result.type_metadata_handoff.protocol_category_composition_summary.invalid_protocol_composition_sites;
  result.class_protocol_category_linking_summary = result.integration_surface.class_protocol_category_linking_summary;
  result.deterministic_class_protocol_category_linking_handoff =
      result.type_metadata_handoff.class_protocol_category_linking_summary.deterministic &&
      result.integration_surface.class_protocol_category_linking_summary.deterministic &&
      IsEquivalentClassProtocolCategoryLinkingSummary(
          result.integration_surface.class_protocol_category_linking_summary,
          result.type_metadata_handoff.class_protocol_category_linking_summary) &&
      result.type_metadata_handoff.class_protocol_category_linking_summary.resolved_interfaces <=
          result.type_metadata_handoff.class_protocol_category_linking_summary.declared_interfaces &&
      result.type_metadata_handoff.class_protocol_category_linking_summary.resolved_implementations <=
          result.type_metadata_handoff.class_protocol_category_linking_summary.declared_implementations &&
      result.type_metadata_handoff.class_protocol_category_linking_summary.interface_method_symbols ==
          result.type_metadata_handoff.interface_implementation_summary.interface_method_symbols &&
      result.type_metadata_handoff.class_protocol_category_linking_summary.implementation_method_symbols ==
          result.type_metadata_handoff.interface_implementation_summary.implementation_method_symbols &&
      result.type_metadata_handoff.class_protocol_category_linking_summary.linked_implementation_symbols ==
          result.type_metadata_handoff.interface_implementation_summary.linked_implementation_symbols &&
      result.type_metadata_handoff.class_protocol_category_linking_summary.protocol_composition_sites ==
          result.type_metadata_handoff.protocol_category_composition_summary.protocol_composition_sites &&
      result.type_metadata_handoff.class_protocol_category_linking_summary.protocol_composition_symbols ==
          result.type_metadata_handoff.protocol_category_composition_summary.protocol_composition_symbols &&
      result.type_metadata_handoff.class_protocol_category_linking_summary.category_composition_sites ==
          result.type_metadata_handoff.protocol_category_composition_summary.category_composition_sites &&
      result.type_metadata_handoff.class_protocol_category_linking_summary.category_composition_symbols ==
          result.type_metadata_handoff.protocol_category_composition_summary.category_composition_symbols &&
      result.type_metadata_handoff.class_protocol_category_linking_summary.invalid_protocol_composition_sites ==
          result.type_metadata_handoff.protocol_category_composition_summary.invalid_protocol_composition_sites &&
      result.type_metadata_handoff.class_protocol_category_linking_summary.invalid_protocol_composition_sites <=
          result.type_metadata_handoff.class_protocol_category_linking_summary.total_composition_sites();
  result.selector_normalization_summary = result.integration_surface.selector_normalization_summary;
  result.deterministic_selector_normalization_handoff =
      result.type_metadata_handoff.selector_normalization_summary.deterministic &&
      result.integration_surface.selector_normalization_summary.deterministic &&
      IsEquivalentSelectorNormalizationSummary(result.integration_surface.selector_normalization_summary,
                                               result.type_metadata_handoff.selector_normalization_summary) &&
      result.type_metadata_handoff.selector_normalization_summary.normalized_methods <=
          result.type_metadata_handoff.selector_normalization_summary.methods_total &&
      result.type_metadata_handoff.selector_normalization_summary.selector_parameter_piece_entries <=
          result.type_metadata_handoff.selector_normalization_summary.selector_piece_entries &&
      result.type_metadata_handoff.selector_normalization_summary.contract_violations() <=
          result.type_metadata_handoff.selector_normalization_summary.methods_total;
  result.property_attribute_summary = result.integration_surface.property_attribute_summary;
  result.deterministic_property_attribute_handoff =
      result.type_metadata_handoff.property_attribute_summary.deterministic &&
      result.integration_surface.property_attribute_summary.deterministic &&
      IsEquivalentPropertyAttributeSummary(result.integration_surface.property_attribute_summary,
                                           result.type_metadata_handoff.property_attribute_summary) &&
      result.type_metadata_handoff.property_attribute_summary.getter_modifiers <=
          result.type_metadata_handoff.property_attribute_summary.properties_total &&
      result.type_metadata_handoff.property_attribute_summary.setter_modifiers <=
          result.type_metadata_handoff.property_attribute_summary.properties_total;
  result.type_annotation_surface_summary = result.integration_surface.type_annotation_surface_summary;
  result.deterministic_type_annotation_surface_handoff =
      result.type_metadata_handoff.type_annotation_surface_summary.deterministic &&
      result.integration_surface.type_annotation_surface_summary.deterministic &&
      IsEquivalentTypeAnnotationSurfaceSummary(result.integration_surface.type_annotation_surface_summary,
                                               result.type_metadata_handoff.type_annotation_surface_summary) &&
      result.type_metadata_handoff.type_annotation_surface_summary.invalid_generic_suffix_sites <=
          result.type_metadata_handoff.type_annotation_surface_summary.generic_suffix_sites &&
      result.type_metadata_handoff.type_annotation_surface_summary.invalid_pointer_declarator_sites <=
          result.type_metadata_handoff.type_annotation_surface_summary.pointer_declarator_sites &&
      result.type_metadata_handoff.type_annotation_surface_summary.invalid_nullability_suffix_sites <=
          result.type_metadata_handoff.type_annotation_surface_summary.nullability_suffix_sites &&
      result.type_metadata_handoff.type_annotation_surface_summary.invalid_ownership_qualifier_sites <=
          result.type_metadata_handoff.type_annotation_surface_summary.ownership_qualifier_sites &&
      result.type_metadata_handoff.type_annotation_surface_summary.invalid_type_annotation_sites() <=
          result.type_metadata_handoff.type_annotation_surface_summary.total_type_annotation_sites();
  result.symbol_graph_scope_resolution_summary = result.integration_surface.symbol_graph_scope_resolution_summary;
  result.deterministic_symbol_graph_scope_resolution_handoff =
      result.type_metadata_handoff.symbol_graph_scope_resolution_summary.deterministic &&
      result.integration_surface.symbol_graph_scope_resolution_summary.deterministic &&
      IsEquivalentSymbolGraphScopeResolutionSummary(result.integration_surface.symbol_graph_scope_resolution_summary,
                                                    result.type_metadata_handoff.symbol_graph_scope_resolution_summary) &&
      result.type_metadata_handoff.symbol_graph_scope_resolution_summary.symbol_nodes_total() ==
          result.type_metadata_handoff.symbol_graph_scope_resolution_summary.top_level_scope_symbols +
              result.type_metadata_handoff.symbol_graph_scope_resolution_summary.nested_scope_symbols &&
      result.type_metadata_handoff.symbol_graph_scope_resolution_summary.implementation_interface_resolution_hits <=
          result.type_metadata_handoff.symbol_graph_scope_resolution_summary.implementation_interface_resolution_sites &&
      result.type_metadata_handoff.symbol_graph_scope_resolution_summary.implementation_interface_resolution_hits +
              result.type_metadata_handoff.symbol_graph_scope_resolution_summary
                  .implementation_interface_resolution_misses ==
          result.type_metadata_handoff.symbol_graph_scope_resolution_summary.implementation_interface_resolution_sites &&
      result.type_metadata_handoff.symbol_graph_scope_resolution_summary.method_resolution_hits <=
          result.type_metadata_handoff.symbol_graph_scope_resolution_summary.method_resolution_sites &&
      result.type_metadata_handoff.symbol_graph_scope_resolution_summary.method_resolution_hits +
              result.type_metadata_handoff.symbol_graph_scope_resolution_summary.method_resolution_misses ==
          result.type_metadata_handoff.symbol_graph_scope_resolution_summary.method_resolution_sites &&
      result.type_metadata_handoff.symbol_graph_scope_resolution_summary.resolution_hits_total() <=
          result.type_metadata_handoff.symbol_graph_scope_resolution_summary.resolution_sites_total() &&
      result.type_metadata_handoff.symbol_graph_scope_resolution_summary.resolution_hits_total() +
              result.type_metadata_handoff.symbol_graph_scope_resolution_summary.resolution_misses_total() ==
          result.type_metadata_handoff.symbol_graph_scope_resolution_summary.resolution_sites_total();
  result.method_lookup_override_conflict_summary =
      result.integration_surface.method_lookup_override_conflict_summary;
  result.deterministic_method_lookup_override_conflict_handoff =
      result.type_metadata_handoff.method_lookup_override_conflict_summary.deterministic &&
      result.integration_surface.method_lookup_override_conflict_summary.deterministic &&
      IsEquivalentMethodLookupOverrideConflictSummary(
          result.integration_surface.method_lookup_override_conflict_summary,
          result.type_metadata_handoff.method_lookup_override_conflict_summary) &&
      result.type_metadata_handoff.method_lookup_override_conflict_summary.method_lookup_hits <=
          result.type_metadata_handoff.method_lookup_override_conflict_summary.method_lookup_sites &&
      result.type_metadata_handoff.method_lookup_override_conflict_summary.method_lookup_hits +
              result.type_metadata_handoff.method_lookup_override_conflict_summary.method_lookup_misses ==
          result.type_metadata_handoff.method_lookup_override_conflict_summary.method_lookup_sites &&
      result.type_metadata_handoff.method_lookup_override_conflict_summary.override_lookup_hits <=
          result.type_metadata_handoff.method_lookup_override_conflict_summary.override_lookup_sites &&
      result.type_metadata_handoff.method_lookup_override_conflict_summary.override_lookup_hits +
              result.type_metadata_handoff.method_lookup_override_conflict_summary.override_lookup_misses ==
          result.type_metadata_handoff.method_lookup_override_conflict_summary.override_lookup_sites &&
      result.type_metadata_handoff.method_lookup_override_conflict_summary.override_conflicts <=
          result.type_metadata_handoff.method_lookup_override_conflict_summary.override_lookup_hits;
  result.property_synthesis_ivar_binding_summary =
      result.integration_surface.property_synthesis_ivar_binding_summary;
  result.deterministic_property_synthesis_ivar_binding_handoff =
      result.type_metadata_handoff.property_synthesis_ivar_binding_summary.deterministic &&
      result.integration_surface.property_synthesis_ivar_binding_summary.deterministic &&
      IsEquivalentPropertySynthesisIvarBindingSummary(
          result.integration_surface.property_synthesis_ivar_binding_summary,
          result.type_metadata_handoff.property_synthesis_ivar_binding_summary) &&
      result.type_metadata_handoff.property_synthesis_ivar_binding_summary
              .property_synthesis_explicit_ivar_bindings +
              result.type_metadata_handoff.property_synthesis_ivar_binding_summary
                  .property_synthesis_default_ivar_bindings ==
          result.type_metadata_handoff.property_synthesis_ivar_binding_summary.property_synthesis_sites &&
      result.type_metadata_handoff.property_synthesis_ivar_binding_summary.ivar_binding_sites ==
          result.type_metadata_handoff.property_synthesis_ivar_binding_summary.property_synthesis_sites &&
      result.type_metadata_handoff.property_synthesis_ivar_binding_summary.ivar_binding_resolved +
              result.type_metadata_handoff.property_synthesis_ivar_binding_summary.ivar_binding_missing +
              result.type_metadata_handoff.property_synthesis_ivar_binding_summary.ivar_binding_conflicts ==
          result.type_metadata_handoff.property_synthesis_ivar_binding_summary.ivar_binding_sites;
  result.id_class_sel_object_pointer_type_checking_summary =
      result.integration_surface.id_class_sel_object_pointer_type_checking_summary;
  result.deterministic_id_class_sel_object_pointer_type_checking_handoff =
      result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary.deterministic &&
      result.integration_surface.id_class_sel_object_pointer_type_checking_summary.deterministic &&
      IsEquivalentIdClassSelObjectPointerTypeCheckingSummary(
          result.integration_surface.id_class_sel_object_pointer_type_checking_summary,
          result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary) &&
      result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary.param_id_spelling_sites +
              result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary.param_class_spelling_sites +
              result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary.param_sel_spelling_sites +
              result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary
                  .param_instancetype_spelling_sites +
              result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary
                  .param_object_pointer_type_sites <=
          result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary.param_type_sites &&
      result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary.return_id_spelling_sites +
              result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary.return_class_spelling_sites +
              result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary.return_sel_spelling_sites +
              result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary
                  .return_instancetype_spelling_sites +
              result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary
                  .return_object_pointer_type_sites <=
          result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary.return_type_sites &&
      result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary.property_id_spelling_sites +
              result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary
                  .property_class_spelling_sites +
              result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary.property_sel_spelling_sites +
              result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary
                  .property_instancetype_spelling_sites +
              result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary
                  .property_object_pointer_type_sites <=
          result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary.property_type_sites;
  result.block_literal_capture_semantics_summary =
      result.integration_surface.block_literal_capture_semantics_summary;
  result.deterministic_block_literal_capture_semantics_handoff =
      result.type_metadata_handoff.block_literal_capture_semantics_summary.deterministic &&
      result.integration_surface.block_literal_capture_semantics_summary.deterministic &&
      IsEquivalentBlockLiteralCaptureSemanticsSummary(
          result.integration_surface.block_literal_capture_semantics_summary,
          result.type_metadata_handoff.block_literal_capture_semantics_summary) &&
      result.type_metadata_handoff.block_literal_capture_semantics_summary.block_empty_capture_sites <=
          result.type_metadata_handoff.block_literal_capture_semantics_summary.block_literal_sites &&
      result.type_metadata_handoff.block_literal_capture_semantics_summary.block_nondeterministic_capture_sites <=
          result.type_metadata_handoff.block_literal_capture_semantics_summary.block_literal_sites &&
      result.type_metadata_handoff.block_literal_capture_semantics_summary.block_non_normalized_sites <=
          result.type_metadata_handoff.block_literal_capture_semantics_summary.block_literal_sites &&
      result.type_metadata_handoff.block_literal_capture_semantics_summary.contract_violation_sites <=
          result.type_metadata_handoff.block_literal_capture_semantics_summary.block_literal_sites;
  result.block_abi_invoke_trampoline_semantics_summary =
      result.integration_surface.block_abi_invoke_trampoline_semantics_summary;
  result.deterministic_block_abi_invoke_trampoline_handoff =
      result.type_metadata_handoff.block_abi_invoke_trampoline_semantics_summary.deterministic &&
      result.integration_surface.block_abi_invoke_trampoline_semantics_summary.deterministic &&
      IsEquivalentBlockAbiInvokeTrampolineSemanticsSummary(
          result.integration_surface.block_abi_invoke_trampoline_semantics_summary,
          result.type_metadata_handoff.block_abi_invoke_trampoline_semantics_summary) &&
      result.type_metadata_handoff.block_abi_invoke_trampoline_semantics_summary
              .descriptor_symbolized_sites <=
          result.type_metadata_handoff.block_abi_invoke_trampoline_semantics_summary.block_literal_sites &&
      result.type_metadata_handoff.block_abi_invoke_trampoline_semantics_summary
              .invoke_trampoline_symbolized_sites <=
          result.type_metadata_handoff.block_abi_invoke_trampoline_semantics_summary.block_literal_sites &&
      result.type_metadata_handoff.block_abi_invoke_trampoline_semantics_summary
              .missing_invoke_trampoline_sites <=
          result.type_metadata_handoff.block_abi_invoke_trampoline_semantics_summary.block_literal_sites &&
      result.type_metadata_handoff.block_abi_invoke_trampoline_semantics_summary
              .non_normalized_layout_sites <=
          result.type_metadata_handoff.block_abi_invoke_trampoline_semantics_summary.block_literal_sites &&
      result.type_metadata_handoff.block_abi_invoke_trampoline_semantics_summary.contract_violation_sites <=
          result.type_metadata_handoff.block_abi_invoke_trampoline_semantics_summary.block_literal_sites &&
      result.type_metadata_handoff.block_abi_invoke_trampoline_semantics_summary
                  .invoke_trampoline_symbolized_sites +
              result.type_metadata_handoff.block_abi_invoke_trampoline_semantics_summary
                  .missing_invoke_trampoline_sites ==
          result.type_metadata_handoff.block_abi_invoke_trampoline_semantics_summary.block_literal_sites &&
      result.type_metadata_handoff.block_abi_invoke_trampoline_semantics_summary
              .invoke_argument_slots_total ==
          result.type_metadata_handoff.block_abi_invoke_trampoline_semantics_summary
              .parameter_entries_total &&
      result.type_metadata_handoff.block_abi_invoke_trampoline_semantics_summary
              .capture_word_count_total ==
          result.type_metadata_handoff.block_abi_invoke_trampoline_semantics_summary.capture_entries_total;
  result.block_storage_escape_semantics_summary =
      result.integration_surface.block_storage_escape_semantics_summary;
  result.deterministic_block_storage_escape_handoff =
      result.type_metadata_handoff.block_storage_escape_semantics_summary.deterministic &&
      result.integration_surface.block_storage_escape_semantics_summary.deterministic &&
      IsEquivalentBlockStorageEscapeSemanticsSummary(
          result.integration_surface.block_storage_escape_semantics_summary,
          result.type_metadata_handoff.block_storage_escape_semantics_summary) &&
      result.type_metadata_handoff.block_storage_escape_semantics_summary.requires_byref_cells_sites <=
          result.type_metadata_handoff.block_storage_escape_semantics_summary.block_literal_sites &&
      result.type_metadata_handoff.block_storage_escape_semantics_summary.escape_analysis_enabled_sites <=
          result.type_metadata_handoff.block_storage_escape_semantics_summary.block_literal_sites &&
      result.type_metadata_handoff.block_storage_escape_semantics_summary.escape_to_heap_sites <=
          result.type_metadata_handoff.block_storage_escape_semantics_summary.block_literal_sites &&
      result.type_metadata_handoff.block_storage_escape_semantics_summary.escape_profile_normalized_sites <=
          result.type_metadata_handoff.block_storage_escape_semantics_summary.block_literal_sites &&
      result.type_metadata_handoff.block_storage_escape_semantics_summary.byref_layout_symbolized_sites <=
          result.type_metadata_handoff.block_storage_escape_semantics_summary.block_literal_sites &&
      result.type_metadata_handoff.block_storage_escape_semantics_summary.contract_violation_sites <=
          result.type_metadata_handoff.block_storage_escape_semantics_summary.block_literal_sites &&
      result.type_metadata_handoff.block_storage_escape_semantics_summary.mutable_capture_count_total ==
          result.type_metadata_handoff.block_storage_escape_semantics_summary.capture_entries_total &&
      result.type_metadata_handoff.block_storage_escape_semantics_summary.byref_slot_count_total ==
          result.type_metadata_handoff.block_storage_escape_semantics_summary.capture_entries_total &&
      result.type_metadata_handoff.block_storage_escape_semantics_summary.escape_analysis_enabled_sites ==
          result.type_metadata_handoff.block_storage_escape_semantics_summary.block_literal_sites &&
      result.type_metadata_handoff.block_storage_escape_semantics_summary.requires_byref_cells_sites ==
          result.type_metadata_handoff.block_storage_escape_semantics_summary.escape_to_heap_sites;
  result.block_copy_dispose_semantics_summary =
      result.integration_surface.block_copy_dispose_semantics_summary;
  result.deterministic_block_copy_dispose_handoff =
      result.type_metadata_handoff.block_copy_dispose_semantics_summary.deterministic &&
      result.integration_surface.block_copy_dispose_semantics_summary.deterministic &&
      IsEquivalentBlockCopyDisposeSemanticsSummary(
          result.integration_surface.block_copy_dispose_semantics_summary,
          result.type_metadata_handoff.block_copy_dispose_semantics_summary) &&
      result.type_metadata_handoff.block_copy_dispose_semantics_summary.copy_helper_required_sites <=
          result.type_metadata_handoff.block_copy_dispose_semantics_summary.block_literal_sites &&
      result.type_metadata_handoff.block_copy_dispose_semantics_summary.dispose_helper_required_sites <=
          result.type_metadata_handoff.block_copy_dispose_semantics_summary.block_literal_sites &&
      result.type_metadata_handoff.block_copy_dispose_semantics_summary.profile_normalized_sites <=
          result.type_metadata_handoff.block_copy_dispose_semantics_summary.block_literal_sites &&
      result.type_metadata_handoff.block_copy_dispose_semantics_summary.copy_helper_symbolized_sites <=
          result.type_metadata_handoff.block_copy_dispose_semantics_summary.block_literal_sites &&
      result.type_metadata_handoff.block_copy_dispose_semantics_summary.dispose_helper_symbolized_sites <=
          result.type_metadata_handoff.block_copy_dispose_semantics_summary.block_literal_sites &&
      result.type_metadata_handoff.block_copy_dispose_semantics_summary.contract_violation_sites <=
          result.type_metadata_handoff.block_copy_dispose_semantics_summary.block_literal_sites &&
      result.type_metadata_handoff.block_copy_dispose_semantics_summary.mutable_capture_count_total ==
          result.type_metadata_handoff.block_copy_dispose_semantics_summary.capture_entries_total &&
      result.type_metadata_handoff.block_copy_dispose_semantics_summary.byref_slot_count_total ==
          result.type_metadata_handoff.block_copy_dispose_semantics_summary.capture_entries_total &&
      result.type_metadata_handoff.block_copy_dispose_semantics_summary.copy_helper_required_sites ==
          result.type_metadata_handoff.block_copy_dispose_semantics_summary.dispose_helper_required_sites;
  result.message_send_selector_lowering_summary =
      result.integration_surface.message_send_selector_lowering_summary;
  result.deterministic_message_send_selector_lowering_handoff =
      result.type_metadata_handoff.message_send_selector_lowering_summary.deterministic &&
      result.integration_surface.message_send_selector_lowering_summary.deterministic &&
      IsEquivalentMessageSendSelectorLoweringSummary(
          result.integration_surface.message_send_selector_lowering_summary,
          result.type_metadata_handoff.message_send_selector_lowering_summary) &&
      result.type_metadata_handoff.message_send_selector_lowering_summary.unary_form_sites +
              result.type_metadata_handoff.message_send_selector_lowering_summary.keyword_form_sites ==
          result.type_metadata_handoff.message_send_selector_lowering_summary.message_send_sites &&
      result.type_metadata_handoff.message_send_selector_lowering_summary.selector_lowering_symbol_sites <=
          result.type_metadata_handoff.message_send_selector_lowering_summary.message_send_sites &&
      result.type_metadata_handoff.message_send_selector_lowering_summary.selector_lowering_argument_piece_entries <=
          result.type_metadata_handoff.message_send_selector_lowering_summary.selector_lowering_piece_entries &&
      result.type_metadata_handoff.message_send_selector_lowering_summary.selector_lowering_normalized_sites <=
          result.type_metadata_handoff.message_send_selector_lowering_summary.selector_lowering_symbol_sites &&
      result.type_metadata_handoff.message_send_selector_lowering_summary.selector_lowering_form_mismatch_sites <=
          result.type_metadata_handoff.message_send_selector_lowering_summary.message_send_sites &&
      result.type_metadata_handoff.message_send_selector_lowering_summary.selector_lowering_arity_mismatch_sites <=
          result.type_metadata_handoff.message_send_selector_lowering_summary.message_send_sites &&
      result.type_metadata_handoff.message_send_selector_lowering_summary.selector_lowering_symbol_mismatch_sites <=
          result.type_metadata_handoff.message_send_selector_lowering_summary.message_send_sites &&
      result.type_metadata_handoff.message_send_selector_lowering_summary.selector_lowering_missing_symbol_sites <=
          result.type_metadata_handoff.message_send_selector_lowering_summary.message_send_sites &&
      result.type_metadata_handoff.message_send_selector_lowering_summary.selector_lowering_contract_violation_sites <=
          result.type_metadata_handoff.message_send_selector_lowering_summary.message_send_sites;
  result.dispatch_abi_marshalling_summary = result.integration_surface.dispatch_abi_marshalling_summary;
  result.deterministic_dispatch_abi_marshalling_handoff =
      result.type_metadata_handoff.dispatch_abi_marshalling_summary.deterministic &&
      result.integration_surface.dispatch_abi_marshalling_summary.deterministic &&
      IsEquivalentDispatchAbiMarshallingSummary(
          result.integration_surface.dispatch_abi_marshalling_summary,
          result.type_metadata_handoff.dispatch_abi_marshalling_summary) &&
      result.type_metadata_handoff.dispatch_abi_marshalling_summary.receiver_slots ==
          result.type_metadata_handoff.dispatch_abi_marshalling_summary.message_send_sites &&
      result.type_metadata_handoff.dispatch_abi_marshalling_summary.selector_symbol_slots +
              result.type_metadata_handoff.dispatch_abi_marshalling_summary.missing_selector_symbol_sites ==
          result.type_metadata_handoff.dispatch_abi_marshalling_summary.message_send_sites &&
      result.type_metadata_handoff.dispatch_abi_marshalling_summary.keyword_argument_slots +
              result.type_metadata_handoff.dispatch_abi_marshalling_summary.unary_argument_slots ==
          result.type_metadata_handoff.dispatch_abi_marshalling_summary.argument_slots &&
      result.type_metadata_handoff.dispatch_abi_marshalling_summary.keyword_argument_slots <=
          result.type_metadata_handoff.dispatch_abi_marshalling_summary.argument_slots &&
      result.type_metadata_handoff.dispatch_abi_marshalling_summary.unary_argument_slots <=
          result.type_metadata_handoff.dispatch_abi_marshalling_summary.argument_slots &&
      result.type_metadata_handoff.dispatch_abi_marshalling_summary.selector_symbol_slots <=
          result.type_metadata_handoff.dispatch_abi_marshalling_summary.message_send_sites &&
      result.type_metadata_handoff.dispatch_abi_marshalling_summary.missing_selector_symbol_sites <=
          result.type_metadata_handoff.dispatch_abi_marshalling_summary.message_send_sites &&
      result.type_metadata_handoff.dispatch_abi_marshalling_summary.arity_mismatch_sites <=
          result.type_metadata_handoff.dispatch_abi_marshalling_summary.message_send_sites &&
      result.type_metadata_handoff.dispatch_abi_marshalling_summary.contract_violation_sites <=
          result.type_metadata_handoff.dispatch_abi_marshalling_summary.message_send_sites;
  result.nil_receiver_semantics_foldability_summary =
      result.integration_surface.nil_receiver_semantics_foldability_summary;
  result.deterministic_nil_receiver_semantics_foldability_handoff =
      result.type_metadata_handoff.nil_receiver_semantics_foldability_summary.deterministic &&
      result.integration_surface.nil_receiver_semantics_foldability_summary.deterministic &&
      IsEquivalentNilReceiverSemanticsFoldabilitySummary(
          result.integration_surface.nil_receiver_semantics_foldability_summary,
          result.type_metadata_handoff.nil_receiver_semantics_foldability_summary) &&
      result.type_metadata_handoff.nil_receiver_semantics_foldability_summary.receiver_nil_literal_sites ==
          result.type_metadata_handoff.nil_receiver_semantics_foldability_summary.nil_receiver_semantics_enabled_sites &&
      result.type_metadata_handoff.nil_receiver_semantics_foldability_summary.nil_receiver_foldable_sites <=
          result.type_metadata_handoff.nil_receiver_semantics_foldability_summary.nil_receiver_semantics_enabled_sites &&
      result.type_metadata_handoff.nil_receiver_semantics_foldability_summary
              .nil_receiver_runtime_dispatch_required_sites +
              result.type_metadata_handoff.nil_receiver_semantics_foldability_summary.nil_receiver_foldable_sites ==
          result.type_metadata_handoff.nil_receiver_semantics_foldability_summary.message_send_sites &&
      result.type_metadata_handoff.nil_receiver_semantics_foldability_summary.nil_receiver_semantics_enabled_sites +
              result.type_metadata_handoff.nil_receiver_semantics_foldability_summary.non_nil_receiver_sites ==
          result.type_metadata_handoff.nil_receiver_semantics_foldability_summary.message_send_sites &&
      result.type_metadata_handoff.nil_receiver_semantics_foldability_summary.contract_violation_sites <=
          result.type_metadata_handoff.nil_receiver_semantics_foldability_summary.message_send_sites;
  result.super_dispatch_method_family_summary =
      result.integration_surface.super_dispatch_method_family_summary;
  result.deterministic_super_dispatch_method_family_handoff =
      result.type_metadata_handoff.super_dispatch_method_family_summary.deterministic &&
      result.integration_surface.super_dispatch_method_family_summary.deterministic &&
      IsEquivalentSuperDispatchMethodFamilySummary(
          result.integration_surface.super_dispatch_method_family_summary,
          result.type_metadata_handoff.super_dispatch_method_family_summary) &&
      result.type_metadata_handoff.super_dispatch_method_family_summary.receiver_super_identifier_sites ==
          result.type_metadata_handoff.super_dispatch_method_family_summary.super_dispatch_enabled_sites &&
      result.type_metadata_handoff.super_dispatch_method_family_summary.super_dispatch_requires_class_context_sites ==
          result.type_metadata_handoff.super_dispatch_method_family_summary.super_dispatch_enabled_sites &&
      result.type_metadata_handoff.super_dispatch_method_family_summary.method_family_init_sites +
              result.type_metadata_handoff.super_dispatch_method_family_summary.method_family_copy_sites +
              result.type_metadata_handoff.super_dispatch_method_family_summary.method_family_mutable_copy_sites +
              result.type_metadata_handoff.super_dispatch_method_family_summary.method_family_new_sites +
              result.type_metadata_handoff.super_dispatch_method_family_summary.method_family_none_sites ==
          result.type_metadata_handoff.super_dispatch_method_family_summary.message_send_sites &&
      result.type_metadata_handoff.super_dispatch_method_family_summary.method_family_returns_related_result_sites <=
          result.type_metadata_handoff.super_dispatch_method_family_summary.method_family_init_sites &&
      result.type_metadata_handoff.super_dispatch_method_family_summary.method_family_returns_retained_result_sites <=
          result.type_metadata_handoff.super_dispatch_method_family_summary.message_send_sites &&
      result.type_metadata_handoff.super_dispatch_method_family_summary.contract_violation_sites <=
          result.type_metadata_handoff.super_dispatch_method_family_summary.message_send_sites;
  result.runtime_shim_host_link_summary = result.integration_surface.runtime_shim_host_link_summary;
  result.deterministic_runtime_shim_host_link_handoff =
      result.type_metadata_handoff.runtime_shim_host_link_summary.deterministic &&
      result.integration_surface.runtime_shim_host_link_summary.deterministic &&
      IsEquivalentRuntimeShimHostLinkSummary(
          result.integration_surface.runtime_shim_host_link_summary,
          result.type_metadata_handoff.runtime_shim_host_link_summary) &&
      result.type_metadata_handoff.runtime_shim_host_link_summary.runtime_shim_required_sites +
              result.type_metadata_handoff.runtime_shim_host_link_summary.runtime_shim_elided_sites ==
          result.type_metadata_handoff.runtime_shim_host_link_summary.message_send_sites &&
      result.type_metadata_handoff.runtime_shim_host_link_summary.contract_violation_sites <=
          result.type_metadata_handoff.runtime_shim_host_link_summary.message_send_sites &&
      (result.type_metadata_handoff.runtime_shim_host_link_summary.message_send_sites == 0 ||
       result.type_metadata_handoff.runtime_shim_host_link_summary.runtime_dispatch_declaration_parameter_count ==
           result.type_metadata_handoff.runtime_shim_host_link_summary.runtime_dispatch_arg_slots + 2u) &&
      (result.type_metadata_handoff.runtime_shim_host_link_summary.default_runtime_dispatch_symbol_binding ==
       (result.type_metadata_handoff.runtime_shim_host_link_summary.runtime_dispatch_symbol ==
        kObjc3RuntimeShimHostLinkDefaultDispatchSymbol));
  result.retain_release_operation_summary = result.integration_surface.retain_release_operation_summary;
  result.deterministic_retain_release_operation_handoff =
      result.type_metadata_handoff.retain_release_operation_summary.deterministic &&
      result.integration_surface.retain_release_operation_summary.deterministic &&
      IsEquivalentRetainReleaseOperationSummary(
          result.integration_surface.retain_release_operation_summary,
          result.type_metadata_handoff.retain_release_operation_summary) &&
      result.type_metadata_handoff.retain_release_operation_summary.retain_insertion_sites <=
          result.type_metadata_handoff.retain_release_operation_summary.ownership_qualified_sites +
              result.type_metadata_handoff.retain_release_operation_summary.contract_violation_sites &&
      result.type_metadata_handoff.retain_release_operation_summary.release_insertion_sites <=
          result.type_metadata_handoff.retain_release_operation_summary.ownership_qualified_sites +
              result.type_metadata_handoff.retain_release_operation_summary.contract_violation_sites &&
      result.type_metadata_handoff.retain_release_operation_summary.autorelease_insertion_sites <=
          result.type_metadata_handoff.retain_release_operation_summary.ownership_qualified_sites +
              result.type_metadata_handoff.retain_release_operation_summary.contract_violation_sites;
  result.weak_unowned_semantics_summary = result.integration_surface.weak_unowned_semantics_summary;
  result.deterministic_weak_unowned_semantics_handoff =
      result.type_metadata_handoff.weak_unowned_semantics_summary.deterministic &&
      result.integration_surface.weak_unowned_semantics_summary.deterministic &&
      IsEquivalentWeakUnownedSemanticsSummary(
          result.integration_surface.weak_unowned_semantics_summary,
          result.type_metadata_handoff.weak_unowned_semantics_summary) &&
      result.type_metadata_handoff.weak_unowned_semantics_summary.unowned_safe_reference_sites <=
          result.type_metadata_handoff.weak_unowned_semantics_summary.unowned_reference_sites &&
      result.type_metadata_handoff.weak_unowned_semantics_summary.weak_unowned_conflict_sites <=
          result.type_metadata_handoff.weak_unowned_semantics_summary.ownership_candidate_sites &&
      result.type_metadata_handoff.weak_unowned_semantics_summary.contract_violation_sites <=
          result.type_metadata_handoff.weak_unowned_semantics_summary.ownership_candidate_sites +
              result.type_metadata_handoff.weak_unowned_semantics_summary.weak_unowned_conflict_sites;
  result.arc_diagnostics_fixit_summary = result.integration_surface.arc_diagnostics_fixit_summary;
  result.deterministic_arc_diagnostics_fixit_handoff =
      result.type_metadata_handoff.arc_diagnostics_fixit_summary.deterministic &&
      result.integration_surface.arc_diagnostics_fixit_summary.deterministic &&
      IsEquivalentArcDiagnosticsFixitSummary(
          result.integration_surface.arc_diagnostics_fixit_summary,
          result.type_metadata_handoff.arc_diagnostics_fixit_summary) &&
      result.type_metadata_handoff.arc_diagnostics_fixit_summary.ownership_arc_fixit_available_sites <=
          result.type_metadata_handoff.arc_diagnostics_fixit_summary.ownership_arc_diagnostic_candidate_sites +
              result.type_metadata_handoff.arc_diagnostics_fixit_summary.contract_violation_sites &&
      result.type_metadata_handoff.arc_diagnostics_fixit_summary.ownership_arc_profiled_sites <=
          result.type_metadata_handoff.arc_diagnostics_fixit_summary.ownership_arc_diagnostic_candidate_sites +
              result.type_metadata_handoff.arc_diagnostics_fixit_summary.contract_violation_sites &&
      result.type_metadata_handoff.arc_diagnostics_fixit_summary
              .ownership_arc_weak_unowned_conflict_diagnostic_sites <=
          result.type_metadata_handoff.arc_diagnostics_fixit_summary.ownership_arc_diagnostic_candidate_sites +
              result.type_metadata_handoff.arc_diagnostics_fixit_summary.contract_violation_sites &&
      result.type_metadata_handoff.arc_diagnostics_fixit_summary.ownership_arc_empty_fixit_hint_sites <=
          result.type_metadata_handoff.arc_diagnostics_fixit_summary.ownership_arc_fixit_available_sites +
              result.type_metadata_handoff.arc_diagnostics_fixit_summary.contract_violation_sites;
  result.autoreleasepool_scope_summary = result.integration_surface.autoreleasepool_scope_summary;
  result.deterministic_autoreleasepool_scope_handoff =
      result.type_metadata_handoff.autoreleasepool_scope_summary.deterministic &&
      result.integration_surface.autoreleasepool_scope_summary.deterministic &&
      IsEquivalentAutoreleasePoolScopeSummary(
          result.integration_surface.autoreleasepool_scope_summary,
          result.type_metadata_handoff.autoreleasepool_scope_summary) &&
      result.type_metadata_handoff.autoreleasepool_scope_summary.scope_symbolized_sites <=
          result.type_metadata_handoff.autoreleasepool_scope_summary.scope_sites &&
      result.type_metadata_handoff.autoreleasepool_scope_summary.contract_violation_sites <=
          result.type_metadata_handoff.autoreleasepool_scope_summary.scope_sites &&
      (result.type_metadata_handoff.autoreleasepool_scope_summary.scope_sites > 0u ||
       result.type_metadata_handoff.autoreleasepool_scope_summary.max_scope_depth == 0u) &&
      result.type_metadata_handoff.autoreleasepool_scope_summary.max_scope_depth <=
          static_cast<unsigned>(result.type_metadata_handoff.autoreleasepool_scope_summary.scope_sites);
  result.atomic_memory_order_mapping = BuildAtomicMemoryOrderMappingSummary(*input.program);
  result.deterministic_atomic_memory_order_mapping = result.atomic_memory_order_mapping.deterministic;
  result.vector_type_lowering = BuildVectorTypeLoweringSummary(result.integration_surface);
  result.deterministic_vector_type_lowering = result.vector_type_lowering.deterministic;
  result.parity_surface.diagnostics_after_pass = result.diagnostics_after_pass;
  result.parity_surface.diagnostics_emitted_by_pass = result.diagnostics_emitted_by_pass;
  result.parity_surface.diagnostics_total = result.diagnostics.size();
  result.parity_surface.globals_total = result.integration_surface.globals.size();
  result.parity_surface.functions_total = result.integration_surface.functions.size();
  result.parity_surface.interfaces_total = result.integration_surface.interfaces.size();
  result.parity_surface.implementations_total = result.integration_surface.implementations.size();
  result.parity_surface.type_metadata_global_entries = result.type_metadata_handoff.global_names_lexicographic.size();
  result.parity_surface.type_metadata_function_entries = result.type_metadata_handoff.functions_lexicographic.size();
  result.parity_surface.type_metadata_interface_entries = result.type_metadata_handoff.interfaces_lexicographic.size();
  result.parity_surface.type_metadata_implementation_entries =
      result.type_metadata_handoff.implementations_lexicographic.size();
  result.parity_surface.interface_implementation_summary = result.type_metadata_handoff.interface_implementation_summary;
  result.parity_surface.interface_method_symbols_total =
      result.parity_surface.interface_implementation_summary.interface_method_symbols;
  result.parity_surface.implementation_method_symbols_total =
      result.parity_surface.interface_implementation_summary.implementation_method_symbols;
  result.parity_surface.linked_implementation_symbols_total =
      result.parity_surface.interface_implementation_summary.linked_implementation_symbols;
  result.parity_surface.protocol_category_composition_summary =
      result.type_metadata_handoff.protocol_category_composition_summary;
  result.parity_surface.protocol_composition_sites_total =
      result.parity_surface.protocol_category_composition_summary.protocol_composition_sites;
  result.parity_surface.protocol_composition_symbols_total =
      result.parity_surface.protocol_category_composition_summary.protocol_composition_symbols;
  result.parity_surface.category_composition_sites_total =
      result.parity_surface.protocol_category_composition_summary.category_composition_sites;
  result.parity_surface.category_composition_symbols_total =
      result.parity_surface.protocol_category_composition_summary.category_composition_symbols;
  result.parity_surface.invalid_protocol_composition_sites_total =
      result.parity_surface.protocol_category_composition_summary.invalid_protocol_composition_sites;
  result.parity_surface.class_protocol_category_linking_summary =
      result.type_metadata_handoff.class_protocol_category_linking_summary;
  result.parity_surface.selector_normalization_summary = result.type_metadata_handoff.selector_normalization_summary;
  result.parity_surface.selector_normalization_methods_total =
      result.parity_surface.selector_normalization_summary.methods_total;
  result.parity_surface.selector_normalization_normalized_methods_total =
      result.parity_surface.selector_normalization_summary.normalized_methods;
  result.parity_surface.selector_normalization_piece_entries_total =
      result.parity_surface.selector_normalization_summary.selector_piece_entries;
  result.parity_surface.selector_normalization_parameter_piece_entries_total =
      result.parity_surface.selector_normalization_summary.selector_parameter_piece_entries;
  result.parity_surface.selector_normalization_pieceless_methods_total =
      result.parity_surface.selector_normalization_summary.selector_pieceless_methods;
  result.parity_surface.selector_normalization_spelling_mismatches_total =
      result.parity_surface.selector_normalization_summary.selector_spelling_mismatches;
  result.parity_surface.selector_normalization_arity_mismatches_total =
      result.parity_surface.selector_normalization_summary.selector_arity_mismatches;
  result.parity_surface.selector_normalization_parameter_linkage_mismatches_total =
      result.parity_surface.selector_normalization_summary.selector_parameter_linkage_mismatches;
  result.parity_surface.selector_normalization_flag_mismatches_total =
      result.parity_surface.selector_normalization_summary.selector_normalization_flag_mismatches;
  result.parity_surface.selector_normalization_missing_keyword_pieces_total =
      result.parity_surface.selector_normalization_summary.selector_missing_keyword_pieces;
  result.parity_surface.property_attribute_summary = result.type_metadata_handoff.property_attribute_summary;
  result.parity_surface.property_attribute_properties_total =
      result.parity_surface.property_attribute_summary.properties_total;
  result.parity_surface.property_attribute_entries_total =
      result.parity_surface.property_attribute_summary.attribute_entries;
  result.parity_surface.property_attribute_readonly_modifiers_total =
      result.parity_surface.property_attribute_summary.readonly_modifiers;
  result.parity_surface.property_attribute_readwrite_modifiers_total =
      result.parity_surface.property_attribute_summary.readwrite_modifiers;
  result.parity_surface.property_attribute_atomic_modifiers_total =
      result.parity_surface.property_attribute_summary.atomic_modifiers;
  result.parity_surface.property_attribute_nonatomic_modifiers_total =
      result.parity_surface.property_attribute_summary.nonatomic_modifiers;
  result.parity_surface.property_attribute_copy_modifiers_total =
      result.parity_surface.property_attribute_summary.copy_modifiers;
  result.parity_surface.property_attribute_strong_modifiers_total =
      result.parity_surface.property_attribute_summary.strong_modifiers;
  result.parity_surface.property_attribute_weak_modifiers_total =
      result.parity_surface.property_attribute_summary.weak_modifiers;
  result.parity_surface.property_attribute_assign_modifiers_total =
      result.parity_surface.property_attribute_summary.assign_modifiers;
  result.parity_surface.property_attribute_getter_modifiers_total =
      result.parity_surface.property_attribute_summary.getter_modifiers;
  result.parity_surface.property_attribute_setter_modifiers_total =
      result.parity_surface.property_attribute_summary.setter_modifiers;
  result.parity_surface.property_attribute_invalid_attribute_entries_total =
      result.parity_surface.property_attribute_summary.invalid_attribute_entries;
  result.parity_surface.property_attribute_contract_violations_total =
      result.parity_surface.property_attribute_summary.property_contract_violations;
  result.parity_surface.type_annotation_surface_summary = result.type_metadata_handoff.type_annotation_surface_summary;
  result.parity_surface.type_annotation_generic_suffix_sites_total =
      result.parity_surface.type_annotation_surface_summary.generic_suffix_sites;
  result.parity_surface.type_annotation_pointer_declarator_sites_total =
      result.parity_surface.type_annotation_surface_summary.pointer_declarator_sites;
  result.parity_surface.type_annotation_nullability_suffix_sites_total =
      result.parity_surface.type_annotation_surface_summary.nullability_suffix_sites;
  result.parity_surface.type_annotation_ownership_qualifier_sites_total =
      result.parity_surface.type_annotation_surface_summary.ownership_qualifier_sites;
  result.parity_surface.type_annotation_object_pointer_type_sites_total =
      result.parity_surface.type_annotation_surface_summary.object_pointer_type_sites;
  result.parity_surface.type_annotation_invalid_generic_suffix_sites_total =
      result.parity_surface.type_annotation_surface_summary.invalid_generic_suffix_sites;
  result.parity_surface.type_annotation_invalid_pointer_declarator_sites_total =
      result.parity_surface.type_annotation_surface_summary.invalid_pointer_declarator_sites;
  result.parity_surface.type_annotation_invalid_nullability_suffix_sites_total =
      result.parity_surface.type_annotation_surface_summary.invalid_nullability_suffix_sites;
  result.parity_surface.type_annotation_invalid_ownership_qualifier_sites_total =
      result.parity_surface.type_annotation_surface_summary.invalid_ownership_qualifier_sites;
  result.parity_surface.symbol_graph_scope_resolution_summary =
      result.type_metadata_handoff.symbol_graph_scope_resolution_summary;
  result.parity_surface.symbol_graph_global_symbol_nodes_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.global_symbol_nodes;
  result.parity_surface.symbol_graph_function_symbol_nodes_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.function_symbol_nodes;
  result.parity_surface.symbol_graph_interface_symbol_nodes_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.interface_symbol_nodes;
  result.parity_surface.symbol_graph_implementation_symbol_nodes_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_symbol_nodes;
  result.parity_surface.symbol_graph_interface_property_symbol_nodes_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.interface_property_symbol_nodes;
  result.parity_surface.symbol_graph_implementation_property_symbol_nodes_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_property_symbol_nodes;
  result.parity_surface.symbol_graph_interface_method_symbol_nodes_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.interface_method_symbol_nodes;
  result.parity_surface.symbol_graph_implementation_method_symbol_nodes_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_method_symbol_nodes;
  result.parity_surface.symbol_graph_top_level_scope_symbols_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.top_level_scope_symbols;
  result.parity_surface.symbol_graph_nested_scope_symbols_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.nested_scope_symbols;
  result.parity_surface.symbol_graph_scope_frames_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.scope_frames_total;
  result.parity_surface.symbol_graph_implementation_interface_resolution_sites_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_sites;
  result.parity_surface.symbol_graph_implementation_interface_resolution_hits_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_hits;
  result.parity_surface.symbol_graph_implementation_interface_resolution_misses_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_misses;
  result.parity_surface.symbol_graph_method_resolution_sites_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.method_resolution_sites;
  result.parity_surface.symbol_graph_method_resolution_hits_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.method_resolution_hits;
  result.parity_surface.symbol_graph_method_resolution_misses_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.method_resolution_misses;
  result.parity_surface.method_lookup_override_conflict_summary =
      result.type_metadata_handoff.method_lookup_override_conflict_summary;
  result.parity_surface.method_lookup_override_conflict_lookup_sites_total =
      result.parity_surface.method_lookup_override_conflict_summary.method_lookup_sites;
  result.parity_surface.method_lookup_override_conflict_lookup_hits_total =
      result.parity_surface.method_lookup_override_conflict_summary.method_lookup_hits;
  result.parity_surface.method_lookup_override_conflict_lookup_misses_total =
      result.parity_surface.method_lookup_override_conflict_summary.method_lookup_misses;
  result.parity_surface.method_lookup_override_conflict_override_sites_total =
      result.parity_surface.method_lookup_override_conflict_summary.override_lookup_sites;
  result.parity_surface.method_lookup_override_conflict_override_hits_total =
      result.parity_surface.method_lookup_override_conflict_summary.override_lookup_hits;
  result.parity_surface.method_lookup_override_conflict_override_misses_total =
      result.parity_surface.method_lookup_override_conflict_summary.override_lookup_misses;
  result.parity_surface.method_lookup_override_conflict_override_conflicts_total =
      result.parity_surface.method_lookup_override_conflict_summary.override_conflicts;
  result.parity_surface.method_lookup_override_conflict_unresolved_base_interfaces_total =
      result.parity_surface.method_lookup_override_conflict_summary.unresolved_base_interfaces;
  result.parity_surface.property_synthesis_ivar_binding_summary =
      result.type_metadata_handoff.property_synthesis_ivar_binding_summary;
  result.parity_surface.property_synthesis_ivar_binding_property_synthesis_sites_total =
      result.parity_surface.property_synthesis_ivar_binding_summary.property_synthesis_sites;
  result.parity_surface.property_synthesis_ivar_binding_explicit_ivar_bindings_total =
      result.parity_surface.property_synthesis_ivar_binding_summary.property_synthesis_explicit_ivar_bindings;
  result.parity_surface.property_synthesis_ivar_binding_default_ivar_bindings_total =
      result.parity_surface.property_synthesis_ivar_binding_summary.property_synthesis_default_ivar_bindings;
  result.parity_surface.property_synthesis_ivar_binding_ivar_binding_sites_total =
      result.parity_surface.property_synthesis_ivar_binding_summary.ivar_binding_sites;
  result.parity_surface.property_synthesis_ivar_binding_ivar_binding_resolved_total =
      result.parity_surface.property_synthesis_ivar_binding_summary.ivar_binding_resolved;
  result.parity_surface.property_synthesis_ivar_binding_ivar_binding_missing_total =
      result.parity_surface.property_synthesis_ivar_binding_summary.ivar_binding_missing;
  result.parity_surface.property_synthesis_ivar_binding_ivar_binding_conflicts_total =
      result.parity_surface.property_synthesis_ivar_binding_summary.ivar_binding_conflicts;
  result.parity_surface.id_class_sel_object_pointer_type_checking_summary =
      result.type_metadata_handoff.id_class_sel_object_pointer_type_checking_summary;
  result.parity_surface.id_class_sel_object_pointer_param_type_sites_total =
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.param_type_sites;
  result.parity_surface.id_class_sel_object_pointer_param_id_spelling_sites_total =
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.param_id_spelling_sites;
  result.parity_surface.id_class_sel_object_pointer_param_class_spelling_sites_total =
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.param_class_spelling_sites;
  result.parity_surface.id_class_sel_object_pointer_param_sel_spelling_sites_total =
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.param_sel_spelling_sites;
  result.parity_surface.id_class_sel_object_pointer_param_instancetype_spelling_sites_total =
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.param_instancetype_spelling_sites;
  result.parity_surface.id_class_sel_object_pointer_param_object_pointer_type_sites_total =
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.param_object_pointer_type_sites;
  result.parity_surface.id_class_sel_object_pointer_return_type_sites_total =
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.return_type_sites;
  result.parity_surface.id_class_sel_object_pointer_return_id_spelling_sites_total =
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.return_id_spelling_sites;
  result.parity_surface.id_class_sel_object_pointer_return_class_spelling_sites_total =
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.return_class_spelling_sites;
  result.parity_surface.id_class_sel_object_pointer_return_sel_spelling_sites_total =
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.return_sel_spelling_sites;
  result.parity_surface.id_class_sel_object_pointer_return_instancetype_spelling_sites_total =
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.return_instancetype_spelling_sites;
  result.parity_surface.id_class_sel_object_pointer_return_object_pointer_type_sites_total =
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.return_object_pointer_type_sites;
  result.parity_surface.id_class_sel_object_pointer_property_type_sites_total =
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.property_type_sites;
  result.parity_surface.id_class_sel_object_pointer_property_id_spelling_sites_total =
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.property_id_spelling_sites;
  result.parity_surface.id_class_sel_object_pointer_property_class_spelling_sites_total =
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.property_class_spelling_sites;
  result.parity_surface.id_class_sel_object_pointer_property_sel_spelling_sites_total =
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.property_sel_spelling_sites;
  result.parity_surface.id_class_sel_object_pointer_property_instancetype_spelling_sites_total =
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.property_instancetype_spelling_sites;
  result.parity_surface.id_class_sel_object_pointer_property_object_pointer_type_sites_total =
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.property_object_pointer_type_sites;
  result.parity_surface.block_literal_capture_semantics_summary =
      result.type_metadata_handoff.block_literal_capture_semantics_summary;
  result.parity_surface.block_literal_capture_semantics_sites_total =
      result.parity_surface.block_literal_capture_semantics_summary.block_literal_sites;
  result.parity_surface.block_literal_capture_semantics_parameter_entries_total =
      result.parity_surface.block_literal_capture_semantics_summary.block_parameter_entries;
  result.parity_surface.block_literal_capture_semantics_capture_entries_total =
      result.parity_surface.block_literal_capture_semantics_summary.block_capture_entries;
  result.parity_surface.block_literal_capture_semantics_body_statement_entries_total =
      result.parity_surface.block_literal_capture_semantics_summary.block_body_statement_entries;
  result.parity_surface.block_literal_capture_semantics_empty_capture_sites_total =
      result.parity_surface.block_literal_capture_semantics_summary.block_empty_capture_sites;
  result.parity_surface.block_literal_capture_semantics_nondeterministic_capture_sites_total =
      result.parity_surface.block_literal_capture_semantics_summary.block_nondeterministic_capture_sites;
  result.parity_surface.block_literal_capture_semantics_non_normalized_sites_total =
      result.parity_surface.block_literal_capture_semantics_summary.block_non_normalized_sites;
  result.parity_surface.block_literal_capture_semantics_contract_violation_sites_total =
      result.parity_surface.block_literal_capture_semantics_summary.contract_violation_sites;
  result.parity_surface.block_abi_invoke_trampoline_semantics_summary =
      result.type_metadata_handoff.block_abi_invoke_trampoline_semantics_summary;
  result.parity_surface.block_abi_invoke_trampoline_sites_total =
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.block_literal_sites;
  result.parity_surface.block_abi_invoke_trampoline_invoke_argument_slots_total =
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.invoke_argument_slots_total;
  result.parity_surface.block_abi_invoke_trampoline_capture_word_count_total =
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.capture_word_count_total;
  result.parity_surface.block_abi_invoke_trampoline_parameter_entries_total =
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.parameter_entries_total;
  result.parity_surface.block_abi_invoke_trampoline_capture_entries_total =
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.capture_entries_total;
  result.parity_surface.block_abi_invoke_trampoline_body_statement_entries_total =
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.body_statement_entries_total;
  result.parity_surface.block_abi_invoke_trampoline_descriptor_symbolized_sites_total =
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.descriptor_symbolized_sites;
  result.parity_surface.block_abi_invoke_trampoline_invoke_symbolized_sites_total =
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.invoke_trampoline_symbolized_sites;
  result.parity_surface.block_abi_invoke_trampoline_missing_invoke_sites_total =
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.missing_invoke_trampoline_sites;
  result.parity_surface.block_abi_invoke_trampoline_non_normalized_layout_sites_total =
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.non_normalized_layout_sites;
  result.parity_surface.block_abi_invoke_trampoline_contract_violation_sites_total =
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.contract_violation_sites;
  result.parity_surface.block_storage_escape_semantics_summary =
      result.type_metadata_handoff.block_storage_escape_semantics_summary;
  result.parity_surface.block_storage_escape_sites_total =
      result.parity_surface.block_storage_escape_semantics_summary.block_literal_sites;
  result.parity_surface.block_storage_escape_mutable_capture_count_total =
      result.parity_surface.block_storage_escape_semantics_summary.mutable_capture_count_total;
  result.parity_surface.block_storage_escape_byref_slot_count_total =
      result.parity_surface.block_storage_escape_semantics_summary.byref_slot_count_total;
  result.parity_surface.block_storage_escape_parameter_entries_total =
      result.parity_surface.block_storage_escape_semantics_summary.parameter_entries_total;
  result.parity_surface.block_storage_escape_capture_entries_total =
      result.parity_surface.block_storage_escape_semantics_summary.capture_entries_total;
  result.parity_surface.block_storage_escape_body_statement_entries_total =
      result.parity_surface.block_storage_escape_semantics_summary.body_statement_entries_total;
  result.parity_surface.block_storage_escape_requires_byref_cells_sites_total =
      result.parity_surface.block_storage_escape_semantics_summary.requires_byref_cells_sites;
  result.parity_surface.block_storage_escape_escape_analysis_enabled_sites_total =
      result.parity_surface.block_storage_escape_semantics_summary.escape_analysis_enabled_sites;
  result.parity_surface.block_storage_escape_escape_to_heap_sites_total =
      result.parity_surface.block_storage_escape_semantics_summary.escape_to_heap_sites;
  result.parity_surface.block_storage_escape_escape_profile_normalized_sites_total =
      result.parity_surface.block_storage_escape_semantics_summary.escape_profile_normalized_sites;
  result.parity_surface.block_storage_escape_byref_layout_symbolized_sites_total =
      result.parity_surface.block_storage_escape_semantics_summary.byref_layout_symbolized_sites;
  result.parity_surface.block_storage_escape_contract_violation_sites_total =
      result.parity_surface.block_storage_escape_semantics_summary.contract_violation_sites;
  result.parity_surface.block_copy_dispose_semantics_summary =
      result.type_metadata_handoff.block_copy_dispose_semantics_summary;
  result.parity_surface.block_copy_dispose_sites_total =
      result.parity_surface.block_copy_dispose_semantics_summary.block_literal_sites;
  result.parity_surface.block_copy_dispose_mutable_capture_count_total =
      result.parity_surface.block_copy_dispose_semantics_summary.mutable_capture_count_total;
  result.parity_surface.block_copy_dispose_byref_slot_count_total =
      result.parity_surface.block_copy_dispose_semantics_summary.byref_slot_count_total;
  result.parity_surface.block_copy_dispose_parameter_entries_total =
      result.parity_surface.block_copy_dispose_semantics_summary.parameter_entries_total;
  result.parity_surface.block_copy_dispose_capture_entries_total =
      result.parity_surface.block_copy_dispose_semantics_summary.capture_entries_total;
  result.parity_surface.block_copy_dispose_body_statement_entries_total =
      result.parity_surface.block_copy_dispose_semantics_summary.body_statement_entries_total;
  result.parity_surface.block_copy_dispose_copy_helper_required_sites_total =
      result.parity_surface.block_copy_dispose_semantics_summary.copy_helper_required_sites;
  result.parity_surface.block_copy_dispose_dispose_helper_required_sites_total =
      result.parity_surface.block_copy_dispose_semantics_summary.dispose_helper_required_sites;
  result.parity_surface.block_copy_dispose_profile_normalized_sites_total =
      result.parity_surface.block_copy_dispose_semantics_summary.profile_normalized_sites;
  result.parity_surface.block_copy_dispose_copy_helper_symbolized_sites_total =
      result.parity_surface.block_copy_dispose_semantics_summary.copy_helper_symbolized_sites;
  result.parity_surface.block_copy_dispose_dispose_helper_symbolized_sites_total =
      result.parity_surface.block_copy_dispose_semantics_summary.dispose_helper_symbolized_sites;
  result.parity_surface.block_copy_dispose_contract_violation_sites_total =
      result.parity_surface.block_copy_dispose_semantics_summary.contract_violation_sites;
  result.parity_surface.message_send_selector_lowering_summary =
      result.type_metadata_handoff.message_send_selector_lowering_summary;
  result.parity_surface.message_send_selector_lowering_sites_total =
      result.parity_surface.message_send_selector_lowering_summary.message_send_sites;
  result.parity_surface.message_send_selector_lowering_unary_form_sites_total =
      result.parity_surface.message_send_selector_lowering_summary.unary_form_sites;
  result.parity_surface.message_send_selector_lowering_keyword_form_sites_total =
      result.parity_surface.message_send_selector_lowering_summary.keyword_form_sites;
  result.parity_surface.message_send_selector_lowering_symbol_sites_total =
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_symbol_sites;
  result.parity_surface.message_send_selector_lowering_piece_entries_total =
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_piece_entries;
  result.parity_surface.message_send_selector_lowering_argument_piece_entries_total =
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_argument_piece_entries;
  result.parity_surface.message_send_selector_lowering_normalized_sites_total =
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_normalized_sites;
  result.parity_surface.message_send_selector_lowering_form_mismatch_sites_total =
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_form_mismatch_sites;
  result.parity_surface.message_send_selector_lowering_arity_mismatch_sites_total =
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_arity_mismatch_sites;
  result.parity_surface.message_send_selector_lowering_symbol_mismatch_sites_total =
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_symbol_mismatch_sites;
  result.parity_surface.message_send_selector_lowering_missing_symbol_sites_total =
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_missing_symbol_sites;
  result.parity_surface.message_send_selector_lowering_contract_violation_sites_total =
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_contract_violation_sites;
  result.parity_surface.dispatch_abi_marshalling_summary =
      result.type_metadata_handoff.dispatch_abi_marshalling_summary;
  result.parity_surface.dispatch_abi_marshalling_sites_total =
      result.parity_surface.dispatch_abi_marshalling_summary.message_send_sites;
  result.parity_surface.dispatch_abi_marshalling_receiver_slots_total =
      result.parity_surface.dispatch_abi_marshalling_summary.receiver_slots;
  result.parity_surface.dispatch_abi_marshalling_selector_symbol_slots_total =
      result.parity_surface.dispatch_abi_marshalling_summary.selector_symbol_slots;
  result.parity_surface.dispatch_abi_marshalling_argument_slots_total =
      result.parity_surface.dispatch_abi_marshalling_summary.argument_slots;
  result.parity_surface.dispatch_abi_marshalling_keyword_argument_slots_total =
      result.parity_surface.dispatch_abi_marshalling_summary.keyword_argument_slots;
  result.parity_surface.dispatch_abi_marshalling_unary_argument_slots_total =
      result.parity_surface.dispatch_abi_marshalling_summary.unary_argument_slots;
  result.parity_surface.dispatch_abi_marshalling_arity_mismatch_sites_total =
      result.parity_surface.dispatch_abi_marshalling_summary.arity_mismatch_sites;
  result.parity_surface.dispatch_abi_marshalling_missing_selector_symbol_sites_total =
      result.parity_surface.dispatch_abi_marshalling_summary.missing_selector_symbol_sites;
  result.parity_surface.dispatch_abi_marshalling_contract_violation_sites_total =
      result.parity_surface.dispatch_abi_marshalling_summary.contract_violation_sites;
  result.parity_surface.nil_receiver_semantics_foldability_summary =
      result.type_metadata_handoff.nil_receiver_semantics_foldability_summary;
  result.parity_surface.nil_receiver_semantics_foldability_sites_total =
      result.parity_surface.nil_receiver_semantics_foldability_summary.message_send_sites;
  result.parity_surface.nil_receiver_semantics_foldability_receiver_nil_literal_sites_total =
      result.parity_surface.nil_receiver_semantics_foldability_summary.receiver_nil_literal_sites;
  result.parity_surface.nil_receiver_semantics_foldability_enabled_sites_total =
      result.parity_surface.nil_receiver_semantics_foldability_summary.nil_receiver_semantics_enabled_sites;
  result.parity_surface.nil_receiver_semantics_foldability_foldable_sites_total =
      result.parity_surface.nil_receiver_semantics_foldability_summary.nil_receiver_foldable_sites;
  result.parity_surface.nil_receiver_semantics_foldability_runtime_dispatch_required_sites_total =
      result.parity_surface.nil_receiver_semantics_foldability_summary.nil_receiver_runtime_dispatch_required_sites;
  result.parity_surface.nil_receiver_semantics_foldability_non_nil_receiver_sites_total =
      result.parity_surface.nil_receiver_semantics_foldability_summary.non_nil_receiver_sites;
  result.parity_surface.nil_receiver_semantics_foldability_contract_violation_sites_total =
      result.parity_surface.nil_receiver_semantics_foldability_summary.contract_violation_sites;
  result.parity_surface.super_dispatch_method_family_summary =
      result.type_metadata_handoff.super_dispatch_method_family_summary;
  result.parity_surface.super_dispatch_method_family_sites_total =
      result.parity_surface.super_dispatch_method_family_summary.message_send_sites;
  result.parity_surface.super_dispatch_method_family_receiver_super_identifier_sites_total =
      result.parity_surface.super_dispatch_method_family_summary.receiver_super_identifier_sites;
  result.parity_surface.super_dispatch_method_family_enabled_sites_total =
      result.parity_surface.super_dispatch_method_family_summary.super_dispatch_enabled_sites;
  result.parity_surface.super_dispatch_method_family_requires_class_context_sites_total =
      result.parity_surface.super_dispatch_method_family_summary.super_dispatch_requires_class_context_sites;
  result.parity_surface.super_dispatch_method_family_init_sites_total =
      result.parity_surface.super_dispatch_method_family_summary.method_family_init_sites;
  result.parity_surface.super_dispatch_method_family_copy_sites_total =
      result.parity_surface.super_dispatch_method_family_summary.method_family_copy_sites;
  result.parity_surface.super_dispatch_method_family_mutable_copy_sites_total =
      result.parity_surface.super_dispatch_method_family_summary.method_family_mutable_copy_sites;
  result.parity_surface.super_dispatch_method_family_new_sites_total =
      result.parity_surface.super_dispatch_method_family_summary.method_family_new_sites;
  result.parity_surface.super_dispatch_method_family_none_sites_total =
      result.parity_surface.super_dispatch_method_family_summary.method_family_none_sites;
  result.parity_surface.super_dispatch_method_family_returns_retained_result_sites_total =
      result.parity_surface.super_dispatch_method_family_summary.method_family_returns_retained_result_sites;
  result.parity_surface.super_dispatch_method_family_returns_related_result_sites_total =
      result.parity_surface.super_dispatch_method_family_summary.method_family_returns_related_result_sites;
  result.parity_surface.super_dispatch_method_family_contract_violation_sites_total =
      result.parity_surface.super_dispatch_method_family_summary.contract_violation_sites;
  result.parity_surface.runtime_shim_host_link_summary =
      result.type_metadata_handoff.runtime_shim_host_link_summary;
  result.parity_surface.runtime_shim_host_link_message_send_sites_total =
      result.parity_surface.runtime_shim_host_link_summary.message_send_sites;
  result.parity_surface.runtime_shim_host_link_required_sites_total =
      result.parity_surface.runtime_shim_host_link_summary.runtime_shim_required_sites;
  result.parity_surface.runtime_shim_host_link_elided_sites_total =
      result.parity_surface.runtime_shim_host_link_summary.runtime_shim_elided_sites;
  result.parity_surface.runtime_shim_host_link_runtime_dispatch_arg_slots_total =
      result.parity_surface.runtime_shim_host_link_summary.runtime_dispatch_arg_slots;
  result.parity_surface.runtime_shim_host_link_runtime_dispatch_declaration_parameter_count_total =
      result.parity_surface.runtime_shim_host_link_summary.runtime_dispatch_declaration_parameter_count;
  result.parity_surface.runtime_shim_host_link_contract_violation_sites_total =
      result.parity_surface.runtime_shim_host_link_summary.contract_violation_sites;
  result.parity_surface.runtime_shim_host_link_runtime_dispatch_symbol =
      result.parity_surface.runtime_shim_host_link_summary.runtime_dispatch_symbol;
  result.parity_surface.runtime_shim_host_link_default_runtime_dispatch_symbol_binding =
      result.parity_surface.runtime_shim_host_link_summary.default_runtime_dispatch_symbol_binding;
  result.parity_surface.retain_release_operation_summary =
      result.type_metadata_handoff.retain_release_operation_summary;
  result.parity_surface.retain_release_operation_ownership_qualified_sites_total =
      result.parity_surface.retain_release_operation_summary.ownership_qualified_sites;
  result.parity_surface.retain_release_operation_retain_insertion_sites_total =
      result.parity_surface.retain_release_operation_summary.retain_insertion_sites;
  result.parity_surface.retain_release_operation_release_insertion_sites_total =
      result.parity_surface.retain_release_operation_summary.release_insertion_sites;
  result.parity_surface.retain_release_operation_autorelease_insertion_sites_total =
      result.parity_surface.retain_release_operation_summary.autorelease_insertion_sites;
  result.parity_surface.retain_release_operation_contract_violation_sites_total =
      result.parity_surface.retain_release_operation_summary.contract_violation_sites;
  result.parity_surface.weak_unowned_semantics_summary =
      result.type_metadata_handoff.weak_unowned_semantics_summary;
  result.parity_surface.weak_unowned_semantics_ownership_candidate_sites_total =
      result.parity_surface.weak_unowned_semantics_summary.ownership_candidate_sites;
  result.parity_surface.weak_unowned_semantics_weak_reference_sites_total =
      result.parity_surface.weak_unowned_semantics_summary.weak_reference_sites;
  result.parity_surface.weak_unowned_semantics_unowned_reference_sites_total =
      result.parity_surface.weak_unowned_semantics_summary.unowned_reference_sites;
  result.parity_surface.weak_unowned_semantics_unowned_safe_reference_sites_total =
      result.parity_surface.weak_unowned_semantics_summary.unowned_safe_reference_sites;
  result.parity_surface.weak_unowned_semantics_conflict_sites_total =
      result.parity_surface.weak_unowned_semantics_summary.weak_unowned_conflict_sites;
  result.parity_surface.weak_unowned_semantics_contract_violation_sites_total =
      result.parity_surface.weak_unowned_semantics_summary.contract_violation_sites;
  result.parity_surface.arc_diagnostics_fixit_summary =
      result.type_metadata_handoff.arc_diagnostics_fixit_summary;
  result.parity_surface.ownership_arc_diagnostic_candidate_sites_total =
      result.parity_surface.arc_diagnostics_fixit_summary.ownership_arc_diagnostic_candidate_sites;
  result.parity_surface.ownership_arc_fixit_available_sites_total =
      result.parity_surface.arc_diagnostics_fixit_summary.ownership_arc_fixit_available_sites;
  result.parity_surface.ownership_arc_profiled_sites_total =
      result.parity_surface.arc_diagnostics_fixit_summary.ownership_arc_profiled_sites;
  result.parity_surface.ownership_arc_weak_unowned_conflict_diagnostic_sites_total =
      result.parity_surface.arc_diagnostics_fixit_summary.ownership_arc_weak_unowned_conflict_diagnostic_sites;
  result.parity_surface.ownership_arc_empty_fixit_hint_sites_total =
      result.parity_surface.arc_diagnostics_fixit_summary.ownership_arc_empty_fixit_hint_sites;
  result.parity_surface.ownership_arc_contract_violation_sites_total =
      result.parity_surface.arc_diagnostics_fixit_summary.contract_violation_sites;
  result.parity_surface.autoreleasepool_scope_summary = result.type_metadata_handoff.autoreleasepool_scope_summary;
  result.parity_surface.autoreleasepool_scope_sites_total =
      result.parity_surface.autoreleasepool_scope_summary.scope_sites;
  result.parity_surface.autoreleasepool_scope_symbolized_sites_total =
      result.parity_surface.autoreleasepool_scope_summary.scope_symbolized_sites;
  result.parity_surface.autoreleasepool_scope_contract_violation_sites_total =
      result.parity_surface.autoreleasepool_scope_summary.contract_violation_sites;
  result.parity_surface.autoreleasepool_scope_max_depth_total =
      result.parity_surface.autoreleasepool_scope_summary.max_scope_depth;
  result.parity_surface.diagnostics_after_pass_monotonic =
      IsMonotonicObjc3SemaDiagnosticsAfterPass(result.diagnostics_after_pass);
  result.parity_surface.deterministic_semantic_diagnostics = result.deterministic_semantic_diagnostics;
  result.parity_surface.deterministic_type_metadata_handoff = result.deterministic_type_metadata_handoff;
  result.parity_surface.deterministic_interface_implementation_handoff =
      result.deterministic_interface_implementation_handoff &&
      result.parity_surface.interfaces_total == result.parity_surface.type_metadata_interface_entries &&
      result.parity_surface.implementations_total == result.parity_surface.type_metadata_implementation_entries &&
      result.parity_surface.interface_implementation_summary.resolved_interfaces ==
          result.parity_surface.type_metadata_interface_entries &&
      result.parity_surface.interface_implementation_summary.resolved_implementations ==
          result.parity_surface.type_metadata_implementation_entries;
  result.parity_surface.deterministic_protocol_category_composition_handoff =
      result.deterministic_protocol_category_composition_handoff &&
      result.parity_surface.protocol_category_composition_summary.protocol_composition_sites ==
          result.parity_surface.protocol_composition_sites_total &&
      result.parity_surface.protocol_category_composition_summary.protocol_composition_symbols ==
          result.parity_surface.protocol_composition_symbols_total &&
      result.parity_surface.protocol_category_composition_summary.category_composition_sites ==
          result.parity_surface.category_composition_sites_total &&
      result.parity_surface.protocol_category_composition_summary.category_composition_symbols ==
          result.parity_surface.category_composition_symbols_total &&
      result.parity_surface.protocol_category_composition_summary.invalid_protocol_composition_sites ==
          result.parity_surface.invalid_protocol_composition_sites_total &&
      result.parity_surface.protocol_category_composition_summary.invalid_protocol_composition_sites <=
          result.parity_surface.protocol_category_composition_summary.total_composition_sites();
  result.parity_surface.deterministic_class_protocol_category_linking_handoff =
      result.deterministic_class_protocol_category_linking_handoff &&
      result.parity_surface.class_protocol_category_linking_summary.declared_interfaces ==
          result.parity_surface.interface_implementation_summary.declared_interfaces &&
      result.parity_surface.class_protocol_category_linking_summary.resolved_interfaces ==
          result.parity_surface.interface_implementation_summary.resolved_interfaces &&
      result.parity_surface.class_protocol_category_linking_summary.declared_implementations ==
          result.parity_surface.interface_implementation_summary.declared_implementations &&
      result.parity_surface.class_protocol_category_linking_summary.resolved_implementations ==
          result.parity_surface.interface_implementation_summary.resolved_implementations &&
      result.parity_surface.class_protocol_category_linking_summary.interface_method_symbols ==
          result.parity_surface.interface_method_symbols_total &&
      result.parity_surface.class_protocol_category_linking_summary.implementation_method_symbols ==
          result.parity_surface.implementation_method_symbols_total &&
      result.parity_surface.class_protocol_category_linking_summary.linked_implementation_symbols ==
          result.parity_surface.linked_implementation_symbols_total &&
      result.parity_surface.class_protocol_category_linking_summary.protocol_composition_sites ==
          result.parity_surface.protocol_composition_sites_total &&
      result.parity_surface.class_protocol_category_linking_summary.protocol_composition_symbols ==
          result.parity_surface.protocol_composition_symbols_total &&
      result.parity_surface.class_protocol_category_linking_summary.category_composition_sites ==
          result.parity_surface.category_composition_sites_total &&
      result.parity_surface.class_protocol_category_linking_summary.category_composition_symbols ==
          result.parity_surface.category_composition_symbols_total &&
      result.parity_surface.class_protocol_category_linking_summary.invalid_protocol_composition_sites ==
          result.parity_surface.invalid_protocol_composition_sites_total &&
      result.parity_surface.class_protocol_category_linking_summary.invalid_protocol_composition_sites <=
          result.parity_surface.class_protocol_category_linking_summary.total_composition_sites() &&
      result.parity_surface.class_protocol_category_linking_summary.deterministic;
  result.parity_surface.deterministic_selector_normalization_handoff =
      result.deterministic_selector_normalization_handoff &&
      result.parity_surface.selector_normalization_summary.methods_total ==
          result.parity_surface.selector_normalization_methods_total &&
      result.parity_surface.selector_normalization_summary.normalized_methods ==
          result.parity_surface.selector_normalization_normalized_methods_total &&
      result.parity_surface.selector_normalization_summary.selector_piece_entries ==
          result.parity_surface.selector_normalization_piece_entries_total &&
      result.parity_surface.selector_normalization_summary.selector_parameter_piece_entries ==
          result.parity_surface.selector_normalization_parameter_piece_entries_total &&
      result.parity_surface.selector_normalization_summary.selector_pieceless_methods ==
          result.parity_surface.selector_normalization_pieceless_methods_total &&
      result.parity_surface.selector_normalization_summary.selector_spelling_mismatches ==
          result.parity_surface.selector_normalization_spelling_mismatches_total &&
      result.parity_surface.selector_normalization_summary.selector_arity_mismatches ==
          result.parity_surface.selector_normalization_arity_mismatches_total &&
      result.parity_surface.selector_normalization_summary.selector_parameter_linkage_mismatches ==
          result.parity_surface.selector_normalization_parameter_linkage_mismatches_total &&
      result.parity_surface.selector_normalization_summary.selector_normalization_flag_mismatches ==
          result.parity_surface.selector_normalization_flag_mismatches_total &&
      result.parity_surface.selector_normalization_summary.selector_missing_keyword_pieces ==
          result.parity_surface.selector_normalization_missing_keyword_pieces_total &&
      result.parity_surface.selector_normalization_summary.normalized_methods <=
          result.parity_surface.selector_normalization_summary.methods_total &&
      result.parity_surface.selector_normalization_summary.selector_parameter_piece_entries <=
          result.parity_surface.selector_normalization_summary.selector_piece_entries &&
      result.parity_surface.selector_normalization_summary.contract_violations() <=
          result.parity_surface.selector_normalization_summary.methods_total &&
      result.parity_surface.selector_normalization_summary.deterministic;
  result.parity_surface.deterministic_property_attribute_handoff =
      result.deterministic_property_attribute_handoff &&
      result.parity_surface.property_attribute_summary.properties_total ==
          result.parity_surface.property_attribute_properties_total &&
      result.parity_surface.property_attribute_summary.attribute_entries ==
          result.parity_surface.property_attribute_entries_total &&
      result.parity_surface.property_attribute_summary.readonly_modifiers ==
          result.parity_surface.property_attribute_readonly_modifiers_total &&
      result.parity_surface.property_attribute_summary.readwrite_modifiers ==
          result.parity_surface.property_attribute_readwrite_modifiers_total &&
      result.parity_surface.property_attribute_summary.atomic_modifiers ==
          result.parity_surface.property_attribute_atomic_modifiers_total &&
      result.parity_surface.property_attribute_summary.nonatomic_modifiers ==
          result.parity_surface.property_attribute_nonatomic_modifiers_total &&
      result.parity_surface.property_attribute_summary.copy_modifiers ==
          result.parity_surface.property_attribute_copy_modifiers_total &&
      result.parity_surface.property_attribute_summary.strong_modifiers ==
          result.parity_surface.property_attribute_strong_modifiers_total &&
      result.parity_surface.property_attribute_summary.weak_modifiers ==
          result.parity_surface.property_attribute_weak_modifiers_total &&
      result.parity_surface.property_attribute_summary.assign_modifiers ==
          result.parity_surface.property_attribute_assign_modifiers_total &&
      result.parity_surface.property_attribute_summary.getter_modifiers ==
          result.parity_surface.property_attribute_getter_modifiers_total &&
      result.parity_surface.property_attribute_summary.setter_modifiers ==
          result.parity_surface.property_attribute_setter_modifiers_total &&
      result.parity_surface.property_attribute_summary.invalid_attribute_entries ==
          result.parity_surface.property_attribute_invalid_attribute_entries_total &&
      result.parity_surface.property_attribute_summary.property_contract_violations ==
          result.parity_surface.property_attribute_contract_violations_total &&
      result.parity_surface.property_attribute_summary.getter_modifiers <=
          result.parity_surface.property_attribute_summary.properties_total &&
      result.parity_surface.property_attribute_summary.setter_modifiers <=
          result.parity_surface.property_attribute_summary.properties_total &&
      result.parity_surface.property_attribute_summary.deterministic;
  result.parity_surface.deterministic_type_annotation_surface_handoff =
      result.deterministic_type_annotation_surface_handoff &&
      result.parity_surface.type_annotation_surface_summary.generic_suffix_sites ==
          result.parity_surface.type_annotation_generic_suffix_sites_total &&
      result.parity_surface.type_annotation_surface_summary.pointer_declarator_sites ==
          result.parity_surface.type_annotation_pointer_declarator_sites_total &&
      result.parity_surface.type_annotation_surface_summary.nullability_suffix_sites ==
          result.parity_surface.type_annotation_nullability_suffix_sites_total &&
      result.parity_surface.type_annotation_surface_summary.ownership_qualifier_sites ==
          result.parity_surface.type_annotation_ownership_qualifier_sites_total &&
      result.parity_surface.type_annotation_surface_summary.object_pointer_type_sites ==
          result.parity_surface.type_annotation_object_pointer_type_sites_total &&
      result.parity_surface.type_annotation_surface_summary.invalid_generic_suffix_sites ==
          result.parity_surface.type_annotation_invalid_generic_suffix_sites_total &&
      result.parity_surface.type_annotation_surface_summary.invalid_pointer_declarator_sites ==
          result.parity_surface.type_annotation_invalid_pointer_declarator_sites_total &&
      result.parity_surface.type_annotation_surface_summary.invalid_nullability_suffix_sites ==
          result.parity_surface.type_annotation_invalid_nullability_suffix_sites_total &&
      result.parity_surface.type_annotation_surface_summary.invalid_ownership_qualifier_sites ==
          result.parity_surface.type_annotation_invalid_ownership_qualifier_sites_total &&
      result.parity_surface.type_annotation_surface_summary.invalid_generic_suffix_sites <=
          result.parity_surface.type_annotation_surface_summary.generic_suffix_sites &&
      result.parity_surface.type_annotation_surface_summary.invalid_pointer_declarator_sites <=
          result.parity_surface.type_annotation_surface_summary.pointer_declarator_sites &&
      result.parity_surface.type_annotation_surface_summary.invalid_nullability_suffix_sites <=
          result.parity_surface.type_annotation_surface_summary.nullability_suffix_sites &&
      result.parity_surface.type_annotation_surface_summary.invalid_ownership_qualifier_sites <=
          result.parity_surface.type_annotation_surface_summary.ownership_qualifier_sites &&
      result.parity_surface.type_annotation_surface_summary.invalid_type_annotation_sites() <=
          result.parity_surface.type_annotation_surface_summary.total_type_annotation_sites() &&
      result.parity_surface.type_annotation_surface_summary.deterministic;
  result.parity_surface.deterministic_symbol_graph_scope_resolution_handoff =
      result.deterministic_symbol_graph_scope_resolution_handoff &&
      result.parity_surface.symbol_graph_scope_resolution_summary.global_symbol_nodes ==
          result.parity_surface.symbol_graph_global_symbol_nodes_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.function_symbol_nodes ==
          result.parity_surface.symbol_graph_function_symbol_nodes_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.interface_symbol_nodes ==
          result.parity_surface.symbol_graph_interface_symbol_nodes_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_symbol_nodes ==
          result.parity_surface.symbol_graph_implementation_symbol_nodes_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.interface_property_symbol_nodes ==
          result.parity_surface.symbol_graph_interface_property_symbol_nodes_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_property_symbol_nodes ==
          result.parity_surface.symbol_graph_implementation_property_symbol_nodes_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.interface_method_symbol_nodes ==
          result.parity_surface.symbol_graph_interface_method_symbol_nodes_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_method_symbol_nodes ==
          result.parity_surface.symbol_graph_implementation_method_symbol_nodes_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.top_level_scope_symbols ==
          result.parity_surface.symbol_graph_top_level_scope_symbols_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.nested_scope_symbols ==
          result.parity_surface.symbol_graph_nested_scope_symbols_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.scope_frames_total ==
          result.parity_surface.symbol_graph_scope_frames_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_sites ==
          result.parity_surface.symbol_graph_implementation_interface_resolution_sites_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_hits ==
          result.parity_surface.symbol_graph_implementation_interface_resolution_hits_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_misses ==
          result.parity_surface.symbol_graph_implementation_interface_resolution_misses_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.method_resolution_sites ==
          result.parity_surface.symbol_graph_method_resolution_sites_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.method_resolution_hits ==
          result.parity_surface.symbol_graph_method_resolution_hits_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.method_resolution_misses ==
          result.parity_surface.symbol_graph_method_resolution_misses_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.symbol_nodes_total() ==
          result.parity_surface.symbol_graph_scope_resolution_summary.top_level_scope_symbols +
              result.parity_surface.symbol_graph_scope_resolution_summary.nested_scope_symbols &&
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_hits <=
          result.parity_surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_sites &&
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_hits +
              result.parity_surface.symbol_graph_scope_resolution_summary
                  .implementation_interface_resolution_misses ==
          result.parity_surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_sites &&
      result.parity_surface.symbol_graph_scope_resolution_summary.method_resolution_hits <=
          result.parity_surface.symbol_graph_scope_resolution_summary.method_resolution_sites &&
      result.parity_surface.symbol_graph_scope_resolution_summary.method_resolution_hits +
              result.parity_surface.symbol_graph_scope_resolution_summary.method_resolution_misses ==
          result.parity_surface.symbol_graph_scope_resolution_summary.method_resolution_sites &&
      result.parity_surface.symbol_graph_scope_resolution_summary.resolution_hits_total() <=
          result.parity_surface.symbol_graph_scope_resolution_summary.resolution_sites_total() &&
      result.parity_surface.symbol_graph_scope_resolution_summary.resolution_hits_total() +
              result.parity_surface.symbol_graph_scope_resolution_summary.resolution_misses_total() ==
          result.parity_surface.symbol_graph_scope_resolution_summary.resolution_sites_total() &&
      result.parity_surface.symbol_graph_scope_resolution_summary.deterministic;
  result.parity_surface.deterministic_method_lookup_override_conflict_handoff =
      result.deterministic_method_lookup_override_conflict_handoff &&
      result.parity_surface.method_lookup_override_conflict_summary.method_lookup_sites ==
          result.parity_surface.method_lookup_override_conflict_lookup_sites_total &&
      result.parity_surface.method_lookup_override_conflict_summary.method_lookup_hits ==
          result.parity_surface.method_lookup_override_conflict_lookup_hits_total &&
      result.parity_surface.method_lookup_override_conflict_summary.method_lookup_misses ==
          result.parity_surface.method_lookup_override_conflict_lookup_misses_total &&
      result.parity_surface.method_lookup_override_conflict_summary.override_lookup_sites ==
          result.parity_surface.method_lookup_override_conflict_override_sites_total &&
      result.parity_surface.method_lookup_override_conflict_summary.override_lookup_hits ==
          result.parity_surface.method_lookup_override_conflict_override_hits_total &&
      result.parity_surface.method_lookup_override_conflict_summary.override_lookup_misses ==
          result.parity_surface.method_lookup_override_conflict_override_misses_total &&
      result.parity_surface.method_lookup_override_conflict_summary.override_conflicts ==
          result.parity_surface.method_lookup_override_conflict_override_conflicts_total &&
      result.parity_surface.method_lookup_override_conflict_summary.unresolved_base_interfaces ==
          result.parity_surface.method_lookup_override_conflict_unresolved_base_interfaces_total &&
      result.parity_surface.method_lookup_override_conflict_summary.method_lookup_hits <=
          result.parity_surface.method_lookup_override_conflict_summary.method_lookup_sites &&
      result.parity_surface.method_lookup_override_conflict_summary.method_lookup_hits +
              result.parity_surface.method_lookup_override_conflict_summary.method_lookup_misses ==
          result.parity_surface.method_lookup_override_conflict_summary.method_lookup_sites &&
      result.parity_surface.method_lookup_override_conflict_summary.override_lookup_hits <=
          result.parity_surface.method_lookup_override_conflict_summary.override_lookup_sites &&
      result.parity_surface.method_lookup_override_conflict_summary.override_lookup_hits +
              result.parity_surface.method_lookup_override_conflict_summary.override_lookup_misses ==
          result.parity_surface.method_lookup_override_conflict_summary.override_lookup_sites &&
      result.parity_surface.method_lookup_override_conflict_summary.override_conflicts <=
          result.parity_surface.method_lookup_override_conflict_summary.override_lookup_hits &&
      result.parity_surface.method_lookup_override_conflict_summary.deterministic;
  result.parity_surface.deterministic_property_synthesis_ivar_binding_handoff =
      result.deterministic_property_synthesis_ivar_binding_handoff &&
      result.parity_surface.property_synthesis_ivar_binding_summary.property_synthesis_sites ==
          result.parity_surface.property_synthesis_ivar_binding_property_synthesis_sites_total &&
      result.parity_surface.property_synthesis_ivar_binding_summary.property_synthesis_explicit_ivar_bindings ==
          result.parity_surface.property_synthesis_ivar_binding_explicit_ivar_bindings_total &&
      result.parity_surface.property_synthesis_ivar_binding_summary.property_synthesis_default_ivar_bindings ==
          result.parity_surface.property_synthesis_ivar_binding_default_ivar_bindings_total &&
      result.parity_surface.property_synthesis_ivar_binding_summary.ivar_binding_sites ==
          result.parity_surface.property_synthesis_ivar_binding_ivar_binding_sites_total &&
      result.parity_surface.property_synthesis_ivar_binding_summary.ivar_binding_resolved ==
          result.parity_surface.property_synthesis_ivar_binding_ivar_binding_resolved_total &&
      result.parity_surface.property_synthesis_ivar_binding_summary.ivar_binding_missing ==
          result.parity_surface.property_synthesis_ivar_binding_ivar_binding_missing_total &&
      result.parity_surface.property_synthesis_ivar_binding_summary.ivar_binding_conflicts ==
          result.parity_surface.property_synthesis_ivar_binding_ivar_binding_conflicts_total &&
      result.parity_surface.property_synthesis_ivar_binding_summary.property_synthesis_explicit_ivar_bindings +
              result.parity_surface.property_synthesis_ivar_binding_summary
                  .property_synthesis_default_ivar_bindings ==
          result.parity_surface.property_synthesis_ivar_binding_summary.property_synthesis_sites &&
      result.parity_surface.property_synthesis_ivar_binding_summary.ivar_binding_sites ==
          result.parity_surface.property_synthesis_ivar_binding_summary.property_synthesis_sites &&
      result.parity_surface.property_synthesis_ivar_binding_summary.ivar_binding_resolved +
              result.parity_surface.property_synthesis_ivar_binding_summary.ivar_binding_missing +
              result.parity_surface.property_synthesis_ivar_binding_summary.ivar_binding_conflicts ==
          result.parity_surface.property_synthesis_ivar_binding_summary.ivar_binding_sites &&
      result.parity_surface.property_synthesis_ivar_binding_summary.deterministic;
  result.parity_surface.deterministic_id_class_sel_object_pointer_type_checking_handoff =
      result.deterministic_id_class_sel_object_pointer_type_checking_handoff &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.param_type_sites ==
          result.parity_surface.id_class_sel_object_pointer_param_type_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.param_id_spelling_sites ==
          result.parity_surface.id_class_sel_object_pointer_param_id_spelling_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.param_class_spelling_sites ==
          result.parity_surface.id_class_sel_object_pointer_param_class_spelling_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.param_sel_spelling_sites ==
          result.parity_surface.id_class_sel_object_pointer_param_sel_spelling_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.param_instancetype_spelling_sites ==
          result.parity_surface.id_class_sel_object_pointer_param_instancetype_spelling_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.param_object_pointer_type_sites ==
          result.parity_surface.id_class_sel_object_pointer_param_object_pointer_type_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.return_type_sites ==
          result.parity_surface.id_class_sel_object_pointer_return_type_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.return_id_spelling_sites ==
          result.parity_surface.id_class_sel_object_pointer_return_id_spelling_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.return_class_spelling_sites ==
          result.parity_surface.id_class_sel_object_pointer_return_class_spelling_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.return_sel_spelling_sites ==
          result.parity_surface.id_class_sel_object_pointer_return_sel_spelling_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.return_instancetype_spelling_sites ==
          result.parity_surface.id_class_sel_object_pointer_return_instancetype_spelling_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.return_object_pointer_type_sites ==
          result.parity_surface.id_class_sel_object_pointer_return_object_pointer_type_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.property_type_sites ==
          result.parity_surface.id_class_sel_object_pointer_property_type_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.property_id_spelling_sites ==
          result.parity_surface.id_class_sel_object_pointer_property_id_spelling_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.property_class_spelling_sites ==
          result.parity_surface.id_class_sel_object_pointer_property_class_spelling_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.property_sel_spelling_sites ==
          result.parity_surface.id_class_sel_object_pointer_property_sel_spelling_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.property_instancetype_spelling_sites ==
          result.parity_surface.id_class_sel_object_pointer_property_instancetype_spelling_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.property_object_pointer_type_sites ==
          result.parity_surface.id_class_sel_object_pointer_property_object_pointer_type_sites_total &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.param_id_spelling_sites +
              result.parity_surface.id_class_sel_object_pointer_type_checking_summary.param_class_spelling_sites +
              result.parity_surface.id_class_sel_object_pointer_type_checking_summary.param_sel_spelling_sites +
              result.parity_surface.id_class_sel_object_pointer_type_checking_summary
                  .param_instancetype_spelling_sites +
              result.parity_surface.id_class_sel_object_pointer_type_checking_summary.param_object_pointer_type_sites <=
          result.parity_surface.id_class_sel_object_pointer_type_checking_summary.param_type_sites &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.return_id_spelling_sites +
              result.parity_surface.id_class_sel_object_pointer_type_checking_summary.return_class_spelling_sites +
              result.parity_surface.id_class_sel_object_pointer_type_checking_summary.return_sel_spelling_sites +
              result.parity_surface.id_class_sel_object_pointer_type_checking_summary
                  .return_instancetype_spelling_sites +
              result.parity_surface.id_class_sel_object_pointer_type_checking_summary
                  .return_object_pointer_type_sites <=
          result.parity_surface.id_class_sel_object_pointer_type_checking_summary.return_type_sites &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.property_id_spelling_sites +
              result.parity_surface.id_class_sel_object_pointer_type_checking_summary
                  .property_class_spelling_sites +
              result.parity_surface.id_class_sel_object_pointer_type_checking_summary.property_sel_spelling_sites +
              result.parity_surface.id_class_sel_object_pointer_type_checking_summary
                  .property_instancetype_spelling_sites +
              result.parity_surface.id_class_sel_object_pointer_type_checking_summary
                  .property_object_pointer_type_sites <=
          result.parity_surface.id_class_sel_object_pointer_type_checking_summary.property_type_sites &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.deterministic;
  result.parity_surface.deterministic_block_literal_capture_semantics_handoff =
      result.deterministic_block_literal_capture_semantics_handoff &&
      result.parity_surface.block_literal_capture_semantics_summary.block_literal_sites ==
          result.parity_surface.block_literal_capture_semantics_sites_total &&
      result.parity_surface.block_literal_capture_semantics_summary.block_parameter_entries ==
          result.parity_surface.block_literal_capture_semantics_parameter_entries_total &&
      result.parity_surface.block_literal_capture_semantics_summary.block_capture_entries ==
          result.parity_surface.block_literal_capture_semantics_capture_entries_total &&
      result.parity_surface.block_literal_capture_semantics_summary.block_body_statement_entries ==
          result.parity_surface.block_literal_capture_semantics_body_statement_entries_total &&
      result.parity_surface.block_literal_capture_semantics_summary.block_empty_capture_sites ==
          result.parity_surface.block_literal_capture_semantics_empty_capture_sites_total &&
      result.parity_surface.block_literal_capture_semantics_summary.block_nondeterministic_capture_sites ==
          result.parity_surface.block_literal_capture_semantics_nondeterministic_capture_sites_total &&
      result.parity_surface.block_literal_capture_semantics_summary.block_non_normalized_sites ==
          result.parity_surface.block_literal_capture_semantics_non_normalized_sites_total &&
      result.parity_surface.block_literal_capture_semantics_summary.contract_violation_sites ==
          result.parity_surface.block_literal_capture_semantics_contract_violation_sites_total &&
      result.parity_surface.block_literal_capture_semantics_summary.block_empty_capture_sites <=
          result.parity_surface.block_literal_capture_semantics_summary.block_literal_sites &&
      result.parity_surface.block_literal_capture_semantics_summary.block_nondeterministic_capture_sites <=
          result.parity_surface.block_literal_capture_semantics_summary.block_literal_sites &&
      result.parity_surface.block_literal_capture_semantics_summary.block_non_normalized_sites <=
          result.parity_surface.block_literal_capture_semantics_summary.block_literal_sites &&
      result.parity_surface.block_literal_capture_semantics_summary.contract_violation_sites <=
          result.parity_surface.block_literal_capture_semantics_summary.block_literal_sites &&
      result.parity_surface.block_literal_capture_semantics_summary.deterministic;
  result.parity_surface.deterministic_block_abi_invoke_trampoline_handoff =
      result.deterministic_block_abi_invoke_trampoline_handoff &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.block_literal_sites ==
          result.parity_surface.block_abi_invoke_trampoline_sites_total &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.invoke_argument_slots_total ==
          result.parity_surface.block_abi_invoke_trampoline_invoke_argument_slots_total &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.capture_word_count_total ==
          result.parity_surface.block_abi_invoke_trampoline_capture_word_count_total &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.parameter_entries_total ==
          result.parity_surface.block_abi_invoke_trampoline_parameter_entries_total &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.capture_entries_total ==
          result.parity_surface.block_abi_invoke_trampoline_capture_entries_total &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.body_statement_entries_total ==
          result.parity_surface.block_abi_invoke_trampoline_body_statement_entries_total &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.descriptor_symbolized_sites ==
          result.parity_surface.block_abi_invoke_trampoline_descriptor_symbolized_sites_total &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.invoke_trampoline_symbolized_sites ==
          result.parity_surface.block_abi_invoke_trampoline_invoke_symbolized_sites_total &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.missing_invoke_trampoline_sites ==
          result.parity_surface.block_abi_invoke_trampoline_missing_invoke_sites_total &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.non_normalized_layout_sites ==
          result.parity_surface.block_abi_invoke_trampoline_non_normalized_layout_sites_total &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.contract_violation_sites ==
          result.parity_surface.block_abi_invoke_trampoline_contract_violation_sites_total &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.descriptor_symbolized_sites <=
          result.parity_surface.block_abi_invoke_trampoline_semantics_summary.block_literal_sites &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.invoke_trampoline_symbolized_sites <=
          result.parity_surface.block_abi_invoke_trampoline_semantics_summary.block_literal_sites &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.missing_invoke_trampoline_sites <=
          result.parity_surface.block_abi_invoke_trampoline_semantics_summary.block_literal_sites &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.non_normalized_layout_sites <=
          result.parity_surface.block_abi_invoke_trampoline_semantics_summary.block_literal_sites &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.contract_violation_sites <=
          result.parity_surface.block_abi_invoke_trampoline_semantics_summary.block_literal_sites &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.invoke_trampoline_symbolized_sites +
              result.parity_surface.block_abi_invoke_trampoline_semantics_summary
                  .missing_invoke_trampoline_sites ==
          result.parity_surface.block_abi_invoke_trampoline_semantics_summary.block_literal_sites &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.invoke_argument_slots_total ==
          result.parity_surface.block_abi_invoke_trampoline_semantics_summary.parameter_entries_total &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.capture_word_count_total ==
          result.parity_surface.block_abi_invoke_trampoline_semantics_summary.capture_entries_total &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.deterministic;
  result.parity_surface.deterministic_block_storage_escape_handoff =
      result.deterministic_block_storage_escape_handoff &&
      result.parity_surface.block_storage_escape_semantics_summary.block_literal_sites ==
          result.parity_surface.block_storage_escape_sites_total &&
      result.parity_surface.block_storage_escape_semantics_summary.mutable_capture_count_total ==
          result.parity_surface.block_storage_escape_mutable_capture_count_total &&
      result.parity_surface.block_storage_escape_semantics_summary.byref_slot_count_total ==
          result.parity_surface.block_storage_escape_byref_slot_count_total &&
      result.parity_surface.block_storage_escape_semantics_summary.parameter_entries_total ==
          result.parity_surface.block_storage_escape_parameter_entries_total &&
      result.parity_surface.block_storage_escape_semantics_summary.capture_entries_total ==
          result.parity_surface.block_storage_escape_capture_entries_total &&
      result.parity_surface.block_storage_escape_semantics_summary.body_statement_entries_total ==
          result.parity_surface.block_storage_escape_body_statement_entries_total &&
      result.parity_surface.block_storage_escape_semantics_summary.requires_byref_cells_sites ==
          result.parity_surface.block_storage_escape_requires_byref_cells_sites_total &&
      result.parity_surface.block_storage_escape_semantics_summary.escape_analysis_enabled_sites ==
          result.parity_surface.block_storage_escape_escape_analysis_enabled_sites_total &&
      result.parity_surface.block_storage_escape_semantics_summary.escape_to_heap_sites ==
          result.parity_surface.block_storage_escape_escape_to_heap_sites_total &&
      result.parity_surface.block_storage_escape_semantics_summary.escape_profile_normalized_sites ==
          result.parity_surface.block_storage_escape_escape_profile_normalized_sites_total &&
      result.parity_surface.block_storage_escape_semantics_summary.byref_layout_symbolized_sites ==
          result.parity_surface.block_storage_escape_byref_layout_symbolized_sites_total &&
      result.parity_surface.block_storage_escape_semantics_summary.contract_violation_sites ==
          result.parity_surface.block_storage_escape_contract_violation_sites_total &&
      result.parity_surface.block_storage_escape_semantics_summary.requires_byref_cells_sites <=
          result.parity_surface.block_storage_escape_semantics_summary.block_literal_sites &&
      result.parity_surface.block_storage_escape_semantics_summary.escape_analysis_enabled_sites <=
          result.parity_surface.block_storage_escape_semantics_summary.block_literal_sites &&
      result.parity_surface.block_storage_escape_semantics_summary.escape_to_heap_sites <=
          result.parity_surface.block_storage_escape_semantics_summary.block_literal_sites &&
      result.parity_surface.block_storage_escape_semantics_summary.escape_profile_normalized_sites <=
          result.parity_surface.block_storage_escape_semantics_summary.block_literal_sites &&
      result.parity_surface.block_storage_escape_semantics_summary.byref_layout_symbolized_sites <=
          result.parity_surface.block_storage_escape_semantics_summary.block_literal_sites &&
      result.parity_surface.block_storage_escape_semantics_summary.contract_violation_sites <=
          result.parity_surface.block_storage_escape_semantics_summary.block_literal_sites &&
      result.parity_surface.block_storage_escape_semantics_summary.mutable_capture_count_total ==
          result.parity_surface.block_storage_escape_semantics_summary.capture_entries_total &&
      result.parity_surface.block_storage_escape_semantics_summary.byref_slot_count_total ==
          result.parity_surface.block_storage_escape_semantics_summary.capture_entries_total &&
      result.parity_surface.block_storage_escape_semantics_summary.escape_analysis_enabled_sites ==
          result.parity_surface.block_storage_escape_semantics_summary.block_literal_sites &&
      result.parity_surface.block_storage_escape_semantics_summary.requires_byref_cells_sites ==
          result.parity_surface.block_storage_escape_semantics_summary.escape_to_heap_sites &&
      result.parity_surface.block_storage_escape_semantics_summary.deterministic;
  result.parity_surface.deterministic_block_copy_dispose_handoff =
      result.deterministic_block_copy_dispose_handoff &&
      result.parity_surface.block_copy_dispose_semantics_summary.block_literal_sites ==
          result.parity_surface.block_copy_dispose_sites_total &&
      result.parity_surface.block_copy_dispose_semantics_summary.mutable_capture_count_total ==
          result.parity_surface.block_copy_dispose_mutable_capture_count_total &&
      result.parity_surface.block_copy_dispose_semantics_summary.byref_slot_count_total ==
          result.parity_surface.block_copy_dispose_byref_slot_count_total &&
      result.parity_surface.block_copy_dispose_semantics_summary.parameter_entries_total ==
          result.parity_surface.block_copy_dispose_parameter_entries_total &&
      result.parity_surface.block_copy_dispose_semantics_summary.capture_entries_total ==
          result.parity_surface.block_copy_dispose_capture_entries_total &&
      result.parity_surface.block_copy_dispose_semantics_summary.body_statement_entries_total ==
          result.parity_surface.block_copy_dispose_body_statement_entries_total &&
      result.parity_surface.block_copy_dispose_semantics_summary.copy_helper_required_sites ==
          result.parity_surface.block_copy_dispose_copy_helper_required_sites_total &&
      result.parity_surface.block_copy_dispose_semantics_summary.dispose_helper_required_sites ==
          result.parity_surface.block_copy_dispose_dispose_helper_required_sites_total &&
      result.parity_surface.block_copy_dispose_semantics_summary.profile_normalized_sites ==
          result.parity_surface.block_copy_dispose_profile_normalized_sites_total &&
      result.parity_surface.block_copy_dispose_semantics_summary.copy_helper_symbolized_sites ==
          result.parity_surface.block_copy_dispose_copy_helper_symbolized_sites_total &&
      result.parity_surface.block_copy_dispose_semantics_summary.dispose_helper_symbolized_sites ==
          result.parity_surface.block_copy_dispose_dispose_helper_symbolized_sites_total &&
      result.parity_surface.block_copy_dispose_semantics_summary.contract_violation_sites ==
          result.parity_surface.block_copy_dispose_contract_violation_sites_total &&
      result.parity_surface.block_copy_dispose_semantics_summary.copy_helper_required_sites <=
          result.parity_surface.block_copy_dispose_semantics_summary.block_literal_sites &&
      result.parity_surface.block_copy_dispose_semantics_summary.dispose_helper_required_sites <=
          result.parity_surface.block_copy_dispose_semantics_summary.block_literal_sites &&
      result.parity_surface.block_copy_dispose_semantics_summary.profile_normalized_sites <=
          result.parity_surface.block_copy_dispose_semantics_summary.block_literal_sites &&
      result.parity_surface.block_copy_dispose_semantics_summary.copy_helper_symbolized_sites <=
          result.parity_surface.block_copy_dispose_semantics_summary.block_literal_sites &&
      result.parity_surface.block_copy_dispose_semantics_summary.dispose_helper_symbolized_sites <=
          result.parity_surface.block_copy_dispose_semantics_summary.block_literal_sites &&
      result.parity_surface.block_copy_dispose_semantics_summary.contract_violation_sites <=
          result.parity_surface.block_copy_dispose_semantics_summary.block_literal_sites &&
      result.parity_surface.block_copy_dispose_semantics_summary.mutable_capture_count_total ==
          result.parity_surface.block_copy_dispose_semantics_summary.capture_entries_total &&
      result.parity_surface.block_copy_dispose_semantics_summary.byref_slot_count_total ==
          result.parity_surface.block_copy_dispose_semantics_summary.capture_entries_total &&
      result.parity_surface.block_copy_dispose_semantics_summary.copy_helper_required_sites ==
          result.parity_surface.block_copy_dispose_semantics_summary.dispose_helper_required_sites &&
      result.parity_surface.block_copy_dispose_semantics_summary.deterministic;
  result.parity_surface.deterministic_message_send_selector_lowering_handoff =
      result.deterministic_message_send_selector_lowering_handoff &&
      result.parity_surface.message_send_selector_lowering_summary.message_send_sites ==
          result.parity_surface.message_send_selector_lowering_sites_total &&
      result.parity_surface.message_send_selector_lowering_summary.unary_form_sites ==
          result.parity_surface.message_send_selector_lowering_unary_form_sites_total &&
      result.parity_surface.message_send_selector_lowering_summary.keyword_form_sites ==
          result.parity_surface.message_send_selector_lowering_keyword_form_sites_total &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_symbol_sites ==
          result.parity_surface.message_send_selector_lowering_symbol_sites_total &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_piece_entries ==
          result.parity_surface.message_send_selector_lowering_piece_entries_total &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_argument_piece_entries ==
          result.parity_surface.message_send_selector_lowering_argument_piece_entries_total &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_normalized_sites ==
          result.parity_surface.message_send_selector_lowering_normalized_sites_total &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_form_mismatch_sites ==
          result.parity_surface.message_send_selector_lowering_form_mismatch_sites_total &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_arity_mismatch_sites ==
          result.parity_surface.message_send_selector_lowering_arity_mismatch_sites_total &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_symbol_mismatch_sites ==
          result.parity_surface.message_send_selector_lowering_symbol_mismatch_sites_total &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_missing_symbol_sites ==
          result.parity_surface.message_send_selector_lowering_missing_symbol_sites_total &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_contract_violation_sites ==
          result.parity_surface.message_send_selector_lowering_contract_violation_sites_total &&
      result.parity_surface.message_send_selector_lowering_summary.unary_form_sites +
              result.parity_surface.message_send_selector_lowering_summary.keyword_form_sites ==
          result.parity_surface.message_send_selector_lowering_summary.message_send_sites &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_symbol_sites <=
          result.parity_surface.message_send_selector_lowering_summary.message_send_sites &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_argument_piece_entries <=
          result.parity_surface.message_send_selector_lowering_summary.selector_lowering_piece_entries &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_normalized_sites <=
          result.parity_surface.message_send_selector_lowering_summary.selector_lowering_symbol_sites &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_form_mismatch_sites <=
          result.parity_surface.message_send_selector_lowering_summary.message_send_sites &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_arity_mismatch_sites <=
          result.parity_surface.message_send_selector_lowering_summary.message_send_sites &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_symbol_mismatch_sites <=
          result.parity_surface.message_send_selector_lowering_summary.message_send_sites &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_missing_symbol_sites <=
          result.parity_surface.message_send_selector_lowering_summary.message_send_sites &&
      result.parity_surface.message_send_selector_lowering_summary.selector_lowering_contract_violation_sites <=
          result.parity_surface.message_send_selector_lowering_summary.message_send_sites &&
      result.parity_surface.message_send_selector_lowering_summary.deterministic;
  result.parity_surface.deterministic_dispatch_abi_marshalling_handoff =
      result.deterministic_dispatch_abi_marshalling_handoff &&
      result.parity_surface.dispatch_abi_marshalling_summary.message_send_sites ==
          result.parity_surface.dispatch_abi_marshalling_sites_total &&
      result.parity_surface.dispatch_abi_marshalling_summary.receiver_slots ==
          result.parity_surface.dispatch_abi_marshalling_receiver_slots_total &&
      result.parity_surface.dispatch_abi_marshalling_summary.selector_symbol_slots ==
          result.parity_surface.dispatch_abi_marshalling_selector_symbol_slots_total &&
      result.parity_surface.dispatch_abi_marshalling_summary.argument_slots ==
          result.parity_surface.dispatch_abi_marshalling_argument_slots_total &&
      result.parity_surface.dispatch_abi_marshalling_summary.keyword_argument_slots ==
          result.parity_surface.dispatch_abi_marshalling_keyword_argument_slots_total &&
      result.parity_surface.dispatch_abi_marshalling_summary.unary_argument_slots ==
          result.parity_surface.dispatch_abi_marshalling_unary_argument_slots_total &&
      result.parity_surface.dispatch_abi_marshalling_summary.arity_mismatch_sites ==
          result.parity_surface.dispatch_abi_marshalling_arity_mismatch_sites_total &&
      result.parity_surface.dispatch_abi_marshalling_summary.missing_selector_symbol_sites ==
          result.parity_surface.dispatch_abi_marshalling_missing_selector_symbol_sites_total &&
      result.parity_surface.dispatch_abi_marshalling_summary.contract_violation_sites ==
          result.parity_surface.dispatch_abi_marshalling_contract_violation_sites_total &&
      result.parity_surface.dispatch_abi_marshalling_summary.receiver_slots ==
          result.parity_surface.dispatch_abi_marshalling_summary.message_send_sites &&
      result.parity_surface.dispatch_abi_marshalling_summary.selector_symbol_slots +
              result.parity_surface.dispatch_abi_marshalling_summary.missing_selector_symbol_sites ==
          result.parity_surface.dispatch_abi_marshalling_summary.message_send_sites &&
      result.parity_surface.dispatch_abi_marshalling_summary.keyword_argument_slots +
              result.parity_surface.dispatch_abi_marshalling_summary.unary_argument_slots ==
          result.parity_surface.dispatch_abi_marshalling_summary.argument_slots &&
      result.parity_surface.dispatch_abi_marshalling_summary.keyword_argument_slots <=
          result.parity_surface.dispatch_abi_marshalling_summary.argument_slots &&
      result.parity_surface.dispatch_abi_marshalling_summary.unary_argument_slots <=
          result.parity_surface.dispatch_abi_marshalling_summary.argument_slots &&
      result.parity_surface.dispatch_abi_marshalling_summary.selector_symbol_slots <=
          result.parity_surface.dispatch_abi_marshalling_summary.message_send_sites &&
      result.parity_surface.dispatch_abi_marshalling_summary.missing_selector_symbol_sites <=
          result.parity_surface.dispatch_abi_marshalling_summary.message_send_sites &&
      result.parity_surface.dispatch_abi_marshalling_summary.arity_mismatch_sites <=
          result.parity_surface.dispatch_abi_marshalling_summary.message_send_sites &&
      result.parity_surface.dispatch_abi_marshalling_summary.contract_violation_sites <=
          result.parity_surface.dispatch_abi_marshalling_summary.message_send_sites &&
      result.parity_surface.dispatch_abi_marshalling_summary.deterministic;
  result.parity_surface.deterministic_nil_receiver_semantics_foldability_handoff =
      result.deterministic_nil_receiver_semantics_foldability_handoff &&
      result.parity_surface.nil_receiver_semantics_foldability_summary.message_send_sites ==
          result.parity_surface.nil_receiver_semantics_foldability_sites_total &&
      result.parity_surface.nil_receiver_semantics_foldability_summary.receiver_nil_literal_sites ==
          result.parity_surface.nil_receiver_semantics_foldability_receiver_nil_literal_sites_total &&
      result.parity_surface.nil_receiver_semantics_foldability_summary.nil_receiver_semantics_enabled_sites ==
          result.parity_surface.nil_receiver_semantics_foldability_enabled_sites_total &&
      result.parity_surface.nil_receiver_semantics_foldability_summary.nil_receiver_foldable_sites ==
          result.parity_surface.nil_receiver_semantics_foldability_foldable_sites_total &&
      result.parity_surface.nil_receiver_semantics_foldability_summary.nil_receiver_runtime_dispatch_required_sites ==
          result.parity_surface.nil_receiver_semantics_foldability_runtime_dispatch_required_sites_total &&
      result.parity_surface.nil_receiver_semantics_foldability_summary.non_nil_receiver_sites ==
          result.parity_surface.nil_receiver_semantics_foldability_non_nil_receiver_sites_total &&
      result.parity_surface.nil_receiver_semantics_foldability_summary.contract_violation_sites ==
          result.parity_surface.nil_receiver_semantics_foldability_contract_violation_sites_total &&
      result.parity_surface.nil_receiver_semantics_foldability_summary.receiver_nil_literal_sites ==
          result.parity_surface.nil_receiver_semantics_foldability_summary.nil_receiver_semantics_enabled_sites &&
      result.parity_surface.nil_receiver_semantics_foldability_summary.nil_receiver_foldable_sites <=
          result.parity_surface.nil_receiver_semantics_foldability_summary.nil_receiver_semantics_enabled_sites &&
      result.parity_surface.nil_receiver_semantics_foldability_summary.nil_receiver_runtime_dispatch_required_sites +
              result.parity_surface.nil_receiver_semantics_foldability_summary.nil_receiver_foldable_sites ==
          result.parity_surface.nil_receiver_semantics_foldability_summary.message_send_sites &&
      result.parity_surface.nil_receiver_semantics_foldability_summary.nil_receiver_semantics_enabled_sites +
              result.parity_surface.nil_receiver_semantics_foldability_summary.non_nil_receiver_sites ==
          result.parity_surface.nil_receiver_semantics_foldability_summary.message_send_sites &&
      result.parity_surface.nil_receiver_semantics_foldability_summary.contract_violation_sites <=
          result.parity_surface.nil_receiver_semantics_foldability_summary.message_send_sites &&
      result.parity_surface.nil_receiver_semantics_foldability_summary.deterministic;
  result.parity_surface.deterministic_super_dispatch_method_family_handoff =
      result.deterministic_super_dispatch_method_family_handoff &&
      result.parity_surface.super_dispatch_method_family_summary.message_send_sites ==
          result.parity_surface.super_dispatch_method_family_sites_total &&
      result.parity_surface.super_dispatch_method_family_summary.receiver_super_identifier_sites ==
          result.parity_surface.super_dispatch_method_family_receiver_super_identifier_sites_total &&
      result.parity_surface.super_dispatch_method_family_summary.super_dispatch_enabled_sites ==
          result.parity_surface.super_dispatch_method_family_enabled_sites_total &&
      result.parity_surface.super_dispatch_method_family_summary.super_dispatch_requires_class_context_sites ==
          result.parity_surface.super_dispatch_method_family_requires_class_context_sites_total &&
      result.parity_surface.super_dispatch_method_family_summary.method_family_init_sites ==
          result.parity_surface.super_dispatch_method_family_init_sites_total &&
      result.parity_surface.super_dispatch_method_family_summary.method_family_copy_sites ==
          result.parity_surface.super_dispatch_method_family_copy_sites_total &&
      result.parity_surface.super_dispatch_method_family_summary.method_family_mutable_copy_sites ==
          result.parity_surface.super_dispatch_method_family_mutable_copy_sites_total &&
      result.parity_surface.super_dispatch_method_family_summary.method_family_new_sites ==
          result.parity_surface.super_dispatch_method_family_new_sites_total &&
      result.parity_surface.super_dispatch_method_family_summary.method_family_none_sites ==
          result.parity_surface.super_dispatch_method_family_none_sites_total &&
      result.parity_surface.super_dispatch_method_family_summary.method_family_returns_retained_result_sites ==
          result.parity_surface.super_dispatch_method_family_returns_retained_result_sites_total &&
      result.parity_surface.super_dispatch_method_family_summary.method_family_returns_related_result_sites ==
          result.parity_surface.super_dispatch_method_family_returns_related_result_sites_total &&
      result.parity_surface.super_dispatch_method_family_summary.contract_violation_sites ==
          result.parity_surface.super_dispatch_method_family_contract_violation_sites_total &&
      result.parity_surface.super_dispatch_method_family_summary.receiver_super_identifier_sites ==
          result.parity_surface.super_dispatch_method_family_summary.super_dispatch_enabled_sites &&
      result.parity_surface.super_dispatch_method_family_summary.super_dispatch_requires_class_context_sites ==
          result.parity_surface.super_dispatch_method_family_summary.super_dispatch_enabled_sites &&
      result.parity_surface.super_dispatch_method_family_summary.method_family_init_sites +
              result.parity_surface.super_dispatch_method_family_summary.method_family_copy_sites +
              result.parity_surface.super_dispatch_method_family_summary.method_family_mutable_copy_sites +
              result.parity_surface.super_dispatch_method_family_summary.method_family_new_sites +
              result.parity_surface.super_dispatch_method_family_summary.method_family_none_sites ==
          result.parity_surface.super_dispatch_method_family_summary.message_send_sites &&
      result.parity_surface.super_dispatch_method_family_summary.method_family_returns_related_result_sites <=
          result.parity_surface.super_dispatch_method_family_summary.method_family_init_sites &&
      result.parity_surface.super_dispatch_method_family_summary.method_family_returns_retained_result_sites <=
          result.parity_surface.super_dispatch_method_family_summary.message_send_sites &&
      result.parity_surface.super_dispatch_method_family_summary.contract_violation_sites <=
          result.parity_surface.super_dispatch_method_family_summary.message_send_sites &&
      result.parity_surface.super_dispatch_method_family_summary.deterministic;
  result.parity_surface.deterministic_runtime_shim_host_link_handoff =
      result.deterministic_runtime_shim_host_link_handoff &&
      result.parity_surface.runtime_shim_host_link_summary.message_send_sites ==
          result.parity_surface.runtime_shim_host_link_message_send_sites_total &&
      result.parity_surface.runtime_shim_host_link_summary.runtime_shim_required_sites ==
          result.parity_surface.runtime_shim_host_link_required_sites_total &&
      result.parity_surface.runtime_shim_host_link_summary.runtime_shim_elided_sites ==
          result.parity_surface.runtime_shim_host_link_elided_sites_total &&
      result.parity_surface.runtime_shim_host_link_summary.runtime_dispatch_arg_slots ==
          result.parity_surface.runtime_shim_host_link_runtime_dispatch_arg_slots_total &&
      result.parity_surface.runtime_shim_host_link_summary.runtime_dispatch_declaration_parameter_count ==
          result.parity_surface.runtime_shim_host_link_runtime_dispatch_declaration_parameter_count_total &&
      result.parity_surface.runtime_shim_host_link_summary.contract_violation_sites ==
          result.parity_surface.runtime_shim_host_link_contract_violation_sites_total &&
      result.parity_surface.runtime_shim_host_link_summary.runtime_dispatch_symbol ==
          result.parity_surface.runtime_shim_host_link_runtime_dispatch_symbol &&
      result.parity_surface.runtime_shim_host_link_summary.default_runtime_dispatch_symbol_binding ==
          result.parity_surface.runtime_shim_host_link_default_runtime_dispatch_symbol_binding &&
      result.parity_surface.runtime_shim_host_link_summary.runtime_shim_required_sites +
              result.parity_surface.runtime_shim_host_link_summary.runtime_shim_elided_sites ==
          result.parity_surface.runtime_shim_host_link_summary.message_send_sites &&
      result.parity_surface.runtime_shim_host_link_summary.contract_violation_sites <=
          result.parity_surface.runtime_shim_host_link_summary.message_send_sites &&
      (result.parity_surface.runtime_shim_host_link_summary.message_send_sites == 0 ||
       result.parity_surface.runtime_shim_host_link_summary.runtime_dispatch_declaration_parameter_count ==
           result.parity_surface.runtime_shim_host_link_summary.runtime_dispatch_arg_slots + 2u) &&
      (result.parity_surface.runtime_shim_host_link_summary.default_runtime_dispatch_symbol_binding ==
       (result.parity_surface.runtime_shim_host_link_summary.runtime_dispatch_symbol ==
        kObjc3RuntimeShimHostLinkDefaultDispatchSymbol)) &&
      result.parity_surface.runtime_shim_host_link_summary.deterministic;
  result.parity_surface.deterministic_retain_release_operation_handoff =
      result.deterministic_retain_release_operation_handoff &&
      result.parity_surface.retain_release_operation_summary.ownership_qualified_sites ==
          result.parity_surface.retain_release_operation_ownership_qualified_sites_total &&
      result.parity_surface.retain_release_operation_summary.retain_insertion_sites ==
          result.parity_surface.retain_release_operation_retain_insertion_sites_total &&
      result.parity_surface.retain_release_operation_summary.release_insertion_sites ==
          result.parity_surface.retain_release_operation_release_insertion_sites_total &&
      result.parity_surface.retain_release_operation_summary.autorelease_insertion_sites ==
          result.parity_surface.retain_release_operation_autorelease_insertion_sites_total &&
      result.parity_surface.retain_release_operation_summary.contract_violation_sites ==
          result.parity_surface.retain_release_operation_contract_violation_sites_total &&
      result.parity_surface.retain_release_operation_summary.retain_insertion_sites <=
          result.parity_surface.retain_release_operation_summary.ownership_qualified_sites +
              result.parity_surface.retain_release_operation_summary.contract_violation_sites &&
      result.parity_surface.retain_release_operation_summary.release_insertion_sites <=
          result.parity_surface.retain_release_operation_summary.ownership_qualified_sites +
              result.parity_surface.retain_release_operation_summary.contract_violation_sites &&
      result.parity_surface.retain_release_operation_summary.autorelease_insertion_sites <=
          result.parity_surface.retain_release_operation_summary.ownership_qualified_sites +
              result.parity_surface.retain_release_operation_summary.contract_violation_sites &&
      result.parity_surface.retain_release_operation_summary.deterministic;
  result.parity_surface.deterministic_weak_unowned_semantics_handoff =
      result.deterministic_weak_unowned_semantics_handoff &&
      result.parity_surface.weak_unowned_semantics_summary.ownership_candidate_sites ==
          result.parity_surface.weak_unowned_semantics_ownership_candidate_sites_total &&
      result.parity_surface.weak_unowned_semantics_summary.weak_reference_sites ==
          result.parity_surface.weak_unowned_semantics_weak_reference_sites_total &&
      result.parity_surface.weak_unowned_semantics_summary.unowned_reference_sites ==
          result.parity_surface.weak_unowned_semantics_unowned_reference_sites_total &&
      result.parity_surface.weak_unowned_semantics_summary.unowned_safe_reference_sites ==
          result.parity_surface.weak_unowned_semantics_unowned_safe_reference_sites_total &&
      result.parity_surface.weak_unowned_semantics_summary.weak_unowned_conflict_sites ==
          result.parity_surface.weak_unowned_semantics_conflict_sites_total &&
      result.parity_surface.weak_unowned_semantics_summary.contract_violation_sites ==
          result.parity_surface.weak_unowned_semantics_contract_violation_sites_total &&
      result.parity_surface.weak_unowned_semantics_summary.unowned_safe_reference_sites <=
          result.parity_surface.weak_unowned_semantics_summary.unowned_reference_sites &&
      result.parity_surface.weak_unowned_semantics_summary.weak_unowned_conflict_sites <=
          result.parity_surface.weak_unowned_semantics_summary.ownership_candidate_sites &&
      result.parity_surface.weak_unowned_semantics_summary.contract_violation_sites <=
          result.parity_surface.weak_unowned_semantics_summary.ownership_candidate_sites +
              result.parity_surface.weak_unowned_semantics_summary.weak_unowned_conflict_sites &&
      result.parity_surface.weak_unowned_semantics_summary.deterministic;
  result.parity_surface.deterministic_arc_diagnostics_fixit_handoff =
      result.deterministic_arc_diagnostics_fixit_handoff &&
      result.parity_surface.arc_diagnostics_fixit_summary.ownership_arc_diagnostic_candidate_sites ==
          result.parity_surface.ownership_arc_diagnostic_candidate_sites_total &&
      result.parity_surface.arc_diagnostics_fixit_summary.ownership_arc_fixit_available_sites ==
          result.parity_surface.ownership_arc_fixit_available_sites_total &&
      result.parity_surface.arc_diagnostics_fixit_summary.ownership_arc_profiled_sites ==
          result.parity_surface.ownership_arc_profiled_sites_total &&
      result.parity_surface.arc_diagnostics_fixit_summary.ownership_arc_weak_unowned_conflict_diagnostic_sites ==
          result.parity_surface.ownership_arc_weak_unowned_conflict_diagnostic_sites_total &&
      result.parity_surface.arc_diagnostics_fixit_summary.ownership_arc_empty_fixit_hint_sites ==
          result.parity_surface.ownership_arc_empty_fixit_hint_sites_total &&
      result.parity_surface.arc_diagnostics_fixit_summary.contract_violation_sites ==
          result.parity_surface.ownership_arc_contract_violation_sites_total &&
      result.parity_surface.arc_diagnostics_fixit_summary.ownership_arc_fixit_available_sites <=
          result.parity_surface.arc_diagnostics_fixit_summary.ownership_arc_diagnostic_candidate_sites +
              result.parity_surface.arc_diagnostics_fixit_summary.contract_violation_sites &&
      result.parity_surface.arc_diagnostics_fixit_summary.ownership_arc_profiled_sites <=
          result.parity_surface.arc_diagnostics_fixit_summary.ownership_arc_diagnostic_candidate_sites +
              result.parity_surface.arc_diagnostics_fixit_summary.contract_violation_sites &&
      result.parity_surface.arc_diagnostics_fixit_summary.ownership_arc_weak_unowned_conflict_diagnostic_sites <=
          result.parity_surface.arc_diagnostics_fixit_summary.ownership_arc_diagnostic_candidate_sites +
              result.parity_surface.arc_diagnostics_fixit_summary.contract_violation_sites &&
      result.parity_surface.arc_diagnostics_fixit_summary.ownership_arc_empty_fixit_hint_sites <=
          result.parity_surface.arc_diagnostics_fixit_summary.ownership_arc_fixit_available_sites +
              result.parity_surface.arc_diagnostics_fixit_summary.contract_violation_sites &&
      result.parity_surface.arc_diagnostics_fixit_summary.deterministic;
  result.parity_surface.deterministic_autoreleasepool_scope_handoff =
      result.deterministic_autoreleasepool_scope_handoff &&
      result.parity_surface.autoreleasepool_scope_summary.scope_sites ==
          result.parity_surface.autoreleasepool_scope_sites_total &&
      result.parity_surface.autoreleasepool_scope_summary.scope_symbolized_sites ==
          result.parity_surface.autoreleasepool_scope_symbolized_sites_total &&
      result.parity_surface.autoreleasepool_scope_summary.contract_violation_sites ==
          result.parity_surface.autoreleasepool_scope_contract_violation_sites_total &&
      result.parity_surface.autoreleasepool_scope_summary.max_scope_depth ==
          result.parity_surface.autoreleasepool_scope_max_depth_total &&
      result.parity_surface.autoreleasepool_scope_summary.scope_symbolized_sites <=
          result.parity_surface.autoreleasepool_scope_summary.scope_sites &&
      result.parity_surface.autoreleasepool_scope_summary.contract_violation_sites <=
          result.parity_surface.autoreleasepool_scope_summary.scope_sites &&
      (result.parity_surface.autoreleasepool_scope_summary.scope_sites > 0u ||
       result.parity_surface.autoreleasepool_scope_summary.max_scope_depth == 0u) &&
      result.parity_surface.autoreleasepool_scope_summary.max_scope_depth <=
          static_cast<unsigned>(result.parity_surface.autoreleasepool_scope_summary.scope_sites) &&
      result.parity_surface.autoreleasepool_scope_summary.deterministic;
  result.parity_surface.atomic_memory_order_mapping = result.atomic_memory_order_mapping;
  result.parity_surface.deterministic_atomic_memory_order_mapping = result.deterministic_atomic_memory_order_mapping;
  result.parity_surface.vector_type_lowering = result.vector_type_lowering;
  result.parity_surface.deterministic_vector_type_lowering = result.deterministic_vector_type_lowering;
  result.parity_surface.ready =
      result.executed && result.parity_surface.diagnostics_after_pass_monotonic &&
      result.parity_surface.deterministic_semantic_diagnostics &&
      result.parity_surface.deterministic_type_metadata_handoff &&
      result.parity_surface.deterministic_atomic_memory_order_mapping &&
      result.parity_surface.deterministic_vector_type_lowering &&
      result.parity_surface.atomic_memory_order_mapping.deterministic &&
      result.parity_surface.vector_type_lowering.deterministic &&
      result.parity_surface.globals_total == result.parity_surface.type_metadata_global_entries &&
      result.parity_surface.functions_total == result.parity_surface.type_metadata_function_entries &&
      result.parity_surface.interfaces_total == result.parity_surface.type_metadata_interface_entries &&
      result.parity_surface.implementations_total == result.parity_surface.type_metadata_implementation_entries &&
      result.parity_surface.interface_implementation_summary.deterministic &&
      result.parity_surface.deterministic_interface_implementation_handoff &&
      result.parity_surface.protocol_category_composition_summary.deterministic &&
      result.parity_surface.deterministic_protocol_category_composition_handoff &&
      result.parity_surface.class_protocol_category_linking_summary.deterministic &&
      result.parity_surface.deterministic_class_protocol_category_linking_handoff &&
      result.parity_surface.selector_normalization_summary.deterministic &&
      result.parity_surface.deterministic_selector_normalization_handoff &&
      result.parity_surface.property_attribute_summary.deterministic &&
      result.parity_surface.deterministic_property_attribute_handoff &&
      result.parity_surface.type_annotation_surface_summary.deterministic &&
      result.parity_surface.deterministic_type_annotation_surface_handoff &&
      result.parity_surface.symbol_graph_scope_resolution_summary.deterministic &&
      result.parity_surface.deterministic_symbol_graph_scope_resolution_handoff &&
      result.parity_surface.method_lookup_override_conflict_summary.deterministic &&
      result.parity_surface.deterministic_method_lookup_override_conflict_handoff &&
      result.parity_surface.property_synthesis_ivar_binding_summary.deterministic &&
      result.parity_surface.deterministic_property_synthesis_ivar_binding_handoff &&
      result.parity_surface.id_class_sel_object_pointer_type_checking_summary.deterministic &&
      result.parity_surface.deterministic_id_class_sel_object_pointer_type_checking_handoff &&
      result.parity_surface.block_literal_capture_semantics_summary.deterministic &&
      result.parity_surface.deterministic_block_literal_capture_semantics_handoff &&
      result.parity_surface.block_abi_invoke_trampoline_semantics_summary.deterministic &&
      result.parity_surface.deterministic_block_abi_invoke_trampoline_handoff &&
      result.parity_surface.block_storage_escape_semantics_summary.deterministic &&
      result.parity_surface.deterministic_block_storage_escape_handoff &&
      result.parity_surface.block_copy_dispose_semantics_summary.deterministic &&
      result.parity_surface.deterministic_block_copy_dispose_handoff &&
      result.parity_surface.message_send_selector_lowering_summary.deterministic &&
      result.parity_surface.deterministic_message_send_selector_lowering_handoff &&
      result.parity_surface.dispatch_abi_marshalling_summary.deterministic &&
      result.parity_surface.deterministic_dispatch_abi_marshalling_handoff &&
      result.parity_surface.nil_receiver_semantics_foldability_summary.deterministic &&
      result.parity_surface.deterministic_nil_receiver_semantics_foldability_handoff &&
      result.parity_surface.super_dispatch_method_family_summary.deterministic &&
      result.parity_surface.deterministic_super_dispatch_method_family_handoff &&
      result.parity_surface.runtime_shim_host_link_summary.deterministic &&
      result.parity_surface.deterministic_runtime_shim_host_link_handoff &&
      result.parity_surface.retain_release_operation_summary.deterministic &&
      result.parity_surface.deterministic_retain_release_operation_handoff &&
      result.parity_surface.weak_unowned_semantics_summary.deterministic &&
      result.parity_surface.deterministic_weak_unowned_semantics_handoff &&
      result.parity_surface.arc_diagnostics_fixit_summary.deterministic &&
      result.parity_surface.deterministic_arc_diagnostics_fixit_handoff &&
      result.parity_surface.autoreleasepool_scope_summary.deterministic &&
      result.parity_surface.deterministic_autoreleasepool_scope_handoff;
  return result;
}
