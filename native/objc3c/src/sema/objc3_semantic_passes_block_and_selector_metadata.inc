static Objc3BlockStorageEscapeSiteMetadata BuildBlockStorageEscapeSiteMetadata(const Expr &expr) {
  auto build_block_storage_escape_profile = [](std::size_t mutable_capture_count,
                                               std::size_t byref_slot_count,
                                               bool escape_to_heap,
                                               std::size_t body_statement_count) {
    std::ostringstream out;
    out << "block-storage:mutable-captures=" << mutable_capture_count
        << ";byref-slots=" << byref_slot_count
        << ";escape=" << (escape_to_heap ? "heap" : "stack")
        << ";body-statements=" << body_statement_count;
    return out.str();
  };
  auto build_block_storage_byref_layout_symbol = [](unsigned line,
                                                    unsigned column,
                                                    std::size_t mutable_capture_count,
                                                    std::size_t byref_slot_count,
                                                    bool escape_to_heap) {
    std::ostringstream out;
    out << "__objc3_block_byref_layout_" << line << "_" << column
        << "_m" << mutable_capture_count
        << "_b" << byref_slot_count
        << "_" << (escape_to_heap ? "heap" : "stack");
    return out.str();
  };
  Objc3BlockStorageEscapeSiteMetadata metadata;
  metadata.mutable_capture_count = expr.block_mutated_capture_count;
  metadata.byref_slot_count = expr.block_byref_capture_count;
  metadata.parameter_count = expr.block_parameter_count;
  metadata.capture_count = expr.block_capture_count;
  metadata.body_statement_count = expr.block_body_statement_count;
  metadata.requires_byref_cells = metadata.byref_slot_count > 0u;
  metadata.escape_analysis_enabled = true;
  metadata.escape_to_heap = expr.block_escape_shape_promotes_to_heap_candidate;
  metadata.escape_profile_is_normalized =
      expr.block_source_storage_annotations_are_normalized;
  metadata.escape_profile =
      build_block_storage_escape_profile(metadata.mutable_capture_count,
                                         metadata.byref_slot_count,
                                         metadata.escape_to_heap,
                                         metadata.body_statement_count);
  metadata.byref_layout_symbol =
      metadata.requires_byref_cells
          ? build_block_storage_byref_layout_symbol(expr.line,
                                                    expr.column,
                                                    metadata.mutable_capture_count,
                                                    metadata.byref_slot_count,
                                                    metadata.escape_to_heap)
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

static void CollectBlockStorageEscapeSiteMetadataFromExpr(
    const Expr *expr,
    std::vector<Objc3BlockStorageEscapeSiteMetadata> &sites) {
  if (expr == nullptr) {
    return;
  }

  if (expr->kind == Expr::Kind::BlockLiteral) {
    sites.push_back(BuildBlockStorageEscapeSiteMetadata(*expr));
  }

  CollectBlockStorageEscapeSiteMetadataFromExpr(expr->receiver.get(), sites);
  CollectBlockStorageEscapeSiteMetadataFromExpr(expr->left.get(), sites);
  CollectBlockStorageEscapeSiteMetadataFromExpr(expr->right.get(), sites);
  CollectBlockStorageEscapeSiteMetadataFromExpr(expr->third.get(), sites);
  for (const auto &arg : expr->args) {
    CollectBlockStorageEscapeSiteMetadataFromExpr(arg.get(), sites);
  }
}

static void CollectBlockStorageEscapeSiteMetadataFromStatements(
    const std::vector<std::unique_ptr<Stmt>> &statements,
    std::vector<Objc3BlockStorageEscapeSiteMetadata> &sites);

static void CollectBlockStorageEscapeSiteMetadataFromForClause(
    const ForClause &clause,
    std::vector<Objc3BlockStorageEscapeSiteMetadata> &sites) {
  CollectBlockStorageEscapeSiteMetadataFromExpr(clause.value.get(), sites);
}

static void CollectBlockStorageEscapeSiteMetadataFromStatement(
    const Stmt *stmt,
    std::vector<Objc3BlockStorageEscapeSiteMetadata> &sites) {
  if (stmt == nullptr) {
    return;
  }

  switch (stmt->kind) {
    case Stmt::Kind::Let:
      CollectBlockStorageEscapeSiteMetadataFromExpr(stmt->let_stmt->value.get(), sites);
      return;
    case Stmt::Kind::Assign:
      CollectBlockStorageEscapeSiteMetadataFromExpr(stmt->assign_stmt->value.get(), sites);
      return;
    case Stmt::Kind::Return:
      CollectBlockStorageEscapeSiteMetadataFromExpr(stmt->return_stmt->value.get(), sites);
      return;
    case Stmt::Kind::If:
      CollectBlockStorageEscapeSiteMetadataFromExpr(stmt->if_stmt->condition.get(), sites);
      CollectBlockStorageEscapeSiteMetadataFromStatements(stmt->if_stmt->then_body, sites);
      CollectBlockStorageEscapeSiteMetadataFromStatements(stmt->if_stmt->else_body, sites);
      return;
    case Stmt::Kind::DoWhile:
      CollectBlockStorageEscapeSiteMetadataFromStatements(stmt->do_while_stmt->body, sites);
      CollectBlockStorageEscapeSiteMetadataFromExpr(stmt->do_while_stmt->condition.get(), sites);
      return;
    case Stmt::Kind::For:
      CollectBlockStorageEscapeSiteMetadataFromForClause(stmt->for_stmt->init, sites);
      CollectBlockStorageEscapeSiteMetadataFromExpr(stmt->for_stmt->condition.get(), sites);
      CollectBlockStorageEscapeSiteMetadataFromForClause(stmt->for_stmt->step, sites);
      CollectBlockStorageEscapeSiteMetadataFromStatements(stmt->for_stmt->body, sites);
      return;
    case Stmt::Kind::Switch:
      CollectBlockStorageEscapeSiteMetadataFromExpr(stmt->switch_stmt->condition.get(), sites);
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        CollectBlockStorageEscapeSiteMetadataFromStatements(switch_case.body, sites);
      }
      return;
    case Stmt::Kind::While:
      CollectBlockStorageEscapeSiteMetadataFromExpr(stmt->while_stmt->condition.get(), sites);
      CollectBlockStorageEscapeSiteMetadataFromStatements(stmt->while_stmt->body, sites);
      return;
    case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
      CollectBlockStorageEscapeSiteMetadataFromStatements(stmt->block_stmt->body, sites);
      return;
    case Stmt::Kind::Expr:
      CollectBlockStorageEscapeSiteMetadataFromExpr(stmt->expr_stmt->value.get(), sites);
      return;
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return;
  }
}

static void CollectBlockStorageEscapeSiteMetadataFromStatements(
    const std::vector<std::unique_ptr<Stmt>> &statements,
    std::vector<Objc3BlockStorageEscapeSiteMetadata> &sites) {
  for (const auto &statement : statements) {
    CollectBlockStorageEscapeSiteMetadataFromStatement(statement.get(), sites);
  }
}

