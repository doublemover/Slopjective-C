static bool ResolveBlockCaptureSemanticType(
    const std::vector<SemanticScope> &scopes,
    const std::unordered_map<std::string, ValueType> &globals,
    const std::string &name,
    SemanticTypeInfo &resolved_type) {
  resolved_type = ScopeLookupType(scopes, name);
  if (!IsUnknownSemanticType(resolved_type)) {
    return true;
  }
  const auto global_it = globals.find(name);
  if (global_it != globals.end()) {
    resolved_type = MakeSemanticTypeFromGlobal(global_it->second);
    return true;
  }
  return false;
}

static void ValidateBlockLiteralCaptureLegality(
    const Expr *expr,
    const std::vector<SemanticScope> &scopes,
    const std::unordered_map<std::string, ValueType> &globals,
    std::vector<std::string> &diagnostics) {
  if (expr == nullptr || expr->kind != Expr::Kind::BlockLiteral) {
    return;
  }

  Expr *mutable_expr = const_cast<Expr *>(expr);
  mutable_expr->block_runtime_owned_object_capture_count = 0;
  mutable_expr->block_runtime_weak_object_capture_count = 0;
  mutable_expr->block_runtime_unowned_object_capture_count = 0;
  mutable_expr->block_runtime_owned_object_capture_names_lexicographic.clear();
  mutable_expr->block_runtime_weak_object_capture_names_lexicographic.clear();
  mutable_expr->block_runtime_unowned_object_capture_names_lexicographic.clear();
  mutable_expr->block_storage_mutable_capture_count = expr->block_mutated_capture_count;
  mutable_expr->block_storage_byref_slot_count = expr->block_byref_capture_count;
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

  if (expr->block_parameter_types_source_order.size() != expr->block_parameter_count) {
    diagnostics.push_back(
        MakeDiag(expr->line,
                 expr->column,
                 "O3S206",
                 "type mismatch: block invocation parameter typing is incomplete"));
  }
  if (expr->block_capture_names_lexicographic.size() != expr->block_capture_count ||
      !IsSortedUniqueStrings(expr->block_capture_names_lexicographic)) {
    diagnostics.push_back(
        MakeDiag(expr->line,
                 expr->column,
                 "O3S206",
                 "type mismatch: block capture inventory is not normalized"));
  }
  if (expr->block_mutated_capture_names_lexicographic.size() != expr->block_mutated_capture_count ||
      !IsSortedUniqueStrings(expr->block_mutated_capture_names_lexicographic)) {
    diagnostics.push_back(
        MakeDiag(expr->line,
                 expr->column,
                 "O3S206",
                 "type mismatch: block mutated-capture inventory is not normalized"));
  }
  if (expr->block_byref_capture_names_lexicographic.size() != expr->block_byref_capture_count ||
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
  for (const auto &mutated_name : expr->block_mutated_capture_names_lexicographic) {
    if (capture_names.count(mutated_name) == 0u) {
      diagnostics.push_back(
          MakeDiag(expr->line,
                   expr->column,
                   "O3S206",
                   "type mismatch: block mutated capture '" + mutated_name +
                       "' is not present in the capture inventory"));
    }
  }
  for (const auto &byref_name : expr->block_byref_capture_names_lexicographic) {
    if (mutated_names.count(byref_name) == 0u &&
        explicit_byref_capture_names.count(byref_name) == 0u) {
      diagnostics.push_back(
          MakeDiag(expr->line,
                   expr->column,
                   "O3S206",
                   "type mismatch: block byref capture '" + byref_name +
                       "' is not present in the mutated-capture inventory or explicit byref capture list"));
    }
  }
  for (const auto &item : expr->block_explicit_capture_items_source_order) {
    if (!explicit_capture_names.insert(item.name).second) {
      diagnostics.push_back(
          MakeDiag(expr->line,
                   expr->column,
                   "O3S301",
                   "capture-list legality failed: duplicate explicit capture '" +
                       item.name + "'"));
      continue;
    }

    SemanticTypeInfo resolved_type = MakeScalarSemanticType(ValueType::Unknown);
    const bool resolved_capture =
        ResolveBlockCaptureSemanticType(scopes, globals, item.name, resolved_type);
    if (!resolved_capture) {
      diagnostics.push_back(
          MakeDiag(expr->line,
                   expr->column,
                   "O3S202",
                   "undefined capture '" + item.name + "' in block literal"));
      continue;
    }
    if (capture_names.count(item.name) == 0u) {
      diagnostics.push_back(
          MakeDiag(expr->line,
                   expr->column,
                   "O3S303",
                   "capture-list legality failed: explicit capture '" +
                       item.name + "' is not used by the block body"));
    }
    if ((item.mode == "weak" || item.mode == "unowned") &&
        !IsObjCReferenceSemanticType(resolved_type)) {
      diagnostics.push_back(
          MakeDiag(expr->line,
                   expr->column,
                   "O3S302",
                   "capture-list legality failed: explicit " + item.mode +
                       " capture '" + item.name +
                       "' requires an Objective-C reference type"));
    }
  }
  for (const auto &capture_name : expr->block_capture_names_lexicographic) {
    SemanticTypeInfo resolved_type = MakeScalarSemanticType(ValueType::Unknown);
    if (!ResolveBlockCaptureSemanticType(scopes, globals, capture_name, resolved_type)) {
      diagnostics.push_back(
          MakeDiag(expr->line,
                   expr->column,
                   "O3S202",
                   "undefined capture '" + capture_name + "' in block literal"));
      continue;
    }

    if (IsOwnedObjCReferenceSemanticType(resolved_type)) {
      ++owned_object_capture_count;
      mutable_expr->block_runtime_owned_object_capture_names_lexicographic
          .push_back(capture_name);
    } else if (IsWeakObjCReferenceSemanticType(resolved_type)) {
      ++weak_object_capture_count;
      mutable_expr->block_runtime_weak_object_capture_names_lexicographic
          .push_back(capture_name);
    } else if (IsUnownedObjCReferenceSemanticType(resolved_type)) {
      ++unowned_object_capture_count;
      mutable_expr->block_runtime_unowned_object_capture_names_lexicographic
          .push_back(capture_name);
    }

    if (mutated_names.count(capture_name) != 0u &&
        (IsWeakObjCReferenceSemanticType(resolved_type) ||
         IsUnownedObjCReferenceSemanticType(resolved_type))) {
      diagnostics.push_back(
          MakeDiag(expr->line,
                   expr->column,
                   "O3S206",
                   "type mismatch: block mutated capture '" + capture_name +
                       "' requires owned runtime-backed storage"));
    }
  }

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
      expr->block_capture_names_lexicographic.size() == expr->block_capture_count &&
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
                       << ";body-statements=" << expr->block_body_statement_count;
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
                          << "_b" << mutable_expr->block_storage_byref_slot_count
                          << "_"
                          << (mutable_expr->block_storage_escape_to_heap ? "heap"
                                                                        : "stack");
    mutable_expr->block_dispose_helper_symbol = dispose_helper_symbol.str();
  }
}
