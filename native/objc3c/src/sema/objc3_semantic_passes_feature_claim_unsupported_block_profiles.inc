static bool BlockLiteralUsesRunnableC003Subset(const Expr &expr) {
  // byref-cell/copy-helper/dispose-helper anchor: the runnable block
  // slice now admits non-escaping byref and ownership-sensitive captures at the
  // unsupported-feature gate as soon as the source model is normalized. The
  // gate intentionally remains parser/source-truth driven so later sema and
  // lowering passes can consume the real byref/helper lowering path without
  // depending on post-sema runtime counters. Heap-promotion remains deferred
  // to later lowering/runtime checks instead of this source admission step.
  return expr.block_literal_is_normalized &&
         expr.block_source_model_is_normalized &&
         expr.block_parameter_count <= 4u &&
         expr.block_source_storage_annotations_are_normalized;
}

static bool BlockLiteralUsesRunnableC002Subset(const Expr &expr) {
  // executable-block-object/invoke-thunk anchor: preserve the
  // original runnable-block gate name as a compatibility surface for the C002
  // readiness contract while C003 widens the admitted non-escaping subset.
  return BlockLiteralUsesRunnableC003Subset(expr);
}

static bool ExprContainsBlockLiteral(const Expr *expr);

static bool StmtContainsBlockLiteral(const Stmt *stmt) {
  if (stmt == nullptr) {
    return false;
  }

  switch (stmt->kind) {
    case Stmt::Kind::Let:
      return stmt->let_stmt != nullptr &&
             ExprContainsBlockLiteral(stmt->let_stmt->value.get());
    case Stmt::Kind::Assign:
      return stmt->assign_stmt != nullptr &&
             ExprContainsBlockLiteral(stmt->assign_stmt->value.get());
    case Stmt::Kind::Return:
      return stmt->return_stmt != nullptr &&
             ExprContainsBlockLiteral(stmt->return_stmt->value.get());
    case Stmt::Kind::If:
      if (stmt->if_stmt == nullptr) {
        return false;
      }
      if (ExprContainsBlockLiteral(stmt->if_stmt->condition.get())) {
        return true;
      }
      for (const auto &then_stmt : stmt->if_stmt->then_body) {
        if (StmtContainsBlockLiteral(then_stmt.get())) {
          return true;
        }
      }
      for (const auto &else_stmt : stmt->if_stmt->else_body) {
        if (StmtContainsBlockLiteral(else_stmt.get())) {
          return true;
        }
      }
      return false;
    case Stmt::Kind::DoWhile:
      if (stmt->do_while_stmt == nullptr) {
        return false;
      }
      for (const auto &body_stmt : stmt->do_while_stmt->body) {
        if (StmtContainsBlockLiteral(body_stmt.get())) {
          return true;
        }
      }
      return ExprContainsBlockLiteral(stmt->do_while_stmt->condition.get());
    case Stmt::Kind::For:
      if (stmt->for_stmt == nullptr) {
        return false;
      }
      if (ExprContainsBlockLiteral(stmt->for_stmt->init.value.get()) ||
          ExprContainsBlockLiteral(stmt->for_stmt->condition.get()) ||
          ExprContainsBlockLiteral(stmt->for_stmt->step.value.get())) {
        return true;
      }
      for (const auto &body_stmt : stmt->for_stmt->body) {
        if (StmtContainsBlockLiteral(body_stmt.get())) {
          return true;
        }
      }
      return false;
    case Stmt::Kind::ForIn:
      if (stmt->for_in_stmt == nullptr) {
        return false;
      }
      if (ExprContainsBlockLiteral(stmt->for_in_stmt->collection.get())) {
        return true;
      }
      for (const auto &body_stmt : stmt->for_in_stmt->body) {
        if (StmtContainsBlockLiteral(body_stmt.get())) {
          return true;
        }
      }
      return false;
    case Stmt::Kind::Switch:
      if (stmt->switch_stmt == nullptr) {
        return false;
      }
      if (ExprContainsBlockLiteral(stmt->switch_stmt->condition.get())) {
        return true;
      }
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        for (const auto &case_stmt : switch_case.body) {
          if (StmtContainsBlockLiteral(case_stmt.get())) {
            return true;
          }
        }
      }
      return false;
    case Stmt::Kind::While:
      if (stmt->while_stmt == nullptr) {
        return false;
      }
      if (ExprContainsBlockLiteral(stmt->while_stmt->condition.get())) {
        return true;
      }
      for (const auto &body_stmt : stmt->while_stmt->body) {
        if (StmtContainsBlockLiteral(body_stmt.get())) {
          return true;
        }
      }
      return false;
    case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
      if (stmt->block_stmt == nullptr) {
        return false;
      }
      for (const auto &body_stmt : stmt->block_stmt->body) {
        if (StmtContainsBlockLiteral(body_stmt.get())) {
          return true;
        }
      }
      return false;
    case Stmt::Kind::Expr:
      return stmt->expr_stmt != nullptr &&
             ExprContainsBlockLiteral(stmt->expr_stmt->value.get());
    case Stmt::Kind::CollectionMutation:
      return stmt->collection_mutation_stmt != nullptr &&
             (ExprContainsBlockLiteral(
                  stmt->collection_mutation_stmt->key_or_index.get()) ||
              ExprContainsBlockLiteral(
                  stmt->collection_mutation_stmt->value.get()));
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return false;
  }
  return false;
}

static bool ExprContainsBlockLiteral(const Expr *expr) {
  if (expr == nullptr) {
    return false;
  }
  if (expr->kind == Expr::Kind::BlockLiteral) {
    return true;
  }

  if (ExprContainsBlockLiteral(expr->left.get()) ||
      ExprContainsBlockLiteral(expr->right.get()) ||
      ExprContainsBlockLiteral(expr->third.get()) ||
      ExprContainsBlockLiteral(expr->receiver.get())) {
    return true;
  }
  for (const auto &key : expr->collection_keys) {
    if (ExprContainsBlockLiteral(key.get())) {
      return true;
    }
  }
  for (const auto &value : expr->collection_values) {
    if (ExprContainsBlockLiteral(value.get())) {
      return true;
    }
  }
  for (const auto &arg : expr->args) {
    if (ExprContainsBlockLiteral(arg.get())) {
      return true;
    }
  }
  return false;
}

static bool FunctionBodyContainsBlockLiteral(const FunctionDecl &fn) {
  for (const auto &stmt : fn.body) {
    if (StmtContainsBlockLiteral(stmt.get())) {
      return true;
    }
  }
  return false;
}

static bool MethodBodyContainsBlockLiteral(const Objc3MethodDecl &method) {
  for (const auto &stmt : method.body) {
    if (StmtContainsBlockLiteral(stmt.get())) {
      return true;
    }
  }
  return false;
}
