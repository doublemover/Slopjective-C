  mutable_expr->block_runtime_owned_object_capture_count =
      owned_object_capture_count;
  mutable_expr->block_runtime_weak_object_capture_count =
      weak_object_capture_count;
  mutable_expr->block_runtime_unowned_object_capture_count =
      unowned_object_capture_count;
  const bool runtime_copy_dispose_required =
      expr->block_byref_capture_count > 0u || owned_object_capture_count > 0u;
  const bool runtime_dispose_required =
      runtime_copy_dispose_required ||
      expr->block_explicit_capture_move_count > 0u;
  mutable_expr->block_runtime_copy_helper_required =
      runtime_copy_dispose_required;
  mutable_expr->block_runtime_dispose_helper_required =
      runtime_dispose_required;
  mutable_expr->block_runtime_capture_ownership_is_normalized =
      expr->block_capture_names_lexicographic.size() ==
          expr->block_capture_count &&
      expr->block_mutated_capture_names_lexicographic.size() ==
          expr->block_mutated_capture_count &&
      expr->block_byref_capture_names_lexicographic.size() ==
          expr->block_byref_capture_count &&
      IsSortedUniqueStrings(expr->block_capture_names_lexicographic) &&
      IsSortedUniqueStrings(expr->block_mutated_capture_names_lexicographic) &&
      IsSortedUniqueStrings(expr->block_byref_capture_names_lexicographic) &&
      expr->block_runtime_owned_object_capture_names_lexicographic.size() ==
          owned_object_capture_count &&
      expr->block_runtime_weak_object_capture_names_lexicographic.size() ==
          weak_object_capture_count &&
      expr->block_runtime_unowned_object_capture_names_lexicographic.size() ==
          unowned_object_capture_count &&
      IsSortedUniqueStrings(
          expr->block_runtime_owned_object_capture_names_lexicographic) &&
      IsSortedUniqueStrings(
          expr->block_runtime_weak_object_capture_names_lexicographic) &&
      IsSortedUniqueStrings(
          expr->block_runtime_unowned_object_capture_names_lexicographic) &&
      expr->block_source_storage_annotations_are_normalized;
  std::ostringstream ownership_profile;
  // byref/copy-dispose/object-ownership anchor: sema upgrades the
  // frozen source-only block path with ownership-sensitive helper eligibility
  // and fail-closed non-owning mutation rejection before runnable block object
  // lowering lands.
  ownership_profile << "block-runtime-capture-ownership:owned="
                    << owned_object_capture_count
                    << ";weak=" << weak_object_capture_count
                    << ";unowned=" << unowned_object_capture_count
                    << ";copy-helper="
                    << (runtime_copy_dispose_required ? "enabled" : "elided")
                    << ";dispose-helper="
                    << (runtime_copy_dispose_required ? "enabled" : "elided");
  mutable_expr->block_runtime_capture_ownership_profile =
      ownership_profile.str();

  std::ostringstream storage_escape_profile;
  storage_escape_profile << "block-storage:mutable-captures="
                         << mutable_expr->block_storage_mutable_capture_count
                         << ";byref-slots="
                         << mutable_expr->block_storage_byref_slot_count
                         << ";escape="
                         << (mutable_expr->block_storage_escape_to_heap ? "heap"
                                                                        : "stack")
                         << ";body-statements="
                         << expr->block_body_statement_count;
  mutable_expr->block_storage_escape_profile = storage_escape_profile.str();
  if (mutable_expr->block_storage_requires_byref_cells) {
    std::ostringstream byref_layout_symbol;
    byref_layout_symbol << "__objc3_block_byref_layout_" << expr->line << "_"
                        << expr->column << "_m"
                        << mutable_expr->block_storage_mutable_capture_count
                        << "_b" << mutable_expr->block_storage_byref_slot_count
                        << "_"
                        << (mutable_expr->block_storage_escape_to_heap ? "heap"
                                                                       : "stack");
    mutable_expr->block_storage_byref_layout_symbol =
        byref_layout_symbol.str();
  }

  std::ostringstream copy_dispose_profile;
  copy_dispose_profile << mutable_expr->block_runtime_capture_ownership_profile
                       << ";block-copy-dispose:copy-helper="
                       << (mutable_expr->block_runtime_copy_helper_required
                               ? "enabled"
                               : "elided")
                       << ";dispose-helper="
                       << (mutable_expr->block_runtime_dispose_helper_required
                               ? "enabled"
                               : "elided")
                       << ";escape="
                       << (mutable_expr->block_storage_escape_to_heap ? "heap"
                                                                      : "stack")
                       << ";body-statements="
                       << expr->block_body_statement_count;
  mutable_expr->block_copy_dispose_profile = copy_dispose_profile.str();
  if (mutable_expr->block_runtime_copy_helper_required) {
    std::ostringstream copy_helper_symbol;
    copy_helper_symbol << "__objc3_block_copy_helper_" << expr->line << "_"
                       << expr->column << "_m"
                       << mutable_expr->block_storage_mutable_capture_count
                       << "_b" << mutable_expr->block_storage_byref_slot_count
                       << "_"
                       << (mutable_expr->block_storage_escape_to_heap ? "heap"
                                                                      : "stack");
    mutable_expr->block_copy_helper_symbol = copy_helper_symbol.str();
  }
  if (mutable_expr->block_runtime_dispose_helper_required) {
    std::ostringstream dispose_helper_symbol;
    dispose_helper_symbol << "__objc3_block_dispose_helper_" << expr->line
                          << "_" << expr->column << "_m"
                          << mutable_expr->block_storage_mutable_capture_count
                          << "_b"
                          << mutable_expr->block_storage_byref_slot_count
                          << "_"
                          << (mutable_expr->block_storage_escape_to_heap ? "heap"
                                                                         : "stack");
    mutable_expr->block_dispose_helper_symbol = dispose_helper_symbol.str();
  }
