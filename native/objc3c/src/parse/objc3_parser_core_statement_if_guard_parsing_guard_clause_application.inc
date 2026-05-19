      for (auto &clause : clauses) {
        if (clause.kind == ParsedGuardConditionClause::Kind::OptionalBinding) {
          stmt->if_stmt->optional_binding_surface_enabled = true;
          stmt->if_stmt->guard_binding_surface_enabled = true;
          ++stmt->if_stmt->optional_binding_clause_count;
          auto synthetic = std::make_unique<Stmt>();
          synthetic->kind = Stmt::Kind::Let;
          synthetic->line = clause.binding.line;
          synthetic->column = clause.binding.column;
          synthetic->let_stmt = std::make_unique<LetStmt>();
          synthetic->let_stmt->name = clause.binding.name;
          synthetic->let_stmt->line = clause.binding.line;
          synthetic->let_stmt->column = clause.binding.column;
          synthetic->let_stmt->value = std::move(clause.binding.value);
          stmt->if_stmt->then_body.push_back(std::move(synthetic));
        } else {
          ++stmt->if_stmt->guard_boolean_condition_clause_count;
          stmt->if_stmt->guard_condition_exprs.push_back(std::move(clause.condition));
        }
      }
