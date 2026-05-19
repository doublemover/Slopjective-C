std::unique_ptr<Expr> BuildObjc3TypedKeyPathLiteralExpr(
    const Objc3LexToken &keypath_token,
    const Objc3LexToken &root_token,
    std::vector<std::string> components) {
  auto expr = std::make_unique<Expr>();
  expr->kind = Expr::Kind::KeyPathLiteral;
  expr->line = keypath_token.line;
  expr->column = keypath_token.column;
  expr->typed_keypath_literal_enabled = true;
  expr->typed_keypath_root_is_self = root_token.text == "self";
  expr->typed_keypath_root_name = root_token.text;
  expr->typed_keypath_components = std::move(components);
  expr->typed_keypath_literal_profile = BuildTypedKeyPathLiteralProfile(
      expr->typed_keypath_root_name,
      expr->typed_keypath_root_is_self,
      expr->typed_keypath_components);
  expr->typed_keypath_literal_is_normalized =
      !expr->typed_keypath_components.empty();
  return expr;
}
