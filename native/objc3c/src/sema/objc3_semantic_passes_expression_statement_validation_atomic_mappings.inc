static void CollectAtomicMemoryOrderMappingsInStatements(const std::vector<std::unique_ptr<Stmt>> &statements,
                                                         Objc3AtomicMemoryOrderMappingSummary &summary);

static void CollectAtomicMemoryOrderMappingsInForClause(const ForClause &clause,
                                                        Objc3AtomicMemoryOrderMappingSummary &summary) {
  if (clause.kind != ForClause::Kind::Assign) {
    return;
  }
  RecordAtomicMemoryOrderMapping(clause.op, summary);
}

static void CollectAtomicMemoryOrderMappingsInStatement(const Stmt *stmt,
                                                        Objc3AtomicMemoryOrderMappingSummary &summary) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
    case Stmt::Kind::Assign:
      if (stmt->assign_stmt != nullptr) {
        RecordAtomicMemoryOrderMapping(stmt->assign_stmt->op, summary);
      }
      return;
    case Stmt::Kind::If:
      if (stmt->if_stmt != nullptr) {
        CollectAtomicMemoryOrderMappingsInStatements(stmt->if_stmt->then_body, summary);
        CollectAtomicMemoryOrderMappingsInStatements(stmt->if_stmt->else_body, summary);
      }
      return;
    case Stmt::Kind::DoWhile:
      if (stmt->do_while_stmt != nullptr) {
        CollectAtomicMemoryOrderMappingsInStatements(stmt->do_while_stmt->body, summary);
      }
      return;
    case Stmt::Kind::For:
      if (stmt->for_stmt != nullptr) {
        CollectAtomicMemoryOrderMappingsInForClause(stmt->for_stmt->init, summary);
        CollectAtomicMemoryOrderMappingsInForClause(stmt->for_stmt->step, summary);
        CollectAtomicMemoryOrderMappingsInStatements(stmt->for_stmt->body, summary);
      }
      return;
    case Stmt::Kind::Switch:
      if (stmt->switch_stmt != nullptr) {
        for (const auto &case_stmt : stmt->switch_stmt->cases) {
          CollectAtomicMemoryOrderMappingsInStatements(case_stmt.body, summary);
        }
      }
      return;
    case Stmt::Kind::While:
      if (stmt->while_stmt != nullptr) {
        CollectAtomicMemoryOrderMappingsInStatements(stmt->while_stmt->body, summary);
      }
      return;
    case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
      if (stmt->block_stmt != nullptr) {
        CollectAtomicMemoryOrderMappingsInStatements(stmt->block_stmt->body, summary);
      }
      return;
    case Stmt::Kind::Let:
    case Stmt::Kind::Return:
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
    case Stmt::Kind::Expr:
      return;
  }
}

static void CollectAtomicMemoryOrderMappingsInStatements(const std::vector<std::unique_ptr<Stmt>> &statements,
                                                         Objc3AtomicMemoryOrderMappingSummary &summary) {
  for (const auto &stmt : statements) {
    CollectAtomicMemoryOrderMappingsInStatement(stmt.get(), summary);
  }
}
