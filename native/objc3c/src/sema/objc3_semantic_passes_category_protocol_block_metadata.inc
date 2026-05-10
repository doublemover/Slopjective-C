#include "sema/objc3_semantic_passes_category_merge_candidate_helpers.inc"

#include "sema/objc3_semantic_passes_category_merge_surface_builder.inc"

#include "sema/objc3_semantic_passes_protocol_conformance_validation.inc"

#include "sema/objc3_semantic_passes_concrete_message_resolution.inc"

#include "sema/objc3_semantic_passes_method_lookup_override_conflicts.inc"

#include "sema/objc3_semantic_passes_property_synthesis_ivar_bindings.inc"

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

#include "sema/objc3_semantic_passes_block_literal_capture_metadata.inc"

#include "sema/objc3_semantic_passes_block_abi_invoke_trampoline_metadata.inc"