static bool IsBlockStorageEscapeSiteMetadataLess(
    const Objc3BlockStorageEscapeSiteMetadata &lhs,
    const Objc3BlockStorageEscapeSiteMetadata &rhs) {
  if (lhs.escape_profile != rhs.escape_profile) {
    return lhs.escape_profile < rhs.escape_profile;
  }
  if (lhs.byref_layout_symbol != rhs.byref_layout_symbol) {
    return lhs.byref_layout_symbol < rhs.byref_layout_symbol;
  }
  if (lhs.mutable_capture_count != rhs.mutable_capture_count) {
    return lhs.mutable_capture_count < rhs.mutable_capture_count;
  }
  if (lhs.byref_slot_count != rhs.byref_slot_count) {
    return lhs.byref_slot_count < rhs.byref_slot_count;
  }
  if (lhs.parameter_count != rhs.parameter_count) {
    return lhs.parameter_count < rhs.parameter_count;
  }
  if (lhs.capture_count != rhs.capture_count) {
    return lhs.capture_count < rhs.capture_count;
  }
  if (lhs.body_statement_count != rhs.body_statement_count) {
    return lhs.body_statement_count < rhs.body_statement_count;
  }
  if (lhs.requires_byref_cells != rhs.requires_byref_cells) {
    return lhs.requires_byref_cells < rhs.requires_byref_cells;
  }
  if (lhs.escape_analysis_enabled != rhs.escape_analysis_enabled) {
    return lhs.escape_analysis_enabled < rhs.escape_analysis_enabled;
  }
  if (lhs.escape_to_heap != rhs.escape_to_heap) {
    return lhs.escape_to_heap < rhs.escape_to_heap;
  }
  if (lhs.escape_profile_is_normalized != rhs.escape_profile_is_normalized) {
    return lhs.escape_profile_is_normalized < rhs.escape_profile_is_normalized;
  }
  if (lhs.has_count_mismatch != rhs.has_count_mismatch) {
    return lhs.has_count_mismatch < rhs.has_count_mismatch;
  }
  if (lhs.line != rhs.line) {
    return lhs.line < rhs.line;
  }
  return lhs.column < rhs.column;
}

static std::vector<Objc3BlockStorageEscapeSiteMetadata>
BuildBlockStorageEscapeSiteMetadataLexicographic(const Objc3Program &ast) {
  std::vector<Objc3BlockStorageEscapeSiteMetadata> sites;
  for (const auto &global : ast.globals) {
    CollectBlockStorageEscapeSiteMetadataFromExpr(global.value.get(), sites);
  }
  for (const auto &fn : ast.functions) {
    CollectBlockStorageEscapeSiteMetadataFromStatements(fn.body, sites);
  }
  std::sort(sites.begin(), sites.end(), IsBlockStorageEscapeSiteMetadataLess);
  return sites;
}

static Objc3BlockStorageEscapeSemanticsSummary BuildBlockStorageEscapeSemanticsSummaryFromSites(
    const std::vector<Objc3BlockStorageEscapeSiteMetadata> &sites) {
  Objc3BlockStorageEscapeSemanticsSummary summary;
  summary.block_literal_sites = sites.size();
  for (const auto &site : sites) {
    summary.mutable_capture_count_total += site.mutable_capture_count;
    summary.byref_slot_count_total += site.byref_slot_count;
    summary.parameter_entries_total += site.parameter_count;
    summary.capture_entries_total += site.capture_count;
    summary.body_statement_entries_total += site.body_statement_count;
    if (site.requires_byref_cells) {
      ++summary.requires_byref_cells_sites;
    }
    if (site.escape_analysis_enabled) {
      ++summary.escape_analysis_enabled_sites;
    }
    if (site.escape_to_heap) {
      ++summary.escape_to_heap_sites;
    }
    if (site.escape_profile_is_normalized) {
      ++summary.escape_profile_normalized_sites;
    }
    if (!site.byref_layout_symbol.empty()) {
      ++summary.byref_layout_symbolized_sites;
    }
    const bool escape_profile_missing = site.escape_analysis_enabled && site.escape_profile.empty();
    const bool byref_layout_symbol_missing = site.requires_byref_cells && site.byref_layout_symbol.empty();
    const bool byref_requirement_mismatch = site.requires_byref_cells != (site.byref_slot_count > 0u);
    const bool count_mismatch = site.mutable_capture_count > site.capture_count ||
                                site.byref_slot_count > site.mutable_capture_count;
    const bool site_contract_violation =
        site.has_count_mismatch || count_mismatch || !site.escape_analysis_enabled ||
        !site.escape_profile_is_normalized || escape_profile_missing ||
        byref_layout_symbol_missing || byref_requirement_mismatch;
    if (site_contract_violation) {
      ++summary.contract_violation_sites;
    }
  }
  summary.deterministic =
      summary.contract_violation_sites == 0u &&
      summary.requires_byref_cells_sites <= summary.block_literal_sites &&
      summary.escape_analysis_enabled_sites <= summary.block_literal_sites &&
      summary.escape_to_heap_sites <= summary.block_literal_sites &&
      summary.escape_profile_normalized_sites <= summary.block_literal_sites &&
      summary.byref_layout_symbolized_sites <= summary.block_literal_sites &&
      summary.contract_violation_sites <= summary.block_literal_sites &&
      summary.mutable_capture_count_total <= summary.capture_entries_total &&
      summary.byref_slot_count_total <= summary.mutable_capture_count_total &&
      summary.escape_analysis_enabled_sites == summary.block_literal_sites &&
      summary.byref_layout_symbolized_sites == summary.requires_byref_cells_sites;
  return summary;
}

static Objc3BlockStorageEscapeSemanticsSummary BuildBlockStorageEscapeSemanticsSummaryFromIntegrationSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  return BuildBlockStorageEscapeSemanticsSummaryFromSites(
      surface.block_storage_escape_sites_lexicographic);
}

static Objc3BlockStorageEscapeSemanticsSummary BuildBlockStorageEscapeSemanticsSummaryFromTypeMetadataHandoff(
    const Objc3SemanticTypeMetadataHandoff &handoff) {
  return BuildBlockStorageEscapeSemanticsSummaryFromSites(
      handoff.block_storage_escape_sites_lexicographic);
}

