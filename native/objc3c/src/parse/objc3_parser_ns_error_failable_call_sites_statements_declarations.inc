std::size_t CountFailableCallSitesInStmt(const Stmt *stmt);

std::size_t CountFailableCallSitesInLetStmt(const Stmt *stmt);

std::size_t CountFailableCallSitesInAssignStmt(const Stmt *stmt);

std::size_t CountFailableCallSitesInReturnStmt(const Stmt *stmt);

std::size_t CountFailableCallSitesInExprStmt(const Stmt *stmt);

std::size_t CountFailableCallSitesInStmtBody(
    const std::vector<std::unique_ptr<Stmt>> &body);

std::size_t CountFailableCallSitesInIfStmt(const Stmt *stmt);

std::size_t CountFailableCallSitesInBlockStmt(const Stmt *stmt);

std::size_t CountFailableCallSitesInDoWhileStmt(const Stmt *stmt);

std::size_t CountFailableCallSitesInForStmt(const Stmt *stmt);

std::size_t CountFailableCallSitesInWhileStmt(const Stmt *stmt);

std::size_t CountFailableCallSitesInSwitchStmt(const Stmt *stmt);
