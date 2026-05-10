  bool RejectUnexpectedFunctionReturnTypeQualifier() {
    if (!At(TokenKind::KwPure) && !At(TokenKind::KwExtern) &&
        !At(TokenKind::KwAsync)) {
      return true;
    }

    const Token qualifier = Advance();
    const std::string message =
        qualifier.kind == TokenKind::KwPure ? "unexpected qualifier 'pure' in function return type annotation"
        : qualifier.kind == TokenKind::KwExtern ? "unexpected qualifier 'extern' in function return type annotation"
                                                : "unexpected qualifier 'async' in function return type annotation";
    diagnostics_.push_back(MakeDiag(
        qualifier.line, qualifier.column, "O3P100", message));
    return false;
  }

  bool ReportExpectedFunctionReturnType() {
    const Token &token = Peek();
    diagnostics_.push_back(MakeDiag(
        token.line, token.column, "O3P114",
        "expected function return type 'i32', 'bool', 'BOOL', 'NSInteger', 'NSUInteger', 'void', 'id', "
        "'Class', 'SEL', 'Protocol', 'instancetype', object pointer spelling, ownership qualifiers "
        "'__strong/__weak/__autoreleasing/__unsafe_unretained', or vector forms "
        "'i32x2/i32x4/i32x8/i32x16' and 'boolx2/boolx4/boolx8/boolx16'"));
    return false;
  }
