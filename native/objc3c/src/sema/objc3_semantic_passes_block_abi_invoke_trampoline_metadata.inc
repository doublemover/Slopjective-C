static Objc3BlockAbiInvokeTrampolineSiteMetadata BuildBlockAbiInvokeTrampolineSiteMetadata(const Expr &expr) {
  Objc3BlockAbiInvokeTrampolineSiteMetadata metadata;
  metadata.invoke_argument_slots = expr.block_abi_invoke_argument_slots;
  metadata.capture_word_count = expr.block_abi_capture_word_count;
  metadata.parameter_count = expr.block_parameter_count;
  metadata.capture_count = expr.block_capture_count;
  metadata.body_statement_count = expr.block_body_statement_count;
  metadata.has_invoke_trampoline = expr.block_abi_has_invoke_trampoline;
  metadata.layout_is_normalized = expr.block_abi_layout_is_normalized;
  metadata.layout_profile = expr.block_abi_layout_profile;
  metadata.descriptor_symbol = expr.block_abi_descriptor_symbol;
  metadata.invoke_trampoline_symbol = expr.block_invoke_trampoline_symbol;
  metadata.line = expr.line;
  metadata.column = expr.column;
  metadata.has_count_mismatch =
      metadata.invoke_argument_slots != metadata.parameter_count ||
      metadata.capture_word_count != metadata.capture_count ||
      !IsSortedUniqueStrings(expr.block_parameter_names_lexicographic) ||
      !IsSortedUniqueStrings(expr.block_capture_names_lexicographic);
  return metadata;
}

static void CollectBlockAbiInvokeTrampolineSiteMetadataFromExpr(
    const Expr *expr,
    std::vector<Objc3BlockAbiInvokeTrampolineSiteMetadata> &sites) {
  if (expr == nullptr) {
    return;
  }

  if (expr->kind == Expr::Kind::BlockLiteral) {
    sites.push_back(BuildBlockAbiInvokeTrampolineSiteMetadata(*expr));
  }

  CollectBlockAbiInvokeTrampolineSiteMetadataFromExpr(expr->receiver.get(), sites);
  CollectBlockAbiInvokeTrampolineSiteMetadataFromExpr(expr->left.get(), sites);
  CollectBlockAbiInvokeTrampolineSiteMetadataFromExpr(expr->right.get(), sites);
  CollectBlockAbiInvokeTrampolineSiteMetadataFromExpr(expr->third.get(), sites);
  for (const auto &arg : expr->args) {
    CollectBlockAbiInvokeTrampolineSiteMetadataFromExpr(arg.get(), sites);
  }
}

static void CollectBlockAbiInvokeTrampolineSiteMetadataFromStatements(
    const std::vector<std::unique_ptr<Stmt>> &statements,
    std::vector<Objc3BlockAbiInvokeTrampolineSiteMetadata> &sites);

static void CollectBlockAbiInvokeTrampolineSiteMetadataFromForClause(
    const ForClause &clause,
    std::vector<Objc3BlockAbiInvokeTrampolineSiteMetadata> &sites) {
  CollectBlockAbiInvokeTrampolineSiteMetadataFromExpr(clause.value.get(), sites);
}

