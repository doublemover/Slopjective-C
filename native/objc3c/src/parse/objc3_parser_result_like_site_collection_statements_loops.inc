void CollectResultLikeDoWhileStmtProfile(const Stmt *stmt,
                                         Objc3ResultLikeProfile &profile) {
  CountResultLikeBranchMergeSite(profile);
  if (stmt->do_while_stmt != nullptr) {
    CollectResultLikeStatementBodyProfile(stmt->do_while_stmt->body, profile);
    CollectResultLikeExprProfile(stmt->do_while_stmt->condition.get(),
                                 profile);
  }
}

void CollectResultLikeForStmtProfile(const Stmt *stmt,
                                     Objc3ResultLikeProfile &profile) {
  CountResultLikeBranchMergeSite(profile);
  if (stmt->for_stmt != nullptr) {
    CollectResultLikeForClauseProfile(stmt->for_stmt->init, profile);
    CollectResultLikeExprProfile(stmt->for_stmt->condition.get(), profile);
    CollectResultLikeForClauseProfile(stmt->for_stmt->step, profile);
    CollectResultLikeStatementBodyProfile(stmt->for_stmt->body, profile);
  }
}

void CollectResultLikeWhileStmtProfile(const Stmt *stmt,
                                       Objc3ResultLikeProfile &profile) {
  CountResultLikeBranchMergeSite(profile);
  if (stmt->while_stmt != nullptr) {
    CollectResultLikeExprProfile(stmt->while_stmt->condition.get(), profile);
    CollectResultLikeStatementBodyProfile(stmt->while_stmt->body, profile);
  }
}

void CollectResultLikeBlockStmtProfile(const Stmt *stmt,
                                       Objc3ResultLikeProfile &profile) {
  if (stmt->block_stmt != nullptr) {
    CollectResultLikeStatementBodyProfile(stmt->block_stmt->body, profile);
  }
}
