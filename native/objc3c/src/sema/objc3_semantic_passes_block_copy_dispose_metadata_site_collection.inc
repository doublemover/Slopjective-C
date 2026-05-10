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
      CollectBlockCopyDisposeSiteMetadataFromExpr(
          stmt->let_stmt->value.get(),
          sites);
      return;
    case Stmt::Kind::Assign:
      CollectBlockCopyDisposeSiteMetadataFromExpr(
          stmt->assign_stmt->value.get(),
          sites);
      return;
    case Stmt::Kind::Return:
      CollectBlockCopyDisposeSiteMetadataFromExpr(
          stmt->return_stmt->value.get(),
          sites);
      return;
    case Stmt::Kind::If:
      CollectBlockCopyDisposeSiteMetadataFromExpr(
          stmt->if_stmt->condition.get(),
          sites);
      CollectBlockCopyDisposeSiteMetadataFromStatements(
          stmt->if_stmt->then_body,
          sites);
      CollectBlockCopyDisposeSiteMetadataFromStatements(
          stmt->if_stmt->else_body,
          sites);
      return;
    case Stmt::Kind::DoWhile:
      CollectBlockCopyDisposeSiteMetadataFromStatements(
          stmt->do_while_stmt->body,
          sites);
      CollectBlockCopyDisposeSiteMetadataFromExpr(
          stmt->do_while_stmt->condition.get(),
          sites);
      return;
    case Stmt::Kind::For:
      CollectBlockCopyDisposeSiteMetadataFromForClause(
          stmt->for_stmt->init,
          sites);
      CollectBlockCopyDisposeSiteMetadataFromExpr(
          stmt->for_stmt->condition.get(),
          sites);
      CollectBlockCopyDisposeSiteMetadataFromForClause(
          stmt->for_stmt->step,
          sites);
      CollectBlockCopyDisposeSiteMetadataFromStatements(
          stmt->for_stmt->body,
          sites);
      return;
    case Stmt::Kind::Switch:
      CollectBlockCopyDisposeSiteMetadataFromExpr(
          stmt->switch_stmt->condition.get(),
          sites);
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        CollectBlockCopyDisposeSiteMetadataFromStatements(
            switch_case.body,
            sites);
      }
      return;
    case Stmt::Kind::While:
      CollectBlockCopyDisposeSiteMetadataFromExpr(
          stmt->while_stmt->condition.get(),
          sites);
      CollectBlockCopyDisposeSiteMetadataFromStatements(
          stmt->while_stmt->body,
          sites);
      return;
    case Stmt::Kind::Block:
    case Stmt::Kind::Defer:
      CollectBlockCopyDisposeSiteMetadataFromStatements(
          stmt->block_stmt->body,
          sites);
      return;
    case Stmt::Kind::Expr:
      CollectBlockCopyDisposeSiteMetadataFromExpr(
          stmt->expr_stmt->value.get(),
          sites);
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
    return lhs.explicit_move_capture_count <
           rhs.explicit_move_capture_count;
  }
  if (lhs.copy_helper_required != rhs.copy_helper_required) {
    return lhs.copy_helper_required < rhs.copy_helper_required;
  }
  if (lhs.dispose_helper_required != rhs.dispose_helper_required) {
    return lhs.dispose_helper_required < rhs.dispose_helper_required;
  }
  if (lhs.copy_dispose_profile_is_normalized !=
      rhs.copy_dispose_profile_is_normalized) {
    return lhs.copy_dispose_profile_is_normalized <
           rhs.copy_dispose_profile_is_normalized;
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