static void CollectBlockAbiInvokeTrampolineSiteMetadataFromStatement(
    const Stmt *stmt,
    std::vector<Objc3BlockAbiInvokeTrampolineSiteMetadata> &sites) {
  if (stmt == nullptr) {
    return;
  }

  switch (stmt->kind) {
    case Stmt::Kind::Let:
      CollectBlockAbiInvokeTrampolineSiteMetadataFromExpr(stmt->let_stmt->value.get(), sites);
      return;
    case Stmt::Kind::Assign:
      CollectBlockAbiInvokeTrampolineSiteMetadataFromExpr(stmt->assign_stmt->value.get(), sites);
      return;
    case Stmt::Kind::Return:
      CollectBlockAbiInvokeTrampolineSiteMetadataFromExpr(stmt->return_stmt->value.get(), sites);
      return;
    case Stmt::Kind::If:
      CollectBlockAbiInvokeTrampolineSiteMetadataFromExpr(stmt->if_stmt->condition.get(), sites);
      CollectBlockAbiInvokeTrampolineSiteMetadataFromStatements(stmt->if_stmt->then_body, sites);
      CollectBlockAbiInvokeTrampolineSiteMetadataFromStatements(stmt->if_stmt->else_body, sites);
      return;
    case Stmt::Kind::DoWhile:
      CollectBlockAbiInvokeTrampolineSiteMetadataFromStatements(stmt->do_while_stmt->body, sites);
      CollectBlockAbiInvokeTrampolineSiteMetadataFromExpr(stmt->do_while_stmt->condition.get(), sites);
      return;
    case Stmt::Kind::For:
      CollectBlockAbiInvokeTrampolineSiteMetadataFromForClause(stmt->for_stmt->init, sites);
      CollectBlockAbiInvokeTrampolineSiteMetadataFromExpr(stmt->for_stmt->condition.get(), sites);
      CollectBlockAbiInvokeTrampolineSiteMetadataFromForClause(stmt->for_stmt->step, sites);
      CollectBlockAbiInvokeTrampolineSiteMetadataFromStatements(stmt->for_stmt->body, sites);
      return;
    case Stmt::Kind::Switch:
      CollectBlockAbiInvokeTrampolineSiteMetadataFromExpr(stmt->switch_stmt->condition.get(), sites);
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        CollectBlockAbiInvokeTrampolineSiteMetadataFromStatements(switch_case.body, sites);
      }
      return;
    case Stmt::Kind::While:
      CollectBlockAbiInvokeTrampolineSiteMetadataFromExpr(stmt->while_stmt->condition.get(), sites);
      CollectBlockAbiInvokeTrampolineSiteMetadataFromStatements(stmt->while_stmt->body, sites);
      return;
    case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
      CollectBlockAbiInvokeTrampolineSiteMetadataFromStatements(stmt->block_stmt->body, sites);
      return;
    case Stmt::Kind::Expr:
      CollectBlockAbiInvokeTrampolineSiteMetadataFromExpr(stmt->expr_stmt->value.get(), sites);
      return;
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return;
  }
}

static void CollectBlockAbiInvokeTrampolineSiteMetadataFromStatements(
    const std::vector<std::unique_ptr<Stmt>> &statements,
    std::vector<Objc3BlockAbiInvokeTrampolineSiteMetadata> &sites) {
  for (const auto &statement : statements) {
    CollectBlockAbiInvokeTrampolineSiteMetadataFromStatement(statement.get(), sites);
  }
}

