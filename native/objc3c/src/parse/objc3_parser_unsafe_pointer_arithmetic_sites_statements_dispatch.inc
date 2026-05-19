void CollectPointerArithmeticStmtSites(const Stmt *stmt, std::size_t &sites) {
  if (stmt == nullptr) {
    return;
  }

  switch (stmt->kind) {
  case Stmt::Kind::Let:
    CollectPointerArithmeticLetStmtSites(stmt, sites);
    return;
  case Stmt::Kind::Assign:
    CollectPointerArithmeticAssignStmtSites(stmt, sites);
    return;
  case Stmt::Kind::Return:
    CollectPointerArithmeticReturnStmtSites(stmt, sites);
    return;
  case Stmt::Kind::If:
    CollectPointerArithmeticIfStmtSites(stmt, sites);
    return;
  case Stmt::Kind::DoWhile:
    CollectPointerArithmeticDoWhileStmtSites(stmt, sites);
    return;
  case Stmt::Kind::For:
    CollectPointerArithmeticForStmtSites(stmt, sites);
    return;
  case Stmt::Kind::Switch:
    CollectPointerArithmeticSwitchStmtSites(stmt, sites);
    return;
  case Stmt::Kind::While:
    CollectPointerArithmeticWhileStmtSites(stmt, sites);
    return;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    CollectPointerArithmeticBlockStmtSites(stmt, sites);
    return;
  case Stmt::Kind::Expr:
    CollectPointerArithmeticExprStmtSites(stmt, sites);
    return;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  }
}
