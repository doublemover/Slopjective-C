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
