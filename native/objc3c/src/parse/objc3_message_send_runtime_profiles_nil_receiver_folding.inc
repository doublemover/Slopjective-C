std::string BuildNilReceiverFoldingSymbol(bool nil_receiver_foldable,
                                          bool requires_runtime_dispatch,
                                          Expr::MessageSendForm form) {
  std::ostringstream out;
  out << "nil-receiver:foldable="
      << (nil_receiver_foldable ? "true" : "false")
      << ";runtime-dispatch="
      << (requires_runtime_dispatch ? "required" : "elided")
      << ";form=" << MessageSendFormName(form);
  return out.str();
}
