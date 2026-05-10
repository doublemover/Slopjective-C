static bool IsMessageSendSelectorLoweringSiteMetadataLess(
    const Objc3MessageSendSelectorLoweringSiteMetadata &lhs,
    const Objc3MessageSendSelectorLoweringSiteMetadata &rhs) {
  if (lhs.selector != rhs.selector) {
    return lhs.selector < rhs.selector;
  }
  if (lhs.selector_lowering_symbol != rhs.selector_lowering_symbol) {
    return lhs.selector_lowering_symbol < rhs.selector_lowering_symbol;
  }
  if (lhs.argument_count != rhs.argument_count) {
    return lhs.argument_count < rhs.argument_count;
  }
  if (lhs.selector_piece_count != rhs.selector_piece_count) {
    return lhs.selector_piece_count < rhs.selector_piece_count;
  }
  if (lhs.selector_argument_piece_count != rhs.selector_argument_piece_count) {
    return lhs.selector_argument_piece_count < rhs.selector_argument_piece_count;
  }
  if (lhs.unary_form != rhs.unary_form) {
    return lhs.unary_form < rhs.unary_form;
  }
  if (lhs.keyword_form != rhs.keyword_form) {
    return lhs.keyword_form < rhs.keyword_form;
  }
  if (lhs.selector_lowering_is_normalized != rhs.selector_lowering_is_normalized) {
    return lhs.selector_lowering_is_normalized < rhs.selector_lowering_is_normalized;
  }
  if (lhs.receiver_is_nil_literal != rhs.receiver_is_nil_literal) {
    return lhs.receiver_is_nil_literal < rhs.receiver_is_nil_literal;
  }
  if (lhs.nil_receiver_semantics_enabled != rhs.nil_receiver_semantics_enabled) {
    return lhs.nil_receiver_semantics_enabled < rhs.nil_receiver_semantics_enabled;
  }
  if (lhs.nil_receiver_foldable != rhs.nil_receiver_foldable) {
    return lhs.nil_receiver_foldable < rhs.nil_receiver_foldable;
  }
  if (lhs.nil_receiver_requires_runtime_dispatch != rhs.nil_receiver_requires_runtime_dispatch) {
    return lhs.nil_receiver_requires_runtime_dispatch < rhs.nil_receiver_requires_runtime_dispatch;
  }
  if (lhs.nil_receiver_semantics_is_normalized != rhs.nil_receiver_semantics_is_normalized) {
    return lhs.nil_receiver_semantics_is_normalized < rhs.nil_receiver_semantics_is_normalized;
  }
  if (lhs.runtime_link_host_link_required != rhs.runtime_link_host_link_required) {
    return lhs.runtime_link_host_link_required < rhs.runtime_link_host_link_required;
  }
  if (lhs.runtime_link_host_link_elided != rhs.runtime_link_host_link_elided) {
    return lhs.runtime_link_host_link_elided < rhs.runtime_link_host_link_elided;
  }
  if (lhs.runtime_link_host_link_runtime_dispatch_arg_slots != rhs.runtime_link_host_link_runtime_dispatch_arg_slots) {
    return lhs.runtime_link_host_link_runtime_dispatch_arg_slots <
           rhs.runtime_link_host_link_runtime_dispatch_arg_slots;
  }
  if (lhs.runtime_link_host_link_declaration_parameter_count !=
      rhs.runtime_link_host_link_declaration_parameter_count) {
    return lhs.runtime_link_host_link_declaration_parameter_count <
           rhs.runtime_link_host_link_declaration_parameter_count;
  }
  if (lhs.runtime_dispatch_bridge_symbol != rhs.runtime_dispatch_bridge_symbol) {
    return lhs.runtime_dispatch_bridge_symbol < rhs.runtime_dispatch_bridge_symbol;
  }
  if (lhs.runtime_link_host_link_symbol != rhs.runtime_link_host_link_symbol) {
    return lhs.runtime_link_host_link_symbol < rhs.runtime_link_host_link_symbol;
  }
  if (lhs.runtime_link_host_link_is_normalized != rhs.runtime_link_host_link_is_normalized) {
    return lhs.runtime_link_host_link_is_normalized < rhs.runtime_link_host_link_is_normalized;
  }
  if (lhs.receiver_is_super_identifier != rhs.receiver_is_super_identifier) {
    return lhs.receiver_is_super_identifier < rhs.receiver_is_super_identifier;
  }
  if (lhs.super_dispatch_enabled != rhs.super_dispatch_enabled) {
    return lhs.super_dispatch_enabled < rhs.super_dispatch_enabled;
  }
  if (lhs.super_dispatch_requires_class_context != rhs.super_dispatch_requires_class_context) {
    return lhs.super_dispatch_requires_class_context < rhs.super_dispatch_requires_class_context;
  }
  if (lhs.super_dispatch_semantics_is_normalized != rhs.super_dispatch_semantics_is_normalized) {
    return lhs.super_dispatch_semantics_is_normalized < rhs.super_dispatch_semantics_is_normalized;
  }
  if (lhs.method_family_name != rhs.method_family_name) {
    return lhs.method_family_name < rhs.method_family_name;
  }
  if (lhs.method_family_returns_retained_result != rhs.method_family_returns_retained_result) {
    return lhs.method_family_returns_retained_result < rhs.method_family_returns_retained_result;
  }
  if (lhs.method_family_returns_related_result != rhs.method_family_returns_related_result) {
    return lhs.method_family_returns_related_result < rhs.method_family_returns_related_result;
  }
  if (lhs.method_family_semantics_is_normalized != rhs.method_family_semantics_is_normalized) {
    return lhs.method_family_semantics_is_normalized < rhs.method_family_semantics_is_normalized;
  }
  if (lhs.line != rhs.line) {
    return lhs.line < rhs.line;
  }
  return lhs.column < rhs.column;
}

static std::vector<Objc3MessageSendSelectorLoweringSiteMetadata>
BuildMessageSendSelectorLoweringSiteMetadataLexicographic(const Objc3Program &ast) {
  std::vector<Objc3MessageSendSelectorLoweringSiteMetadata> sites;
  for (const auto &global : ast.globals) {
    CollectMessageSendSelectorLoweringSiteMetadataFromExpr(global.value.get(), sites);
  }
  for (const auto &fn : ast.functions) {
    CollectMessageSendSelectorLoweringSiteMetadataFromStatements(fn.body, sites);
  }
  // method-family/runtime evidence anchor: implementation method
  // bodies must contribute selector-lowering sites so super/dynamic legality
  // and runtime-link host-link summaries stay aligned with sema parity.
  for (const auto &implementation_decl : ast.implementations) {
    for (const auto &method : implementation_decl.methods) {
      if (!method.has_body) {
        continue;
      }
      CollectMessageSendSelectorLoweringSiteMetadataFromStatements(method.body,
                                                                   sites);
    }
  }
  std::sort(sites.begin(), sites.end(), IsMessageSendSelectorLoweringSiteMetadataLess);
  return sites;
}