static Objc3BlockCopyDisposeSiteMetadata BuildBlockCopyDisposeSiteMetadata(const Expr &expr) {
  auto build_block_copy_dispose_profile = [](bool copy_helper_required,
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
  };
  auto build_block_copy_helper_symbol = [](unsigned line,
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
  };
  auto build_block_dispose_helper_symbol = [](unsigned line,
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
  };
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
  metadata.copy_helper_required =
      expr.block_runtime_capture_ownership_is_normalized
          ? expr.block_runtime_copy_helper_required
          : metadata.byref_slot_count > 0u;
  metadata.dispose_helper_required =
      expr.block_runtime_capture_ownership_is_normalized
          ? expr.block_runtime_dispose_helper_required
          : metadata.byref_slot_count > 0u ||
                metadata.explicit_move_capture_count > 0u;
  metadata.copy_dispose_profile_is_normalized =
      expr.block_source_storage_annotations_are_normalized &&
      (!expr.block_runtime_capture_ownership_is_normalized ||
       !expr.block_runtime_capture_ownership_profile.empty());
  metadata.ownership_helper_eligibility_is_normalized =
      expr.block_runtime_capture_ownership_is_normalized;
  metadata.copy_dispose_profile =
      expr.block_runtime_capture_ownership_is_normalized &&
              !expr.block_runtime_capture_ownership_profile.empty()
          ? expr.block_runtime_capture_ownership_profile + ";" +
                build_block_copy_dispose_profile(
                    metadata.copy_helper_required,
                    metadata.dispose_helper_required,
                    expr.block_escape_shape_promotes_to_heap_candidate,
                    metadata.body_statement_count,
                    metadata.explicit_move_capture_count)
          : build_block_copy_dispose_profile(
                metadata.copy_helper_required,
                metadata.dispose_helper_required,
                expr.block_escape_shape_promotes_to_heap_candidate,
                metadata.body_statement_count,
                metadata.explicit_move_capture_count);
  metadata.copy_helper_symbol =
      metadata.copy_helper_required
          ? build_block_copy_helper_symbol(expr.line,
                                           expr.column,
                                           metadata.mutable_capture_count,
                                           metadata.byref_slot_count,
                                           expr.block_escape_shape_promotes_to_heap_candidate)
          : "";
  metadata.dispose_helper_symbol =
      metadata.dispose_helper_required
          ? build_block_dispose_helper_symbol(expr.line,
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

static void CollectBlockCopyDisposeSiteMetadataFromExpr(
    const Expr *expr,
    std::vector<Objc3BlockCopyDisposeSiteMetadata> &sites) {
  if (expr == nullptr) {
    return;
  }

  if (expr->kind == Expr::Kind::BlockLiteral) {
    sites.push_back(BuildBlockCopyDisposeSiteMetadata(*expr));
  }

  CollectBlockCopyDisposeSiteMetadataFromExpr(expr->receiver.get(), sites);
  CollectBlockCopyDisposeSiteMetadataFromExpr(expr->left.get(), sites);
  CollectBlockCopyDisposeSiteMetadataFromExpr(expr->right.get(), sites);
  CollectBlockCopyDisposeSiteMetadataFromExpr(expr->third.get(), sites);
  for (const auto &arg : expr->args) {
    CollectBlockCopyDisposeSiteMetadataFromExpr(arg.get(), sites);
  }
}

static void CollectBlockCopyDisposeSiteMetadataFromStatements(
    const std::vector<std::unique_ptr<Stmt>> &statements,
    std::vector<Objc3BlockCopyDisposeSiteMetadata> &sites);

static void CollectBlockCopyDisposeSiteMetadataFromForClause(
    const ForClause &clause,
    std::vector<Objc3BlockCopyDisposeSiteMetadata> &sites) {
  CollectBlockCopyDisposeSiteMetadataFromExpr(clause.value.get(), sites);
}

static void CollectBlockCopyDisposeSiteMetadataFromStatement(
    const Stmt *stmt,
    std::vector<Objc3BlockCopyDisposeSiteMetadata> &sites) {
  if (stmt == nullptr) {
    return;
  }

  switch (stmt->kind) {
    case Stmt::Kind::Let:
      CollectBlockCopyDisposeSiteMetadataFromExpr(stmt->let_stmt->value.get(), sites);
      return;
    case Stmt::Kind::Assign:
      CollectBlockCopyDisposeSiteMetadataFromExpr(stmt->assign_stmt->value.get(), sites);
      return;
    case Stmt::Kind::Return:
      CollectBlockCopyDisposeSiteMetadataFromExpr(stmt->return_stmt->value.get(), sites);
      return;
    case Stmt::Kind::If:
      CollectBlockCopyDisposeSiteMetadataFromExpr(stmt->if_stmt->condition.get(), sites);
      CollectBlockCopyDisposeSiteMetadataFromStatements(stmt->if_stmt->then_body, sites);
      CollectBlockCopyDisposeSiteMetadataFromStatements(stmt->if_stmt->else_body, sites);
      return;
    case Stmt::Kind::DoWhile:
      CollectBlockCopyDisposeSiteMetadataFromStatements(stmt->do_while_stmt->body, sites);
      CollectBlockCopyDisposeSiteMetadataFromExpr(stmt->do_while_stmt->condition.get(), sites);
      return;
    case Stmt::Kind::For:
      CollectBlockCopyDisposeSiteMetadataFromForClause(stmt->for_stmt->init, sites);
      CollectBlockCopyDisposeSiteMetadataFromExpr(stmt->for_stmt->condition.get(), sites);
      CollectBlockCopyDisposeSiteMetadataFromForClause(stmt->for_stmt->step, sites);
      CollectBlockCopyDisposeSiteMetadataFromStatements(stmt->for_stmt->body, sites);
      return;
    case Stmt::Kind::Switch:
      CollectBlockCopyDisposeSiteMetadataFromExpr(stmt->switch_stmt->condition.get(), sites);
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        CollectBlockCopyDisposeSiteMetadataFromStatements(switch_case.body, sites);
      }
      return;
    case Stmt::Kind::While:
      CollectBlockCopyDisposeSiteMetadataFromExpr(stmt->while_stmt->condition.get(), sites);
      CollectBlockCopyDisposeSiteMetadataFromStatements(stmt->while_stmt->body, sites);
      return;
    case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
      CollectBlockCopyDisposeSiteMetadataFromStatements(stmt->block_stmt->body, sites);
      return;
    case Stmt::Kind::Expr:
      CollectBlockCopyDisposeSiteMetadataFromExpr(stmt->expr_stmt->value.get(), sites);
      return;
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return;
  }
}

static void CollectBlockCopyDisposeSiteMetadataFromStatements(
    const std::vector<std::unique_ptr<Stmt>> &statements,
    std::vector<Objc3BlockCopyDisposeSiteMetadata> &sites) {
  for (const auto &statement : statements) {
    CollectBlockCopyDisposeSiteMetadataFromStatement(statement.get(), sites);
  }
}

static bool IsBlockCopyDisposeSiteMetadataLess(
    const Objc3BlockCopyDisposeSiteMetadata &lhs,
    const Objc3BlockCopyDisposeSiteMetadata &rhs) {
  if (lhs.copy_dispose_profile != rhs.copy_dispose_profile) {
    return lhs.copy_dispose_profile < rhs.copy_dispose_profile;
  }
  if (lhs.copy_helper_symbol != rhs.copy_helper_symbol) {
    return lhs.copy_helper_symbol < rhs.copy_helper_symbol;
  }
  if (lhs.dispose_helper_symbol != rhs.dispose_helper_symbol) {
    return lhs.dispose_helper_symbol < rhs.dispose_helper_symbol;
  }
  if (lhs.mutable_capture_count != rhs.mutable_capture_count) {
    return lhs.mutable_capture_count < rhs.mutable_capture_count;
  }
  if (lhs.byref_slot_count != rhs.byref_slot_count) {
    return lhs.byref_slot_count < rhs.byref_slot_count;
  }
  if (lhs.parameter_count != rhs.parameter_count) {
    return lhs.parameter_count < rhs.parameter_count;
  }
  if (lhs.capture_count != rhs.capture_count) {
    return lhs.capture_count < rhs.capture_count;
  }
  if (lhs.body_statement_count != rhs.body_statement_count) {
    return lhs.body_statement_count < rhs.body_statement_count;
  }
  if (lhs.explicit_move_capture_count != rhs.explicit_move_capture_count) {
    return lhs.explicit_move_capture_count < rhs.explicit_move_capture_count;
  }
  if (lhs.copy_helper_required != rhs.copy_helper_required) {
    return lhs.copy_helper_required < rhs.copy_helper_required;
  }
  if (lhs.dispose_helper_required != rhs.dispose_helper_required) {
    return lhs.dispose_helper_required < rhs.dispose_helper_required;
  }
  if (lhs.copy_dispose_profile_is_normalized != rhs.copy_dispose_profile_is_normalized) {
    return lhs.copy_dispose_profile_is_normalized < rhs.copy_dispose_profile_is_normalized;
  }
  if (lhs.has_count_mismatch != rhs.has_count_mismatch) {
    return lhs.has_count_mismatch < rhs.has_count_mismatch;
  }
  if (lhs.line != rhs.line) {
    return lhs.line < rhs.line;
  }
  return lhs.column < rhs.column;
}

static std::vector<Objc3BlockCopyDisposeSiteMetadata>
BuildBlockCopyDisposeSiteMetadataLexicographic(const Objc3Program &ast) {
  std::vector<Objc3BlockCopyDisposeSiteMetadata> sites;
  for (const auto &global : ast.globals) {
    CollectBlockCopyDisposeSiteMetadataFromExpr(global.value.get(), sites);
  }
  for (const auto &fn : ast.functions) {
    CollectBlockCopyDisposeSiteMetadataFromStatements(fn.body, sites);
  }
  std::sort(sites.begin(), sites.end(), IsBlockCopyDisposeSiteMetadataLess);
  return sites;
}

static Objc3BlockCopyDisposeSemanticsSummary BuildBlockCopyDisposeSemanticsSummaryFromSites(
    const std::vector<Objc3BlockCopyDisposeSiteMetadata> &sites) {
  Objc3BlockCopyDisposeSemanticsSummary summary;
  summary.block_literal_sites = sites.size();
  for (const auto &site : sites) {
    summary.mutable_capture_count_total += site.mutable_capture_count;
    summary.byref_slot_count_total += site.byref_slot_count;
    summary.parameter_entries_total += site.parameter_count;
    summary.capture_entries_total += site.capture_count;
    summary.body_statement_entries_total += site.body_statement_count;
    if (site.copy_helper_required) {
      ++summary.copy_helper_required_sites;
    }
    if (site.dispose_helper_required) {
      ++summary.dispose_helper_required_sites;
    }
    if (site.copy_dispose_profile_is_normalized) {
      ++summary.profile_normalized_sites;
    }
    if (!site.copy_helper_symbol.empty()) {
      ++summary.copy_helper_symbolized_sites;
    }
    if (!site.dispose_helper_symbol.empty()) {
      ++summary.dispose_helper_symbolized_sites;
    }
    const bool copy_dispose_profile_missing =
        (site.copy_helper_required || site.dispose_helper_required) && site.copy_dispose_profile.empty();
    const bool copy_helper_symbol_missing = site.copy_helper_required && site.copy_helper_symbol.empty();
    const bool dispose_helper_symbol_missing = site.dispose_helper_required && site.dispose_helper_symbol.empty();
    const bool copy_helper_requirement_mismatch =
        site.copy_helper_required !=
        (site.byref_slot_count > 0u || site.owned_object_capture_count > 0u);
    const bool dispose_helper_requirement_mismatch =
        site.dispose_helper_required !=
        (site.byref_slot_count > 0u || site.owned_object_capture_count > 0u ||
         site.explicit_move_capture_count > 0u);
    const bool count_mismatch = site.mutable_capture_count > site.capture_count ||
                                site.byref_slot_count > site.mutable_capture_count;
    const bool site_contract_violation =
        site.has_count_mismatch || count_mismatch || !site.copy_dispose_profile_is_normalized ||
        !site.ownership_helper_eligibility_is_normalized ||
        copy_dispose_profile_missing || copy_helper_symbol_missing ||
        dispose_helper_symbol_missing || copy_helper_requirement_mismatch ||
        dispose_helper_requirement_mismatch;
    if (site_contract_violation) {
      ++summary.contract_violation_sites;
    }
  }
  summary.deterministic =
      summary.contract_violation_sites == 0u &&
      summary.copy_helper_required_sites <= summary.block_literal_sites &&
      summary.dispose_helper_required_sites <= summary.block_literal_sites &&
      summary.profile_normalized_sites <= summary.block_literal_sites &&
      summary.copy_helper_symbolized_sites <= summary.block_literal_sites &&
      summary.dispose_helper_symbolized_sites <= summary.block_literal_sites &&
      summary.contract_violation_sites <= summary.block_literal_sites &&
      summary.mutable_capture_count_total <= summary.capture_entries_total &&
      summary.byref_slot_count_total <= summary.mutable_capture_count_total &&
      summary.copy_helper_required_sites <= summary.dispose_helper_required_sites;
  return summary;
}

static Objc3BlockCopyDisposeSemanticsSummary BuildBlockCopyDisposeSemanticsSummaryFromIntegrationSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  return BuildBlockCopyDisposeSemanticsSummaryFromSites(
      surface.block_copy_dispose_sites_lexicographic);
}

