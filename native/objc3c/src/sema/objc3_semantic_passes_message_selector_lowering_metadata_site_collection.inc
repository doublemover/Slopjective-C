static void CollectMessageSendSelectorLoweringSiteMetadataFromExpr(
    const Expr *expr, std::vector<Objc3MessageSendSelectorLoweringSiteMetadata> &sites) {
  if (expr == nullptr) {
    return;
  }

  if (expr->kind == Expr::Kind::MessageSend) {
    sites.push_back(BuildMessageSendSelectorLoweringSiteMetadata(*expr));
    CollectMessageSendSelectorLoweringSiteMetadataFromExpr(expr->receiver.get(), sites);
    for (const auto &arg : expr->args) {
      CollectMessageSendSelectorLoweringSiteMetadataFromExpr(arg.get(), sites);
    }
    return;
  }

  if (expr->kind == Expr::Kind::Binary) {
    CollectMessageSendSelectorLoweringSiteMetadataFromExpr(expr->left.get(), sites);
    CollectMessageSendSelectorLoweringSiteMetadataFromExpr(expr->right.get(), sites);
    return;
  }

  if (expr->kind == Expr::Kind::Conditional) {
    CollectMessageSendSelectorLoweringSiteMetadataFromExpr(expr->left.get(), sites);
    CollectMessageSendSelectorLoweringSiteMetadataFromExpr(expr->right.get(), sites);
    CollectMessageSendSelectorLoweringSiteMetadataFromExpr(expr->third.get(), sites);
    return;
  }

  if (expr->kind == Expr::Kind::CollectionLiteral) {
    for (const auto &key : expr->collection_keys) {
      CollectMessageSendSelectorLoweringSiteMetadataFromExpr(key.get(), sites);
    }
    for (const auto &value : expr->collection_values) {
      CollectMessageSendSelectorLoweringSiteMetadataFromExpr(value.get(), sites);
    }
    return;
  }

  if (expr->kind == Expr::Kind::IndexAccess) {
    CollectMessageSendSelectorLoweringSiteMetadataFromExpr(expr->left.get(), sites);
    CollectMessageSendSelectorLoweringSiteMetadataFromExpr(expr->right.get(), sites);
    return;
  }

  if (expr->kind == Expr::Kind::Call) {
    for (const auto &arg : expr->args) {
      CollectMessageSendSelectorLoweringSiteMetadataFromExpr(arg.get(), sites);
    }
  }
}

static void CollectMessageSendSelectorLoweringSiteMetadataFromStatements(
    const std::vector<std::unique_ptr<Stmt>> &statements,
    std::vector<Objc3MessageSendSelectorLoweringSiteMetadata> &sites);

static void CollectMessageSendSelectorLoweringSiteMetadataFromForClause(
    const ForClause &clause,
    std::vector<Objc3MessageSendSelectorLoweringSiteMetadata> &sites) {
  CollectMessageSendSelectorLoweringSiteMetadataFromExpr(clause.value.get(), sites);
}

static void CollectMessageSendSelectorLoweringSiteMetadataFromStatement(
    const Stmt *stmt, std::vector<Objc3MessageSendSelectorLoweringSiteMetadata> &sites) {
  if (stmt == nullptr) {
    return;
  }

  switch (stmt->kind) {
    case Stmt::Kind::Let:
      CollectMessageSendSelectorLoweringSiteMetadataFromExpr(stmt->let_stmt->value.get(), sites);
      return;
    case Stmt::Kind::Assign:
      CollectMessageSendSelectorLoweringSiteMetadataFromExpr(stmt->assign_stmt->value.get(), sites);
      return;
    case Stmt::Kind::Return:
      CollectMessageSendSelectorLoweringSiteMetadataFromExpr(stmt->return_stmt->value.get(), sites);
      return;
    case Stmt::Kind::If:
      CollectMessageSendSelectorLoweringSiteMetadataFromExpr(stmt->if_stmt->condition.get(), sites);
      CollectMessageSendSelectorLoweringSiteMetadataFromStatements(stmt->if_stmt->then_body, sites);
      CollectMessageSendSelectorLoweringSiteMetadataFromStatements(stmt->if_stmt->else_body, sites);
      return;
    case Stmt::Kind::DoWhile:
      CollectMessageSendSelectorLoweringSiteMetadataFromStatements(stmt->do_while_stmt->body, sites);
      CollectMessageSendSelectorLoweringSiteMetadataFromExpr(stmt->do_while_stmt->condition.get(), sites);
      return;
    case Stmt::Kind::For:
      CollectMessageSendSelectorLoweringSiteMetadataFromForClause(stmt->for_stmt->init, sites);
      CollectMessageSendSelectorLoweringSiteMetadataFromExpr(stmt->for_stmt->condition.get(), sites);
      CollectMessageSendSelectorLoweringSiteMetadataFromForClause(stmt->for_stmt->step, sites);
      CollectMessageSendSelectorLoweringSiteMetadataFromStatements(stmt->for_stmt->body, sites);
      return;
    case Stmt::Kind::ForIn:
      CollectMessageSendSelectorLoweringSiteMetadataFromExpr(stmt->for_in_stmt->collection.get(), sites);
      CollectMessageSendSelectorLoweringSiteMetadataFromStatements(stmt->for_in_stmt->body, sites);
      return;
    case Stmt::Kind::Switch:
      CollectMessageSendSelectorLoweringSiteMetadataFromExpr(stmt->switch_stmt->condition.get(), sites);
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        CollectMessageSendSelectorLoweringSiteMetadataFromStatements(switch_case.body, sites);
      }
      return;
    case Stmt::Kind::While:
      CollectMessageSendSelectorLoweringSiteMetadataFromExpr(stmt->while_stmt->condition.get(), sites);
      CollectMessageSendSelectorLoweringSiteMetadataFromStatements(stmt->while_stmt->body, sites);
      return;
    case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
      CollectMessageSendSelectorLoweringSiteMetadataFromStatements(stmt->block_stmt->body, sites);
      return;
    case Stmt::Kind::Expr:
      CollectMessageSendSelectorLoweringSiteMetadataFromExpr(stmt->expr_stmt->value.get(), sites);
      return;
    case Stmt::Kind::CollectionMutation:
      CollectMessageSendSelectorLoweringSiteMetadataFromExpr(stmt->collection_mutation_stmt->key_or_index.get(), sites);
      CollectMessageSendSelectorLoweringSiteMetadataFromExpr(stmt->collection_mutation_stmt->value.get(), sites);
      return;
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return;
  }
}

static void CollectMessageSendSelectorLoweringSiteMetadataFromStatements(
    const std::vector<std::unique_ptr<Stmt>> &statements,
    std::vector<Objc3MessageSendSelectorLoweringSiteMetadata> &sites) {
  for (const auto &statement : statements) {
    CollectMessageSendSelectorLoweringSiteMetadataFromStatement(statement.get(), sites);
  }
}
