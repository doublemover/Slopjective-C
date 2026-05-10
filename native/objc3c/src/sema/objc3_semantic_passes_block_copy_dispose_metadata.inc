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