static Objc3BlockCopyDisposeSemanticsSummary BuildBlockCopyDisposeSemanticsSummaryFromTypeMetadataHandoff(
    const Objc3SemanticTypeMetadataHandoff &handoff) {
  return BuildBlockCopyDisposeSemanticsSummaryFromSites(
      handoff.block_copy_dispose_sites_lexicographic);
}

static Objc3BlockDeterminismPerfBaselineSiteMetadata BuildBlockDeterminismPerfBaselineSiteMetadata(const Expr &expr) {
  Objc3BlockDeterminismPerfBaselineSiteMetadata metadata;
  metadata.parameter_count = expr.block_parameter_count;
  metadata.capture_count = expr.block_capture_count;
  metadata.body_statement_count = expr.block_body_statement_count;
  metadata.baseline_weight = expr.block_determinism_perf_baseline_weight;
  metadata.capture_set_deterministic = expr.block_capture_set_deterministic;
  metadata.baseline_profile_is_normalized =
      expr.block_determinism_perf_baseline_profile_is_normalized;
  metadata.baseline_profile = expr.block_determinism_perf_baseline_profile;
  metadata.line = expr.line;
  metadata.column = expr.column;
  return metadata;
}

static void CollectBlockDeterminismPerfBaselineSiteMetadataFromExpr(
    const Expr *expr,
    std::vector<Objc3BlockDeterminismPerfBaselineSiteMetadata> &sites) {
  if (expr == nullptr) {
    return;
  }

  if (expr->kind == Expr::Kind::BlockLiteral) {
    sites.push_back(BuildBlockDeterminismPerfBaselineSiteMetadata(*expr));
  }

  CollectBlockDeterminismPerfBaselineSiteMetadataFromExpr(expr->receiver.get(), sites);
  CollectBlockDeterminismPerfBaselineSiteMetadataFromExpr(expr->left.get(), sites);
  CollectBlockDeterminismPerfBaselineSiteMetadataFromExpr(expr->right.get(), sites);
  CollectBlockDeterminismPerfBaselineSiteMetadataFromExpr(expr->third.get(), sites);
  for (const auto &arg : expr->args) {
    CollectBlockDeterminismPerfBaselineSiteMetadataFromExpr(arg.get(), sites);
  }
}

static void CollectBlockDeterminismPerfBaselineSiteMetadataFromStatements(
    const std::vector<std::unique_ptr<Stmt>> &statements,
    std::vector<Objc3BlockDeterminismPerfBaselineSiteMetadata> &sites);

static void CollectBlockDeterminismPerfBaselineSiteMetadataFromForClause(
    const ForClause &clause,
    std::vector<Objc3BlockDeterminismPerfBaselineSiteMetadata> &sites) {
  CollectBlockDeterminismPerfBaselineSiteMetadataFromExpr(clause.value.get(), sites);
}

