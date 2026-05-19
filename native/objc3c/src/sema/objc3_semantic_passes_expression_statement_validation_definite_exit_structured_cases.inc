    case Stmt::Kind::Block:
      return stmt->block_stmt != nullptr &&
             StatementListDefinitelyExitsScope(stmt->block_stmt->body);
    case Stmt::Kind::Defer:
      return false;
    case Stmt::Kind::If:
      return stmt->if_stmt != nullptr &&
             StatementListDefinitelyExitsScope(stmt->if_stmt->then_body) &&
             StatementListDefinitelyExitsScope(stmt->if_stmt->else_body);
