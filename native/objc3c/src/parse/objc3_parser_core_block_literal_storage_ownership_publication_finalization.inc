  void PublishBlockLiteralStorageOwnershipSurface(Expr &block) {
    block.block_storage_escape_profile =
        BuildBlockStorageEscapeProfile(
            block.block_storage_mutable_capture_count,
            block.block_storage_byref_slot_count,
            block.block_storage_escape_to_heap,
            block.block_body_statement_count);
    block.block_storage_byref_layout_symbol =
        BuildBlockStorageByrefLayoutSymbol(
            block.line,
            block.column,
            block.block_storage_mutable_capture_count,
            block.block_storage_byref_slot_count,
            block.block_storage_escape_to_heap);
    block.block_storage_escape_profile_is_normalized =
        block.block_literal_is_normalized &&
        block.block_capture_set_deterministic;
  }

  void FinalizeBlockLiteralCopyDisposeOwnership(Expr &block) {
    block.block_copy_helper_required =
        block.block_storage_byref_slot_count > 0u;
    block.block_dispose_helper_required =
        block.block_storage_byref_slot_count > 0u;
    block.block_copy_dispose_profile =
        BuildBlockCopyDisposeProfile(
            block.block_storage_mutable_capture_count,
            block.block_storage_byref_slot_count,
            block.block_storage_escape_to_heap,
            block.block_body_statement_count);
    block.block_copy_helper_symbol =
        BuildBlockCopyHelperSymbol(
            block.line,
            block.column,
            block.block_storage_mutable_capture_count,
            block.block_storage_byref_slot_count,
            block.block_storage_escape_to_heap);
    block.block_dispose_helper_symbol =
        BuildBlockDisposeHelperSymbol(
            block.line,
            block.column,
            block.block_storage_mutable_capture_count,
            block.block_storage_byref_slot_count,
            block.block_storage_escape_to_heap);
    block.block_copy_dispose_profile_is_normalized =
        block.block_storage_escape_profile_is_normalized &&
        block.block_copy_helper_required == block.block_dispose_helper_required;
  }
