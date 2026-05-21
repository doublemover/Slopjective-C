static void DiagnoseOwnershipBorrowedEscapeExpr(
    const Expr *expr, std::vector<OwnershipBorrowedScope> &scopes,
    const std::unordered_map<std::string, OwnershipBorrowedCallableContract>
        &callable_contracts,
    std::vector<std::string> &diagnostics);

static void DiagnoseOwnershipBorrowedEscapeStatements(
    const std::vector<std::unique_ptr<Stmt>> &statements,
    std::vector<OwnershipBorrowedScope> &scopes,
    const std::unordered_map<std::string, OwnershipBorrowedCallableContract>
        &callable_contracts,
    const OwnershipBorrowedReturnContract &return_contract,
    std::vector<std::string> &diagnostics);

static void DiagnoseOwnershipBorrowedEscapingBlockCapture(
    const Expr *expr, const std::vector<OwnershipBorrowedScope> &scopes,
    std::vector<std::string> &diagnostics) {
  const bool escaping_block = expr->block_storage_escape_to_heap ||
                              expr->block_escape_shape_promotes_to_heap_candidate;
  if (!escaping_block) {
    return;
  }
  for (const auto &capture_name : expr->block_capture_names_lexicographic) {
    const auto *binding = LookupOwnershipBorrowedBinding(scopes, capture_name);
    if (binding != nullptr && binding->borrowed) {
      diagnostics.push_back(MakeDiag(
          expr->line, expr->column, "O3S299",
          "borrowed pointer cannot be captured by an escaping block; copy to owned storage, extend owner lifetime, or mark explicitly unsafe"));
      break;
    }
  }
}

static void DiagnoseOwnershipBorrowedCallArgumentEscape(
    const Expr *arg, const OwnershipBorrowedCallableContract *contract,
    std::size_t index, const std::vector<OwnershipBorrowedScope> &scopes,
    const std::unordered_map<std::string, OwnershipBorrowedCallableContract>
        &callable_contracts,
    std::vector<std::string> &diagnostics) {
  if (IsOwnershipConsumedCallableParameter(contract, index)) {
    return;
  }
  if (!IsOwnershipBorrowedExpr(arg, scopes, callable_contracts)) {
    return;
  }
  if (!IsOwnershipBorrowedCallableParameterBorrowed(contract, index)) {
    diagnostics.push_back(MakeDiag(
        arg->line, arg->column, "O3S298",
        "borrowed pointer passed to parameter not proven non-escaping; copy to owned storage, extend owner lifetime, or mark explicitly unsafe"));
  }
}

static void DiagnoseOwnershipConsumedIdentifierUse(
    const Expr *expr, const std::vector<OwnershipBorrowedScope> &scopes,
    std::vector<std::string> &diagnostics) {
  if (expr == nullptr || expr->kind != Expr::Kind::Identifier) {
    return;
  }
  const auto *binding = LookupOwnershipBorrowedBinding(scopes, expr->ident);
  if (binding != nullptr && binding->consumed) {
    diagnostics.push_back(MakeDiag(
        expr->line, expr->column, "O3S308",
        "ownership value used after being consumed; reacquire or assign a new owned value before use"));
  }
}

static void DiagnoseOwnershipConsumedCallArgumentTransfer(
    const Expr *arg, const OwnershipBorrowedCallableContract *contract,
    std::size_t index, std::vector<OwnershipBorrowedScope> &scopes,
    const std::unordered_map<std::string, OwnershipBorrowedCallableContract>
        &callable_contracts,
    std::vector<std::string> &diagnostics) {
  if (!IsOwnershipConsumedCallableParameter(contract, index)) {
    return;
  }
  if (IsOwnershipBorrowedExpr(arg, scopes, callable_contracts)) {
    diagnostics.push_back(MakeDiag(
        arg->line, arg->column, "O3S307",
        "borrowed pointer cannot satisfy a consumed ownership parameter; retain, copy, or extend ownership before transfer"));
    return;
  }
  if (arg != nullptr && arg->kind == Expr::Kind::Identifier) {
    if (auto *binding = LookupOwnershipBorrowedBinding(scopes, arg->ident)) {
      binding->consumed = true;
      binding->borrowed = false;
    }
  }
}

