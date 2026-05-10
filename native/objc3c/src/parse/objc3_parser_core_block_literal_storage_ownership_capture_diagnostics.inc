  bool BlockLiteralStorageOwnershipCapturesAreCanonical(
      const Expr &block) const {
    return block.block_literal_is_normalized &&
           block.block_capture_set_deterministic &&
           IsSortedUniqueStrings(
               block.block_mutated_capture_names_lexicographic) &&
           IsSortedUniqueStrings(
               block.block_byref_capture_names_lexicographic);
  }

  bool BlockLiteralStorageOwnershipProfilesArePublishable(
      const Expr &block) const {
    return !block.block_helper_intent_profile.empty() &&
           !block.block_escape_shape_symbol.empty() &&
           !block.block_escape_shape_profile.empty();
  }