static void CollectBlockDeterminismPerfBaselineSiteMetadataFromStatement(
    const Stmt *stmt,
    std::vector<Objc3BlockDeterminismPerfBaselineSiteMetadata> &sites) {
  if (stmt == nullptr) {
    return;
  }

  switch (stmt->kind) {
    case Stmt::Kind::Let:
      CollectBlockDeterminismPerfBaselineSiteMetadataFromExpr(stmt->let_stmt->value.get(), sites);
      return;
    case Stmt::Kind::Assign:
      CollectBlockDeterminismPerfBaselineSiteMetadataFromExpr(stmt->assign_stmt->value.get(), sites);
      return;
    case Stmt::Kind::Return:
      CollectBlockDeterminismPerfBaselineSiteMetadataFromExpr(stmt->return_stmt->value.get(), sites);
      return;
    case Stmt::Kind::If:
      CollectBlockDeterminismPerfBaselineSiteMetadataFromExpr(stmt->if_stmt->condition.get(), sites);
      CollectBlockDeterminismPerfBaselineSiteMetadataFromStatements(stmt->if_stmt->then_body, sites);
      CollectBlockDeterminismPerfBaselineSiteMetadataFromStatements(stmt->if_stmt->else_body, sites);
      return;
    case Stmt::Kind::DoWhile:
      CollectBlockDeterminismPerfBaselineSiteMetadataFromStatements(stmt->do_while_stmt->body, sites);
      CollectBlockDeterminismPerfBaselineSiteMetadataFromExpr(stmt->do_while_stmt->condition.get(), sites);
      return;
    case Stmt::Kind::For:
      CollectBlockDeterminismPerfBaselineSiteMetadataFromForClause(stmt->for_stmt->init, sites);
      CollectBlockDeterminismPerfBaselineSiteMetadataFromExpr(stmt->for_stmt->condition.get(), sites);
      CollectBlockDeterminismPerfBaselineSiteMetadataFromForClause(stmt->for_stmt->step, sites);
      CollectBlockDeterminismPerfBaselineSiteMetadataFromStatements(stmt->for_stmt->body, sites);
      return;
    case Stmt::Kind::Switch:
      CollectBlockDeterminismPerfBaselineSiteMetadataFromExpr(stmt->switch_stmt->condition.get(), sites);
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        CollectBlockDeterminismPerfBaselineSiteMetadataFromStatements(switch_case.body, sites);
      }
      return;
    case Stmt::Kind::While:
      CollectBlockDeterminismPerfBaselineSiteMetadataFromExpr(stmt->while_stmt->condition.get(), sites);
      CollectBlockDeterminismPerfBaselineSiteMetadataFromStatements(stmt->while_stmt->body, sites);
      return;
    case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
      CollectBlockDeterminismPerfBaselineSiteMetadataFromStatements(stmt->block_stmt->body, sites);
      return;
    case Stmt::Kind::Expr:
      CollectBlockDeterminismPerfBaselineSiteMetadataFromExpr(stmt->expr_stmt->value.get(), sites);
      return;
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return;
  }
}

static void CollectBlockDeterminismPerfBaselineSiteMetadataFromStatements(
    const std::vector<std::unique_ptr<Stmt>> &statements,
    std::vector<Objc3BlockDeterminismPerfBaselineSiteMetadata> &sites) {
  for (const auto &statement : statements) {
    CollectBlockDeterminismPerfBaselineSiteMetadataFromStatement(statement.get(), sites);
  }
}

static bool IsBlockDeterminismPerfBaselineSiteMetadataLess(
    const Objc3BlockDeterminismPerfBaselineSiteMetadata &lhs,
    const Objc3BlockDeterminismPerfBaselineSiteMetadata &rhs) {
  if (lhs.baseline_profile != rhs.baseline_profile) {
    return lhs.baseline_profile < rhs.baseline_profile;
  }
  if (lhs.baseline_weight != rhs.baseline_weight) {
    return lhs.baseline_weight < rhs.baseline_weight;
  }
  if (lhs.parameter_count != rhs.parameter_count) {
    return lhs.parameter_count < rhs.parameter_count;
  }
  if (lhs.capture_count != rhs.capture_count) {
    return lhs.capture_count < rhs.capture_count;
  }
  if (lhs.body_statement_count != rhs.body_statement_count) {
    return lhs.body_statement_count < rhs.body_statement_count;
  }
  if (lhs.capture_set_deterministic != rhs.capture_set_deterministic) {
    return lhs.capture_set_deterministic < rhs.capture_set_deterministic;
  }
  if (lhs.baseline_profile_is_normalized != rhs.baseline_profile_is_normalized) {
    return lhs.baseline_profile_is_normalized < rhs.baseline_profile_is_normalized;
  }
  if (lhs.line != rhs.line) {
    return lhs.line < rhs.line;
  }
  return lhs.column < rhs.column;
}

static std::vector<Objc3BlockDeterminismPerfBaselineSiteMetadata>
BuildBlockDeterminismPerfBaselineSiteMetadataLexicographic(const Objc3Program &ast) {
  std::vector<Objc3BlockDeterminismPerfBaselineSiteMetadata> sites;
  for (const auto &global : ast.globals) {
    CollectBlockDeterminismPerfBaselineSiteMetadataFromExpr(global.value.get(), sites);
  }
  for (const auto &fn : ast.functions) {
    CollectBlockDeterminismPerfBaselineSiteMetadataFromStatements(fn.body, sites);
  }
  std::sort(sites.begin(), sites.end(), IsBlockDeterminismPerfBaselineSiteMetadataLess);
  return sites;
}

static Objc3BlockDeterminismPerfBaselineSummary BuildBlockDeterminismPerfBaselineSummaryFromSites(
    const std::vector<Objc3BlockDeterminismPerfBaselineSiteMetadata> &sites) {
  Objc3BlockDeterminismPerfBaselineSummary summary;
  summary.block_literal_sites = sites.size();
  for (const auto &site : sites) {
    summary.baseline_weight_total += site.baseline_weight;
    summary.parameter_entries_total += site.parameter_count;
    summary.capture_entries_total += site.capture_count;
    summary.body_statement_entries_total += site.body_statement_count;
    if (site.capture_set_deterministic) {
      ++summary.deterministic_capture_sites;
    }
    if (site.baseline_profile.find(";tier=heavy") != std::string::npos) {
      ++summary.heavy_tier_sites;
    }
    if (site.baseline_profile_is_normalized) {
      ++summary.normalized_profile_sites;
    }
    const bool profile_missing = site.baseline_profile.empty();
    const bool weight_underflow = site.baseline_weight < site.capture_count;
    const bool site_contract_violation =
        !site.capture_set_deterministic || !site.baseline_profile_is_normalized ||
        profile_missing || weight_underflow;
    if (site_contract_violation) {
      ++summary.contract_violation_sites;
    }
  }
  summary.deterministic =
      summary.contract_violation_sites == 0u &&
      summary.deterministic_capture_sites <= summary.block_literal_sites &&
      summary.heavy_tier_sites <= summary.block_literal_sites &&
      summary.normalized_profile_sites <= summary.block_literal_sites &&
      summary.contract_violation_sites <= summary.block_literal_sites;
  return summary;
}

static Objc3BlockDeterminismPerfBaselineSummary BuildBlockDeterminismPerfBaselineSummaryFromIntegrationSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  return BuildBlockDeterminismPerfBaselineSummaryFromSites(
      surface.block_determinism_perf_baseline_sites_lexicographic);
}

