  bool ParseTopLevelCStyleFunctionPrototypeOrBody(Objc3ParsedProgram &program,
                                                  FunctionDecl &fn) {
    if (Match(TokenKind::Semicolon)) {
      fn.is_prototype = true;
      PublishTopLevelCStyleFunctionDecl(program, fn);
      return true;
    }

    if (!At(TokenKind::LBrace)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P110", "missing '{' to start block"));
      return false;
    }

    fn.body = ParseBlock();
    if (block_failed_) {
      block_failed_ = false;
      return false;
    }
    if (RejectValueOptionalExecutableFunction(fn)) {
      return false;
    }

    PublishTopLevelCStyleFunctionDecl(program, fn);
    return true;
  }
