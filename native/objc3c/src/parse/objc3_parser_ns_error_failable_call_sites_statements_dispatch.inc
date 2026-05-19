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
  case Stmt::Kind::Switch:
    return CountFailableCallSitesInSwitchStmt(stmt);
  case Stmt::Kind::While:
    return CountFailableCallSitesInWhileStmt(stmt);
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    return CountFailableCallSitesInBlockStmt(stmt);
  case Stmt::Kind::Expr:
    return CountFailableCallSitesInExprStmt(stmt);
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return 0;
  }
}
