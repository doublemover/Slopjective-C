void CountResultLikeBranchMergeSite(Objc3ResultLikeProfile &profile) {
  profile.result_like_sites += 1u;
  profile.result_branch_sites += 1u;
  profile.branch_merge_sites += 1u;
}

void CollectResultLikeStatementBodyProfile(
    const std::vector<std::unique_ptr<Stmt>> &body,
    Objc3ResultLikeProfile &profile) {
  for (const auto &body_stmt : body) {
    CollectResultLikeStmtProfile(body_stmt.get(), profile);
  }
}

void CollectResultLikeIfStmtProfile(const Stmt *stmt,
                                    Objc3ResultLikeProfile &profile) {
  CountResultLikeBranchMergeSite(profile);
  if (stmt->if_stmt != nullptr) {
    CollectResultLikeExprProfile(stmt->if_stmt->condition.get(), profile);
    CollectResultLikeStatementBodyProfile(stmt->if_stmt->then_body, profile);
    CollectResultLikeStatementBodyProfile(stmt->if_stmt->else_body, profile);
  }
}

void CollectResultLikeSwitchStmtProfile(const Stmt *stmt,
                                        Objc3ResultLikeProfile &profile) {
  CountResultLikeBranchMergeSite(profile);
  if (stmt->switch_stmt != nullptr) {
    CollectResultLikeExprProfile(stmt->switch_stmt->condition.get(), profile);
    for (const auto &switch_case : stmt->switch_stmt->cases) {
      CollectResultLikeStatementBodyProfile(switch_case.body, profile);
    }
  }
}
