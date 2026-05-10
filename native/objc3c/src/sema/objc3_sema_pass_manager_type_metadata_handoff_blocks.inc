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
      // executable-block-object/invoke-thunk anchor: the current
      // runnable slice allows readonly scalar captures without forcing the old
      // mutable==capture or byref==capture equalities. Later C003 work widens
      // this to real byref/helper cases.
      result.type_metadata_handoff.block_storage_escape_semantics_summary.mutable_capture_count_total <=
          result.type_metadata_handoff.block_storage_escape_semantics_summary.capture_entries_total &&
      result.type_metadata_handoff.block_storage_escape_semantics_summary.byref_slot_count_total <=
          result.type_metadata_handoff.block_storage_escape_semantics_summary.mutable_capture_count_total &&
      result.type_metadata_handoff.block_storage_escape_semantics_summary.escape_analysis_enabled_sites ==
          result.type_metadata_handoff.block_storage_escape_semantics_summary.block_literal_sites;
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
      // executable-block-object/invoke-thunk anchor: copy/dispose
      // determinism must also admit the readonly-capture slice even though no
      // helper bodies are emitted yet.
      result.type_metadata_handoff.block_copy_dispose_semantics_summary.mutable_capture_count_total <=
          result.type_metadata_handoff.block_copy_dispose_semantics_summary.capture_entries_total &&
      result.type_metadata_handoff.block_copy_dispose_semantics_summary.byref_slot_count_total <=
          result.type_metadata_handoff.block_copy_dispose_semantics_summary.mutable_capture_count_total &&
      result.type_metadata_handoff.block_copy_dispose_semantics_summary.copy_helper_required_sites <=
          result.type_metadata_handoff.block_copy_dispose_semantics_summary.dispose_helper_required_sites;
  result.block_determinism_perf_baseline_summary =
      result.integration_surface.block_determinism_perf_baseline_summary;
  result.deterministic_block_determinism_perf_baseline_handoff =
      result.type_metadata_handoff.block_determinism_perf_baseline_summary.deterministic &&
      result.integration_surface.block_determinism_perf_baseline_summary.deterministic &&
      IsEquivalentBlockDeterminismPerfBaselineSummary(
          result.integration_surface.block_determinism_perf_baseline_summary,
          result.type_metadata_handoff.block_determinism_perf_baseline_summary) &&
      AreEquivalentBlockDeterminismPerfBaselineSites(
          result.integration_surface.block_determinism_perf_baseline_sites_lexicographic,
          result.type_metadata_handoff.block_determinism_perf_baseline_sites_lexicographic) &&
      result.type_metadata_handoff.block_determinism_perf_baseline_summary.deterministic_capture_sites <=
          result.type_metadata_handoff.block_determinism_perf_baseline_summary.block_literal_sites &&
      result.type_metadata_handoff.block_determinism_perf_baseline_summary.heavy_tier_sites <=
          result.type_metadata_handoff.block_determinism_perf_baseline_summary.block_literal_sites &&
      result.type_metadata_handoff.block_determinism_perf_baseline_summary.normalized_profile_sites <=
          result.type_metadata_handoff.block_determinism_perf_baseline_summary.block_literal_sites &&
      result.type_metadata_handoff.block_determinism_perf_baseline_summary.contract_violation_sites <=
          result.type_metadata_handoff.block_determinism_perf_baseline_summary.block_literal_sites;
  const bool recovery_and_block_determinism_hardening_consistent =
      result.deterministic_error_diagnostics_recovery_handoff &&
      result.deterministic_block_determinism_perf_baseline_handoff;
  result.deterministic_type_metadata_handoff =
      result.deterministic_type_metadata_handoff &&
      recovery_and_block_determinism_hardening_consistent;
  if (!recovery_and_block_determinism_hardening_consistent) {
    return result;
  }
