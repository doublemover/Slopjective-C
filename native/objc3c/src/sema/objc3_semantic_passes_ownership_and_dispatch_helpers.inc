      auto body_scopes = scopes;
      body_scopes.push_back({});
      DiagnoseOwnershipResourceMoveStatements(do_while_stmt->body, body_scopes,
                                          diagnostics);
      MergeOwnershipResourceMoveState(scopes, body_scopes);
      DiagnoseOwnershipResourceMoveExpr(do_while_stmt->condition.get(), scopes,
                                    diagnostics);
      return;
    }
    case Stmt::Kind::For: {
      const ForStmt *for_stmt = stmt->for_stmt.get();
      if (for_stmt == nullptr) {
        return;
      }
      auto loop_scopes = scopes;
      loop_scopes.push_back({});
      DiagnoseOwnershipResourceMoveForClause(for_stmt->init, loop_scopes,
                                         diagnostics);
      DiagnoseOwnershipResourceMoveExpr(for_stmt->condition.get(), loop_scopes,
                                    diagnostics);
      DiagnoseOwnershipResourceMoveForClause(for_stmt->step, loop_scopes,
                                         diagnostics);
      loop_scopes.push_back({});
      DiagnoseOwnershipResourceMoveStatements(for_stmt->body, loop_scopes,
                                          diagnostics);
      MergeOwnershipResourceMoveState(scopes, loop_scopes);
      return;
    }
    case Stmt::Kind::Switch: {
      const SwitchStmt *switch_stmt = stmt->switch_stmt.get();
      if (switch_stmt == nullptr) {
        return;
      }
      DiagnoseOwnershipResourceMoveExpr(switch_stmt->condition.get(), scopes,
                                    diagnostics);
      for (const auto &case_stmt : switch_stmt->cases) {
        auto case_scopes = scopes;
        case_scopes.push_back({});
        DiagnoseOwnershipResourceMoveStatements(case_stmt.body, case_scopes,
                                            diagnostics);
        MergeOwnershipResourceMoveState(scopes, case_scopes);
      }
      return;
    }
    case Stmt::Kind::While: {
      const WhileStmt *while_stmt = stmt->while_stmt.get();
      if (while_stmt == nullptr) {
        return;
      }
      DiagnoseOwnershipResourceMoveExpr(while_stmt->condition.get(), scopes,
                                    diagnostics);
      auto body_scopes = scopes;
      body_scopes.push_back({});
      DiagnoseOwnershipResourceMoveStatements(while_stmt->body, body_scopes,
                                          diagnostics);
      MergeOwnershipResourceMoveState(scopes, body_scopes);
      return;
    }
    case Stmt::Kind::Block:
      if (stmt->block_stmt != nullptr) {
        auto block_scopes = scopes;
        block_scopes.push_back({});
        DiagnoseOwnershipResourceMoveStatements(stmt->block_stmt->body, block_scopes,
                                            diagnostics);
        MergeOwnershipResourceMoveState(scopes, block_scopes);
      }
      return;
    case Stmt::Kind::Defer:
      if (stmt->block_stmt != nullptr) {
        auto defer_scopes = scopes;
        defer_scopes.push_back({});
        DiagnoseOwnershipResourceMoveStatements(stmt->block_stmt->body, defer_scopes,
                                            diagnostics);
      }
      return;
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return;
  }
}

static void DiagnoseOwnershipResourceMoveStatements(
    const std::vector<std::unique_ptr<Stmt>> &statements,
    std::vector<OwnershipResourceMoveScope> &scopes,
    std::vector<std::string> &diagnostics) {
  for (const auto &stmt : statements) {
    DiagnoseOwnershipResourceMoveStatement(stmt.get(), scopes, diagnostics);
  }
}

