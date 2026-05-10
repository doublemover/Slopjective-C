std::string BuildRuntimeLinkHostLinkSymbol(
    bool runtime_link_required,
    bool runtime_link_elided,
    unsigned runtime_dispatch_arg_slots,
    unsigned runtime_dispatch_declaration_parameter_count,
    const std::string &runtime_dispatch_symbol,
    Expr::MessageSendForm form) {
  std::ostringstream out;
  out << "runtime-link-host-link:required="
      << (runtime_link_required ? "true" : "false")
      << ";elided=" << (runtime_link_elided ? "true" : "false")
      << ";runtime-slots=" << runtime_dispatch_arg_slots
      << ";decl-params=" << runtime_dispatch_declaration_parameter_count
      << ";symbol=" << runtime_dispatch_symbol
      << ";form=" << MessageSendFormName(form);
  return out.str();
}
