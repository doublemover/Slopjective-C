struct OwnershipBorrowedEscapeSiteProfile {
  std::size_t borrowed_parameter_sites = 0;
  std::size_t borrowed_return_callable_sites = 0;
};

struct OwnershipCaptureListRetainableFamilySiteProfile {
  std::size_t explicit_capture_list_sites = 0;
  std::size_t explicit_capture_item_sites = 0;
  std::size_t explicit_capture_ownership_mode_sites = 0;
  std::size_t retainable_family_callable_sites = 0;
  std::size_t retainable_family_operation_callable_sites = 0;
  std::size_t retainable_family_alias_callable_sites = 0;
};

static void CollectOwnershipBorrowedEscapeExprSites(
    const Expr *expr, const std::unordered_map<std::string, OwnershipBorrowedCallableContract> &callable_contracts,
    OwnershipBorrowedEscapeSiteProfile &profile) {
  if (expr == nullptr) {
    return;
  }
  if (expr->kind == Expr::Kind::Call) {
    auto contract_it = callable_contracts.find(expr->ident);
    if (contract_it != callable_contracts.end() &&
        contract_it->second.return_borrowed) {
      ++profile.borrowed_return_callable_sites;
    }
  }
  CollectOwnershipBorrowedEscapeExprSites(expr->receiver.get(), callable_contracts,
                                      profile);
  CollectOwnershipBorrowedEscapeExprSites(expr->left.get(), callable_contracts,
                                      profile);
  CollectOwnershipBorrowedEscapeExprSites(expr->right.get(), callable_contracts,
                                      profile);
  CollectOwnershipBorrowedEscapeExprSites(expr->third.get(), callable_contracts,
                                      profile);
  for (const auto &arg : expr->args) {
    CollectOwnershipBorrowedEscapeExprSites(arg.get(), callable_contracts, profile);
  }
}

static void CollectOwnershipBorrowedEscapeStmtSites(
    const Stmt *stmt,
    const std::unordered_map<std::string, OwnershipBorrowedCallableContract>
        &callable_contracts,
    OwnershipBorrowedEscapeSiteProfile &profile) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
    case Stmt::Kind::Let:
      if (stmt->let_stmt != nullptr) {
        CollectOwnershipBorrowedEscapeExprSites(stmt->let_stmt->value.get(),
                                            callable_contracts, profile);
      }
      return;
    case Stmt::Kind::Assign:
      if (stmt->assign_stmt != nullptr) {
        CollectOwnershipBorrowedEscapeExprSites(stmt->assign_stmt->value.get(),
                                            callable_contracts, profile);
      }
      return;
    case Stmt::Kind::Return:
      if (stmt->return_stmt != nullptr) {
        CollectOwnershipBorrowedEscapeExprSites(stmt->return_stmt->value.get(),
                                            callable_contracts, profile);
      }
      return;
    case Stmt::Kind::If:
      if (stmt->if_stmt != nullptr) {
        CollectOwnershipBorrowedEscapeExprSites(stmt->if_stmt->condition.get(),
                                            callable_contracts, profile);
        for (const auto &body_stmt : stmt->if_stmt->then_body) {
          CollectOwnershipBorrowedEscapeStmtSites(body_stmt.get(), callable_contracts,
                                              profile);
        }
        for (const auto &body_stmt : stmt->if_stmt->else_body) {
          CollectOwnershipBorrowedEscapeStmtSites(body_stmt.get(), callable_contracts,
                                              profile);
        }
      }
      return;
    case Stmt::Kind::DoWhile:
      if (stmt->do_while_stmt != nullptr) {
        for (const auto &body_stmt : stmt->do_while_stmt->body) {
          CollectOwnershipBorrowedEscapeStmtSites(body_stmt.get(),
                                              callable_contracts, profile);
        }
        CollectOwnershipBorrowedEscapeExprSites(stmt->do_while_stmt->condition.get(),
                                            callable_contracts, profile);
      }
      return;
    case Stmt::Kind::For:
      if (stmt->for_stmt != nullptr) {
        CollectOwnershipBorrowedEscapeExprSites(stmt->for_stmt->init.value.get(),
                                            callable_contracts, profile);
        CollectOwnershipBorrowedEscapeExprSites(stmt->for_stmt->condition.get(),
                                            callable_contracts, profile);
        CollectOwnershipBorrowedEscapeExprSites(stmt->for_stmt->step.value.get(),
                                            callable_contracts, profile);
        for (const auto &body_stmt : stmt->for_stmt->body) {
          CollectOwnershipBorrowedEscapeStmtSites(body_stmt.get(),
                                              callable_contracts, profile);
        }
      }
      return;
    case Stmt::Kind::Switch:
      if (stmt->switch_stmt != nullptr) {
        CollectOwnershipBorrowedEscapeExprSites(stmt->switch_stmt->condition.get(),
                                            callable_contracts, profile);
        for (const auto &case_stmt : stmt->switch_stmt->cases) {
          for (const auto &body_stmt : case_stmt.body) {
            CollectOwnershipBorrowedEscapeStmtSites(body_stmt.get(),
                                                callable_contracts, profile);
          }
        }
      }
      return;
    case Stmt::Kind::While:
      if (stmt->while_stmt != nullptr) {
        CollectOwnershipBorrowedEscapeExprSites(stmt->while_stmt->condition.get(),
                                            callable_contracts, profile);
        for (const auto &body_stmt : stmt->while_stmt->body) {
          CollectOwnershipBorrowedEscapeStmtSites(body_stmt.get(),
                                              callable_contracts, profile);
        }
      }
      return;
    case Stmt::Kind::Block:
    case Stmt::Kind::Defer:
      if (stmt->block_stmt != nullptr) {
        for (const auto &body_stmt : stmt->block_stmt->body) {
          CollectOwnershipBorrowedEscapeStmtSites(body_stmt.get(),
                                              callable_contracts, profile);
        }
      }
      return;
    case Stmt::Kind::Expr:
      if (stmt->expr_stmt != nullptr) {
        CollectOwnershipBorrowedEscapeExprSites(stmt->expr_stmt->value.get(),
                                            callable_contracts, profile);
      }
      return;
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return;
  }
}

