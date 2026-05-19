  bool ParseMessageSelectorArgumentExpression(Expr &message) {
    auto arg = ParseExpressionWithBlockLiteralSourceUse(
        BlockLiteralSourceUseKind::MessageArgument);
    if (arg == nullptr) {
      return false;
    }
    message.args.push_back(std::move(arg));
    return true;
  }
