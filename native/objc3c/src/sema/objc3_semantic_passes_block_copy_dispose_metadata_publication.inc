static std::string BuildBlockCopyDisposeProfile(
    bool copy_helper_required,
    bool dispose_helper_required,
    bool escape_to_heap,
    std::size_t body_statement_count,
    std::size_t explicit_move_capture_count) {
  std::ostringstream out;
  out << "block-copy-dispose:copy-helper="
      << (copy_helper_required ? "enabled" : "elided")
      << ";dispose-helper="
      << (dispose_helper_required ? "enabled" : "elided")
      << ";escape=" << (escape_to_heap ? "heap" : "stack")
      << ";body-statements=" << body_statement_count
      << ";move-captures=" << explicit_move_capture_count;
  return out.str();
}

static std::string BuildBlockCopyHelperSymbol(
    unsigned line,
    unsigned column,
    std::size_t mutable_capture_count,
    std::size_t byref_slot_count,
    bool escape_to_heap) {
  std::ostringstream out;
  out << "__objc3_block_copy_helper_" << line << "_" << column
      << "_m" << mutable_capture_count
      << "_b" << byref_slot_count
      << "_" << (escape_to_heap ? "heap" : "stack");
  return out.str();
}

static std::string BuildBlockDisposeHelperSymbol(
    unsigned line,
    unsigned column,
    std::size_t mutable_capture_count,
    std::size_t byref_slot_count,
    bool escape_to_heap) {
  std::ostringstream out;
  out << "__objc3_block_dispose_helper_" << line << "_" << column
      << "_m" << mutable_capture_count
      << "_b" << byref_slot_count
      << "_" << (escape_to_heap ? "heap" : "stack");
  return out.str();
}

static Objc3BlockCopyDisposeSiteMetadata BuildBlockCopyDisposeSiteMetadata(
    const Expr &expr) {
  Objc3BlockCopyDisposeSiteMetadata metadata;
  metadata.mutable_capture_count = expr.block_mutated_capture_count;
  metadata.byref_slot_count = expr.block_byref_capture_count;
  metadata.parameter_count = expr.block_parameter_count;
  metadata.capture_count = expr.block_capture_count;
  metadata.body_statement_count = expr.block_body_statement_count;
  metadata.owned_object_capture_count =
      expr.block_runtime_owned_object_capture_count;
  metadata.weak_object_capture_count =
      expr.block_runtime_weak_object_capture_count;
  metadata.unowned_object_capture_count =
      expr.block_runtime_unowned_object_capture_count;
  metadata.explicit_move_capture_count = expr.block_explicit_capture_move_count;

  const auto helper_intent =
      ClassifyBlockCopyDisposeHelperIntent(expr, metadata);
  metadata.copy_helper_required = helper_intent.copy_helper_required;
  metadata.dispose_helper_required = helper_intent.dispose_helper_required;
  metadata.copy_dispose_profile_is_normalized =
      helper_intent.copy_dispose_profile_is_normalized;
  metadata.ownership_helper_eligibility_is_normalized =
      helper_intent.ownership_helper_eligibility_is_normalized;

  metadata.copy_dispose_profile =
      expr.block_runtime_capture_ownership_is_normalized &&
              !expr.block_runtime_capture_ownership_profile.empty()
          ? expr.block_runtime_capture_ownership_profile + ";" +
                BuildBlockCopyDisposeProfile(
                    metadata.copy_helper_required,
                    metadata.dispose_helper_required,
                    expr.block_escape_shape_promotes_to_heap_candidate,
                    metadata.body_statement_count,
                    metadata.explicit_move_capture_count)
          : BuildBlockCopyDisposeProfile(
                metadata.copy_helper_required,
                metadata.dispose_helper_required,
                expr.block_escape_shape_promotes_to_heap_candidate,
                metadata.body_statement_count,
                metadata.explicit_move_capture_count);
  metadata.copy_helper_symbol =
      metadata.copy_helper_required
          ? BuildBlockCopyHelperSymbol(
                expr.line,
                expr.column,
                metadata.mutable_capture_count,
                metadata.byref_slot_count,
                expr.block_escape_shape_promotes_to_heap_candidate)
          : "";
  metadata.dispose_helper_symbol =
      metadata.dispose_helper_required
          ? BuildBlockDisposeHelperSymbol(
                expr.line,
                expr.column,
                metadata.mutable_capture_count,
                metadata.byref_slot_count,
                expr.block_escape_shape_promotes_to_heap_candidate)
          : "";
  metadata.line = expr.line;
  metadata.column = expr.column;
  metadata.has_count_mismatch =
      metadata.mutable_capture_count > metadata.capture_count ||
      metadata.byref_slot_count > metadata.mutable_capture_count ||
      !IsSortedUniqueStrings(expr.block_parameter_names_lexicographic) ||
      !IsSortedUniqueStrings(expr.block_capture_names_lexicographic);
  return metadata;
}
