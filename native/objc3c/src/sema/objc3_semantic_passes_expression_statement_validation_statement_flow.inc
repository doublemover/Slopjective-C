static bool StatementDefinitelyExitsScope(const Stmt *stmt);

static bool StatementListDefinitelyExitsScope(
    const std::vector<std::unique_ptr<Stmt>> &statements) {
  if (statements.empty()) {
    return false;
  }
  return StatementDefinitelyExitsScope(statements.back().get());
}

static bool StatementDefinitelyExitsScope(const Stmt *stmt) {
  if (stmt == nullptr) {
    return false;
  }
  switch (stmt->kind) {
    case Stmt::Kind::Return:
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
      return true;
    case Stmt::Kind::Block:
      return stmt->block_stmt != nullptr &&
             StatementListDefinitelyExitsScope(stmt->block_stmt->body);
    case Stmt::Kind::Defer:
      return false;
    case Stmt::Kind::If:
      return stmt->if_stmt != nullptr &&
             StatementListDefinitelyExitsScope(stmt->if_stmt->then_body) &&
             StatementListDefinitelyExitsScope(stmt->if_stmt->else_body);
    case Stmt::Kind::Assign:
    case Stmt::Kind::Let:
    case Stmt::Kind::DoWhile:
    case Stmt::Kind::For:
    case Stmt::Kind::Switch:
    case Stmt::Kind::While:
    case Stmt::Kind::Empty:
    case Stmt::Kind::Expr:
      return false;
  }
  return false;
}

struct MatchExhaustivenessInfo {
  bool exhaustive = false;
  bool bool_exhaustive = false;
  bool result_case_exhaustive = false;
};

static bool IsMatchCatchAllPattern(const SwitchCase &case_stmt) {
  return case_stmt.is_default ||
         case_stmt.match_pattern_kind == MatchPatternKind::Wildcard ||
         case_stmt.match_pattern_kind == MatchPatternKind::Binding;
}

static MatchExhaustivenessInfo ClassifyMatchExhaustiveness(
    const SwitchStmt &switch_stmt) {
  MatchExhaustivenessInfo info;
  bool saw_true = false;
  bool saw_false = false;
  bool saw_ok = false;
  bool saw_err = false;
  for (const auto &case_stmt : switch_stmt.cases) {
    if (IsMatchCatchAllPattern(case_stmt)) {
      info.exhaustive = true;
      return info;
    }
    if (case_stmt.match_pattern_kind == MatchPatternKind::LiteralBool) {
      if (case_stmt.value == 0) {
        saw_false = true;
      } else {
        saw_true = true;
      }
    } else if (case_stmt.match_pattern_kind == MatchPatternKind::ResultCase) {
      if (case_stmt.match_result_case_name == "Ok") {
        saw_ok = true;
      } else if (case_stmt.match_result_case_name == "Err") {
        saw_err = true;
      }
    }
  }
  info.bool_exhaustive = saw_true && saw_false;
  info.result_case_exhaustive = saw_ok && saw_err;
  info.exhaustive = info.bool_exhaustive || info.result_case_exhaustive;
  return info;
}

struct BranchNonnullRefinement {
  std::string symbol_name;
  bool refine_then = false;
  bool refine_else = false;
};

static BranchNonnullRefinement AnalyzeNonnullBranchRefinement(
    const Expr *condition) {
  BranchNonnullRefinement refinement;
  if (condition == nullptr || condition->kind != Expr::Kind::Binary) {
    return refinement;
  }
  if (condition->op != "==" && condition->op != "!=") {
    return refinement;
  }

  const Expr *identifier_expr = nullptr;
  const Expr *nil_expr = nullptr;
  if (condition->left != nullptr && condition->left->kind == Expr::Kind::Identifier &&
      condition->right != nullptr &&
      condition->right->kind == Expr::Kind::NilLiteral) {
    identifier_expr = condition->left.get();
    nil_expr = condition->right.get();
  } else if (condition->right != nullptr &&
             condition->right->kind == Expr::Kind::Identifier &&
             condition->left != nullptr &&
             condition->left->kind == Expr::Kind::NilLiteral) {
    identifier_expr = condition->right.get();
    nil_expr = condition->left.get();
  }
  if (identifier_expr == nullptr || nil_expr == nullptr) {
    return refinement;
  }

  refinement.symbol_name = identifier_expr->ident;
  refinement.refine_then = condition->op == "!=";
  refinement.refine_else = condition->op == "==";
  return refinement;
}

static void ApplyNonnullRefinementToScope(
    const BranchNonnullRefinement &refinement, bool refine_active,
    const std::vector<SemanticScope> &source_scopes, SemanticScope &target_scope) {
  if (!refine_active || refinement.symbol_name.empty()) {
    return;
  }
  const SemanticTypeInfo current_type =
      ScopeLookupType(source_scopes, refinement.symbol_name);
  if (IsNullableObjCReferenceSemanticType(current_type)) {
    target_scope[refinement.symbol_name] =
        MakeNonnullRefinedSemanticType(current_type);
  }
}
