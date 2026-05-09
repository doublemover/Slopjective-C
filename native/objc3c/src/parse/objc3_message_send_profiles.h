#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "ast/objc3_ast_core.h"

namespace objc3c::parse {

inline constexpr unsigned kDispatchAbiMarshallingRuntimeArgSlots = 4u;
inline constexpr const char *kRuntimeLinkHostLinkDispatchSymbol =
    "objc3_runtime_dispatch_i32";

std::string BuildMessageSendFormSymbol(Expr::MessageSendForm form);
std::string BuildOptionalSendSymbol(bool enabled);
std::string BuildMessageSendSelectorLoweringSymbol(
    const std::vector<Expr::MessageSendSelectorPiece> &pieces);
unsigned ComputeDispatchAbiArgumentPaddingSlots(std::size_t argument_count,
                                                unsigned runtime_arg_slots);
std::string BuildDispatchAbiMarshallingSymbol(unsigned receiver_slots,
                                              unsigned selector_slots,
                                              unsigned argument_value_slots,
                                              unsigned argument_padding_slots,
                                              unsigned argument_total_slots,
                                              unsigned total_slots,
                                              unsigned runtime_arg_slots);
std::string BuildNilReceiverFoldingSymbol(bool nil_receiver_foldable,
                                          bool requires_runtime_dispatch,
                                          Expr::MessageSendForm form);
bool IsSuperDispatchReceiver(const Expr &receiver);
std::string BuildSuperDispatchSymbol(bool super_dispatch_enabled,
                                     bool super_dispatch_requires_class_context,
                                     Expr::MessageSendForm form);
std::string BuildMethodFamilySemanticsSymbol(const std::string &method_family_name,
                                             bool returns_retained_result,
                                             bool returns_related_result);
std::string BuildRuntimeLinkHostLinkSymbol(
    bool runtime_link_required,
    bool runtime_link_elided,
    unsigned runtime_dispatch_arg_slots,
    unsigned runtime_dispatch_declaration_parameter_count,
    const std::string &runtime_dispatch_symbol,
    Expr::MessageSendForm form);

}  // namespace objc3c::parse
