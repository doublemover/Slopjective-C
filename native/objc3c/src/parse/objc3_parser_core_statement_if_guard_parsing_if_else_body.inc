        if (Match(TokenKind::KwElse)) {
          stmt->if_stmt->else_body = ParseControlBody();
          if (block_failed_) {
            block_failed_ = false;
            return nullptr;
          }
        }