static Objc3BlockDeterminismPerfBaselineSummary BuildBlockDeterminismPerfBaselineSummaryFromTypeMetadataHandoff(
    const Objc3SemanticTypeMetadataHandoff &handoff) {
  return BuildBlockDeterminismPerfBaselineSummaryFromSites(
      handoff.block_determinism_perf_baseline_sites_lexicographic);
}

static Objc3MessageSendSelectorLoweringSiteMetadata BuildMessageSendSelectorLoweringSiteMetadata(const Expr &expr) {
  Objc3MessageSendSelectorLoweringSiteMetadata metadata;
  metadata.selector = expr.selector;
  metadata.selector_lowering_symbol =
      expr.selector_lowering_symbol.empty() ? expr.selector : expr.selector_lowering_symbol;
  metadata.argument_count = expr.args.size();
  const Expr::MessageSendForm resolved_form = ResolveMessageSendForm(expr);
  metadata.unary_form = resolved_form == Expr::MessageSendForm::Unary;
  metadata.keyword_form = resolved_form == Expr::MessageSendForm::Keyword;
  metadata.line = expr.line;
  metadata.column = expr.column;

  if (!expr.selector_lowering_pieces.empty()) {
    metadata.selector_piece_count = expr.selector_lowering_pieces.size();
    for (const Expr::MessageSendSelectorPiece &piece : expr.selector_lowering_pieces) {
      if (piece.has_argument) {
        ++metadata.selector_argument_piece_count;
      }
    }
  } else if (metadata.keyword_form) {
    const std::string selector_symbol =
        metadata.selector_lowering_symbol.empty() ? metadata.selector : metadata.selector_lowering_symbol;
    metadata.selector_piece_count = CountSelectorKeywordPieces(selector_symbol);
    metadata.selector_argument_piece_count = metadata.selector_piece_count;
  }

  metadata.selector_lowering_is_normalized =
      expr.selector_lowering_is_normalized ||
      (!metadata.selector.empty() && !metadata.selector_lowering_symbol.empty() &&
       metadata.selector == metadata.selector_lowering_symbol);
  metadata.receiver_is_nil_literal = expr.receiver != nullptr && expr.receiver->kind == Expr::Kind::NilLiteral;
  const objc3c::support::Objc3NilReceiverProfile nil_receiver_profile =
      objc3c::support::BuildNilReceiverProfile(
          metadata.receiver_is_nil_literal,
          expr.nil_receiver_semantics_is_normalized,
          expr.nil_receiver_semantics_enabled, expr.nil_receiver_foldable,
          expr.nil_receiver_requires_runtime_dispatch);
  metadata.nil_receiver_semantics_enabled =
      nil_receiver_profile.semantics_enabled;
  metadata.nil_receiver_foldable = nil_receiver_profile.foldable;
  metadata.nil_receiver_requires_runtime_dispatch =
      nil_receiver_profile.requires_runtime_dispatch;
  metadata.nil_receiver_semantics_is_normalized =
      nil_receiver_profile.normalized;
  metadata.runtime_link_host_link_required =
      expr.runtime_link_host_link_is_normalized ? expr.runtime_link_host_link_required
                                                : metadata.nil_receiver_requires_runtime_dispatch;
  metadata.runtime_link_host_link_elided =
      expr.runtime_link_host_link_is_normalized ? expr.runtime_link_host_link_elided
                                                : !metadata.runtime_link_host_link_required;
  metadata.runtime_link_host_link_runtime_dispatch_arg_slots = expr.dispatch_abi_runtime_arg_slots;
  metadata.runtime_link_host_link_declaration_parameter_count =
      expr.runtime_link_host_link_is_normalized
          ? static_cast<std::size_t>(expr.runtime_link_host_link_declaration_parameter_count)
          : metadata.runtime_link_host_link_runtime_dispatch_arg_slots + 2u;
  metadata.runtime_dispatch_bridge_symbol = expr.runtime_dispatch_bridge_symbol.empty()
                                                ? kObjc3RuntimeLinkHostLinkDefaultDispatchSymbol
                                                : expr.runtime_dispatch_bridge_symbol;
  metadata.runtime_link_host_link_symbol = expr.runtime_link_host_link_symbol;
  metadata.runtime_link_host_link_is_normalized =
      expr.runtime_link_host_link_is_normalized ||
      (metadata.runtime_link_host_link_required == metadata.nil_receiver_requires_runtime_dispatch &&
       metadata.runtime_link_host_link_elided == !metadata.runtime_link_host_link_required &&
       metadata.runtime_link_host_link_declaration_parameter_count ==
           metadata.runtime_link_host_link_runtime_dispatch_arg_slots + 2u &&
       !metadata.runtime_dispatch_bridge_symbol.empty());
  metadata.receiver_is_super_identifier =
      expr.receiver != nullptr && expr.receiver->kind == Expr::Kind::Identifier && expr.receiver->ident == "super";
  const objc3c::support::Objc3SuperDispatchProfile super_dispatch_profile =
      objc3c::support::BuildSuperDispatchProfile(
          metadata.receiver_is_super_identifier,
          expr.super_dispatch_semantics_is_normalized,
          expr.super_dispatch_enabled,
          expr.super_dispatch_requires_class_context);
  metadata.super_dispatch_enabled = super_dispatch_profile.enabled;
  metadata.super_dispatch_requires_class_context =
      super_dispatch_profile.requires_class_context;
  metadata.super_dispatch_semantics_is_normalized =
      super_dispatch_profile.normalized;
  const objc3c::support::Objc3MethodFamilyProfile method_family_profile =
      objc3c::support::BuildMethodFamilyProfile(
          metadata.selector, expr.method_family_semantics_is_normalized,
          expr.method_family_name, expr.method_family_returns_retained_result,
          expr.method_family_returns_related_result);
  metadata.method_family_name = method_family_profile.name;
  metadata.method_family_returns_retained_result =
      method_family_profile.returns_retained_result;
  metadata.method_family_returns_related_result =
      method_family_profile.returns_related_result;
  metadata.method_family_semantics_is_normalized =
      method_family_profile.normalized;
  return metadata;
}

static void CollectMessageSendSelectorLoweringSiteMetadataFromExpr(
    const Expr *expr, std::vector<Objc3MessageSendSelectorLoweringSiteMetadata> &sites) {
  if (expr == nullptr) {
    return;
  }

  if (expr->kind == Expr::Kind::MessageSend) {
    sites.push_back(BuildMessageSendSelectorLoweringSiteMetadata(*expr));
    CollectMessageSendSelectorLoweringSiteMetadataFromExpr(expr->receiver.get(), sites);
    for (const auto &arg : expr->args) {
      CollectMessageSendSelectorLoweringSiteMetadataFromExpr(arg.get(), sites);
    }
    return;
  }

  if (expr->kind == Expr::Kind::Binary) {
    CollectMessageSendSelectorLoweringSiteMetadataFromExpr(expr->left.get(), sites);
    CollectMessageSendSelectorLoweringSiteMetadataFromExpr(expr->right.get(), sites);
    return;
  }

  if (expr->kind == Expr::Kind::Conditional) {
    CollectMessageSendSelectorLoweringSiteMetadataFromExpr(expr->left.get(), sites);
    CollectMessageSendSelectorLoweringSiteMetadataFromExpr(expr->right.get(), sites);
    CollectMessageSendSelectorLoweringSiteMetadataFromExpr(expr->third.get(), sites);
    return;
  }

  if (expr->kind == Expr::Kind::Call) {
    for (const auto &arg : expr->args) {
      CollectMessageSendSelectorLoweringSiteMetadataFromExpr(arg.get(), sites);
    }
  }
}

