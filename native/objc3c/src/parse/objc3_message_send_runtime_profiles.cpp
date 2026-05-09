#include "parse/objc3_message_send_profiles.h"

#include <sstream>

namespace objc3c::parse {

namespace {

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

}  // namespace

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

std::string BuildMethodFamilySemanticsSymbol(
    const std::string &method_family_name,
    bool returns_retained_result,
    bool returns_related_result) {
  std::ostringstream out;
  out << "method-family:name=" << method_family_name
      << ";returns-retained=" << (returns_retained_result ? "true" : "false")
      << ";returns-related=" << (returns_related_result ? "true" : "false");
  return out.str();
}

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

}  // namespace objc3c::parse
