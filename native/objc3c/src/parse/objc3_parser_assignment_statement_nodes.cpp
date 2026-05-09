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

}  // namespace objc3c::parse
