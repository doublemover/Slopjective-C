#include "artifacts/objc3_frontend_ownership_semantic_artifacts.h"

#include <algorithm>
#include <string>
#include <vector>

#include "artifacts/objc3_frontend_artifact_semantic_contract_records.h"
#include "ast/objc3_ast_core.h"
#include "ast/objc3_ast_declarations.h"
#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend {
Objc3OwnershipQualifierLoweringContract BuildOwnershipQualifierLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3OwnershipQualifierLoweringContract contract;
  contract.ownership_qualifier_sites =
      sema_parity_surface.type_annotation_ownership_qualifier_sites_total;
  contract.invalid_ownership_qualifier_sites =
      sema_parity_surface.type_annotation_invalid_ownership_qualifier_sites_total;
  contract.object_pointer_type_annotation_sites =
      sema_parity_surface.type_annotation_object_pointer_type_sites_total;
  contract.deterministic =
      sema_parity_surface.type_annotation_surface_summary.deterministic &&
      sema_parity_surface.deterministic_type_annotation_surface_handoff;
  return contract;
}

Objc3RetainReleaseOperationLoweringContract
BuildRetainReleaseOperationLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3RetainReleaseOperationLoweringContract contract;
  contract.ownership_qualified_sites =
      sema_parity_surface.retain_release_operation_ownership_qualified_sites_total;
  contract.retain_insertion_sites =
      sema_parity_surface.retain_release_operation_retain_insertion_sites_total;
  contract.release_insertion_sites =
      sema_parity_surface.retain_release_operation_release_insertion_sites_total;
  contract.autorelease_insertion_sites =
      sema_parity_surface
          .retain_release_operation_autorelease_insertion_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface.retain_release_operation_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.retain_release_operation_summary.deterministic &&
      sema_parity_surface.deterministic_retain_release_operation_handoff;
  return contract;
}

Objc3AutoreleasePoolScopeLoweringContract
BuildAutoreleasePoolScopeLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3AutoreleasePoolScopeLoweringContract contract;
  contract.scope_sites = sema_parity_surface.autoreleasepool_scope_sites_total;
  contract.scope_symbolized_sites =
      sema_parity_surface.autoreleasepool_scope_symbolized_sites_total;
  contract.max_scope_depth =
      sema_parity_surface.autoreleasepool_scope_max_depth_total;
  contract.scope_entry_transition_sites =
      sema_parity_surface.autoreleasepool_scope_sites_total;
  contract.scope_exit_transition_sites =
      sema_parity_surface.autoreleasepool_scope_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface.autoreleasepool_scope_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.autoreleasepool_scope_summary.deterministic &&
      sema_parity_surface.deterministic_autoreleasepool_scope_handoff;
  return contract;
}

Objc3WeakUnownedSemanticsLoweringContract
BuildWeakUnownedSemanticsLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3WeakUnownedSemanticsLoweringContract contract;
  contract.ownership_candidate_sites =
      sema_parity_surface.weak_unowned_semantics_ownership_candidate_sites_total;
  contract.weak_reference_sites =
      sema_parity_surface.weak_unowned_semantics_weak_reference_sites_total;
  contract.unowned_reference_sites =
      sema_parity_surface.weak_unowned_semantics_unowned_reference_sites_total;
  contract.unowned_safe_reference_sites =
      sema_parity_surface
          .weak_unowned_semantics_unowned_safe_reference_sites_total;
  contract.weak_unowned_conflict_sites =
      sema_parity_surface.weak_unowned_semantics_conflict_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface.weak_unowned_semantics_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.weak_unowned_semantics_summary.deterministic &&
      sema_parity_surface.deterministic_weak_unowned_semantics_handoff;
  return contract;
}