static void DiagnoseOwnershipResourceMoveSemantics(
    const std::vector<std::unique_ptr<Stmt>> &statements,
    const std::vector<SemanticScope> &semantic_scopes,
    std::vector<std::string> &diagnostics) {
  std::vector<OwnershipResourceMoveScope> scopes;
  scopes.reserve(semantic_scopes.size());
  for (const auto &semantic_scope : semantic_scopes) {
    OwnershipResourceMoveScope scope;
    for (const auto &[name, _] : semantic_scope) {
      scope[name] = {};
    }
    scopes.push_back(std::move(scope));
  }
  DiagnoseOwnershipResourceMoveStatements(statements, scopes, diagnostics);
}

struct OwnershipResourceMoveSiteProfile {
  std::size_t cleanup_owned_local_sites = 0;
  std::size_t resource_move_capture_sites = 0;
};

static void CollectOwnershipResourceMoveExprSites(const Expr *expr,
                                              OwnershipResourceMoveSiteProfile &profile);

static void CollectOwnershipResourceMoveStmtSites(const Stmt *stmt,
                                              OwnershipResourceMoveSiteProfile &profile) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
    case Stmt::Kind::Let:
      if (stmt->let_stmt != nullptr) {
        if (IsOwnershipCleanupOwnedLocal(*stmt->let_stmt)) {
          ++profile.cleanup_owned_local_sites;
        }
        CollectOwnershipResourceMoveExprSites(stmt->let_stmt->value.get(), profile);
      }
      return;
    case Stmt::Kind::Assign:
      if (stmt->assign_stmt != nullptr) {
        CollectOwnershipResourceMoveExprSites(stmt->assign_stmt->value.get(), profile);
      }
      return;
    case Stmt::Kind::Return:
      if (stmt->return_stmt != nullptr) {
        CollectOwnershipResourceMoveExprSites(stmt->return_stmt->value.get(), profile);
      }
      return;
    case Stmt::Kind::If:
      if (stmt->if_stmt != nullptr) {
        CollectOwnershipResourceMoveExprSites(stmt->if_stmt->condition.get(), profile);
        for (const auto &body_stmt : stmt->if_stmt->then_body) {
          CollectOwnershipResourceMoveStmtSites(body_stmt.get(), profile);
        }
        for (const auto &body_stmt : stmt->if_stmt->else_body) {
          CollectOwnershipResourceMoveStmtSites(body_stmt.get(), profile);
        }
      }
      return;
    case Stmt::Kind::DoWhile:
      if (stmt->do_while_stmt != nullptr) {
        for (const auto &body_stmt : stmt->do_while_stmt->body) {
          CollectOwnershipResourceMoveStmtSites(body_stmt.get(), profile);
        }
        CollectOwnershipResourceMoveExprSites(stmt->do_while_stmt->condition.get(), profile);
      }
      return;
    case Stmt::Kind::For:
      if (stmt->for_stmt != nullptr) {
        CollectOwnershipResourceMoveExprSites(stmt->for_stmt->init.value.get(), profile);
        CollectOwnershipResourceMoveExprSites(stmt->for_stmt->condition.get(), profile);
        CollectOwnershipResourceMoveExprSites(stmt->for_stmt->step.value.get(), profile);
        for (const auto &body_stmt : stmt->for_stmt->body) {
          CollectOwnershipResourceMoveStmtSites(body_stmt.get(), profile);
        }
      }
      return;
    case Stmt::Kind::Switch:
      if (stmt->switch_stmt != nullptr) {
        CollectOwnershipResourceMoveExprSites(stmt->switch_stmt->condition.get(), profile);
        for (const auto &case_stmt : stmt->switch_stmt->cases) {
          for (const auto &body_stmt : case_stmt.body) {
            CollectOwnershipResourceMoveStmtSites(body_stmt.get(), profile);
          }
        }
      }
      return;
    case Stmt::Kind::While:
      if (stmt->while_stmt != nullptr) {
        CollectOwnershipResourceMoveExprSites(stmt->while_stmt->condition.get(), profile);
        for (const auto &body_stmt : stmt->while_stmt->body) {
          CollectOwnershipResourceMoveStmtSites(body_stmt.get(), profile);
        }
      }
      return;
    case Stmt::Kind::Block:
    case Stmt::Kind::Defer:
      if (stmt->block_stmt != nullptr) {
        for (const auto &body_stmt : stmt->block_stmt->body) {
          CollectOwnershipResourceMoveStmtSites(body_stmt.get(), profile);
        }
      }
      return;
    case Stmt::Kind::Expr:
      if (stmt->expr_stmt != nullptr) {
        CollectOwnershipResourceMoveExprSites(stmt->expr_stmt->value.get(), profile);
      }
      return;
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return;
  }
}

