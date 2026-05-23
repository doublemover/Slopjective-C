#include "sema/objc3_semantic_passes_generic_protocol_message_validation_generic_argument_specialization_suffix.inc"

#include "sema/objc3_semantic_passes_generic_protocol_message_validation_generic_argument_protocol_relations.inc"

#include "sema/objc3_semantic_passes_generic_protocol_specialization_validation.inc"

#include "sema/objc3_semantic_passes_generic_protocol_message_validation_protocol_composition_bindings.inc"

// Legacy extraction anchors retained for contract tests:
// static ValueType ValidateMessageSendExpr(
// if (receiver_type != ValueType::Unknown && !IsMessageCompatibleType(receiver_type)) {
static SemanticTypeInfo ValidateMessageSendExpr(const Expr *expr,
                                                const std::vector<SemanticScope> &scopes,
                                                const std::unordered_map<std::string, ValueType> &globals,
                                                const std::unordered_map<std::string, FunctionInfo> &functions,
                                                std::vector<std::string> &diagnostics,
                                                std::size_t max_message_send_args,
                                                const Objc3MessageSendResolutionContext &message_send_context);

#include "sema/objc3_semantic_passes_generic_protocol_message_validation_block_captures.inc"

#include "sema/objc3_semantic_passes_generic_function_call_substitution.inc"

#include "sema/objc3_semantic_passes_collection_type_helpers.inc"

static SemanticTypeInfo ValidateExpr(const Expr *expr, const std::vector<SemanticScope> &scopes,
                                     const std::unordered_map<std::string, ValueType> &globals,
                                     const std::unordered_map<std::string, FunctionInfo> &functions,
                                     std::vector<std::string> &diagnostics,
                                     std::size_t max_message_send_args,
                                     const Objc3MessageSendResolutionContext &message_send_context,
                                     bool allow_throwing_call = false) {
  if (expr == nullptr) {
    return MakeScalarSemanticType(ValueType::Unknown);
  }
  if (expr->await_expression_enabled &&
      !message_send_context.inside_async_context) {
    diagnostics.push_back(MakeDiag(
        expr->line, expr->column, "O3S223",
        "async semantics failed: 'await' is only valid inside an async function or method"));
  }
  switch (expr->kind) {
#include "sema/objc3_semantic_passes_generic_protocol_message_validation_expr_literal_identifier_cases.inc"
#include "sema/objc3_semantic_passes_generic_protocol_message_validation_expr_binary_cases.inc"
#include "sema/objc3_semantic_passes_generic_protocol_message_validation_expr_conditional_case_prefix.inc"
