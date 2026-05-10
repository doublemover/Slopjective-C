          stmt->for_stmt->init.kind = ForClause::Kind::Let;
          const Token &name_token = Peek();
          if (!Match(TokenKind::Identifier)) {
            emit_do_for_diagnostic(name_token, "O3P101",
                                   "invalid declaration identifier");
            return nullptr;
          }
          stmt->for_stmt->init.name = Previous().text;
          stmt->for_stmt->init.line = Previous().line;
          stmt->for_stmt->init.column = Previous().column;

          if (!Match(TokenKind::Equal)) {
            const Token &token = Peek();
            emit_do_for_diagnostic(token, "O3P102", "missing '='");
            return nullptr;
          }

          stmt->for_stmt->init.value = ParseExpressionWithBlockLiteralSourceUse(
              BlockLiteralSourceUseKind::ExpressionSite);
          if (stmt->for_stmt->init.value == nullptr) {
            return nullptr;
          }
