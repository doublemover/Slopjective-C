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
    case Stmt::Kind::ForIn:
      if (stmt->for_in_stmt != nullptr) {
        CollectOwnershipCaptureListRetainableFamilyExprSites(
            stmt->for_in_stmt->collection.get(), profile);
        for (const auto &body_stmt : stmt->for_in_stmt->body) {
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
    case Stmt::Kind::CollectionMutation:
      if (stmt->collection_mutation_stmt != nullptr) {
        CollectOwnershipCaptureListRetainableFamilyExprSites(
            stmt->collection_mutation_stmt->key_or_index.get(), profile);
        CollectOwnershipCaptureListRetainableFamilyExprSites(
            stmt->collection_mutation_stmt->value.get(), profile);
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
  for (const auto &key : expr->collection_keys) {
    CollectOwnershipCaptureListRetainableFamilyExprSites(key.get(), profile);
  }
  for (const auto &value : expr->collection_values) {
    CollectOwnershipCaptureListRetainableFamilyExprSites(value.get(), profile);
  }
  for (const auto &arg : expr->args) {
    CollectOwnershipCaptureListRetainableFamilyExprSites(arg.get(), profile);
  }
}