static void CollectOwnershipResourceMoveExprSites(const Expr *expr,
                                              OwnershipResourceMoveSiteProfile &profile) {
  if (expr == nullptr) {
    return;
  }
  if (expr->kind == Expr::Kind::BlockLiteral) {
    for (const auto &item : expr->block_explicit_capture_items_source_order) {
      if (item.mode == "move") {
        ++profile.resource_move_capture_sites;
      }
    }
  }
  CollectOwnershipResourceMoveExprSites(expr->receiver.get(), profile);
  CollectOwnershipResourceMoveExprSites(expr->left.get(), profile);
  CollectOwnershipResourceMoveExprSites(expr->right.get(), profile);
  CollectOwnershipResourceMoveExprSites(expr->third.get(), profile);
  for (const auto &arg : expr->args) {
    CollectOwnershipResourceMoveExprSites(arg.get(), profile);
  }
}

struct OwnershipBorrowedBindingState {
  bool borrowed = false;
};

using OwnershipBorrowedScope =
    std::unordered_map<std::string, OwnershipBorrowedBindingState>;

struct OwnershipBorrowedCallableContract {
  std::vector<bool> param_borrowed;
  bool return_borrowed = false;
};

struct OwnershipBorrowedReturnContract {
  bool valid = false;
};

static OwnershipBorrowedBindingState *LookupOwnershipBorrowedBinding(
    std::vector<OwnershipBorrowedScope> &scopes, const std::string &name) {
  for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
    auto found = it->find(name);
    if (found != it->end()) {
      return &found->second;
    }
  }
  return nullptr;
}

static const OwnershipBorrowedBindingState *LookupOwnershipBorrowedBinding(
    const std::vector<OwnershipBorrowedScope> &scopes, const std::string &name) {
  for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
    auto found = it->find(name);
    if (found != it->end()) {
      return &found->second;
    }
  }
  return nullptr;
}

static void MergeOwnershipBorrowedState(
    std::vector<OwnershipBorrowedScope> &target_scopes,
    const std::vector<OwnershipBorrowedScope> &branch_scopes) {
  const std::size_t scope_count =
      std::min(target_scopes.size(), branch_scopes.size());
  for (std::size_t scope_index = 0; scope_index < scope_count; ++scope_index) {
    for (auto &[name, target_state] : target_scopes[scope_index]) {
      auto branch_it = branch_scopes[scope_index].find(name);
      if (branch_it == branch_scopes[scope_index].end()) {
        continue;
      }
      target_state.borrowed = target_state.borrowed || branch_it->second.borrowed;
    }
  }
}

static OwnershipBorrowedCallableContract BuildOwnershipBorrowedCallableContract(
    const FunctionDecl &fn) {
  OwnershipBorrowedCallableContract contract;
  contract.param_borrowed.reserve(fn.params.size());
  for (const auto &param : fn.params) {
    contract.param_borrowed.push_back(param.borrowed_pointer_qualified);
  }
  contract.return_borrowed =
      fn.return_borrowed_pointer_qualified || fn.objc_returns_borrowed_declared;
  return contract;
}

static std::unordered_map<std::string, OwnershipBorrowedCallableContract>
BuildOwnershipBorrowedCallableContracts(const Objc3Program &program) {
  std::unordered_map<std::string, OwnershipBorrowedCallableContract> contracts;
  for (const auto &fn : program.functions) {
    contracts.emplace(fn.name, BuildOwnershipBorrowedCallableContract(fn));
  }
  return contracts;
}

