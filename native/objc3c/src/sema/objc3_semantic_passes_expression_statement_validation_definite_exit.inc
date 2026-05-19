static bool StatementDefinitelyExitsScope(const Stmt *stmt);

static bool StatementListDefinitelyExitsScope(
    const std::vector<std::unique_ptr<Stmt>> &statements) {
  if (statements.empty()) {
    return false;
  }
  return StatementDefinitelyExitsScope(statements.back().get());
}

static bool StatementDefinitelyExitsScope(const Stmt *stmt) {
  if (stmt == nullptr) {
    return false;
  }
  switch (stmt->kind) {
    case Stmt::Kind::Return:
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
      return true;
#include "sema/objc3_semantic_passes_expression_statement_validation_definite_exit_structured_cases.inc"
#include "sema/objc3_semantic_passes_expression_statement_validation_definite_exit_non_exiting_cases.inc"
  }
  return false;
}
