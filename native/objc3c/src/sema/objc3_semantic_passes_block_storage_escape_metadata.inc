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
