#include "parse/objc3_parser_statement_nodes.h"

#include <utility>

namespace objc3c::parse {

std::unique_ptr<Stmt> BuildObjc3AssignmentStatement(
    const Objc3LexToken &name,
    const std::string &op,
    std::unique_ptr<Expr> value) {
  auto stmt = std::make_unique<Stmt>();
  stmt->kind = Stmt::Kind::Assign;
  stmt->assign_stmt = std::make_unique<AssignStmt>();
  stmt->line = name.line;
  stmt->column = name.column;
  stmt->assign_stmt->line = name.line;
  stmt->assign_stmt->column = name.column;
  stmt->assign_stmt->name = name.text;
  stmt->assign_stmt->op = op;
  stmt->assign_stmt->value = std::move(value);
  return stmt;
}

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

std::unique_ptr<Stmt> BuildObjc3LoopControlStatement(
    Stmt::Kind kind,
    const Objc3LexToken &anchor) {
  auto stmt = std::make_unique<Stmt>();
  stmt->kind = kind;
  stmt->line = anchor.line;
  stmt->column = anchor.column;
  return stmt;
}

}  // namespace objc3c::parse
