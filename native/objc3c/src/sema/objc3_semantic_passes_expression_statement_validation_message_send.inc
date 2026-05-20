static void DiagnoseRetainedMethodFamilyReturnTypeMismatch(
    const Expr *expr, const std::string &selector,
    const std::string &receiver_label, const SemanticTypeInfo &return_type,
    std::vector<std::string> &diagnostics) {
  if (expr == nullptr || !expr->method_family_returns_retained_result ||
      IsUnknownSemanticType(return_type) ||
      IsObjCReferenceSemanticType(return_type)) {
    return;
  }
  const std::string method_family =
      expr->method_family_name.empty() ? "retained" : expr->method_family_name;
  diagnostics.push_back(MakeDiag(
      expr->line, expr->column, "O3S305",
      "retainable-family legality failed: selector '" + selector + "' on " +
          receiver_label + " belongs to Objective-C method family '" +
          method_family + "' but resolves to non-object return type '" +
          SemanticTypeName(return_type) + "'"));
}

static SemanticTypeInfo ValidateMessageSendExpr(const Expr *expr,
                                                const std::vector<SemanticScope> &scopes,
                                                const std::unordered_map<std::string, ValueType> &globals,
                                                const std::unordered_map<std::string, FunctionInfo> &functions,
                                                std::vector<std::string> &diagnostics,
                                                std::size_t max_message_send_args,
                                                const Objc3MessageSendResolutionContext &message_send_context) {
#include "sema/objc3_semantic_passes_expression_statement_validation_message_send_site_collection.inc"
#include "sema/objc3_semantic_passes_expression_statement_validation_message_send_receiver_selector_checks.inc"
#include "sema/objc3_semantic_passes_expression_statement_validation_message_send_protocol_qualified.inc"
#include "sema/objc3_semantic_passes_expression_statement_validation_message_send_typed_receiver_diagnostics.inc"
#include "sema/objc3_semantic_passes_expression_statement_validation_message_send_concrete_dispatch_diagnostics.inc"
}