static bool IsBlockAbiInvokeTrampolineSiteMetadataLess(
    const Objc3BlockAbiInvokeTrampolineSiteMetadata &lhs,
    const Objc3BlockAbiInvokeTrampolineSiteMetadata &rhs) {
  if (lhs.layout_profile != rhs.layout_profile) {
    return lhs.layout_profile < rhs.layout_profile;
  }
  if (lhs.descriptor_symbol != rhs.descriptor_symbol) {
    return lhs.descriptor_symbol < rhs.descriptor_symbol;
  }
  if (lhs.invoke_trampoline_symbol != rhs.invoke_trampoline_symbol) {
    return lhs.invoke_trampoline_symbol < rhs.invoke_trampoline_symbol;
  }
  if (lhs.invoke_argument_slots != rhs.invoke_argument_slots) {
    return lhs.invoke_argument_slots < rhs.invoke_argument_slots;
  }
  if (lhs.capture_word_count != rhs.capture_word_count) {
    return lhs.capture_word_count < rhs.capture_word_count;
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
  if (lhs.has_invoke_trampoline != rhs.has_invoke_trampoline) {
    return lhs.has_invoke_trampoline < rhs.has_invoke_trampoline;
  }
  if (lhs.layout_is_normalized != rhs.layout_is_normalized) {
    return lhs.layout_is_normalized < rhs.layout_is_normalized;
  }
  if (lhs.has_count_mismatch != rhs.has_count_mismatch) {
    return lhs.has_count_mismatch < rhs.has_count_mismatch;
  }
  if (lhs.line != rhs.line) {
    return lhs.line < rhs.line;
  }
  return lhs.column < rhs.column;
}

static std::vector<Objc3BlockAbiInvokeTrampolineSiteMetadata>
BuildBlockAbiInvokeTrampolineSiteMetadataLexicographic(const Objc3Program &ast) {
  std::vector<Objc3BlockAbiInvokeTrampolineSiteMetadata> sites;
  for (const auto &global : ast.globals) {
    CollectBlockAbiInvokeTrampolineSiteMetadataFromExpr(global.value.get(), sites);
  }
  for (const auto &fn : ast.functions) {
    CollectBlockAbiInvokeTrampolineSiteMetadataFromStatements(fn.body, sites);
  }
  std::sort(sites.begin(), sites.end(), IsBlockAbiInvokeTrampolineSiteMetadataLess);
  return sites;
}

static Objc3BlockAbiInvokeTrampolineSemanticsSummary BuildBlockAbiInvokeTrampolineSemanticsSummaryFromSites(
    const std::vector<Objc3BlockAbiInvokeTrampolineSiteMetadata> &sites) {
  Objc3BlockAbiInvokeTrampolineSemanticsSummary summary;
  summary.block_literal_sites = sites.size();
  for (const auto &site : sites) {
    summary.invoke_argument_slots_total += site.invoke_argument_slots;
    summary.capture_word_count_total += site.capture_word_count;
    summary.parameter_entries_total += site.parameter_count;
    summary.capture_entries_total += site.capture_count;
    summary.body_statement_entries_total += site.body_statement_count;
    if (!site.descriptor_symbol.empty()) {
      ++summary.descriptor_symbolized_sites;
    }
    if (!site.invoke_trampoline_symbol.empty()) {
      ++summary.invoke_trampoline_symbolized_sites;
    }
    if (!site.has_invoke_trampoline) {
      ++summary.missing_invoke_trampoline_sites;
    }
    if (!site.layout_is_normalized) {
      ++summary.non_normalized_layout_sites;
    }
    const bool layout_profile_missing = site.layout_profile.empty();
    const bool descriptor_symbol_missing = site.descriptor_symbol.empty();
    const bool invoke_trampoline_symbol_missing =
        site.has_invoke_trampoline && site.invoke_trampoline_symbol.empty();
    const bool invoke_trampoline_symbol_mismatch =
        !site.has_invoke_trampoline && !site.invoke_trampoline_symbol.empty();
    const bool invoke_argument_slot_mismatch = site.invoke_argument_slots != site.parameter_count;
    const bool capture_word_count_mismatch = site.capture_word_count != site.capture_count;
    const bool site_contract_violation =
        site.has_count_mismatch || !site.has_invoke_trampoline || !site.layout_is_normalized ||
        layout_profile_missing || descriptor_symbol_missing || invoke_trampoline_symbol_missing ||
        invoke_trampoline_symbol_mismatch || invoke_argument_slot_mismatch ||
        capture_word_count_mismatch;
    if (site_contract_violation) {
      ++summary.contract_violation_sites;
    }
  }
  summary.deterministic =
      summary.contract_violation_sites == 0u &&
      summary.descriptor_symbolized_sites <= summary.block_literal_sites &&
      summary.invoke_trampoline_symbolized_sites <= summary.block_literal_sites &&
      summary.missing_invoke_trampoline_sites <= summary.block_literal_sites &&
      summary.non_normalized_layout_sites <= summary.block_literal_sites &&
      summary.invoke_trampoline_symbolized_sites + summary.missing_invoke_trampoline_sites ==
          summary.block_literal_sites &&
      summary.invoke_argument_slots_total == summary.parameter_entries_total &&
      summary.capture_word_count_total == summary.capture_entries_total;
  return summary;
}

static Objc3BlockAbiInvokeTrampolineSemanticsSummary BuildBlockAbiInvokeTrampolineSemanticsSummaryFromIntegrationSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  return BuildBlockAbiInvokeTrampolineSemanticsSummaryFromSites(
      surface.block_abi_invoke_trampoline_sites_lexicographic);
}

static Objc3BlockAbiInvokeTrampolineSemanticsSummary BuildBlockAbiInvokeTrampolineSemanticsSummaryFromTypeMetadataHandoff(
    const Objc3SemanticTypeMetadataHandoff &handoff) {
  return BuildBlockAbiInvokeTrampolineSemanticsSummaryFromSites(
      handoff.block_abi_invoke_trampoline_sites_lexicographic);
}