static void DiagnoseOwnershipBorrowedReturnEscape(
    const ReturnStmt *return_stmt, bool borrowed,
    const OwnershipBorrowedReturnContract &return_contract,
    std::vector<std::string> &diagnostics) {
  if (borrowed && !return_contract.valid) {
    diagnostics.push_back(MakeDiag(
        return_stmt->line, return_stmt->column, "O3S300",
        "borrowed return requires objc_returns_borrowed(owner_index=...) naming a valid owner parameter"));
  }
}

static void DiagnoseOwnershipBorrowedEscapeExpr(
    const Expr *expr, std::vector<OwnershipBorrowedScope> &scopes,
    const std::unordered_map<std::string, OwnershipBorrowedCallableContract>
        &callable_contracts,
    std::vector<std::string> &diagnostics) {
  if (expr == nullptr) {
    return;
  }
  switch (expr->kind) {
    case Expr::Kind::BlockLiteral:
      for (const auto &arg : expr->args) {
        DiagnoseOwnershipBorrowedEscapeExpr(arg.get(), scopes,
                                            callable_contracts, diagnostics);
      }
      DiagnoseOwnershipBorrowedEscapingBlockCapture(expr, scopes, diagnostics);
      return;
    case Expr::Kind::Call: {
      DiagnoseOwnershipBorrowedEscapeExpr(expr->receiver.get(), scopes,
                                          callable_contracts, diagnostics);
      const auto *contract =
          FindOwnershipBorrowedCallableContract(callable_contracts, expr->ident);
      for (std::size_t index = 0; index < expr->args.size(); ++index) {
        const Expr *arg = expr->args[index].get();
        DiagnoseOwnershipBorrowedEscapeExpr(arg, scopes, callable_contracts,
                                            diagnostics);
        DiagnoseOwnershipBorrowedCallArgumentEscape(
            arg, contract, index, scopes, callable_contracts, diagnostics);
        DiagnoseOwnershipConsumedCallArgumentTransfer(
            arg, contract, index, scopes, callable_contracts, diagnostics);
      }
      return;
    }
    case Expr::Kind::MessageSend:
      DiagnoseOwnershipBorrowedEscapeExpr(expr->receiver.get(), scopes,
                                          callable_contracts, diagnostics);
      for (const auto &arg : expr->args) {
        DiagnoseOwnershipBorrowedEscapeExpr(arg.get(), scopes,
                                            callable_contracts, diagnostics);
      }
      return;
    case Expr::Kind::Binary:
      DiagnoseOwnershipBorrowedEscapeExpr(expr->left.get(), scopes,
                                          callable_contracts, diagnostics);
      DiagnoseOwnershipBorrowedEscapeExpr(expr->right.get(), scopes,
                                          callable_contracts, diagnostics);
      return;
    case Expr::Kind::Conditional:
      DiagnoseOwnershipBorrowedEscapeExpr(expr->left.get(), scopes,
                                          callable_contracts, diagnostics);
      DiagnoseOwnershipBorrowedEscapeExpr(expr->right.get(), scopes,
                                          callable_contracts, diagnostics);
      DiagnoseOwnershipBorrowedEscapeExpr(expr->third.get(), scopes,
                                          callable_contracts, diagnostics);
      return;
    case Expr::Kind::CollectionLiteral:
      for (const auto &key : expr->collection_keys) {
        DiagnoseOwnershipBorrowedEscapeExpr(key.get(), scopes,
                                            callable_contracts, diagnostics);
      }
      for (const auto &value : expr->collection_values) {
        DiagnoseOwnershipBorrowedEscapeExpr(value.get(), scopes,
                                            callable_contracts, diagnostics);
      }
      return;
    case Expr::Kind::IndexAccess:
      DiagnoseOwnershipBorrowedEscapeExpr(expr->left.get(), scopes,
                                          callable_contracts, diagnostics);
      DiagnoseOwnershipBorrowedEscapeExpr(expr->right.get(), scopes,
                                          callable_contracts, diagnostics);
      return;
    case Expr::Kind::Try:
    case Expr::Kind::Throw:
      for (const auto &arg : expr->args) {
        DiagnoseOwnershipBorrowedEscapeExpr(arg.get(), scopes,
                                            callable_contracts, diagnostics);
      }
      return;
    case Expr::Kind::Identifier:
      DiagnoseOwnershipConsumedIdentifierUse(expr, scopes, diagnostics);
      return;
    case Expr::Kind::KeyPathLiteral:
    case Expr::Kind::Number:
    case Expr::Kind::BoolLiteral:
    case Expr::Kind::NilLiteral:
    case Expr::Kind::StringLiteral:
      return;
  }
}

