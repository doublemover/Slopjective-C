        auto stmt = std::make_unique<Stmt>();
        stmt->kind = Stmt::Kind::Block;
        stmt->block_stmt = std::make_unique<BlockStmt>();
        stmt->line = do_token.line;
        stmt->column = do_token.column;
        stmt->block_stmt->line = do_token.line;
        stmt->block_stmt->column = do_token.column;
        stmt->block_stmt->is_do_catch_scope = true;
        stmt->block_stmt->body = std::move(body);
        do {
          BlockStmt::CatchClause clause;
          if (!ParseCatchClause(clause)) {
            return nullptr;
          }
          stmt->block_stmt->catch_clauses.push_back(std::move(clause));
        } while (Match(TokenKind::KwCatch));
        collect_do_catch_profile(*stmt->block_stmt);
        return stmt;
