  bool RequireBlockLiteralBodyStart() {
    // executable-block-source-closure anchor: block literals enter
    // the parser-owned source surface at the brace-owned ParseBlock() helper,
    // so the freeze boundary must only probe for '{' here and leave actual
    // block consumption to the shared block parser.
    // block-lowering-ABI/artifact-boundary freeze anchor: parser
    // owns only the source spelling, parameter list, capture inventory, and
    // body shape that later lowering consumes. It does not assign emitted
    // block-object layout slots, invoke-thunk bodies, byref cells, or helper
    // symbols.
    if (At(TokenKind::LBrace)) {
      return true;
    }
    const Token &token = Peek();
    diagnostics_.push_back(
        MakeDiag(token.line, token.column, "O3P166",
                 "expected '{' before block literal body"));
    return false;
  }