static OwnershipBorrowedReturnContract BuildOwnershipBorrowedReturnContract(
    const FunctionDecl &fn) {
  OwnershipBorrowedReturnContract contract;
  contract.valid = fn.return_borrowed_pointer_qualified &&
                   fn.objc_returns_borrowed_declared &&
                   fn.objc_returns_borrowed_owner_index < fn.params.size();
  return contract;
}

static OwnershipBorrowedReturnContract BuildOwnershipBorrowedReturnContract(
    const Objc3MethodDecl &method) {
  OwnershipBorrowedReturnContract contract;
  contract.valid = method.return_borrowed_pointer_qualified &&
                   method.objc_returns_borrowed_declared &&
                   method.objc_returns_borrowed_owner_index < method.params.size();
  return contract;
}

static bool IsRetainableFamilyCanonicalOperationAttribute(
    const std::string &attribute_name) {
  return attribute_name == "objc_family_retain" ||
         attribute_name == "objc_family_release" ||
         attribute_name == "objc_family_autorelease";
}

static bool IsRetainableFamilyReturnAliasAttribute(
    const std::string &attribute_name) {
  return attribute_name == "os_returns_retained" ||
         attribute_name == "os_returns_not_retained" ||
         attribute_name == "cf_returns_retained" ||
         attribute_name == "cf_returns_not_retained" ||
         attribute_name == "ns_returns_retained" ||
         attribute_name == "ns_returns_not_retained";
}

static bool IsRetainableFamilyConsumedAliasAttribute(
    const std::string &attribute_name) {
  return attribute_name == "os_consumed" ||
         attribute_name == "cf_consumed" ||
         attribute_name == "ns_consumed";
}

template <typename TCallableDecl>
static bool HasObjCReferenceParameter(const TCallableDecl &decl) {
  return std::any_of(
      decl.params.begin(), decl.params.end(), [](const auto &param) {
        return IsObjCReferenceSemanticType(MakeSemanticTypeFromParam(param));
      });
}

template <typename TCallableDecl>
static bool ReturnsObjCReference(const TCallableDecl &decl) {
  if constexpr (std::is_same_v<TCallableDecl, FunctionDecl>) {
    return IsObjCReferenceSemanticType(MakeSemanticTypeFromFunctionReturn(decl));
  } else {
    return IsObjCReferenceSemanticType(MakeSemanticTypeFromMethodReturn(decl));
  }
}

template <typename TCallableDecl>
static bool ReturnsVoid(const TCallableDecl &decl) {
  if constexpr (std::is_same_v<TCallableDecl, FunctionDecl>) {
    return IsVoidSemanticType(MakeSemanticTypeFromFunctionReturn(decl));
  } else {
    return IsVoidSemanticType(MakeSemanticTypeFromMethodReturn(decl));
  }
}

