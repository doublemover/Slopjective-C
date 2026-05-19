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
        ResolveBlockCaptureSemanticType(scopes, globals, item.name,
                                        resolved_type);
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
    if (!ResolveBlockCaptureSemanticType(scopes, globals, capture_name,
                                         resolved_type)) {
      diagnostics.push_back(
          MakeDiag(expr->line,
                   expr->column,
                   "O3S202",
                   "undefined capture '" + capture_name +
                       "' in block literal"));
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
