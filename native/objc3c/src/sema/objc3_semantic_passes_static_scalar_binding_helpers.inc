static void CollectAssignedIdentifiers(const std::vector<std::unique_ptr<Stmt>> &statements,
                                       std::unordered_set<std::string> &assigned);

static void CollectAssignedIdentifiersFromStmt(const Stmt *stmt, std::unordered_set<std::string> &assigned) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
    case Stmt::Kind::Assign:
      if (stmt->assign_stmt != nullptr) {
        assigned.insert(stmt->assign_stmt->name);
      }
      return;
    case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
      if (stmt->block_stmt != nullptr) {
        CollectAssignedIdentifiers(stmt->block_stmt->body, assigned);
      }
      return;
    case Stmt::Kind::If:
      if (stmt->if_stmt != nullptr) {
        CollectAssignedIdentifiers(stmt->if_stmt->then_body, assigned);
        CollectAssignedIdentifiers(stmt->if_stmt->else_body, assigned);
      }
      return;
    case Stmt::Kind::DoWhile:
      if (stmt->do_while_stmt != nullptr) {
        CollectAssignedIdentifiers(stmt->do_while_stmt->body, assigned);
      }
      return;
    case Stmt::Kind::For:
      if (stmt->for_stmt != nullptr) {
        if (stmt->for_stmt->init.kind == ForClause::Kind::Assign) {
          assigned.insert(stmt->for_stmt->init.name);
        }
        if (stmt->for_stmt->step.kind == ForClause::Kind::Assign) {
          assigned.insert(stmt->for_stmt->step.name);
        }
        CollectAssignedIdentifiers(stmt->for_stmt->body, assigned);
      }
      return;
    case Stmt::Kind::Switch:
      if (stmt->switch_stmt != nullptr) {
        for (const auto &case_stmt : stmt->switch_stmt->cases) {
          CollectAssignedIdentifiers(case_stmt.body, assigned);
        }
      }
      return;
    case Stmt::Kind::While:
      if (stmt->while_stmt != nullptr) {
        CollectAssignedIdentifiers(stmt->while_stmt->body, assigned);
      }
      return;
    default:
      return;
  }
}

static void CollectAssignedIdentifiers(const std::vector<std::unique_ptr<Stmt>> &statements,
                                       std::unordered_set<std::string> &assigned) {
  for (const auto &stmt : statements) {
    CollectAssignedIdentifiersFromStmt(stmt.get(), assigned);
  }
}

static void CollectNonTopLevelLetNames(const std::vector<std::unique_ptr<Stmt>> &statements, bool is_top_level,
                                       std::unordered_set<std::string> &names);

static void CollectNonTopLevelLetNamesFromStmt(const Stmt *stmt, bool is_top_level,
                                               std::unordered_set<std::string> &names) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
    case Stmt::Kind::Let:
      if (!is_top_level && stmt->let_stmt != nullptr) {
        names.insert(stmt->let_stmt->name);
      }
      return;
    case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
      if (stmt->block_stmt != nullptr) {
        CollectNonTopLevelLetNames(stmt->block_stmt->body, false, names);
      }
      return;
    case Stmt::Kind::If:
      if (stmt->if_stmt != nullptr) {
        CollectNonTopLevelLetNames(stmt->if_stmt->then_body, false, names);
        CollectNonTopLevelLetNames(stmt->if_stmt->else_body, false, names);
      }
      return;
    case Stmt::Kind::DoWhile:
      if (stmt->do_while_stmt != nullptr) {
        CollectNonTopLevelLetNames(stmt->do_while_stmt->body, false, names);
      }
      return;
    case Stmt::Kind::For:
      if (stmt->for_stmt != nullptr) {
        if (stmt->for_stmt->init.kind == ForClause::Kind::Let) {
          names.insert(stmt->for_stmt->init.name);
        }
        CollectNonTopLevelLetNames(stmt->for_stmt->body, false, names);
      }
      return;
    case Stmt::Kind::Switch:
      if (stmt->switch_stmt != nullptr) {
        for (const auto &case_stmt : stmt->switch_stmt->cases) {
          CollectNonTopLevelLetNames(case_stmt.body, false, names);
        }
      }
      return;
    case Stmt::Kind::While:
      if (stmt->while_stmt != nullptr) {
        CollectNonTopLevelLetNames(stmt->while_stmt->body, false, names);
      }
      return;
    default:
      return;
  }
}