template <typename TCallableDecl>
static void DiagnoseOwnershipRetainableFamilyCallableLegality(
    const TCallableDecl &decl, const std::string &callable_label,
    std::vector<std::string> &diagnostics) {
  std::size_t canonical_operation_count = 0;
  bool has_returns_retained_alias = false;
  bool has_returns_not_retained_alias = false;
  bool has_consumed_alias = false;
  bool has_retain = false;
  bool has_release = false;
  bool has_autorelease = false;

  for (const auto &attribute_name :
       decl.retainable_c_family_callable_attributes) {
    if (IsRetainableFamilyCanonicalOperationAttribute(attribute_name)) {
      ++canonical_operation_count;
    }
    if (attribute_name == "objc_family_retain") {
      has_retain = true;
    } else if (attribute_name == "objc_family_release") {
      has_release = true;
    } else if (attribute_name == "objc_family_autorelease") {
      has_autorelease = true;
    } else if (attribute_name == "os_returns_retained" ||
               attribute_name == "cf_returns_retained" ||
               attribute_name == "ns_returns_retained") {
      has_returns_retained_alias = true;
    } else if (attribute_name == "os_returns_not_retained" ||
               attribute_name == "cf_returns_not_retained" ||
               attribute_name == "ns_returns_not_retained") {
      has_returns_not_retained_alias = true;
    } else if (IsRetainableFamilyConsumedAliasAttribute(attribute_name)) {
      has_consumed_alias = true;
    }
  }

  if (canonical_operation_count > 1u ||
      (has_returns_retained_alias && has_returns_not_retained_alias)) {
    diagnostics.push_back(MakeDiag(
        decl.line, decl.column, "O3S304",
        "retainable-family legality failed: conflicting retainable-family annotations on " +
            callable_label));
  }

  const bool returns_objc_reference = ReturnsObjCReference(decl);
  const bool has_objc_reference_parameter = HasObjCReferenceParameter(decl);
  if ((has_retain || has_autorelease) &&
      (!returns_objc_reference || !has_objc_reference_parameter)) {
    diagnostics.push_back(MakeDiag(
        decl.line, decl.column, "O3S305",
        "retainable-family legality failed: " + callable_label +
            " must return an Objective-C reference and accept an Objective-C reference parameter for family retain/autorelease annotations"));
  } else if (has_release &&
             (!ReturnsVoid(decl) || !has_objc_reference_parameter)) {
    diagnostics.push_back(MakeDiag(
        decl.line, decl.column, "O3S305",
        "retainable-family legality failed: " + callable_label +
            " must return void and accept an Objective-C reference parameter for family release annotations"));
  }

  if ((has_returns_retained_alias || has_returns_not_retained_alias) &&
      !returns_objc_reference) {
    diagnostics.push_back(MakeDiag(
        decl.line, decl.column, "O3S306",
        "retainable-family legality failed: returns-retained compatibility aliases on " +
            callable_label +
            " require an Objective-C reference return type"));
  }
  if (has_consumed_alias && !has_objc_reference_parameter) {
    diagnostics.push_back(MakeDiag(
        decl.line, decl.column, "O3S306",
        "retainable-family legality failed: consumed compatibility aliases on " +
            callable_label +
            " require at least one Objective-C reference parameter"));
  }
}

static bool IsOwnershipBorrowedExpr(
    const Expr *expr, const std::vector<OwnershipBorrowedScope> &scopes,
    const std::unordered_map<std::string, OwnershipBorrowedCallableContract>
        &callable_contracts) {
  if (expr == nullptr) {
    return false;
  }
  switch (expr->kind) {
    case Expr::Kind::Identifier: {
      const auto *binding = LookupOwnershipBorrowedBinding(scopes, expr->ident);
      return binding != nullptr && binding->borrowed;
    }
    case Expr::Kind::Call: {
      auto contract_it = callable_contracts.find(expr->ident);
      return contract_it != callable_contracts.end() &&
             contract_it->second.return_borrowed;
    }
    case Expr::Kind::Conditional:
      return IsOwnershipBorrowedExpr(expr->right.get(), scopes, callable_contracts) ||
             IsOwnershipBorrowedExpr(expr->third.get(), scopes, callable_contracts);
    default:
      return false;
  }
}

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

