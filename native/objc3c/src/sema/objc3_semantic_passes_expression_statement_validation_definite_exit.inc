static bool StatementDefinitelyExitsScope(const Stmt *stmt);

static bool StatementListDefinitelyExitsScope(
    const std::vector<std::unique_ptr<Stmt>> &statements) {
  if (statements.empty()) {
    return false;
  }
  return StatementDefinitelyExitsScope(statements.back().get());
}

static bool StatementDefinitelyExitsScope(const Stmt *stmt) {
  if (stmt == nullptr) {
    return false;
  }
  switch (stmt->kind) {
    case Stmt::Kind::Return:
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
      return true;
    case Stmt::Kind::Block:
      return stmt->block_stmt != nullptr &&
             StatementListDefinitelyExitsScope(stmt->block_stmt->body);
    case Stmt::Kind::Defer:
      return false;
    case Stmt::Kind::If:
      return stmt->if_stmt != nullptr &&
             StatementListDefinitelyExitsScope(stmt->if_stmt->then_body) &&
             StatementListDefinitelyExitsScope(stmt->if_stmt->else_body);
    case Stmt::Kind::Assign:
    case Stmt::Kind::Let:
    case Stmt::Kind::DoWhile:
    case Stmt::Kind::For:
    case Stmt::Kind::Switch:
    case Stmt::Kind::While:
    case Stmt::Kind::Empty:
    case Stmt::Kind::Expr:
      return false;
  }
  return false;
}