static void CollectNonTopLevelLetNames(const std::vector<std::unique_ptr<Stmt>> &statements, bool is_top_level,
                                       std::unordered_set<std::string> &names) {
  for (const auto &stmt : statements) {
    CollectNonTopLevelLetNamesFromStmt(stmt.get(), is_top_level, names);
  }
}

static void CollectSwitchConditionIdentifierNames(const std::vector<std::unique_ptr<Stmt>> &statements,
                                                  std::unordered_set<std::string> &names);

static void CollectSwitchConditionIdentifierNamesFromStmt(const Stmt *stmt, std::unordered_set<std::string> &names) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
    case Stmt::Kind::Switch:
      if (stmt->switch_stmt != nullptr) {
        const Expr *condition = stmt->switch_stmt->condition.get();
        if (condition != nullptr && condition->kind == Expr::Kind::Identifier && !condition->ident.empty()) {
          names.insert(condition->ident);
        }
        for (const auto &case_stmt : stmt->switch_stmt->cases) {
          CollectSwitchConditionIdentifierNames(case_stmt.body, names);
        }
      }
      return;
    case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
      if (stmt->block_stmt != nullptr) {
        CollectSwitchConditionIdentifierNames(stmt->block_stmt->body, names);
      }
      return;
    case Stmt::Kind::If:
      if (stmt->if_stmt != nullptr) {
        CollectSwitchConditionIdentifierNames(stmt->if_stmt->then_body, names);
        CollectSwitchConditionIdentifierNames(stmt->if_stmt->else_body, names);
      }
      return;
    case Stmt::Kind::DoWhile:
      if (stmt->do_while_stmt != nullptr) {
        CollectSwitchConditionIdentifierNames(stmt->do_while_stmt->body, names);
      }
      return;
    case Stmt::Kind::For:
      if (stmt->for_stmt != nullptr) {
        CollectSwitchConditionIdentifierNames(stmt->for_stmt->body, names);
      }
      return;
    case Stmt::Kind::While:
      if (stmt->while_stmt != nullptr) {
        CollectSwitchConditionIdentifierNames(stmt->while_stmt->body, names);
      }
      return;
    default:
      return;
  }
}

static void CollectSwitchConditionIdentifierNames(const std::vector<std::unique_ptr<Stmt>> &statements,
                                                  std::unordered_set<std::string> &names) {
  for (const auto &stmt : statements) {
    CollectSwitchConditionIdentifierNamesFromStmt(stmt.get(), names);
  }
}

static StaticScalarBindings CollectFunctionStaticScalarBindings(const FunctionDecl &fn,
                                                               const StaticScalarBindings *global_bindings = nullptr) {
  std::unordered_set<std::string> assigned;
  CollectAssignedIdentifiers(fn.body, assigned);

  std::unordered_set<std::string> non_top_level_lets;
  CollectNonTopLevelLetNames(fn.body, true, non_top_level_lets);

  std::unordered_set<std::string> switch_condition_identifiers;
  CollectSwitchConditionIdentifierNames(fn.body, switch_condition_identifiers);

  StaticScalarBindings bindings;
  for (const auto &stmt : fn.body) {
    if (stmt == nullptr || stmt->kind != Stmt::Kind::Let || stmt->let_stmt == nullptr || stmt->let_stmt->value == nullptr) {
      continue;
    }
    const std::string &name = stmt->let_stmt->name;
    if (assigned.find(name) != assigned.end() || non_top_level_lets.find(name) != non_top_level_lets.end() ||
        switch_condition_identifiers.find(name) != switch_condition_identifiers.end()) {
      continue;
    }
    int value = 0;
    if (TryEvalStaticScalarValue(stmt->let_stmt->value.get(), value, &bindings)) {
      bindings[name] = value;
    }
  }

  if (global_bindings != nullptr) {
    for (const auto &[name, value] : *global_bindings) {
      if (bindings.find(name) != bindings.end()) {
        continue;
      }
      if (assigned.find(name) != assigned.end() || non_top_level_lets.find(name) != non_top_level_lets.end() ||
          switch_condition_identifiers.find(name) != switch_condition_identifiers.end()) {
        continue;
      }
      bindings[name] = value;
    }
  }
  return bindings;
}
