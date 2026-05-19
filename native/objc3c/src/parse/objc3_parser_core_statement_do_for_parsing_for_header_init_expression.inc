          stmt->for_stmt->init.kind = ForClause::Kind::Expr;
          stmt->for_stmt->init.line = Peek().line;
          stmt->for_stmt->init.column = Peek().column;
          stmt->for_stmt->init.value = ParseExpressionWithBlockLiteralSourceUse(
              BlockLiteralSourceUseKind::ExpressionSite);
          if (stmt->for_stmt->init.value == nullptr) {
            return nullptr;
          }
