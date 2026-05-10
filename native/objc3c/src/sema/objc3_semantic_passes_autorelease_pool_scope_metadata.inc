static Objc3AutoreleasePoolScopeSiteMetadata BuildAutoreleasePoolScopeSiteMetadata(const BlockStmt &block) {
  Objc3AutoreleasePoolScopeSiteMetadata metadata;
  metadata.scope_symbol = block.autoreleasepool_scope_symbol;
  metadata.scope_depth = block.autoreleasepool_scope_depth;
  metadata.line = block.line;
  metadata.column = block.column;
  return metadata;
}

static void CollectAutoreleasePoolScopeSiteMetadataFromStatements(
    const std::vector<std::unique_ptr<Stmt>> &statements,
    std::vector<Objc3AutoreleasePoolScopeSiteMetadata> &sites);

static void CollectAutoreleasePoolScopeSiteMetadataFromStatement(
    const Stmt *stmt, std::vector<Objc3AutoreleasePoolScopeSiteMetadata> &sites) {
  if (stmt == nullptr) {
    return;
  }

  switch (stmt->kind) {
    case Stmt::Kind::If:
      CollectAutoreleasePoolScopeSiteMetadataFromStatements(stmt->if_stmt->then_body, sites);
      CollectAutoreleasePoolScopeSiteMetadataFromStatements(stmt->if_stmt->else_body, sites);
      return;
    case Stmt::Kind::DoWhile:
      CollectAutoreleasePoolScopeSiteMetadataFromStatements(stmt->do_while_stmt->body, sites);
      return;
    case Stmt::Kind::For:
      CollectAutoreleasePoolScopeSiteMetadataFromStatements(stmt->for_stmt->body, sites);
      return;
    case Stmt::Kind::Switch:
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        CollectAutoreleasePoolScopeSiteMetadataFromStatements(switch_case.body, sites);
      }
      return;
    case Stmt::Kind::While:
      CollectAutoreleasePoolScopeSiteMetadataFromStatements(stmt->while_stmt->body, sites);
      return;
    case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
      if (stmt->block_stmt->is_autoreleasepool_scope) {
        sites.push_back(BuildAutoreleasePoolScopeSiteMetadata(*stmt->block_stmt));
      }
      CollectAutoreleasePoolScopeSiteMetadataFromStatements(stmt->block_stmt->body, sites);
      return;
    case Stmt::Kind::Let:
    case Stmt::Kind::Assign:
    case Stmt::Kind::Return:
    case Stmt::Kind::Expr:
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return;
  }
}

static void CollectAutoreleasePoolScopeSiteMetadataFromStatements(
    const std::vector<std::unique_ptr<Stmt>> &statements,
    std::vector<Objc3AutoreleasePoolScopeSiteMetadata> &sites) {
  for (const auto &statement : statements) {
    CollectAutoreleasePoolScopeSiteMetadataFromStatement(statement.get(), sites);
  }
}

static bool IsAutoreleasePoolScopeSiteMetadataLess(const Objc3AutoreleasePoolScopeSiteMetadata &lhs,
                                                   const Objc3AutoreleasePoolScopeSiteMetadata &rhs) {
  if (lhs.scope_symbol != rhs.scope_symbol) {
    return lhs.scope_symbol < rhs.scope_symbol;
  }
  if (lhs.scope_depth != rhs.scope_depth) {
    return lhs.scope_depth < rhs.scope_depth;
  }
  if (lhs.line != rhs.line) {
    return lhs.line < rhs.line;
  }
  return lhs.column < rhs.column;
}

static std::vector<Objc3AutoreleasePoolScopeSiteMetadata>
BuildAutoreleasePoolScopeSiteMetadataLexicographic(const Objc3Program &ast) {
  std::vector<Objc3AutoreleasePoolScopeSiteMetadata> sites;
  for (const auto &fn : ast.functions) {
    CollectAutoreleasePoolScopeSiteMetadataFromStatements(fn.body, sites);
  }
  std::sort(sites.begin(), sites.end(), IsAutoreleasePoolScopeSiteMetadataLess);
  return sites;
}
