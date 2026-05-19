const char *MessageSendFormName(Expr::MessageSendForm form) {
  switch (form) {
  case Expr::MessageSendForm::Unary:
    return "unary";
  case Expr::MessageSendForm::Keyword:
    return "keyword";
  case Expr::MessageSendForm::None:
  default:
    return "none";
  }
}
