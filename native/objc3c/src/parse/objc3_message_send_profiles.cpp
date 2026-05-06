#include "parse/objc3_message_send_profiles.h"

#include <sstream>

namespace objc3c::parse {

std::string BuildMessageSendFormSymbol(Expr::MessageSendForm form) {
  switch (form) {
  case Expr::MessageSendForm::Unary:
    return "message-send-form:unary";
  case Expr::MessageSendForm::Keyword:
    return "message-send-form:keyword";
  case Expr::MessageSendForm::None:
  default:
    return "message-send-form:none";
  }
}

std::string BuildOptionalSendSymbol(bool enabled) {
  return enabled ? "optional-send:enabled" : "optional-send:disabled";
}

std::string BuildMessageSendSelectorLoweringSymbol(
    const std::vector<Expr::MessageSendSelectorPiece> &pieces) {
  std::string normalized_selector;
  for (const auto &piece : pieces) {
    normalized_selector += piece.keyword;
    if (piece.has_argument) {
      normalized_selector += ":";
    }
  }
  return "selector-lowering:" + normalized_selector;
}

unsigned ComputeDispatchAbiArgumentPaddingSlots(std::size_t argument_count,
                                                unsigned runtime_arg_slots) {
  if (runtime_arg_slots == 0u) {
    return 0u;
  }
  const std::size_t remainder = argument_count % runtime_arg_slots;
  if (remainder == 0u) {
    return 0u;
  }
  return static_cast<unsigned>(runtime_arg_slots - static_cast<unsigned>(remainder));
}

std::string BuildDispatchAbiMarshallingSymbol(unsigned receiver_slots,
                                              unsigned selector_slots,
                                              unsigned argument_value_slots,
                                              unsigned argument_padding_slots,
                                              unsigned argument_total_slots,
                                              unsigned total_slots,
                                              unsigned runtime_arg_slots) {
  std::ostringstream out;
  out << "dispatch-abi-marshalling:recv=" << receiver_slots << ";sel=" << selector_slots
      << ";arg-values=" << argument_value_slots << ";arg-padding=" << argument_padding_slots
      << ";arg-total=" << argument_total_slots << ";total=" << total_slots
      << ";runtime-slots=" << runtime_arg_slots;
  return out.str();
}

std::string BuildNilReceiverFoldingSymbol(bool nil_receiver_foldable,
                                          bool requires_runtime_dispatch,
                                          Expr::MessageSendForm form) {
  std::ostringstream out;
  out << "nil-receiver:foldable=" << (nil_receiver_foldable ? "true" : "false")
      << ";runtime-dispatch=" << (requires_runtime_dispatch ? "required" : "elided")
      << ";form=";
  switch (form) {
  case Expr::MessageSendForm::Unary:
    out << "unary";
    break;
  case Expr::MessageSendForm::Keyword:
    out << "keyword";
    break;
  case Expr::MessageSendForm::None:
  default:
    out << "none";
    break;
  }
  return out.str();
}

bool IsSuperDispatchReceiver(const Expr &receiver) {
  return receiver.kind == Expr::Kind::Identifier && receiver.ident == "super";
}

std::string BuildSuperDispatchSymbol(bool super_dispatch_enabled,
                                     bool super_dispatch_requires_class_context,
                                     Expr::MessageSendForm form) {
  std::ostringstream out;
  out << "super-dispatch:enabled=" << (super_dispatch_enabled ? "true" : "false")
      << ";class-context=" << (super_dispatch_requires_class_context ? "required" : "not-required")
      << ";form=";
  switch (form) {
  case Expr::MessageSendForm::Unary:
    out << "unary";
    break;
  case Expr::MessageSendForm::Keyword:
    out << "keyword";
    break;
  case Expr::MessageSendForm::None:
  default:
    out << "none";
    break;
  }
  return out.str();
}

std::string BuildMethodFamilySemanticsSymbol(const std::string &method_family_name,
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
  out << "runtime-link-host-link:required=" << (runtime_link_required ? "true" : "false")
      << ";elided=" << (runtime_link_elided ? "true" : "false")
      << ";runtime-slots=" << runtime_dispatch_arg_slots
      << ";decl-params=" << runtime_dispatch_declaration_parameter_count
      << ";symbol=" << runtime_dispatch_symbol
      << ";form=";
  switch (form) {
  case Expr::MessageSendForm::Unary:
    out << "unary";
    break;
  case Expr::MessageSendForm::Keyword:
    out << "keyword";
    break;
  case Expr::MessageSendForm::None:
  default:
    out << "none";
    break;
  }
  return out.str();
}

}  // namespace objc3c::parse
