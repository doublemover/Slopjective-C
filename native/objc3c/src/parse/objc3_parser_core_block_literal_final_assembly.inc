  void ApplyBlockLiteralInvokeAbiAssembly(Expr &block) {
    block.block_abi_invoke_argument_slots = block.block_parameter_count;
    block.block_abi_capture_word_count = block.block_capture_count;
    block.block_abi_layout_profile = BuildBlockLiteralAbiLayoutProfile(
        block.block_parameter_count,
        block.block_capture_count,
        block.block_body_statement_count);
    block.block_abi_descriptor_symbol = BuildBlockLiteralAbiDescriptorSymbol(
        block.line,
        block.column,
        block.block_parameter_count,
        block.block_capture_count);
    block.block_invoke_trampoline_symbol = BuildBlockLiteralInvokeTrampolineSymbol(
        block.line,
        block.column,
        block.block_parameter_count,
        block.block_capture_count);
    block.block_invoke_surface_entries_lexicographic =
        BuildBlockInvokeSurfaceEntriesLexicographic(block);
    block.block_invoke_surface_profile =
        BuildBlockInvokeSurfaceProfile(block);
    block.block_abi_has_invoke_trampoline = true;
    block.block_abi_layout_is_normalized =
        block.block_literal_is_normalized && block.block_capture_set_deterministic;
  }

  void ApplyBlockLiteralFinalAssembly(Expr &block) {
    block.block_determinism_perf_baseline_weight =
        BuildBlockDeterminismPerfBaselineWeight(
            block.block_parameter_count,
            block.block_capture_count,
            block.block_body_statement_count,
            block.block_copy_helper_required,
            block.block_dispose_helper_required);
    block.block_determinism_perf_baseline_profile =
        BuildBlockDeterminismPerfBaselineProfile(
            block.block_parameter_count,
            block.block_capture_count,
            block.block_body_statement_count,
            block.block_copy_helper_required,
            block.block_dispose_helper_required,
            block.block_capture_set_deterministic,
            block.block_copy_dispose_profile_is_normalized,
            block.block_determinism_perf_baseline_weight);
    block.block_determinism_perf_baseline_profile_is_normalized =
        block.block_copy_dispose_profile_is_normalized &&
        block.block_determinism_perf_baseline_weight >= block.block_capture_count;
    block.block_source_model_is_normalized =
        block.block_literal_is_normalized &&
        block.block_capture_set_deterministic &&
        block.block_abi_layout_is_normalized &&
        block.block_storage_escape_profile_is_normalized &&
        block.block_copy_dispose_profile_is_normalized &&
        block.block_determinism_perf_baseline_profile_is_normalized;
    block.block_source_model_replay_key =
        BuildBlockSourceModelReplayKey(block);
  }
