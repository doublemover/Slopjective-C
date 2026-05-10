#include "sema/objc3_semantic_passes_expression_statement_validation_statement_binding_cases.inc"
#include "sema/objc3_semantic_passes_expression_statement_validation_statement_return_case.inc"
    case Stmt::Kind::Expr:
      if (stmt->expr_stmt != nullptr) {
        (void)ValidateExpr(stmt->expr_stmt->value.get(), scopes, globals, functions, diagnostics,
                           max_message_send_args, message_send_context);
      }
      return;
