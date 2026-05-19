static std::size_t CountSelectorKeywordPieces(const std::string &selector_symbol) {
  return static_cast<std::size_t>(std::count(selector_symbol.begin(), selector_symbol.end(), ':'));
}

static Expr::MessageSendForm ResolveMessageSendForm(const Expr &expr) {
  if (expr.message_send_form == Expr::MessageSendForm::Unary ||
      expr.message_send_form == Expr::MessageSendForm::Keyword) {
    return expr.message_send_form;
  }
  return expr.args.empty() ? Expr::MessageSendForm::Unary : Expr::MessageSendForm::Keyword;
}
