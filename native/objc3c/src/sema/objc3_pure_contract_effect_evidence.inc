static bool IsEarlierPureContractSourceLoc(
    unsigned line,
    unsigned column,
    const PureContractEffectInfo::SourceLoc &loc) {
  if (!loc.present) {
    return true;
  }
  if (line < loc.line) {
    return true;
  }
  if (line > loc.line) {
    return false;
  }
  return column < loc.column;
}

static void RecordPureContractSourceLoc(PureContractEffectInfo::SourceLoc &loc,
                                        unsigned line,
                                        unsigned column) {
  if (!IsEarlierPureContractSourceLoc(line, column, loc)) {
    return;
  }
  loc.line = line;
  loc.column = column;
  loc.present = true;
}

static bool IsNameBoundInSemanticScopes(
    const std::vector<std::unordered_set<std::string>> &scopes,
    const std::string &name) {
  for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
    if (it->find(name) != it->end()) {
      return true;
    }
  }
  return false;
}

static bool IsPureContractGlobalWriteTarget(
    const std::string &name,
    const std::vector<std::unordered_set<std::string>> &scopes,
    const std::unordered_set<std::string> &globals) {
  if (name.empty() || IsNameBoundInSemanticScopes(scopes, name)) {
    return false;
  }
  return globals.find(name) != globals.end();
}

static void CollectPureContractEffectExpr(
    const Expr *expr,
    std::vector<std::unordered_set<std::string>> &scopes,
    PureContractEffectInfo &info);

static void CollectPureContractEffectForClause(
    const ForClause &clause,
    std::vector<std::unordered_set<std::string>> &scopes,
    const std::unordered_set<std::string> &globals,
    PureContractEffectInfo &info) {
  switch (clause.kind) {
    case ForClause::Kind::None:
      return;
    case ForClause::Kind::Expr:
      CollectPureContractEffectExpr(clause.value.get(), scopes, info);
      return;
    case ForClause::Kind::Let:
      CollectPureContractEffectExpr(clause.value.get(), scopes, info);
      if (!scopes.empty() && !clause.name.empty()) {
        scopes.back().insert(clause.name);
      }
      return;
    case ForClause::Kind::Assign:
      if (IsPureContractGlobalWriteTarget(clause.name, scopes, globals)) {
        RecordPureContractSourceLoc(info.global_write_site, clause.line,
                                    clause.column);
      }
      CollectPureContractEffectExpr(clause.value.get(), scopes, info);
      return;
  }
}

static void CollectPureContractEffectStmt(
    const Stmt *stmt,
    std::vector<std::unordered_set<std::string>> &scopes,
    const std::unordered_set<std::string> &globals,
    PureContractEffectInfo &info) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
    case Stmt::Kind::Let:
      if (stmt->let_stmt == nullptr) {
        return;
      }
      CollectPureContractEffectExpr(stmt->let_stmt->value.get(), scopes, info);
      if (!scopes.empty() && !stmt->let_stmt->name.empty()) {
        scopes.back().insert(stmt->let_stmt->name);
      }
      return;
    case Stmt::Kind::Assign:
      if (stmt->assign_stmt == nullptr) {
        return;
      }
      if (IsPureContractGlobalWriteTarget(stmt->assign_stmt->name, scopes,
                                          globals)) {
        RecordPureContractSourceLoc(info.global_write_site,
                                    stmt->assign_stmt->line,
                                    stmt->assign_stmt->column);
      }
      CollectPureContractEffectExpr(stmt->assign_stmt->value.get(), scopes,
                                    info);
      return;
    case Stmt::Kind::Return:
      if (stmt->return_stmt != nullptr) {
        CollectPureContractEffectExpr(stmt->return_stmt->value.get(), scopes,
                                      info);
      }
      return;
    case Stmt::Kind::Expr:
      if (stmt->expr_stmt != nullptr) {
        CollectPureContractEffectExpr(stmt->expr_stmt->value.get(), scopes,
                                      info);
      }
      return;
    case Stmt::Kind::If:
      if (stmt->if_stmt == nullptr) {
        return;
      }
      CollectPureContractEffectExpr(stmt->if_stmt->condition.get(), scopes,
                                    info);
      scopes.push_back({});
      for (const auto &then_stmt : stmt->if_stmt->then_body) {
        CollectPureContractEffectStmt(then_stmt.get(), scopes, globals, info);
      }
      scopes.pop_back();
      scopes.push_back({});
      for (const auto &else_stmt : stmt->if_stmt->else_body) {
        CollectPureContractEffectStmt(else_stmt.get(), scopes, globals, info);
      }
      scopes.pop_back();
      return;
    case Stmt::Kind::DoWhile:
      if (stmt->do_while_stmt == nullptr) {
        return;
      }
      scopes.push_back({});
      for (const auto &loop_stmt : stmt->do_while_stmt->body) {
        CollectPureContractEffectStmt(loop_stmt.get(), scopes, globals, info);
      }
      scopes.pop_back();
      CollectPureContractEffectExpr(stmt->do_while_stmt->condition.get(),
                                    scopes, info);
      return;
    case Stmt::Kind::For:
      if (stmt->for_stmt == nullptr) {
        return;
      }
      scopes.push_back({});
      CollectPureContractEffectForClause(stmt->for_stmt->init, scopes, globals,
                                         info);
      CollectPureContractEffectExpr(stmt->for_stmt->condition.get(), scopes,
                                    info);
      scopes.push_back({});
      for (const auto &loop_stmt : stmt->for_stmt->body) {
        CollectPureContractEffectStmt(loop_stmt.get(), scopes, globals, info);
      }
      scopes.pop_back();
      CollectPureContractEffectForClause(stmt->for_stmt->step, scopes, globals,
                                         info);
      scopes.pop_back();
      return;
    case Stmt::Kind::Switch:
      if (stmt->switch_stmt == nullptr) {
        return;
      }
      CollectPureContractEffectExpr(stmt->switch_stmt->condition.get(), scopes,
                                    info);
      for (const auto &case_stmt : stmt->switch_stmt->cases) {
        scopes.push_back({});
        for (const auto &case_body_stmt : case_stmt.body) {
          CollectPureContractEffectStmt(case_body_stmt.get(), scopes, globals,
                                        info);
        }
        scopes.pop_back();
      }
      return;
    case Stmt::Kind::While:
      if (stmt->while_stmt == nullptr) {
        return;
      }
      CollectPureContractEffectExpr(stmt->while_stmt->condition.get(), scopes,
                                    info);
      scopes.push_back({});
      for (const auto &loop_stmt : stmt->while_stmt->body) {
        CollectPureContractEffectStmt(loop_stmt.get(), scopes, globals, info);
      }
      scopes.pop_back();
      return;
    case Stmt::Kind::Block:
    case Stmt::Kind::Defer:
      if (stmt->block_stmt == nullptr) {
        return;
      }
      scopes.push_back({});
      for (const auto &nested_stmt : stmt->block_stmt->body) {
        CollectPureContractEffectStmt(nested_stmt.get(), scopes, globals,
                                      info);
      }
      scopes.pop_back();
      return;
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return;
  }
}