static void CollectMessageSendSelectorLoweringSiteMetadataFromStatements(
    const std::vector<std::unique_ptr<Stmt>> &statements,
    std::vector<Objc3MessageSendSelectorLoweringSiteMetadata> &sites);

static void CollectMessageSendSelectorLoweringSiteMetadataFromForClause(
    const ForClause &clause,
    std::vector<Objc3MessageSendSelectorLoweringSiteMetadata> &sites) {
  CollectMessageSendSelectorLoweringSiteMetadataFromExpr(clause.value.get(), sites);
}

static void CollectMessageSendSelectorLoweringSiteMetadataFromStatement(
    const Stmt *stmt, std::vector<Objc3MessageSendSelectorLoweringSiteMetadata> &sites) {
  if (stmt == nullptr) {
    return;
  }

  switch (stmt->kind) {
    case Stmt::Kind::Let:
      CollectMessageSendSelectorLoweringSiteMetadataFromExpr(stmt->let_stmt->value.get(), sites);
      return;
    case Stmt::Kind::Assign:
      CollectMessageSendSelectorLoweringSiteMetadataFromExpr(stmt->assign_stmt->value.get(), sites);
      return;
    case Stmt::Kind::Return:
      CollectMessageSendSelectorLoweringSiteMetadataFromExpr(stmt->return_stmt->value.get(), sites);
      return;
    case Stmt::Kind::If:
      CollectMessageSendSelectorLoweringSiteMetadataFromExpr(stmt->if_stmt->condition.get(), sites);
      CollectMessageSendSelectorLoweringSiteMetadataFromStatements(stmt->if_stmt->then_body, sites);
      CollectMessageSendSelectorLoweringSiteMetadataFromStatements(stmt->if_stmt->else_body, sites);
      return;
    case Stmt::Kind::DoWhile:
      CollectMessageSendSelectorLoweringSiteMetadataFromStatements(stmt->do_while_stmt->body, sites);
      CollectMessageSendSelectorLoweringSiteMetadataFromExpr(stmt->do_while_stmt->condition.get(), sites);
      return;
    case Stmt::Kind::For:
      CollectMessageSendSelectorLoweringSiteMetadataFromForClause(stmt->for_stmt->init, sites);
      CollectMessageSendSelectorLoweringSiteMetadataFromExpr(stmt->for_stmt->condition.get(), sites);
      CollectMessageSendSelectorLoweringSiteMetadataFromForClause(stmt->for_stmt->step, sites);
      CollectMessageSendSelectorLoweringSiteMetadataFromStatements(stmt->for_stmt->body, sites);
      return;
    case Stmt::Kind::Switch:
      CollectMessageSendSelectorLoweringSiteMetadataFromExpr(stmt->switch_stmt->condition.get(), sites);
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        CollectMessageSendSelectorLoweringSiteMetadataFromStatements(switch_case.body, sites);
      }
      return;
    case Stmt::Kind::While:
      CollectMessageSendSelectorLoweringSiteMetadataFromExpr(stmt->while_stmt->condition.get(), sites);
      CollectMessageSendSelectorLoweringSiteMetadataFromStatements(stmt->while_stmt->body, sites);
      return;
    case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
      CollectMessageSendSelectorLoweringSiteMetadataFromStatements(stmt->block_stmt->body, sites);
      return;
    case Stmt::Kind::Expr:
      CollectMessageSendSelectorLoweringSiteMetadataFromExpr(stmt->expr_stmt->value.get(), sites);
      return;
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return;
  }
}

static void CollectMessageSendSelectorLoweringSiteMetadataFromStatements(
    const std::vector<std::unique_ptr<Stmt>> &statements,
    std::vector<Objc3MessageSendSelectorLoweringSiteMetadata> &sites) {
  for (const auto &statement : statements) {
    CollectMessageSendSelectorLoweringSiteMetadataFromStatement(statement.get(), sites);
  }
}

static bool IsMessageSendSelectorLoweringSiteMetadataLess(
    const Objc3MessageSendSelectorLoweringSiteMetadata &lhs,
    const Objc3MessageSendSelectorLoweringSiteMetadata &rhs) {
  if (lhs.selector != rhs.selector) {
    return lhs.selector < rhs.selector;
  }
  if (lhs.selector_lowering_symbol != rhs.selector_lowering_symbol) {
    return lhs.selector_lowering_symbol < rhs.selector_lowering_symbol;
  }
  if (lhs.argument_count != rhs.argument_count) {
    return lhs.argument_count < rhs.argument_count;
  }
  if (lhs.selector_piece_count != rhs.selector_piece_count) {
    return lhs.selector_piece_count < rhs.selector_piece_count;
  }
  if (lhs.selector_argument_piece_count != rhs.selector_argument_piece_count) {
    return lhs.selector_argument_piece_count < rhs.selector_argument_piece_count;
  }
  if (lhs.unary_form != rhs.unary_form) {
    return lhs.unary_form < rhs.unary_form;
  }
  if (lhs.keyword_form != rhs.keyword_form) {
    return lhs.keyword_form < rhs.keyword_form;
  }
  if (lhs.selector_lowering_is_normalized != rhs.selector_lowering_is_normalized) {
    return lhs.selector_lowering_is_normalized < rhs.selector_lowering_is_normalized;
  }
  if (lhs.receiver_is_nil_literal != rhs.receiver_is_nil_literal) {
    return lhs.receiver_is_nil_literal < rhs.receiver_is_nil_literal;
  }
  if (lhs.nil_receiver_semantics_enabled != rhs.nil_receiver_semantics_enabled) {
    return lhs.nil_receiver_semantics_enabled < rhs.nil_receiver_semantics_enabled;
  }
  if (lhs.nil_receiver_foldable != rhs.nil_receiver_foldable) {
    return lhs.nil_receiver_foldable < rhs.nil_receiver_foldable;
  }
  if (lhs.nil_receiver_requires_runtime_dispatch != rhs.nil_receiver_requires_runtime_dispatch) {
    return lhs.nil_receiver_requires_runtime_dispatch < rhs.nil_receiver_requires_runtime_dispatch;
  }
  if (lhs.nil_receiver_semantics_is_normalized != rhs.nil_receiver_semantics_is_normalized) {
    return lhs.nil_receiver_semantics_is_normalized < rhs.nil_receiver_semantics_is_normalized;
  }
  if (lhs.runtime_link_host_link_required != rhs.runtime_link_host_link_required) {
    return lhs.runtime_link_host_link_required < rhs.runtime_link_host_link_required;
  }
  if (lhs.runtime_link_host_link_elided != rhs.runtime_link_host_link_elided) {
    return lhs.runtime_link_host_link_elided < rhs.runtime_link_host_link_elided;
  }
  if (lhs.runtime_link_host_link_runtime_dispatch_arg_slots != rhs.runtime_link_host_link_runtime_dispatch_arg_slots) {
    return lhs.runtime_link_host_link_runtime_dispatch_arg_slots <
           rhs.runtime_link_host_link_runtime_dispatch_arg_slots;
  }
  if (lhs.runtime_link_host_link_declaration_parameter_count !=
      rhs.runtime_link_host_link_declaration_parameter_count) {
    return lhs.runtime_link_host_link_declaration_parameter_count <
           rhs.runtime_link_host_link_declaration_parameter_count;
  }
  if (lhs.runtime_dispatch_bridge_symbol != rhs.runtime_dispatch_bridge_symbol) {
    return lhs.runtime_dispatch_bridge_symbol < rhs.runtime_dispatch_bridge_symbol;
  }
  if (lhs.runtime_link_host_link_symbol != rhs.runtime_link_host_link_symbol) {
    return lhs.runtime_link_host_link_symbol < rhs.runtime_link_host_link_symbol;
  }
  if (lhs.runtime_link_host_link_is_normalized != rhs.runtime_link_host_link_is_normalized) {
    return lhs.runtime_link_host_link_is_normalized < rhs.runtime_link_host_link_is_normalized;
  }
  if (lhs.receiver_is_super_identifier != rhs.receiver_is_super_identifier) {
    return lhs.receiver_is_super_identifier < rhs.receiver_is_super_identifier;
  }
  if (lhs.super_dispatch_enabled != rhs.super_dispatch_enabled) {
    return lhs.super_dispatch_enabled < rhs.super_dispatch_enabled;
  }
  if (lhs.super_dispatch_requires_class_context != rhs.super_dispatch_requires_class_context) {
    return lhs.super_dispatch_requires_class_context < rhs.super_dispatch_requires_class_context;
  }
  if (lhs.super_dispatch_semantics_is_normalized != rhs.super_dispatch_semantics_is_normalized) {
    return lhs.super_dispatch_semantics_is_normalized < rhs.super_dispatch_semantics_is_normalized;
  }
  if (lhs.method_family_name != rhs.method_family_name) {
    return lhs.method_family_name < rhs.method_family_name;
  }
  if (lhs.method_family_returns_retained_result != rhs.method_family_returns_retained_result) {
    return lhs.method_family_returns_retained_result < rhs.method_family_returns_retained_result;
  }
  if (lhs.method_family_returns_related_result != rhs.method_family_returns_related_result) {
    return lhs.method_family_returns_related_result < rhs.method_family_returns_related_result;
  }
  if (lhs.method_family_semantics_is_normalized != rhs.method_family_semantics_is_normalized) {
    return lhs.method_family_semantics_is_normalized < rhs.method_family_semantics_is_normalized;
  }
  if (lhs.line != rhs.line) {
    return lhs.line < rhs.line;
  }
  return lhs.column < rhs.column;
}

