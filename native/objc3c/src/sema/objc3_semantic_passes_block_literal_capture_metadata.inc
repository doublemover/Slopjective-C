static Objc3BlockLiteralCaptureSiteMetadata BuildBlockLiteralCaptureSiteMetadata(const Expr &expr) {
  Objc3BlockLiteralCaptureSiteMetadata metadata;
  metadata.parameter_count = expr.block_parameter_count;
  metadata.capture_count = expr.block_capture_count;
  metadata.body_statement_count = expr.block_body_statement_count;
  metadata.capture_set_deterministic = expr.block_capture_set_deterministic;
  metadata.literal_is_normalized = expr.block_literal_is_normalized;
  metadata.capture_profile = expr.block_capture_profile;
  metadata.line = expr.line;
  metadata.column = expr.column;
  metadata.has_count_mismatch =
      expr.block_parameter_names_lexicographic.size() != metadata.parameter_count ||
      expr.block_capture_names_lexicographic.size() != metadata.capture_count ||
      !IsSortedUniqueStrings(expr.block_parameter_names_lexicographic) ||
      !IsSortedUniqueStrings(expr.block_capture_names_lexicographic);
  return metadata;
}

static void CollectBlockLiteralCaptureSiteMetadataFromExpr(const Expr *expr,
                                                           std::vector<Objc3BlockLiteralCaptureSiteMetadata> &sites) {
  if (expr == nullptr) {
    return;
  }

  if (expr->kind == Expr::Kind::BlockLiteral) {
    sites.push_back(BuildBlockLiteralCaptureSiteMetadata(*expr));
  }

  CollectBlockLiteralCaptureSiteMetadataFromExpr(expr->receiver.get(), sites);
  CollectBlockLiteralCaptureSiteMetadataFromExpr(expr->left.get(), sites);
  CollectBlockLiteralCaptureSiteMetadataFromExpr(expr->right.get(), sites);
  CollectBlockLiteralCaptureSiteMetadataFromExpr(expr->third.get(), sites);
  for (const auto &key : expr->collection_keys) {
    CollectBlockLiteralCaptureSiteMetadataFromExpr(key.get(), sites);
  }
  for (const auto &value : expr->collection_values) {
    CollectBlockLiteralCaptureSiteMetadataFromExpr(value.get(), sites);
  }
  for (const auto &arg : expr->args) {
    CollectBlockLiteralCaptureSiteMetadataFromExpr(arg.get(), sites);
  }
}

static void CollectBlockLiteralCaptureSiteMetadataFromStatements(
    const std::vector<std::unique_ptr<Stmt>> &statements,
    std::vector<Objc3BlockLiteralCaptureSiteMetadata> &sites);

static void CollectBlockLiteralCaptureSiteMetadataFromForClause(
    const ForClause &clause,
    std::vector<Objc3BlockLiteralCaptureSiteMetadata> &sites) {
  CollectBlockLiteralCaptureSiteMetadataFromExpr(clause.value.get(), sites);
}

static void CollectBlockLiteralCaptureSiteMetadataFromStatement(
    const Stmt *stmt, std::vector<Objc3BlockLiteralCaptureSiteMetadata> &sites) {
  if (stmt == nullptr) {
    return;
  }

  switch (stmt->kind) {
    case Stmt::Kind::Let:
      CollectBlockLiteralCaptureSiteMetadataFromExpr(stmt->let_stmt->value.get(), sites);
      return;
    case Stmt::Kind::Assign:
      CollectBlockLiteralCaptureSiteMetadataFromExpr(stmt->assign_stmt->value.get(), sites);
      return;
    case Stmt::Kind::Return:
      CollectBlockLiteralCaptureSiteMetadataFromExpr(stmt->return_stmt->value.get(), sites);
      return;
    case Stmt::Kind::If:
      CollectBlockLiteralCaptureSiteMetadataFromExpr(stmt->if_stmt->condition.get(), sites);
      CollectBlockLiteralCaptureSiteMetadataFromStatements(stmt->if_stmt->then_body, sites);
      CollectBlockLiteralCaptureSiteMetadataFromStatements(stmt->if_stmt->else_body, sites);
      return;
    case Stmt::Kind::DoWhile:
      CollectBlockLiteralCaptureSiteMetadataFromStatements(stmt->do_while_stmt->body, sites);
      CollectBlockLiteralCaptureSiteMetadataFromExpr(stmt->do_while_stmt->condition.get(), sites);
      return;
    case Stmt::Kind::For:
      CollectBlockLiteralCaptureSiteMetadataFromForClause(stmt->for_stmt->init, sites);
      CollectBlockLiteralCaptureSiteMetadataFromExpr(stmt->for_stmt->condition.get(), sites);
      CollectBlockLiteralCaptureSiteMetadataFromForClause(stmt->for_stmt->step, sites);
      CollectBlockLiteralCaptureSiteMetadataFromStatements(stmt->for_stmt->body, sites);
      return;
    case Stmt::Kind::ForIn:
      CollectBlockLiteralCaptureSiteMetadataFromExpr(stmt->for_in_stmt->collection.get(), sites);
      CollectBlockLiteralCaptureSiteMetadataFromStatements(stmt->for_in_stmt->body, sites);
      return;
    case Stmt::Kind::Switch:
      CollectBlockLiteralCaptureSiteMetadataFromExpr(stmt->switch_stmt->condition.get(), sites);
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        CollectBlockLiteralCaptureSiteMetadataFromStatements(switch_case.body, sites);
      }
      return;
    case Stmt::Kind::While:
      CollectBlockLiteralCaptureSiteMetadataFromExpr(stmt->while_stmt->condition.get(), sites);
      CollectBlockLiteralCaptureSiteMetadataFromStatements(stmt->while_stmt->body, sites);
      return;
    case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
      CollectBlockLiteralCaptureSiteMetadataFromStatements(stmt->block_stmt->body, sites);
      return;
    case Stmt::Kind::Expr:
      CollectBlockLiteralCaptureSiteMetadataFromExpr(stmt->expr_stmt->value.get(), sites);
      return;
    case Stmt::Kind::CollectionMutation:
      CollectBlockLiteralCaptureSiteMetadataFromExpr(stmt->collection_mutation_stmt->key_or_index.get(), sites);
      CollectBlockLiteralCaptureSiteMetadataFromExpr(stmt->collection_mutation_stmt->value.get(), sites);
      return;
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return;
  }
}

