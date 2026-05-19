std::unique_ptr<Expr> BuildObjc3IdentifierExpr(
    const std::string &identifier,
    const Objc3LexToken &token) {
  auto expr = std::make_unique<Expr>();
  expr->kind = Expr::Kind::Identifier;
  expr->line = token.line;
  expr->column = token.column;
  expr->ident = identifier;
  return expr;
}
