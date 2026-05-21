static const OwnershipBorrowedCallableContract *
FindOwnershipBorrowedCallableContract(
    const std::unordered_map<std::string, OwnershipBorrowedCallableContract>
        &callable_contracts,
    const std::string &callable_name) {
  auto contract_it = callable_contracts.find(callable_name);
  if (contract_it == callable_contracts.end()) {
    return nullptr;
  }
  return &contract_it->second;
}

static bool IsOwnershipBorrowedCallableReturnBorrowed(
    const std::unordered_map<std::string, OwnershipBorrowedCallableContract>
        &callable_contracts,
    const std::string &callable_name) {
  const auto *contract =
      FindOwnershipBorrowedCallableContract(callable_contracts, callable_name);
  return contract != nullptr && contract->return_borrowed;
}

static bool IsOwnershipBorrowedCallableParameterBorrowed(
    const OwnershipBorrowedCallableContract *contract, std::size_t index) {
  return contract != nullptr && index < contract->param_borrowed.size() &&
         contract->param_borrowed[index];
}

static bool IsOwnershipConsumedCallableParameter(
    const OwnershipBorrowedCallableContract *contract, std::size_t index) {
  return contract != nullptr && index < contract->param_consumed.size() &&
         contract->param_consumed[index];
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
      return binding != nullptr && binding->borrowed && !binding->consumed;
    }
    case Expr::Kind::Call:
      return IsOwnershipBorrowedCallableReturnBorrowed(callable_contracts,
                                                       expr->ident);
    case Expr::Kind::Try:
      return !expr->args.empty() &&
             IsOwnershipBorrowedExpr(expr->args.front().get(), scopes,
                                     callable_contracts);
    case Expr::Kind::Throw:
    case Expr::Kind::KeyPathLiteral:
    case Expr::Kind::StringLiteral:
      return false;
    case Expr::Kind::Conditional:
      return IsOwnershipBorrowedExpr(expr->right.get(), scopes,
                                     callable_contracts) ||
             IsOwnershipBorrowedExpr(expr->third.get(), scopes,
                                     callable_contracts);
    default:
      return false;
  }
}
