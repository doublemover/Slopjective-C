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
