#include "parse/objc3_parser_statement_nodes.h"

#include <utility>

namespace objc3c::parse {

std::unique_ptr<Stmt> BuildObjc3ExpressionStatement(
    const Objc3LexToken &anchor,
    std::unique_ptr<Expr> value) {
  auto stmt = std::make_unique<Stmt>();
  stmt->kind = Stmt::Kind::Expr;
  stmt->expr_stmt = std::make_unique<ExprStmt>();
  stmt->line = anchor.line;
  stmt->column = anchor.column;
  stmt->expr_stmt->line = anchor.line;
  stmt->expr_stmt->column = anchor.column;
  stmt->expr_stmt->value = std::move(value);
  return stmt;
}

}  // namespace objc3c::parse