static void DiagnoseOwnershipBorrowedEscapeExpr(
    const Expr *expr, std::vector<OwnershipBorrowedScope> &scopes,
    const std::unordered_map<std::string, OwnershipBorrowedCallableContract>
        &callable_contracts,
    std::vector<std::string> &diagnostics) {
  if (expr == nullptr) {
    return;
  }
  switch (expr->kind) {
    case Expr::Kind::BlockLiteral: {
      for (const auto &arg : expr->args) {
        DiagnoseOwnershipBorrowedEscapeExpr(arg.get(), scopes, callable_contracts,
                                        diagnostics);
      }
      const bool escaping_block =
          expr->block_storage_escape_to_heap ||
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
      return;
    }
    case Expr::Kind::Call: {
      DiagnoseOwnershipBorrowedEscapeExpr(expr->receiver.get(), scopes,
                                      callable_contracts, diagnostics);
      auto contract_it = callable_contracts.find(expr->ident);
      for (std::size_t index = 0; index < expr->args.size(); ++index) {
        const Expr *arg = expr->args[index].get();
        DiagnoseOwnershipBorrowedEscapeExpr(arg, scopes, callable_contracts,
                                        diagnostics);
        if (!IsOwnershipBorrowedExpr(arg, scopes, callable_contracts)) {
          continue;
        }
        const bool borrowed_boundary_proven =
            contract_it != callable_contracts.end() &&
            index < contract_it->second.param_borrowed.size() &&
            contract_it->second.param_borrowed[index];
        if (!borrowed_boundary_proven) {
          diagnostics.push_back(MakeDiag(
              arg->line, arg->column, "O3S298",
              "borrowed pointer passed to parameter not proven non-escaping; copy to owned storage, extend owner lifetime, or mark explicitly unsafe"));
        }
      }
      return;
    }
    case Expr::Kind::MessageSend:
      DiagnoseOwnershipBorrowedEscapeExpr(expr->receiver.get(), scopes,
                                      callable_contracts, diagnostics);
      for (const auto &arg : expr->args) {
        DiagnoseOwnershipBorrowedEscapeExpr(arg.get(), scopes, callable_contracts,
                                        diagnostics);
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
    case Expr::Kind::Identifier:
    case Expr::Kind::Number:
    case Expr::Kind::BoolLiteral:
    case Expr::Kind::NilLiteral:
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
        if (borrowed && !return_contract.valid) {
          diagnostics.push_back(MakeDiag(
              stmt->return_stmt->line, stmt->return_stmt->column, "O3S300",
              "borrowed return requires objc_returns_borrowed(owner_index=...) naming a valid owner parameter"));
        }
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
                                              block_scopes, callable_contracts,
                                              return_contract, diagnostics);
        MergeOwnershipBorrowedState(scopes, block_scopes);
      }
      return;
    case Stmt::Kind::Defer:
      if (stmt->block_stmt != nullptr) {
        auto defer_scopes = scopes;
        defer_scopes.push_back({});
        DiagnoseOwnershipBorrowedEscapeStatements(stmt->block_stmt->body,
                                              defer_scopes, callable_contracts,
                                              return_contract, diagnostics);
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
    DiagnoseOwnershipBorrowedEscapeStatement(stmt.get(), scopes, callable_contracts,
                                         return_contract, diagnostics);
  }
}

static void DiagnoseOwnershipBorrowedPointerEscapeSemantics(
    const std::vector<std::unique_ptr<Stmt>> &statements,
    const std::vector<SemanticScope> &semantic_scopes,
    const std::unordered_set<std::string> &borrowed_parameter_names,
    const std::unordered_map<std::string, OwnershipBorrowedCallableContract>
        &callable_contracts,
    const OwnershipBorrowedReturnContract &return_contract,
    std::vector<std::string> &diagnostics) {
  std::vector<OwnershipBorrowedScope> scopes;
  scopes.reserve(semantic_scopes.size());
  for (const auto &semantic_scope : semantic_scopes) {
    OwnershipBorrowedScope scope;
    for (const auto &[name, _] : semantic_scope) {
      OwnershipBorrowedBindingState state;
      state.borrowed = borrowed_parameter_names.find(name) !=
                       borrowed_parameter_names.end();
      scope[name] = state;
    }
    scopes.push_back(std::move(scope));
  }
  DiagnoseOwnershipBorrowedEscapeStatements(statements, scopes, callable_contracts,
                                        return_contract, diagnostics);
}

#include "sema/objc3_semantic_passes_ownership_site_profiles.inc"

