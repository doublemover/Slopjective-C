  bool ParseFunctionParameters(FunctionDecl &fn) {
    if (At(TokenKind::RParen)) {
      return true;
    }

    while (true) {
      if (At(TokenKind::KwPure) || At(TokenKind::KwExtern) || At(TokenKind::KwAsync)) {
        const Token qualifier = Advance();
        const std::string message =
            qualifier.kind == TokenKind::KwPure ? "unexpected qualifier 'pure' in parameter identifier position"
            : qualifier.kind == TokenKind::KwExtern ? "unexpected qualifier 'extern' in parameter identifier position"
                                                    : "unexpected qualifier 'async' in parameter identifier position";
        diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
        return false;
      }

      if (!At(TokenKind::Identifier)) {
        const Token &token = Peek();
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P101", "invalid parameter identifier"));
        return false;
      }

      FuncParam param;
      param.name = Advance().text;
      param.line = Previous().line;
      param.column = Previous().column;

      if (At(TokenKind::KwPure) || At(TokenKind::KwExtern) || At(TokenKind::KwAsync)) {
        const Token qualifier = Advance();
        const std::string message =
            qualifier.kind == TokenKind::KwPure ? "unexpected qualifier 'pure' after parameter name"
            : qualifier.kind == TokenKind::KwExtern ? "unexpected qualifier 'extern' after parameter name"
                                                    : "unexpected qualifier 'async' after parameter name";
        diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
        return false;
      }

      if (!Match(TokenKind::Colon)) {
        const Token &token = Peek();
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P107", "missing ':' after parameter name"));
        return false;
      }
      if (!ParseParameterType(param)) {
        return false;
      }

      if (At(TokenKind::KwPure) || At(TokenKind::KwExtern) || At(TokenKind::KwAsync)) {
        const Token qualifier = Advance();
        const std::string message =
            qualifier.kind == TokenKind::KwPure ? "unexpected qualifier 'pure' after parameter type annotation"
            : qualifier.kind == TokenKind::KwExtern ? "unexpected qualifier 'extern' after parameter type annotation"
                                                    : "unexpected qualifier 'async' after parameter type annotation";
        diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
        return false;
      }

      fn.params.push_back(std::move(param));
      if (!Match(TokenKind::Comma)) {
        return true;
      }
    }
  }
