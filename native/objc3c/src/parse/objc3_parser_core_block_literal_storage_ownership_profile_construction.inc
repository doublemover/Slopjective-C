  void BuildBlockLiteralStorageOwnershipProfiles(
      Expr &block,
      BlockLiteralSourceUseKind source_use_kind) {
    block.block_helper_intent_profile =
        BuildObjc3BlockHelperIntentProfile(
            block.block_mutated_capture_count,
            block.block_byref_capture_count,
            block.block_copy_helper_intent_required,
            block.block_dispose_helper_intent_required,
            source_use_kind);
    block.block_escape_shape_profile =
        BuildObjc3BlockEscapeShapeProfile(
            source_use_kind,
            block.block_escape_shape_promotes_to_heap_candidate,
            block.block_capture_count,
            block.block_byref_capture_count);
    block.block_source_storage_annotations_are_normalized =
        BlockLiteralStorageOwnershipCapturesAreCanonical(block) &&
        BlockLiteralStorageOwnershipProfilesArePublishable(block);
  }
