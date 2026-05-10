void CollectResultLikeStmtProfile(const Stmt *stmt,
                                  Objc3ResultLikeProfile &profile);

void CollectResultLikeStatementBodyProfile(
    const std::vector<std::unique_ptr<Stmt>> &body,
    Objc3ResultLikeProfile &profile);

void CountResultLikeBranchMergeSite(Objc3ResultLikeProfile &profile);

void CollectResultLikeLetStmtProfile(const Stmt *stmt,
                                     Objc3ResultLikeProfile &profile);

void CollectResultLikeAssignStmtProfile(const Stmt *stmt,
                                        Objc3ResultLikeProfile &profile);

void CollectResultLikeReturnStmtProfile(const Stmt *stmt,
                                        Objc3ResultLikeProfile &profile);

void CollectResultLikeExprStmtProfile(const Stmt *stmt,
                                      Objc3ResultLikeProfile &profile);

void CollectResultLikeIfStmtProfile(const Stmt *stmt,
                                    Objc3ResultLikeProfile &profile);

void CollectResultLikeSwitchStmtProfile(const Stmt *stmt,
                                        Objc3ResultLikeProfile &profile);

void CollectResultLikeDoWhileStmtProfile(const Stmt *stmt,
                                         Objc3ResultLikeProfile &profile);

void CollectResultLikeForStmtProfile(const Stmt *stmt,
                                     Objc3ResultLikeProfile &profile);

void CollectResultLikeWhileStmtProfile(const Stmt *stmt,
                                       Objc3ResultLikeProfile &profile);

void CollectResultLikeBlockStmtProfile(const Stmt *stmt,
                                       Objc3ResultLikeProfile &profile);