static const Objc3PropertyInfo *FindTypedKeyPathPropertyOnOwner(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &owner_name,
    const std::string &property_name) {
  auto merge_it = surface.category_merge_surfaces.find(owner_name);
  if (merge_it != surface.category_merge_surfaces.end()) {
    auto property_it = merge_it->second.merged_properties.find(property_name);
    if (property_it != merge_it->second.merged_properties.end()) {
      return &property_it->second;
    }
  }
  auto interface_it = surface.interfaces.find(owner_name);
  if (interface_it != surface.interfaces.end()) {
    auto property_it = interface_it->second.properties.find(property_name);
    if (property_it != interface_it->second.properties.end()) {
      return &property_it->second;
    }
  }
  auto implementation_it = surface.implementations.find(owner_name);
  if (implementation_it != surface.implementations.end()) {
    auto property_it = implementation_it->second.properties.find(property_name);
    if (property_it != implementation_it->second.properties.end()) {
      return &property_it->second;
    }
  }
  return nullptr;
}

static bool ResolveTypedKeyPathClassRootOwner(
    const std::string &root_name,
    const Objc3MessageSendResolutionContext &context,
    std::string &owner_name) {
  if (context.surface == nullptr || root_name.empty()) {
    return false;
  }
  if (context.surface->interfaces.find(root_name) !=
      context.surface->interfaces.end()) {
    owner_name = root_name;
    return true;
  }
  if (context.surface->implementations.find(root_name) !=
      context.surface->implementations.end()) {
    owner_name = root_name;
    return true;
  }
  return false;
}

static bool TryResolveTypedKeyPathValidatedOwner(
    const Expr &expr,
    const std::vector<SemanticScope> &scopes,
    const std::unordered_map<std::string, ValueType> &globals,
    const std::unordered_map<std::string, FunctionInfo> &functions,
    const Objc3MessageSendResolutionContext &context,
    std::string &owner_name,
    bool &is_class_root) {
  owner_name.clear();
  is_class_root = false;
  if (expr.kind != Expr::Kind::Identifier ||
      !expr.typed_keypath_literal_enabled) {
    return false;
  }
  if (expr.typed_keypath_root_is_self) {
    if (context.inside_method &&
        !context.current_implementation_name.empty()) {
      owner_name = context.current_implementation_name;
    }
    return true;
  }

  const SemanticTypeInfo local_type =
      ScopeLookupType(scopes, expr.typed_keypath_root_name);
  if (!IsUnknownSemanticType(local_type)) {
    return IsObjCReferenceSemanticType(local_type);
  }

  auto global_it = globals.find(expr.typed_keypath_root_name);
  if (global_it != globals.end()) {
    if (global_it->second == ValueType::ObjCClass &&
        ResolveTypedKeyPathClassRootOwner(expr.typed_keypath_root_name, context,
                                          owner_name)) {
      is_class_root = true;
      return true;
    }
    return IsObjCReferenceSemanticType(
        MakeSemanticTypeFromGlobal(global_it->second));
  }

  if (functions.find(expr.typed_keypath_root_name) != functions.end()) {
    return false;
  }

  if (ResolveTypedKeyPathClassRootOwner(expr.typed_keypath_root_name, context,
                                        owner_name)) {
    is_class_root = true;
    return true;
  }

  return false;
}

static Objc3ResolvedMessageSendMethod ResolveConcreteMessageSendMethod(
    const Expr &expr,
    const Objc3MessageSendResolutionContext &context);
static Objc3ResolvedMessageSendMethod ResolveConcreteOwnerMethod(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &owner_name,
    const std::string &selector,
    bool is_class_method);
static Objc3ResolvedMessageSendPropertyAccessor
ResolveConcreteOwnerPropertyAccessor(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &owner_name,
    const std::string &selector,
    bool is_class_property);
static Expr::DispatchSurfaceKind ResolveNormalizedMessageSendDispatchSurfaceKind(
    const Expr &expr,
    const Objc3MessageSendResolutionContext &context);

struct ProtocolCompositionInfo {
  bool has_protocol_composition = false;
  std::vector<std::string> names_lexicographic;
  bool has_invalid_protocol_composition = false;
};