Objc3ArcDiagnosticsFixitLoweringContract BuildArcDiagnosticsFixitLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3ArcDiagnosticsFixitLoweringContract contract;
  contract.ownership_arc_diagnostic_candidate_sites =
      sema_parity_surface.ownership_arc_diagnostic_candidate_sites_total;
  contract.ownership_arc_fixit_available_sites =
      sema_parity_surface.ownership_arc_fixit_available_sites_total;
  contract.ownership_arc_profiled_sites =
      sema_parity_surface.ownership_arc_profiled_sites_total;
  contract.ownership_arc_weak_unowned_conflict_diagnostic_sites =
      sema_parity_surface
          .ownership_arc_weak_unowned_conflict_diagnostic_sites_total;
  contract.ownership_arc_empty_fixit_hint_sites =
      sema_parity_surface.ownership_arc_empty_fixit_hint_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface.ownership_arc_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.arc_diagnostics_fixit_summary.deterministic &&
      sema_parity_surface.deterministic_arc_diagnostics_fixit_handoff;
  return contract;
}

Objc3BlockLiteralCaptureLoweringContract BuildBlockLiteralCaptureLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3BlockLiteralCaptureLoweringContract contract;
  contract.block_literal_sites =
      sema_parity_surface.block_literal_capture_semantics_sites_total;
  contract.block_parameter_entries =
      sema_parity_surface
          .block_literal_capture_semantics_parameter_entries_total;
  contract.block_capture_entries =
      sema_parity_surface.block_literal_capture_semantics_capture_entries_total;
  contract.block_body_statement_entries =
      sema_parity_surface
          .block_literal_capture_semantics_body_statement_entries_total;
  contract.block_empty_capture_sites =
      sema_parity_surface
          .block_literal_capture_semantics_empty_capture_sites_total;
  contract.block_nondeterministic_capture_sites =
      sema_parity_surface
          .block_literal_capture_semantics_nondeterministic_capture_sites_total;
  contract.block_non_normalized_sites =
      sema_parity_surface
          .block_literal_capture_semantics_non_normalized_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface
          .block_literal_capture_semantics_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.block_literal_capture_semantics_summary
          .deterministic &&
      sema_parity_surface
          .deterministic_block_literal_capture_semantics_handoff;
  return contract;
}

Objc3BlockAbiInvokeTrampolineLoweringContract
BuildBlockAbiInvokeTrampolineLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3BlockAbiInvokeTrampolineLoweringContract contract;
  contract.block_literal_sites =
      sema_parity_surface.block_abi_invoke_trampoline_sites_total;
  contract.invoke_argument_slots_total =
      sema_parity_surface
          .block_abi_invoke_trampoline_invoke_argument_slots_total;
  contract.capture_word_count_total =
      sema_parity_surface
          .block_abi_invoke_trampoline_capture_word_count_total;
  contract.parameter_entries_total =
      sema_parity_surface
          .block_abi_invoke_trampoline_parameter_entries_total;
  contract.capture_entries_total =
      sema_parity_surface.block_abi_invoke_trampoline_capture_entries_total;
  contract.body_statement_entries_total =
      sema_parity_surface
          .block_abi_invoke_trampoline_body_statement_entries_total;
  contract.descriptor_symbolized_sites =
      sema_parity_surface
          .block_abi_invoke_trampoline_descriptor_symbolized_sites_total;
  contract.invoke_trampoline_symbolized_sites =
      sema_parity_surface
          .block_abi_invoke_trampoline_invoke_symbolized_sites_total;
  contract.missing_invoke_trampoline_sites =
      sema_parity_surface
          .block_abi_invoke_trampoline_missing_invoke_sites_total;
  contract.non_normalized_layout_sites =
      sema_parity_surface
          .block_abi_invoke_trampoline_non_normalized_layout_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface
          .block_abi_invoke_trampoline_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.block_abi_invoke_trampoline_semantics_summary
          .deterministic &&
      sema_parity_surface.deterministic_block_abi_invoke_trampoline_handoff;
  return contract;
}