static std::vector<Objc3MessageSendSelectorLoweringSiteMetadata>
BuildMessageSendSelectorLoweringSiteMetadataLexicographic(const Objc3Program &ast) {
  std::vector<Objc3MessageSendSelectorLoweringSiteMetadata> sites;
  for (const auto &global : ast.globals) {
    CollectMessageSendSelectorLoweringSiteMetadataFromExpr(global.value.get(), sites);
  }
  for (const auto &fn : ast.functions) {
    CollectMessageSendSelectorLoweringSiteMetadataFromStatements(fn.body, sites);
  }
  // method-family/runtime evidence anchor: implementation method
  // bodies must contribute selector-lowering sites so super/dynamic legality
  // and runtime-link host-link summaries stay aligned with sema parity.
  for (const auto &implementation_decl : ast.implementations) {
    for (const auto &method : implementation_decl.methods) {
      if (!method.has_body) {
        continue;
      }
      CollectMessageSendSelectorLoweringSiteMetadataFromStatements(method.body,
                                                                   sites);
    }
  }
  std::sort(sites.begin(), sites.end(), IsMessageSendSelectorLoweringSiteMetadataLess);
  return sites;
}

static Objc3AutoreleasePoolScopeSiteMetadata BuildAutoreleasePoolScopeSiteMetadata(const BlockStmt &block) {
  Objc3AutoreleasePoolScopeSiteMetadata metadata;
  metadata.scope_symbol = block.autoreleasepool_scope_symbol;
  metadata.scope_depth = block.autoreleasepool_scope_depth;
  metadata.line = block.line;
  metadata.column = block.column;
  return metadata;
}

static void CollectAutoreleasePoolScopeSiteMetadataFromStatements(
    const std::vector<std::unique_ptr<Stmt>> &statements,
    std::vector<Objc3AutoreleasePoolScopeSiteMetadata> &sites);

static void CollectAutoreleasePoolScopeSiteMetadataFromStatement(
    const Stmt *stmt, std::vector<Objc3AutoreleasePoolScopeSiteMetadata> &sites) {
  if (stmt == nullptr) {
    return;
  }

  switch (stmt->kind) {
    case Stmt::Kind::If:
      CollectAutoreleasePoolScopeSiteMetadataFromStatements(stmt->if_stmt->then_body, sites);
      CollectAutoreleasePoolScopeSiteMetadataFromStatements(stmt->if_stmt->else_body, sites);
      return;
    case Stmt::Kind::DoWhile:
      CollectAutoreleasePoolScopeSiteMetadataFromStatements(stmt->do_while_stmt->body, sites);
      return;
    case Stmt::Kind::For:
      CollectAutoreleasePoolScopeSiteMetadataFromStatements(stmt->for_stmt->body, sites);
      return;
    case Stmt::Kind::Switch:
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        CollectAutoreleasePoolScopeSiteMetadataFromStatements(switch_case.body, sites);
      }
      return;
    case Stmt::Kind::While:
      CollectAutoreleasePoolScopeSiteMetadataFromStatements(stmt->while_stmt->body, sites);
      return;
    case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
      if (stmt->block_stmt->is_autoreleasepool_scope) {
        sites.push_back(BuildAutoreleasePoolScopeSiteMetadata(*stmt->block_stmt));
      }
      CollectAutoreleasePoolScopeSiteMetadataFromStatements(stmt->block_stmt->body, sites);
      return;
    case Stmt::Kind::Let:
    case Stmt::Kind::Assign:
    case Stmt::Kind::Return:
    case Stmt::Kind::Expr:
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return;
  }
}

static void CollectAutoreleasePoolScopeSiteMetadataFromStatements(
    const std::vector<std::unique_ptr<Stmt>> &statements,
    std::vector<Objc3AutoreleasePoolScopeSiteMetadata> &sites) {
  for (const auto &statement : statements) {
    CollectAutoreleasePoolScopeSiteMetadataFromStatement(statement.get(), sites);
  }
}

static bool IsAutoreleasePoolScopeSiteMetadataLess(const Objc3AutoreleasePoolScopeSiteMetadata &lhs,
                                                   const Objc3AutoreleasePoolScopeSiteMetadata &rhs) {
  if (lhs.scope_symbol != rhs.scope_symbol) {
    return lhs.scope_symbol < rhs.scope_symbol;
  }
  if (lhs.scope_depth != rhs.scope_depth) {
    return lhs.scope_depth < rhs.scope_depth;
  }
  if (lhs.line != rhs.line) {
    return lhs.line < rhs.line;
  }
  return lhs.column < rhs.column;
}

static std::vector<Objc3AutoreleasePoolScopeSiteMetadata>
BuildAutoreleasePoolScopeSiteMetadataLexicographic(const Objc3Program &ast) {
  std::vector<Objc3AutoreleasePoolScopeSiteMetadata> sites;
  for (const auto &fn : ast.functions) {
    CollectAutoreleasePoolScopeSiteMetadataFromStatements(fn.body, sites);
  }
  std::sort(sites.begin(), sites.end(), IsAutoreleasePoolScopeSiteMetadataLess);