static void CollectBlockLiteralCaptureSiteMetadataFromStatements(
    const std::vector<std::unique_ptr<Stmt>> &statements,
    std::vector<Objc3BlockLiteralCaptureSiteMetadata> &sites) {
  for (const auto &statement : statements) {
    CollectBlockLiteralCaptureSiteMetadataFromStatement(statement.get(), sites);
  }
}

static bool IsBlockLiteralCaptureSiteMetadataLess(const Objc3BlockLiteralCaptureSiteMetadata &lhs,
                                                  const Objc3BlockLiteralCaptureSiteMetadata &rhs) {
  if (lhs.capture_profile != rhs.capture_profile) {
    return lhs.capture_profile < rhs.capture_profile;
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
  if (lhs.literal_is_normalized != rhs.literal_is_normalized) {
    return lhs.literal_is_normalized < rhs.literal_is_normalized;
  }
  if (lhs.has_count_mismatch != rhs.has_count_mismatch) {
    return lhs.has_count_mismatch < rhs.has_count_mismatch;
  }
  if (lhs.line != rhs.line) {
    return lhs.line < rhs.line;
  }
  return lhs.column < rhs.column;
}

static std::vector<Objc3BlockLiteralCaptureSiteMetadata>
BuildBlockLiteralCaptureSiteMetadataLexicographic(const Objc3Program &ast) {
  std::vector<Objc3BlockLiteralCaptureSiteMetadata> sites;
  for (const auto &global : ast.globals) {
    CollectBlockLiteralCaptureSiteMetadataFromExpr(global.value.get(), sites);
  }
  for (const auto &fn : ast.functions) {
    CollectBlockLiteralCaptureSiteMetadataFromStatements(fn.body, sites);
  }
  std::sort(sites.begin(), sites.end(), IsBlockLiteralCaptureSiteMetadataLess);
  return sites;
}

static Objc3BlockLiteralCaptureSemanticsSummary BuildBlockLiteralCaptureSemanticsSummaryFromSites(
    const std::vector<Objc3BlockLiteralCaptureSiteMetadata> &sites) {
  Objc3BlockLiteralCaptureSemanticsSummary summary;
  summary.block_literal_sites = sites.size();
  for (const auto &site : sites) {
    summary.block_parameter_entries += site.parameter_count;
    summary.block_capture_entries += site.capture_count;
    summary.block_body_statement_entries += site.body_statement_count;
    if (site.capture_count == 0u) {
      ++summary.block_empty_capture_sites;
    }
    if (!site.capture_set_deterministic) {
      ++summary.block_nondeterministic_capture_sites;
    }
    if (!site.literal_is_normalized) {
      ++summary.block_non_normalized_sites;
    }
    const bool profile_missing = site.capture_count > 0u && site.capture_profile.empty();
    const bool site_contract_violation = site.has_count_mismatch || !site.capture_set_deterministic ||
                                         !site.literal_is_normalized || profile_missing;
    if (site_contract_violation) {
      ++summary.contract_violation_sites;
    }
  }
  summary.deterministic =
      summary.contract_violation_sites == 0u &&
      summary.block_empty_capture_sites <= summary.block_literal_sites &&
      summary.block_nondeterministic_capture_sites <= summary.block_literal_sites &&
      summary.block_non_normalized_sites <= summary.block_literal_sites &&
      summary.contract_violation_sites <= summary.block_literal_sites;
  return summary;
}

static Objc3BlockLiteralCaptureSemanticsSummary BuildBlockLiteralCaptureSemanticsSummaryFromIntegrationSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  return BuildBlockLiteralCaptureSemanticsSummaryFromSites(surface.block_literal_capture_sites_lexicographic);
}

static Objc3BlockLiteralCaptureSemanticsSummary BuildBlockLiteralCaptureSemanticsSummaryFromTypeMetadataHandoff(
    const Objc3SemanticTypeMetadataHandoff &handoff) {
  return BuildBlockLiteralCaptureSemanticsSummaryFromSites(handoff.block_literal_capture_sites_lexicographic);
}
