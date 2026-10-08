void CollectResultLikeExprProfile(const Expr *expr,
                                  Objc3ResultLikeProfile &profile) {
  if (expr == nullptr) {
    return;
  }
  switch (expr->kind) {
  case Expr::Kind::Binary:
    CollectResultLikeExprProfile(expr->left.get(), profile);
    CollectResultLikeExprProfile(expr->right.get(), profile);
    return;
  case Expr::Kind::Conditional:
    profile.result_like_sites += 1u;
    profile.result_branch_sites += 1u;
    profile.branch_merge_sites += 1u;
    CollectResultLikeExprProfile(expr->left.get(), profile);
    CollectResultLikeExprProfile(expr->right.get(), profile);
    CollectResultLikeExprProfile(expr->third.get(), profile);
    return;
  case Expr::Kind::CollectionLiteral:
    for (const auto &key : expr->collection_keys) {
      CollectResultLikeExprProfile(key.get(), profile);
    }
    for (const auto &value : expr->collection_values) {
      CollectResultLikeExprProfile(value.get(), profile);
    }
    return;
  case Expr::Kind::IndexAccess:
    CollectResultLikeExprProfile(expr->left.get(), profile);
    CollectResultLikeExprProfile(expr->right.get(), profile);
    return;
  case Expr::Kind::Call:
  case Expr::Kind::Try:
  case Expr::Kind::Throw:
    for (const auto &arg : expr->args) {
      CollectResultLikeExprProfile(arg.get(), profile);
    }
    return;
  case Expr::Kind::MessageSend:
    CollectResultLikeExprProfile(expr->receiver.get(), profile);
    for (const auto &arg : expr->args) {
      CollectResultLikeExprProfile(arg.get(), profile);
    }
    return;
  case Expr::Kind::StringInterpolation:
    for (const auto &arg : expr->args) {
      CollectResultLikeExprProfile(arg.get(), profile);
    }
    return;
  case Expr::Kind::MatchExpression:
    profile.result_like_sites += 1u;
    profile.result_branch_sites += expr->match_expression_arms.size();
    profile.branch_merge_sites += 1u;
    CollectResultLikeExprProfile(expr->match_expression_scrutinee.get(),
                                 profile);
    for (const auto &arm : expr->match_expression_arms) {
      CollectResultLikeExprProfile(arm.guard_condition.get(), profile);
      CollectResultLikeExprProfile(arm.value.get(), profile);
    }
    return;
  case Expr::Kind::BlockLiteral:
  case Expr::Kind::Identifier:
  case Expr::Kind::KeyPathLiteral:
  case Expr::Kind::Number:
  case Expr::Kind::BoolLiteral:
  case Expr::Kind::NilLiteral:
  case Expr::Kind::StringLiteral:
  default:
    return;
  }
}