static void CollectPureContractEffectExpr(
    const Expr *expr,
    std::vector<std::unordered_set<std::string>> &scopes,
    PureContractEffectInfo &info) {
  if (expr == nullptr) {
    return;
  }
  switch (expr->kind) {
    case Expr::Kind::Number:
    case Expr::Kind::BoolLiteral:
    case Expr::Kind::NilLiteral:
    case Expr::Kind::Identifier:
    case Expr::Kind::KeyPathLiteral:
    case Expr::Kind::BlockLiteral:
      return;
    case Expr::Kind::Binary:
      CollectPureContractEffectExpr(expr->left.get(), scopes, info);
      CollectPureContractEffectExpr(expr->right.get(), scopes, info);
      return;
    case Expr::Kind::Conditional:
      CollectPureContractEffectExpr(expr->left.get(), scopes, info);
      CollectPureContractEffectExpr(expr->right.get(), scopes, info);
      CollectPureContractEffectExpr(expr->third.get(), scopes, info);
      return;
    case Expr::Kind::Call:
      RecordPureContractSourceLoc(info.called_functions[expr->ident],
                                  expr->line, expr->column);
      for (const auto &arg : expr->args) {
        CollectPureContractEffectExpr(arg.get(), scopes, info);
      }
      return;
    case Expr::Kind::Try:
    case Expr::Kind::Throw:
      for (const auto &arg : expr->args) {
        CollectPureContractEffectExpr(arg.get(), scopes, info);
      }
      return;
    case Expr::Kind::MessageSend:
      RecordPureContractSourceLoc(info.message_send_site, expr->line,
                                  expr->column);
      CollectPureContractEffectExpr(expr->receiver.get(), scopes, info);
      for (const auto &arg : expr->args) {
        CollectPureContractEffectExpr(arg.get(), scopes, info);
      }
      return;
  }
}

static std::unordered_set<std::string> BuildPureContractGlobalNameSet(
    const Objc3Program &ast) {
  std::unordered_set<std::string> globals;
  for (const auto &global : ast.globals) {
    globals.insert(global.name);
  }
  return globals;
}

static std::unordered_map<std::string, PureContractEffectInfo>
CollectPureContractFunctionEffects(
    const Objc3Program &ast,
    const std::unordered_set<std::string> &globals,
    std::unordered_set<std::string> &defined_functions) {
  std::unordered_map<std::string, PureContractEffectInfo> function_effects;
  for (const auto &fn : ast.functions) {
    if (fn.is_prototype) {
      continue;
    }
    defined_functions.insert(fn.name);

    PureContractEffectInfo info;
    std::vector<std::unordered_set<std::string>> scopes;
    scopes.push_back({});
    for (const auto &param : fn.params) {
      scopes.back().insert(param.name);
    }
    for (const auto &stmt : fn.body) {
      CollectPureContractEffectStmt(stmt.get(), scopes, globals, info);
    }
    function_effects[fn.name] = info;
  }
  return function_effects;
}
