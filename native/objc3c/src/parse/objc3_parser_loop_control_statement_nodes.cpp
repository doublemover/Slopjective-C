#include "parse/objc3_parser_statement_nodes.h"

namespace objc3c::parse {

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
