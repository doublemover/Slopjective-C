void CollectResultLikeLetStmtProfile(const Stmt *stmt,
                                     Objc3ResultLikeProfile &profile) {
  if (stmt->let_stmt != nullptr) {
    CollectResultLikeExprProfile(stmt->let_stmt->value.get(), profile);
  }
}

void CollectResultLikeAssignStmtProfile(const Stmt *stmt,
                                        Objc3ResultLikeProfile &profile) {
  if (stmt->assign_stmt != nullptr) {
    CollectResultLikeExprProfile(stmt->assign_stmt->value.get(), profile);
  }
}

void CollectResultLikeReturnStmtProfile(const Stmt *stmt,
                                        Objc3ResultLikeProfile &profile) {
  profile.result_like_sites += 1u;
  profile.normalized_sites += 1u;
  if (stmt->return_stmt != nullptr && stmt->return_stmt->value != nullptr) {
    profile.result_payload_sites += 1u;
    CollectResultLikeExprProfile(stmt->return_stmt->value.get(), profile);
    if (IsResultLikeFailureExpr(stmt->return_stmt->value.get())) {
      profile.result_failure_sites += 1u;
    } else {
      profile.result_success_sites += 1u;
    }
  } else {
    profile.result_success_sites += 1u;
  }
}

void CollectResultLikeExprStmtProfile(const Stmt *stmt,
                                      Objc3ResultLikeProfile &profile) {
  if (stmt->expr_stmt != nullptr) {
    CollectResultLikeExprProfile(stmt->expr_stmt->value.get(), profile);
  }
}
