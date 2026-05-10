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

static Objc3BlockAbiInvokeTrampolineSiteMetadata BuildBlockAbiInvokeTrampolineSiteMetadata(const Expr &expr) {
  Objc3BlockAbiInvokeTrampolineSiteMetadata metadata;
  metadata.invoke_argument_slots = expr.block_abi_invoke_argument_slots;
  metadata.capture_word_count = expr.block_abi_capture_word_count;
  metadata.parameter_count = expr.block_parameter_count;
  metadata.capture_count = expr.block_capture_count;
  metadata.body_statement_count = expr.block_body_statement_count;
  metadata.has_invoke_trampoline = expr.block_abi_has_invoke_trampoline;
  metadata.layout_is_normalized = expr.block_abi_layout_is_normalized;
  metadata.layout_profile = expr.block_abi_layout_profile;
  metadata.descriptor_symbol = expr.block_abi_descriptor_symbol;
  metadata.invoke_trampoline_symbol = expr.block_invoke_trampoline_symbol;
  metadata.line = expr.line;
  metadata.column = expr.column;
  metadata.has_count_mismatch =
      metadata.invoke_argument_slots != metadata.parameter_count ||
      metadata.capture_word_count != metadata.capture_count ||
      !IsSortedUniqueStrings(expr.block_parameter_names_lexicographic) ||
      !IsSortedUniqueStrings(expr.block_capture_names_lexicographic);
  return metadata;
}

static void CollectBlockAbiInvokeTrampolineSiteMetadataFromExpr(
    const Expr *expr,
    std::vector<Objc3BlockAbiInvokeTrampolineSiteMetadata> &sites) {
  if (expr == nullptr) {
    return;
  }

  if (expr->kind == Expr::Kind::BlockLiteral) {
    sites.push_back(BuildBlockAbiInvokeTrampolineSiteMetadata(*expr));
  }

  CollectBlockAbiInvokeTrampolineSiteMetadataFromExpr(expr->receiver.get(), sites);
  CollectBlockAbiInvokeTrampolineSiteMetadataFromExpr(expr->left.get(), sites);
  CollectBlockAbiInvokeTrampolineSiteMetadataFromExpr(expr->right.get(), sites);
  CollectBlockAbiInvokeTrampolineSiteMetadataFromExpr(expr->third.get(), sites);
  for (const auto &arg : expr->args) {
    CollectBlockAbiInvokeTrampolineSiteMetadataFromExpr(arg.get(), sites);
  }
}

static void CollectBlockAbiInvokeTrampolineSiteMetadataFromStatements(
    const std::vector<std::unique_ptr<Stmt>> &statements,
    std::vector<Objc3BlockAbiInvokeTrampolineSiteMetadata> &sites);

static void CollectBlockAbiInvokeTrampolineSiteMetadataFromForClause(
    const ForClause &clause,
    std::vector<Objc3BlockAbiInvokeTrampolineSiteMetadata> &sites) {
  CollectBlockAbiInvokeTrampolineSiteMetadataFromExpr(clause.value.get(), sites);
}

static void CollectBlockAbiInvokeTrampolineSiteMetadataFromStatement(
    const Stmt *stmt,
    std::vector<Objc3BlockAbiInvokeTrampolineSiteMetadata> &sites) {
  if (stmt == nullptr) {
    return;
  }