static void DiagnoseOwnershipBorrowedEscapeForClause(
    const ForClause &clause, std::vector<OwnershipBorrowedScope> &scopes,
    const std::unordered_map<std::string, OwnershipBorrowedCallableContract>
        &callable_contracts,
    std::vector<std::string> &diagnostics) {
  switch (clause.kind) {
    case ForClause::Kind::None:
      return;
    case ForClause::Kind::Expr:
      DiagnoseOwnershipBorrowedEscapeExpr(clause.value.get(), scopes,
                                          callable_contracts, diagnostics);
      return;
    case ForClause::Kind::Let: {
      DiagnoseOwnershipBorrowedEscapeExpr(clause.value.get(), scopes,
                                          callable_contracts, diagnostics);
      if (!scopes.empty()) {
        scopes.back()[clause.name] = {IsOwnershipBorrowedExpr(
            clause.value.get(), scopes, callable_contracts)};
      }
      return;
    }
    case ForClause::Kind::Assign: {
      const bool value_is_borrowed =
          IsOwnershipBorrowedExpr(clause.value.get(), scopes, callable_contracts);
      DiagnoseOwnershipBorrowedEscapeExpr(clause.value.get(), scopes,
                                          callable_contracts, diagnostics);
      if (auto *binding = LookupOwnershipBorrowedBinding(scopes, clause.name)) {
        binding->borrowed = value_is_borrowed;
        binding->consumed = false;
      }
      return;
    }
  }
}