Objc3BlockStorageEscapeLoweringContract BuildBlockStorageEscapeLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3BlockStorageEscapeLoweringContract contract;
  contract.block_literal_sites =
      sema_parity_surface.block_storage_escape_sites_total;
  contract.mutable_capture_count_total =
      sema_parity_surface.block_storage_escape_mutable_capture_count_total;
  contract.byref_slot_count_total =
      sema_parity_surface.block_storage_escape_byref_slot_count_total;
  contract.parameter_entries_total =
      sema_parity_surface.block_storage_escape_parameter_entries_total;
  contract.capture_entries_total =
      sema_parity_surface.block_storage_escape_capture_entries_total;
  contract.body_statement_entries_total =
      sema_parity_surface.block_storage_escape_body_statement_entries_total;
  contract.requires_byref_cells_sites =
      sema_parity_surface
          .block_storage_escape_requires_byref_cells_sites_total;
  contract.escape_analysis_enabled_sites =
      sema_parity_surface
          .block_storage_escape_escape_analysis_enabled_sites_total;
  contract.escape_to_heap_sites =
      sema_parity_surface.block_storage_escape_escape_to_heap_sites_total;
  contract.escape_profile_normalized_sites =
      sema_parity_surface
          .block_storage_escape_escape_profile_normalized_sites_total;
  contract.byref_layout_symbolized_sites =
      sema_parity_surface
          .block_storage_escape_byref_layout_symbolized_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface.block_storage_escape_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.block_storage_escape_semantics_summary.deterministic &&
      sema_parity_surface.deterministic_block_storage_escape_handoff;
  return contract;
}

Objc3BlockCopyDisposeLoweringContract BuildBlockCopyDisposeLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3BlockCopyDisposeLoweringContract contract;
  contract.block_literal_sites =
      sema_parity_surface.block_copy_dispose_sites_total;
  contract.mutable_capture_count_total =
      sema_parity_surface.block_copy_dispose_mutable_capture_count_total;
  contract.byref_slot_count_total =
      sema_parity_surface.block_copy_dispose_byref_slot_count_total;
  contract.parameter_entries_total =
      sema_parity_surface.block_copy_dispose_parameter_entries_total;
  contract.capture_entries_total =
      sema_parity_surface.block_copy_dispose_capture_entries_total;
  contract.body_statement_entries_total =
      sema_parity_surface.block_copy_dispose_body_statement_entries_total;
  contract.copy_helper_required_sites =
      sema_parity_surface.block_copy_dispose_copy_helper_required_sites_total;
  contract.dispose_helper_required_sites =
      sema_parity_surface
          .block_copy_dispose_dispose_helper_required_sites_total;
  contract.profile_normalized_sites =
      sema_parity_surface.block_copy_dispose_profile_normalized_sites_total;
  contract.copy_helper_symbolized_sites =
      sema_parity_surface
          .block_copy_dispose_copy_helper_symbolized_sites_total;
  contract.dispose_helper_symbolized_sites =
      sema_parity_surface
          .block_copy_dispose_dispose_helper_symbolized_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface.block_copy_dispose_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.block_copy_dispose_semantics_summary.deterministic &&
      sema_parity_surface.deterministic_block_copy_dispose_handoff;
  return contract;
}

Objc3BlockDeterminismPerfBaselineLoweringContract
BuildBlockDeterminismPerfBaselineLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3BlockDeterminismPerfBaselineLoweringContract contract;
  contract.block_literal_sites =
      sema_parity_surface.block_determinism_perf_baseline_sites_total;
  contract.baseline_weight_total =
      sema_parity_surface.block_determinism_perf_baseline_weight_total;
  contract.parameter_entries_total =
      sema_parity_surface
          .block_determinism_perf_baseline_parameter_entries_total;
  contract.capture_entries_total =
      sema_parity_surface.block_determinism_perf_baseline_capture_entries_total;
  contract.body_statement_entries_total =
      sema_parity_surface
          .block_determinism_perf_baseline_body_statement_entries_total;
  contract.deterministic_capture_sites =
      sema_parity_surface
          .block_determinism_perf_baseline_deterministic_capture_sites_total;
  contract.heavy_tier_sites =
      sema_parity_surface.block_determinism_perf_baseline_heavy_tier_sites_total;
  contract.normalized_profile_sites =
      sema_parity_surface
          .block_determinism_perf_baseline_normalized_profile_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface
          .block_determinism_perf_baseline_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.block_determinism_perf_baseline_summary
          .deterministic &&
      sema_parity_surface.deterministic_block_determinism_perf_baseline_handoff;
  return contract;
}

}  // namespace objc3::artifacts::frontend
