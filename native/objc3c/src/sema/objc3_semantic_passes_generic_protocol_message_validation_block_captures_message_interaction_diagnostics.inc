  for (const auto &mutated_name :
       expr->block_mutated_capture_names_lexicographic) {
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
