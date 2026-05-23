std::unique_ptr<Expr> BuildObjc3NumericLiteralExpr(
    int value,
    const Objc3LexToken &token) {
  auto literal = std::make_unique<Expr>();
  literal->kind = Expr::Kind::Number;
  literal->number = value;
  literal->line = token.line;
  literal->column = token.column;
  return literal;
}

std::unique_ptr<Expr> BuildObjc3BoolLiteralExpr(
    bool value,
    const Objc3LexToken &token) {
  return BuildObjc3BoolLiteralExprAt(value, token.line, token.column);
}

std::unique_ptr<Expr> BuildObjc3BoolLiteralExprAt(
    bool value,
    unsigned line,
    unsigned column) {
  auto literal = std::make_unique<Expr>();
  literal->kind = Expr::Kind::BoolLiteral;
  literal->bool_value = value;
  literal->line = line;
  literal->column = column;
  return literal;
}

std::unique_ptr<Expr> BuildObjc3NilLiteralExpr(const Objc3LexToken &token) {
  auto literal = std::make_unique<Expr>();
  literal->kind = Expr::Kind::NilLiteral;
  literal->line = token.line;
  literal->column = token.column;
  return literal;
}

std::unique_ptr<Expr> BuildObjc3StringLiteralExpr(
    const std::string &value,
    int byte_count,
    int unit_count,
    const Objc3LexToken &token) {
  auto literal = std::make_unique<Expr>();
  literal->kind = Expr::Kind::StringLiteral;
  literal->string_literal_value = value;
  literal->string_literal_byte_count = byte_count;
  literal->string_literal_unit_count = unit_count;
  literal->string_literal_valid_utf8 = true;
  literal->line = token.line;
  literal->column = token.column;
  return literal;
}

std::unique_ptr<Expr> BuildObjc3StringInterpolationExpr(
    std::vector<std::string> segments,
    std::vector<std::unique_ptr<Expr>> payloads,
    const Objc3LexToken &token) {
  auto literal = std::make_unique<Expr>();
  literal->kind = Expr::Kind::StringInterpolation;
  literal->string_interpolation_segments = std::move(segments);
  literal->args = std::move(payloads);
  literal->string_interpolation_payload_is_text.resize(literal->args.size());
  literal->line = token.line;
  literal->column = token.column;
  return literal;
}
