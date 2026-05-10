bool IsSuperDispatchReceiver(const Expr &receiver) {
  return receiver.kind == Expr::Kind::Identifier && receiver.ident == "super";
}

std::string BuildSuperDispatchSymbol(bool super_dispatch_enabled,
                                     bool super_dispatch_requires_class_context,
                                     Expr::MessageSendForm form) {
  std::ostringstream out;
  out << "super-dispatch:enabled="
      << (super_dispatch_enabled ? "true" : "false")
      << ";class-context="
      << (super_dispatch_requires_class_context ? "required" : "not-required")
      << ";form=" << MessageSendFormName(form);
  return out.str();
}
