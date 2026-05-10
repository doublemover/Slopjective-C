  void ApplyBlockLiteralStorageOwnershipSlotModel(Expr &block) {
    block.block_storage_mutable_capture_count =
        block.block_mutated_capture_count;
    block.block_storage_byref_slot_count =
        block.block_byref_capture_count;
    block.block_storage_requires_byref_cells =
        block.block_storage_byref_slot_count > 0u;
    block.block_storage_escape_analysis_enabled = true;
    block.block_storage_escape_to_heap =
        block.block_escape_shape_promotes_to_heap_candidate;
  }
