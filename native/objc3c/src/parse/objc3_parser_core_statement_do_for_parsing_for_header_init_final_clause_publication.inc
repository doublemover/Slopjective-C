        if (!Match(TokenKind::Semicolon)) {
          const Token &token = Peek();
          emit_do_for_diagnostic(token, "O3P104", "missing ';' after for init");
          return nullptr;
        }
