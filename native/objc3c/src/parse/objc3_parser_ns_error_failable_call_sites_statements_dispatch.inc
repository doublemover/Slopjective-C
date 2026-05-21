std::size_t CountFailableCallSitesInStmt(const Stmt *stmt) {
  if (stmt == nullptr) {
    return 0;
  }

  switch (stmt->kind) {
  case Stmt::Kind::Let:
    return CountFailableCallSitesInLetStmt(stmt);
  case Stmt::Kind::Assign:
    return CountFailableCallSitesInAssignStmt(stmt);
  case Stmt::Kind::Return:
    return CountFailableCallSitesInReturnStmt(stmt);
  case Stmt::Kind::If:
    return CountFailableCallSitesInIfStmt(stmt);
  case Stmt::Kind::DoWhile:
    return CountFailableCallSitesInDoWhileStmt(stmt);
  case Stmt::Kind::For:
    return CountFailableCallSitesInForStmt(stmt);
  case Stmt::Kind::ForIn: {
    std::size_t count =
        CountFailableCallSitesInExpr(stmt->for_in_stmt->collection.get());
    for (const auto &body_stmt : stmt->for_in_stmt->body) {
      count += CountFailableCallSitesInStmt(body_stmt.get());
    }
    return count;
  }
  case Stmt::Kind::Switch:
    return CountFailableCallSitesInSwitchStmt(stmt);
  case Stmt::Kind::While:
    return CountFailableCallSitesInWhileStmt(stmt);
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    return CountFailableCallSitesInBlockStmt(stmt);
  case Stmt::Kind::Expr:
    return CountFailableCallSitesInExprStmt(stmt);
  case Stmt::Kind::CollectionMutation: {
    std::size_t count = CountFailableCallSitesInExpr(
        stmt->collection_mutation_stmt->key_or_index.get());
    if (stmt->collection_mutation_stmt->value) {
      count += CountFailableCallSitesInExpr(
          stmt->collection_mutation_stmt->value.get());
    }
    return count;
  }
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return 0;
  }
  return 0;
}
