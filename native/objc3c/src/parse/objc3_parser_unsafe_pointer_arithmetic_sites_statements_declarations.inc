void CollectPointerArithmeticStmtSites(const Stmt *stmt, std::size_t &sites);

void CollectPointerArithmeticLetStmtSites(const Stmt *stmt, std::size_t &sites);

void CollectPointerArithmeticAssignStmtSites(const Stmt *stmt,
                                            std::size_t &sites);

void CollectPointerArithmeticReturnStmtSites(const Stmt *stmt,
                                            std::size_t &sites);

void CollectPointerArithmeticExprStmtSites(const Stmt *stmt,
                                          std::size_t &sites);

void CollectPointerArithmeticStmtBodySites(
    const std::vector<std::unique_ptr<Stmt>> &body, std::size_t &sites);

void CollectPointerArithmeticIfStmtSites(const Stmt *stmt, std::size_t &sites);

void CollectPointerArithmeticBlockStmtSites(const Stmt *stmt,
                                           std::size_t &sites);

void CollectPointerArithmeticForClauseSites(const ForClause &clause,
                                            std::size_t &sites);

void CollectPointerArithmeticDoWhileStmtSites(const Stmt *stmt,
                                             std::size_t &sites);

void CollectPointerArithmeticForStmtSites(const Stmt *stmt, std::size_t &sites);

void CollectPointerArithmeticWhileStmtSites(const Stmt *stmt,
                                           std::size_t &sites);

void CollectPointerArithmeticSwitchStmtSites(const Stmt *stmt,
                                            std::size_t &sites);
