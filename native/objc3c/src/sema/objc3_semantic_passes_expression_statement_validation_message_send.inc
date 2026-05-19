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
