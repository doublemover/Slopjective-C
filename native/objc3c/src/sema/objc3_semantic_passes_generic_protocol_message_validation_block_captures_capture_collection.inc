  mutable_expr->block_runtime_owned_object_capture_count = 0;
  mutable_expr->block_runtime_weak_object_capture_count = 0;
  mutable_expr->block_runtime_unowned_object_capture_count = 0;
  mutable_expr->block_runtime_owned_object_capture_names_lexicographic.clear();
  mutable_expr->block_runtime_weak_object_capture_names_lexicographic.clear();
  mutable_expr->block_runtime_unowned_object_capture_names_lexicographic.clear();
  mutable_expr->block_storage_mutable_capture_count =
      expr->block_mutated_capture_count;
  mutable_expr->block_storage_byref_slot_count =
      expr->block_byref_capture_count;
  mutable_expr->block_storage_requires_byref_cells =
      expr->block_byref_capture_count > 0u;
  mutable_expr->block_storage_escape_analysis_enabled = true;
  // escaping-block runtime-hook anchor: the live lowering profile
  // now inherits the parser-owned escape-shape heap candidate directly, so
  // later lane-C emission can distinguish true escaping block values from the
  // nonescaping stack-only slice.
  mutable_expr->block_storage_escape_to_heap =
      expr->block_escape_shape_promotes_to_heap_candidate;
  mutable_expr->block_runtime_copy_helper_required = false;
  mutable_expr->block_runtime_dispose_helper_required = false;
  mutable_expr->block_runtime_capture_ownership_is_normalized = false;
  mutable_expr->block_runtime_capture_ownership_profile.clear();
  mutable_expr->block_storage_escape_profile_is_normalized =
      expr->block_source_storage_annotations_are_normalized;
  mutable_expr->block_copy_dispose_profile_is_normalized =
      expr->block_source_storage_annotations_are_normalized;
  mutable_expr->block_storage_escape_profile.clear();
  mutable_expr->block_storage_byref_layout_symbol.clear();
  mutable_expr->block_copy_dispose_profile.clear();
  mutable_expr->block_copy_helper_symbol.clear();
  mutable_expr->block_dispose_helper_symbol.clear();

  if (expr->block_parameter_types_source_order.size() !=
      expr->block_parameter_count) {
    diagnostics.push_back(
        MakeDiag(expr->line,
                 expr->column,
                 "O3S206",
                 "type mismatch: block invocation parameter typing is incomplete"));
  }
  if (expr->block_capture_names_lexicographic.size() !=
          expr->block_capture_count ||
      !IsSortedUniqueStrings(expr->block_capture_names_lexicographic)) {
    diagnostics.push_back(
        MakeDiag(expr->line,
                 expr->column,
                 "O3S206",
                 "type mismatch: block capture inventory is not normalized"));
  }
  if (expr->block_mutated_capture_names_lexicographic.size() !=
          expr->block_mutated_capture_count ||
      !IsSortedUniqueStrings(expr->block_mutated_capture_names_lexicographic)) {
    diagnostics.push_back(
        MakeDiag(expr->line,
                 expr->column,
                 "O3S206",
                 "type mismatch: block mutated-capture inventory is not normalized"));
  }
  if (expr->block_byref_capture_names_lexicographic.size() !=
          expr->block_byref_capture_count ||
      !IsSortedUniqueStrings(expr->block_byref_capture_names_lexicographic)) {
    diagnostics.push_back(
        MakeDiag(expr->line,
                 expr->column,
                 "O3S206",
                 "type mismatch: block byref-capture inventory is not normalized"));
  }
  if (!expr->block_source_storage_annotations_are_normalized) {
    diagnostics.push_back(
        MakeDiag(expr->line,
                 expr->column,
                 "O3S206",
                 "type mismatch: block source storage annotations are not normalized"));
  }

  std::unordered_set<std::string> capture_names(
      expr->block_capture_names_lexicographic.begin(),
      expr->block_capture_names_lexicographic.end());
  std::unordered_set<std::string> mutated_names(
      expr->block_mutated_capture_names_lexicographic.begin(),
      expr->block_mutated_capture_names_lexicographic.end());
  std::unordered_set<std::string> explicit_byref_capture_names;
  for (const auto &item : expr->block_explicit_capture_items_source_order) {
    if (item.mode == "byref") {
      explicit_byref_capture_names.insert(item.name);
    }
  }
  std::unordered_set<std::string> explicit_capture_names;
  std::size_t owned_object_capture_count = 0;
  std::size_t weak_object_capture_count = 0;
  std::size_t unowned_object_capture_count = 0;