static void CollectOwnershipCaptureListRetainableFamilyExprSites(
    const Expr *expr, OwnershipCaptureListRetainableFamilySiteProfile &profile);

static void CollectOwnershipCaptureListRetainableFamilyStmtSites(
    const Stmt *stmt, OwnershipCaptureListRetainableFamilySiteProfile &profile) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
    case Stmt::Kind::Let:
      if (stmt->let_stmt != nullptr) {
        CollectOwnershipCaptureListRetainableFamilyExprSites(
            stmt->let_stmt->value.get(), profile);
      }
      return;
    case Stmt::Kind::Assign:
      if (stmt->assign_stmt != nullptr) {
        CollectOwnershipCaptureListRetainableFamilyExprSites(
            stmt->assign_stmt->value.get(), profile);
      }
      return;
    case Stmt::Kind::Return:
      if (stmt->return_stmt != nullptr) {
        CollectOwnershipCaptureListRetainableFamilyExprSites(
            stmt->return_stmt->value.get(), profile);
      }
      return;
    case Stmt::Kind::If:
      if (stmt->if_stmt != nullptr) {
        CollectOwnershipCaptureListRetainableFamilyExprSites(
            stmt->if_stmt->condition.get(), profile);
        for (const auto &body_stmt : stmt->if_stmt->then_body) {
          CollectOwnershipCaptureListRetainableFamilyStmtSites(body_stmt.get(),
                                                           profile);
        }
        for (const auto &body_stmt : stmt->if_stmt->else_body) {
          CollectOwnershipCaptureListRetainableFamilyStmtSites(body_stmt.get(),
                                                           profile);
        }
      }
      return;
    case Stmt::Kind::DoWhile:
      if (stmt->do_while_stmt != nullptr) {
        for (const auto &body_stmt : stmt->do_while_stmt->body) {
          CollectOwnershipCaptureListRetainableFamilyStmtSites(body_stmt.get(),
                                                           profile);
        }
        CollectOwnershipCaptureListRetainableFamilyExprSites(
            stmt->do_while_stmt->condition.get(), profile);
      }
      return;
    case Stmt::Kind::For:
      if (stmt->for_stmt != nullptr) {
        CollectOwnershipCaptureListRetainableFamilyExprSites(
            stmt->for_stmt->init.value.get(), profile);
        CollectOwnershipCaptureListRetainableFamilyExprSites(
            stmt->for_stmt->condition.get(), profile);
        CollectOwnershipCaptureListRetainableFamilyExprSites(
            stmt->for_stmt->step.value.get(), profile);
        for (const auto &body_stmt : stmt->for_stmt->body) {
          CollectOwnershipCaptureListRetainableFamilyStmtSites(body_stmt.get(),
                                                           profile);
        }
      }
      return;
    case Stmt::Kind::Switch:
      if (stmt->switch_stmt != nullptr) {
        CollectOwnershipCaptureListRetainableFamilyExprSites(
            stmt->switch_stmt->condition.get(), profile);
        for (const auto &switch_case : stmt->switch_stmt->cases) {
          for (const auto &body_stmt : switch_case.body) {
            CollectOwnershipCaptureListRetainableFamilyStmtSites(body_stmt.get(),
                                                             profile);
          }
        }
      }
      return;
    case Stmt::Kind::While:
      if (stmt->while_stmt != nullptr) {
        CollectOwnershipCaptureListRetainableFamilyExprSites(
            stmt->while_stmt->condition.get(), profile);
        for (const auto &body_stmt : stmt->while_stmt->body) {
          CollectOwnershipCaptureListRetainableFamilyStmtSites(body_stmt.get(),
                                                           profile);
        }
      }
      return;
    case Stmt::Kind::Block:
    case Stmt::Kind::Defer:
      if (stmt->block_stmt != nullptr) {
        for (const auto &body_stmt : stmt->block_stmt->body) {
          CollectOwnershipCaptureListRetainableFamilyStmtSites(body_stmt.get(),
                                                           profile);
        }
      }
      return;
    case Stmt::Kind::Expr:
      if (stmt->expr_stmt != nullptr) {
        CollectOwnershipCaptureListRetainableFamilyExprSites(
            stmt->expr_stmt->value.get(), profile);
      }
      return;
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return;
  }
}