static void DiagnoseOwnershipBorrowedEscapeStatement(
    const Stmt *stmt, std::vector<OwnershipBorrowedScope> &scopes,
    const std::unordered_map<std::string, OwnershipBorrowedCallableContract>
        &callable_contracts,
    const OwnershipBorrowedReturnContract &return_contract,
    std::vector<std::string> &diagnostics) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
    case Stmt::Kind::Let: {
      const LetStmt *let = stmt->let_stmt.get();
      if (let == nullptr || scopes.empty()) {
        return;
      }
      const bool borrowed =
          IsOwnershipBorrowedExpr(let->value.get(), scopes, callable_contracts);
      DiagnoseOwnershipBorrowedEscapeExpr(let->value.get(), scopes,
                                          callable_contracts, diagnostics);
      scopes.back()[let->name] = {borrowed};
      return;
    }
    case Stmt::Kind::Assign: {
      const AssignStmt *assign = stmt->assign_stmt.get();
      if (assign == nullptr) {
        return;
      }
      const bool borrowed =
          IsOwnershipBorrowedExpr(assign->value.get(), scopes, callable_contracts);
      DiagnoseOwnershipBorrowedEscapeExpr(assign->value.get(), scopes,
                                          callable_contracts, diagnostics);
      if (auto *binding = LookupOwnershipBorrowedBinding(scopes, assign->name)) {
        binding->borrowed = borrowed;
        binding->consumed = false;
      }
      return;
    }
    case Stmt::Kind::Return:
      if (stmt->return_stmt != nullptr) {
        const Expr *value = stmt->return_stmt->value.get();
        const bool borrowed =
            IsOwnershipBorrowedExpr(value, scopes, callable_contracts);
        DiagnoseOwnershipBorrowedEscapeExpr(value, scopes, callable_contracts,
                                            diagnostics);
        DiagnoseOwnershipBorrowedReturnEscape(stmt->return_stmt.get(), borrowed,
                                              return_contract, diagnostics);
      }
      return;
    case Stmt::Kind::Expr:
      if (stmt->expr_stmt != nullptr) {
        DiagnoseOwnershipBorrowedEscapeExpr(stmt->expr_stmt->value.get(), scopes,
                                            callable_contracts, diagnostics);
      }
      return;
    case Stmt::Kind::If: {
      const IfStmt *if_stmt = stmt->if_stmt.get();
      if (if_stmt == nullptr) {
        return;
      }
      DiagnoseOwnershipBorrowedEscapeExpr(if_stmt->condition.get(), scopes,
                                          callable_contracts, diagnostics);
      auto then_scopes = scopes;
      then_scopes.push_back({});
      DiagnoseOwnershipBorrowedEscapeStatements(if_stmt->then_body, then_scopes,
                                                callable_contracts,
                                                return_contract, diagnostics);
      auto else_scopes = scopes;
      else_scopes.push_back({});
      DiagnoseOwnershipBorrowedEscapeStatements(if_stmt->else_body, else_scopes,
                                                callable_contracts,
                                                return_contract, diagnostics);
      MergeOwnershipBorrowedState(scopes, then_scopes);
      MergeOwnershipBorrowedState(scopes, else_scopes);
      return;
    }
    case Stmt::Kind::DoWhile: {
      const DoWhileStmt *do_while_stmt = stmt->do_while_stmt.get();
      if (do_while_stmt == nullptr) {
        return;
      }
      auto body_scopes = scopes;
      body_scopes.push_back({});
      DiagnoseOwnershipBorrowedEscapeStatements(do_while_stmt->body, body_scopes,
                                                callable_contracts,
                                                return_contract, diagnostics);
      MergeOwnershipBorrowedState(scopes, body_scopes);
      DiagnoseOwnershipBorrowedEscapeExpr(do_while_stmt->condition.get(), scopes,
                                          callable_contracts, diagnostics);
      return;
    }
    case Stmt::Kind::For: {
      const ForStmt *for_stmt = stmt->for_stmt.get();
      if (for_stmt == nullptr) {
        return;
      }
      auto loop_scopes = scopes;
      loop_scopes.push_back({});
      DiagnoseOwnershipBorrowedEscapeForClause(for_stmt->init, loop_scopes,
                                               callable_contracts, diagnostics);
      DiagnoseOwnershipBorrowedEscapeExpr(for_stmt->condition.get(), loop_scopes,
                                          callable_contracts, diagnostics);
      DiagnoseOwnershipBorrowedEscapeForClause(for_stmt->step, loop_scopes,
                                               callable_contracts, diagnostics);
      loop_scopes.push_back({});
      DiagnoseOwnershipBorrowedEscapeStatements(for_stmt->body, loop_scopes,
                                                callable_contracts,
                                                return_contract, diagnostics);
      MergeOwnershipBorrowedState(scopes, loop_scopes);
      return;
    }
    case Stmt::Kind::ForIn: {
      const ForInStmt *for_in_stmt = stmt->for_in_stmt.get();
      if (for_in_stmt == nullptr) {
        return;
      }
      DiagnoseOwnershipBorrowedEscapeExpr(for_in_stmt->collection.get(),
                                          scopes, callable_contracts,
                                          diagnostics);
      auto loop_scopes = scopes;
      loop_scopes.push_back({});
      if (!for_in_stmt->value_name.empty()) {
        loop_scopes.back()[for_in_stmt->value_name] = {};
      }
      if (for_in_stmt->has_key_binding && !for_in_stmt->key_name.empty()) {
        loop_scopes.back()[for_in_stmt->key_name] = {};
      }
      DiagnoseOwnershipBorrowedEscapeStatements(for_in_stmt->body, loop_scopes,
                                                callable_contracts,
                                                return_contract, diagnostics);
      MergeOwnershipBorrowedState(scopes, loop_scopes);
      return;
    }
    case Stmt::Kind::Switch: {
      const SwitchStmt *switch_stmt = stmt->switch_stmt.get();
      if (switch_stmt == nullptr) {
        return;
      }
      DiagnoseOwnershipBorrowedEscapeExpr(switch_stmt->condition.get(), scopes,
                                          callable_contracts, diagnostics);
      for (const auto &case_stmt : switch_stmt->cases) {
        auto case_scopes = scopes;
        case_scopes.push_back({});
        DiagnoseOwnershipBorrowedEscapeStatements(case_stmt.body, case_scopes,
                                                  callable_contracts,
                                                  return_contract, diagnostics);
        MergeOwnershipBorrowedState(scopes, case_scopes);
      }
      return;
    }
    case Stmt::Kind::While: {
      const WhileStmt *while_stmt = stmt->while_stmt.get();
      if (while_stmt == nullptr) {
        return;
      }
      DiagnoseOwnershipBorrowedEscapeExpr(while_stmt->condition.get(), scopes,
                                          callable_contracts, diagnostics);
      auto body_scopes = scopes;
      body_scopes.push_back({});
      DiagnoseOwnershipBorrowedEscapeStatements(while_stmt->body, body_scopes,
                                                callable_contracts,
                                                return_contract, diagnostics);
      MergeOwnershipBorrowedState(scopes, body_scopes);
      return;
    }
    case Stmt::Kind::Block:
      if (stmt->block_stmt != nullptr) {
        auto block_scopes = scopes;
        block_scopes.push_back({});
        DiagnoseOwnershipBorrowedEscapeStatements(stmt->block_stmt->body,
                                                  block_scopes,
                                                  callable_contracts,
                                                  return_contract, diagnostics);
        MergeOwnershipBorrowedState(scopes, block_scopes);
      }
      return;
    case Stmt::Kind::Defer:
      if (stmt->block_stmt != nullptr) {
        auto defer_scopes = scopes;
        defer_scopes.push_back({});
        DiagnoseOwnershipBorrowedEscapeStatements(stmt->block_stmt->body,
                                                  defer_scopes,
                                                  callable_contracts,
                                                  return_contract, diagnostics);
      }
      return;
    case Stmt::Kind::CollectionMutation:
      if (stmt->collection_mutation_stmt != nullptr) {
        DiagnoseOwnershipBorrowedEscapeExpr(
            stmt->collection_mutation_stmt->key_or_index.get(), scopes,
            callable_contracts, diagnostics);
        DiagnoseOwnershipBorrowedEscapeExpr(
            stmt->collection_mutation_stmt->value.get(), scopes,
            callable_contracts, diagnostics);
      }
      return;
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return;
  }
}

static void DiagnoseOwnershipBorrowedEscapeStatements(
    const std::vector<std::unique_ptr<Stmt>> &statements,
    std::vector<OwnershipBorrowedScope> &scopes,
    const std::unordered_map<std::string, OwnershipBorrowedCallableContract>
        &callable_contracts,
    const OwnershipBorrowedReturnContract &return_contract,
    std::vector<std::string> &diagnostics) {
  for (const auto &stmt : statements) {
    DiagnoseOwnershipBorrowedEscapeStatement(stmt.get(), scopes,
                                             callable_contracts,
                                             return_contract, diagnostics);
  }
}