static std::string NormalizeMetaprogrammingRawDeriveToken(std::string token) {
  token = TrimAsciiWhitespace(token);
  if (token.size() >= 2u &&
      ((token.front() == '"' && token.back() == '"') ||
       (token.front() == '\'' && token.back() == '\''))) {
    token = token.substr(1, token.size() - 2u);
  }
  return token;
}

static std::string NormalizeMetaprogrammingRawMacroToken(std::string token) {
  token = TrimAsciiWhitespace(token);
  if (token.size() >= 2u &&
      ((token.front() == '"' && token.back() == '"') ||
       (token.front() == '\'' && token.back() == '\''))) {
    token = token.substr(1, token.size() - 2u);
  }
  return token;
}

static std::string NormalizeMetaprogrammingRawPropertyBehaviorToken(std::string token) {
  token = TrimAsciiWhitespace(token);
  if (token.size() >= 2u &&
      ((token.front() == '"' && token.back() == '"') ||
       (token.front() == '\'' && token.back() == '\''))) {
    token = token.substr(1, token.size() - 2u);
  }
  return token;
}

static std::string BuildMetaprogrammingLowercaseDeriveToken(std::string token) {
  token = NormalizeMetaprogrammingRawDeriveToken(std::move(token));
  return objc3c::support::LowercaseAscii(token);
}

static std::string NormalizeMetaprogrammingSupportedDeriveName(
    const std::string &derive_name) {
  const std::string lowered = BuildMetaprogrammingLowercaseDeriveToken(derive_name);
  if (lowered == "equatable" || lowered == "equality") {
    return "Equality";
  }
  if (lowered == "hash") {
    return "Hash";
  }
  if (lowered == "debugdescription") {
    return "DebugDescription";
  }
  return "";
}

static std::string BuildMetaprogrammingLowercasePropertyBehaviorToken(std::string token) {
  token = NormalizeMetaprogrammingRawPropertyBehaviorToken(std::move(token));
  return objc3c::support::LowercaseAscii(token);
}

static std::string NormalizeMetaprogrammingSupportedPropertyBehaviorName(
    const std::string &behavior_name) {
  const std::string lowered =
      BuildMetaprogrammingLowercasePropertyBehaviorToken(behavior_name);
  if (lowered == "observed") {
    return "Observed";
  }
  if (lowered == "projected") {
    return "Projected";
  }
  return "";
}

static std::string BuildMetaprogrammingLowercaseMacroToken(std::string token) {
  token = NormalizeMetaprogrammingRawMacroToken(std::move(token));
  return objc3c::support::LowercaseAscii(token);
}

static bool IsSupportedMetaprogrammingMacroPackageName(const std::string &package_name) {
  const std::string lowered = BuildMetaprogrammingLowercaseMacroToken(package_name);
  return lowered == "std.metaprogramming" ||
         lowered.rfind("std.metaprogramming.", 0) == 0u;
}

static bool IsSupportedMetaprogrammingMacroProvenanceToken(
    const std::string &provenance_name) {
  const std::string normalized =
      NormalizeMetaprogrammingRawMacroToken(provenance_name);
  const std::string lowered = BuildMetaprogrammingLowercaseMacroToken(provenance_name);
  if (normalized != lowered) {
    return false;
  }
  constexpr std::string_view prefix = "sha256:";
  if (lowered.rfind(prefix, 0) != 0u) {
    return false;
  }
  const std::string_view digest = std::string_view(lowered).substr(prefix.size());
  if (digest.size() < 6u || digest.size() > 64u) {
    return false;
  }
  return std::all_of(digest.begin(), digest.end(), [](char c) {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
  });
}

static std::string BuildMetaprogrammingDerivedSelectorForInventoryRow(
    const std::string &canonical_derive_name) {
  if (canonical_derive_name == "Equality") {
    return "isEqual:";
  }
  if (canonical_derive_name == "Hash") {
    return "hash";
  }
  if (canonical_derive_name == "DebugDescription") {