static void CollectOwnershipCaptureListRetainableFamilyExprSites(
    const Expr *expr, OwnershipCaptureListRetainableFamilySiteProfile &profile) {
  if (expr == nullptr) {
    return;
  }
  if (expr->kind == Expr::Kind::BlockLiteral) {
    if (expr->block_has_explicit_capture_list) {
      ++profile.explicit_capture_list_sites;
      profile.explicit_capture_item_sites += expr->block_explicit_capture_count;
      profile.explicit_capture_ownership_mode_sites +=
          expr->block_explicit_capture_weak_count +
          expr->block_explicit_capture_unowned_count;
    }
    for (const auto &stmt : expr->block_body) {
      CollectOwnershipCaptureListRetainableFamilyStmtSites(stmt.get(), profile);
    }
    return;
  }
  CollectOwnershipCaptureListRetainableFamilyExprSites(expr->receiver.get(), profile);
  CollectOwnershipCaptureListRetainableFamilyExprSites(expr->left.get(), profile);
  CollectOwnershipCaptureListRetainableFamilyExprSites(expr->right.get(), profile);
  CollectOwnershipCaptureListRetainableFamilyExprSites(expr->third.get(), profile);
  for (const auto &arg : expr->args) {
    CollectOwnershipCaptureListRetainableFamilyExprSites(arg.get(), profile);
  }
}

template <typename TCallableDecl>
static void CollectOwnershipRetainableFamilyCallableSites(
    const TCallableDecl &decl,
    OwnershipCaptureListRetainableFamilySiteProfile &profile) {
  bool has_callable_attributes = false;
  bool has_canonical_operation = false;
  bool has_compatibility_alias = false;
  for (const auto &attribute_name :
       decl.retainable_c_family_callable_attributes) {
    has_callable_attributes = true;
    has_canonical_operation =
        has_canonical_operation ||
        IsRetainableFamilyCanonicalOperationAttribute(attribute_name);
    has_compatibility_alias =
        has_compatibility_alias ||
        IsRetainableFamilyReturnAliasAttribute(attribute_name) ||
        IsRetainableFamilyConsumedAliasAttribute(attribute_name);
  }
  if (has_callable_attributes) {
    ++profile.retainable_family_callable_sites;
  }
  if (has_canonical_operation) {
    ++profile.retainable_family_operation_callable_sites;
  }
  if (has_compatibility_alias) {
    ++profile.retainable_family_alias_callable_sites;
  }
}
