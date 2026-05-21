void CollectResultLikeStmtProfile(const Stmt *stmt,
                                  Objc3ResultLikeProfile &profile) {
  if (stmt == nullptr) {
    return;
  }

  switch (stmt->kind) {
  case Stmt::Kind::Let:
    CollectResultLikeLetStmtProfile(stmt, profile);
    return;
  case Stmt::Kind::Assign:
    CollectResultLikeAssignStmtProfile(stmt, profile);
    return;
  case Stmt::Kind::Return:
    CollectResultLikeReturnStmtProfile(stmt, profile);
    return;
  case Stmt::Kind::If:
    CollectResultLikeIfStmtProfile(stmt, profile);
    return;
  case Stmt::Kind::DoWhile:
    CollectResultLikeDoWhileStmtProfile(stmt, profile);
    return;
  case Stmt::Kind::For:
    CollectResultLikeForStmtProfile(stmt, profile);
    return;
  case Stmt::Kind::ForIn:
    CountResultLikeBranchMergeSite(profile);
    if (stmt->for_in_stmt != nullptr) {
      CollectResultLikeExprProfile(stmt->for_in_stmt->collection.get(),
                                   profile);
      CollectResultLikeStatementBodyProfile(stmt->for_in_stmt->body, profile);
    }
    return;
  case Stmt::Kind::Switch:
    CollectResultLikeSwitchStmtProfile(stmt, profile);
    return;
  case Stmt::Kind::While:
    CollectResultLikeWhileStmtProfile(stmt, profile);
    return;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    CollectResultLikeBlockStmtProfile(stmt, profile);
    return;
  case Stmt::Kind::Expr:
    CollectResultLikeExprStmtProfile(stmt, profile);
    return;
  case Stmt::Kind::CollectionMutation:
    if (stmt->collection_mutation_stmt != nullptr) {
      CollectResultLikeExprProfile(
          stmt->collection_mutation_stmt->key_or_index.get(), profile);
      CollectResultLikeExprProfile(
          stmt->collection_mutation_stmt->value.get(), profile);
    }
    return;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  }
}
