      if (At(TokenKind::KwLet) || At(TokenKind::KwVar)) {
        auto clauses = ParseOptionalBindingClauses();
        if (clauses.empty()) {
          return nullptr;
        }
        stmt->if_stmt->optional_binding_surface_enabled = true;
        stmt->if_stmt->optional_binding_clause_count = clauses.size();
        stmt->if_stmt->condition =
            BuildObjc3BoolLiteralExprAt(true, stmt->line, stmt->column);
        auto user_then_body = ParseControlBody();
        if (block_failed_) {
          block_failed_ = false;
          return nullptr;
        }
        for (auto &clause : clauses) {
          auto synthetic = std::make_unique<Stmt>();
          synthetic->kind = Stmt::Kind::Let;
          synthetic->line = clause.line;
          synthetic->column = clause.column;
          synthetic->let_stmt = std::make_unique<LetStmt>();
          synthetic->let_stmt->name = clause.name;
          synthetic->let_stmt->line = clause.line;
          synthetic->let_stmt->column = clause.column;
          synthetic->let_stmt->value = std::move(clause.value);
          stmt->if_stmt->then_body.push_back(std::move(synthetic));
        }
        for (auto &nested : user_then_body) {
          stmt->if_stmt->then_body.push_back(std::move(nested));
        }
#include "parse/objc3_parser_core_statement_if_guard_parsing_if_else_body.inc"
        return stmt;
      }
