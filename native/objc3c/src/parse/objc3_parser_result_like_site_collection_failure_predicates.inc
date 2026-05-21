bool IsResultLikeFailureExpr(const Expr *expr) {
  if (expr == nullptr) {
    return false;
  }
  switch (expr->kind) {
  case Expr::Kind::NilLiteral:
    return true;
  case Expr::Kind::BoolLiteral:
    return !expr->bool_value;
  case Expr::Kind::Number:
    return expr->number == 0;
  case Expr::Kind::Identifier:
    return expr->ident == "err" || expr->ident == "error" ||
           expr->ident == "failure";
  case Expr::Kind::CollectionLiteral:
  case Expr::Kind::IndexAccess:
  case Expr::Kind::StringLiteral:
    return false;
  default:
    return false;
  }
}
