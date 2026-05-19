static Objc3MessageSendSelectorLoweringSiteMetadata BuildMessageSendSelectorLoweringSiteMetadata(const Expr &expr) {
  Objc3MessageSendSelectorLoweringSiteMetadata metadata;
  metadata.selector = expr.selector;
  metadata.selector_lowering_symbol =
      expr.selector_lowering_symbol.empty() ? expr.selector : expr.selector_lowering_symbol;
  metadata.argument_count = expr.args.size();
  const Expr::MessageSendForm resolved_form = ResolveMessageSendForm(expr);
  metadata.unary_form = resolved_form == Expr::MessageSendForm::Unary;
  metadata.keyword_form = resolved_form == Expr::MessageSendForm::Keyword;
  metadata.line = expr.line;
  metadata.column = expr.column;

  if (!expr.selector_lowering_pieces.empty()) {
    metadata.selector_piece_count = expr.selector_lowering_pieces.size();
    for (const Expr::MessageSendSelectorPiece &piece : expr.selector_lowering_pieces) {
      if (piece.has_argument) {
        ++metadata.selector_argument_piece_count;
      }
    }
  } else if (metadata.keyword_form) {
    const std::string selector_symbol =
        metadata.selector_lowering_symbol.empty() ? metadata.selector : metadata.selector_lowering_symbol;
    metadata.selector_piece_count = CountSelectorKeywordPieces(selector_symbol);
    metadata.selector_argument_piece_count = metadata.selector_piece_count;
  }

  metadata.selector_lowering_is_normalized =
      expr.selector_lowering_is_normalized ||
      (!metadata.selector.empty() && !metadata.selector_lowering_symbol.empty() &&
       metadata.selector == metadata.selector_lowering_symbol);
  metadata.receiver_is_nil_literal = expr.receiver != nullptr && expr.receiver->kind == Expr::Kind::NilLiteral;
  const objc3c::support::Objc3NilReceiverProfile nil_receiver_profile =
      objc3c::support::BuildNilReceiverProfile(
          metadata.receiver_is_nil_literal,
          expr.nil_receiver_semantics_is_normalized,
          expr.nil_receiver_semantics_enabled, expr.nil_receiver_foldable,
          expr.nil_receiver_requires_runtime_dispatch);
  metadata.nil_receiver_semantics_enabled =
      nil_receiver_profile.semantics_enabled;
  metadata.nil_receiver_foldable = nil_receiver_profile.foldable;
  metadata.nil_receiver_requires_runtime_dispatch =
      nil_receiver_profile.requires_runtime_dispatch;
  metadata.nil_receiver_semantics_is_normalized =
      nil_receiver_profile.normalized;
  metadata.runtime_link_host_link_required =
      expr.runtime_link_host_link_is_normalized ? expr.runtime_link_host_link_required
                                                : metadata.nil_receiver_requires_runtime_dispatch;
  metadata.runtime_link_host_link_elided =
      expr.runtime_link_host_link_is_normalized ? expr.runtime_link_host_link_elided
                                                : !metadata.runtime_link_host_link_required;
  metadata.runtime_link_host_link_runtime_dispatch_arg_slots = expr.dispatch_abi_runtime_arg_slots;
  metadata.runtime_link_host_link_declaration_parameter_count =
      expr.runtime_link_host_link_is_normalized
          ? static_cast<std::size_t>(expr.runtime_link_host_link_declaration_parameter_count)
          : metadata.runtime_link_host_link_runtime_dispatch_arg_slots + 2u;
  metadata.runtime_dispatch_bridge_symbol = expr.runtime_dispatch_bridge_symbol.empty()
                                                ? kObjc3RuntimeLinkHostLinkDefaultDispatchSymbol
                                                : expr.runtime_dispatch_bridge_symbol;
  metadata.runtime_link_host_link_symbol = expr.runtime_link_host_link_symbol;
  metadata.runtime_link_host_link_is_normalized =
      expr.runtime_link_host_link_is_normalized ||
      (metadata.runtime_link_host_link_required == metadata.nil_receiver_requires_runtime_dispatch &&
       metadata.runtime_link_host_link_elided == !metadata.runtime_link_host_link_required &&
       metadata.runtime_link_host_link_declaration_parameter_count ==
           metadata.runtime_link_host_link_runtime_dispatch_arg_slots + 2u &&
       !metadata.runtime_dispatch_bridge_symbol.empty());
  metadata.receiver_is_super_identifier =
      expr.receiver != nullptr && expr.receiver->kind == Expr::Kind::Identifier && expr.receiver->ident == "super";
  const objc3c::support::Objc3SuperDispatchProfile super_dispatch_profile =
      objc3c::support::BuildSuperDispatchProfile(
          metadata.receiver_is_super_identifier,
          expr.super_dispatch_semantics_is_normalized,
          expr.super_dispatch_enabled,
          expr.super_dispatch_requires_class_context);
  metadata.super_dispatch_enabled = super_dispatch_profile.enabled;
  metadata.super_dispatch_requires_class_context =
      super_dispatch_profile.requires_class_context;
  metadata.super_dispatch_semantics_is_normalized =
      super_dispatch_profile.normalized;
  const objc3c::support::Objc3MethodFamilyProfile method_family_profile =
      objc3c::support::BuildMethodFamilyProfile(
          metadata.selector, expr.method_family_semantics_is_normalized,
          expr.method_family_name, expr.method_family_returns_retained_result,
          expr.method_family_returns_related_result);
  metadata.method_family_name = method_family_profile.name;
  metadata.method_family_returns_retained_result =
      method_family_profile.returns_retained_result;
  metadata.method_family_returns_related_result =
      method_family_profile.returns_related_result;
  metadata.method_family_semantics_is_normalized =
      method_family_profile.normalized;
  return metadata;
}
