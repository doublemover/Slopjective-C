    }
    case Stmt::Kind::DoWhile: {
      const DoWhileStmt *do_while_stmt = stmt->do_while_stmt.get();
      if (do_while_stmt == nullptr) {
        return;
      }
      scopes.push_back({});
      ValidateStatements(do_while_stmt->body, scopes, globals, functions, expected_return_type, function_name,
                         diagnostics, loop_depth + 1, switch_depth,
                         defer_loop_base_depth, defer_switch_base_depth,
                         inside_defer_body,
                         max_message_send_args, message_send_context);
      scopes.pop_back();

      const SemanticTypeInfo condition_type = ValidateExpr(
          do_while_stmt->condition.get(), scopes, globals, functions, diagnostics,
          max_message_send_args, message_send_context);
      if (!IsUnknownSemanticType(condition_type) && !IsScalarBoolCompatibleType(condition_type)) {
        diagnostics.push_back(MakeDiag(do_while_stmt->line, do_while_stmt->column, "O3S206",
                                       "type mismatch: do-while condition must be bool-compatible"));
      }
      return;
    }
    case Stmt::Kind::For: {
      const ForStmt *for_stmt = stmt->for_stmt.get();
      if (for_stmt == nullptr) {
        return;
      }
      scopes.push_back({});
      validate_for_clause(for_stmt->init);
      if (for_stmt->condition != nullptr) {
        const SemanticTypeInfo condition_type = ValidateExpr(
            for_stmt->condition.get(), scopes, globals, functions, diagnostics,
            max_message_send_args, message_send_context);
        if (!IsUnknownSemanticType(condition_type) && !IsScalarBoolCompatibleType(condition_type)) {
          diagnostics.push_back(MakeDiag(for_stmt->line, for_stmt->column, "O3S206",
                                         "type mismatch: for condition must be bool-compatible"));
        }
      }
      validate_for_clause(for_stmt->step);
      scopes.push_back({});
      ValidateStatements(for_stmt->body, scopes, globals, functions, expected_return_type, function_name, diagnostics,
                         loop_depth + 1, switch_depth, defer_loop_base_depth,
                         defer_switch_base_depth, inside_defer_body,
                         max_message_send_args, message_send_context);
      scopes.pop_back();
      scopes.pop_back();
      return;
    }
    case Stmt::Kind::Switch: {
      const SwitchStmt *switch_stmt = stmt->switch_stmt.get();
      if (switch_stmt == nullptr) {
        return;
      }
      const SemanticTypeInfo condition_type = ValidateExpr(
          switch_stmt->condition.get(), scopes, globals, functions, diagnostics,
          max_message_send_args, message_send_context);
      if (switch_stmt->match_surface_enabled) {
        const MatchExhaustivenessInfo exhaustiveness =
            ClassifyMatchExhaustiveness(*switch_stmt);
        if (!exhaustiveness.exhaustive) {
          diagnostics.push_back(MakeDiag(
              switch_stmt->line, switch_stmt->column, "O3S206",
              "type mismatch: match statement must be exhaustive for the admitted pattern surface"));
        }
        for (const auto &case_stmt : switch_stmt->cases) {
          scopes.push_back({});
          if ((case_stmt.match_pattern_kind == MatchPatternKind::Binding ||
               case_stmt.match_pattern_kind == MatchPatternKind::ResultCase) &&
              !case_stmt.match_binding_name.empty()) {
            scopes.back()[case_stmt.match_binding_name] = condition_type;
          }
          ValidateStatements(case_stmt.body, scopes, globals, functions,
                             expected_return_type, function_name, diagnostics,
                             loop_depth, switch_depth + 1,
                             defer_loop_base_depth, defer_switch_base_depth,
                             inside_defer_body,
                             max_message_send_args, message_send_context);
          scopes.pop_back();
        }
        return;
      }
      if (!IsUnknownSemanticType(condition_type) && !IsScalarBoolCompatibleType(condition_type)) {
        diagnostics.push_back(MakeDiag(switch_stmt->line, switch_stmt->column, "O3S206",
                                       "type mismatch: switch condition must be i32-compatible"));
      }

      std::unordered_set<int> seen_case_values;
      bool seen_default = false;
      for (const auto &case_stmt : switch_stmt->cases) {
        if (case_stmt.is_default) {
          if (seen_default) {
            diagnostics.push_back(MakeDiag(case_stmt.line, case_stmt.column, "O3S206",
                                           "type mismatch: duplicate default label in switch"));
          }
          seen_default = true;
        } else {
          if (!seen_case_values.insert(case_stmt.value).second) {
            diagnostics.push_back(MakeDiag(case_stmt.value_line, case_stmt.value_column, "O3S206",
                                           "type mismatch: duplicate case label '" +
                                               std::to_string(case_stmt.value) + "' in switch"));
          }
        }
        scopes.push_back({});
        ValidateStatements(case_stmt.body, scopes, globals, functions, expected_return_type, function_name,
                           diagnostics, loop_depth, switch_depth + 1,
                           defer_loop_base_depth, defer_switch_base_depth,
                           inside_defer_body,
                           max_message_send_args, message_send_context);
        scopes.pop_back();
      }
      return;
    }
    case Stmt::Kind::While: {
      const WhileStmt *while_stmt = stmt->while_stmt.get();
      if (while_stmt == nullptr) {
        return;
      }
      const SemanticTypeInfo condition_type = ValidateExpr(
          while_stmt->condition.get(), scopes, globals, functions, diagnostics,
          max_message_send_args, message_send_context);
      if (!IsUnknownSemanticType(condition_type) && !IsScalarBoolCompatibleType(condition_type)) {
        diagnostics.push_back(MakeDiag(while_stmt->line, while_stmt->column, "O3S206",
                                       "type mismatch: while condition must be bool-compatible"));
      }
      scopes.push_back({});
      ValidateStatements(while_stmt->body, scopes, globals, functions, expected_return_type, function_name,
                         diagnostics, loop_depth + 1, switch_depth,
                         defer_loop_base_depth, defer_switch_base_depth,
                         inside_defer_body,
                         max_message_send_args, message_send_context);
      scopes.pop_back();
      return;
    }
    case Stmt::Kind::Block:
    {
      const BlockStmt *block_stmt = stmt->block_stmt.get();
      if (block_stmt == nullptr) {
        return;
      }
      scopes.push_back({});
      ValidateStatements(block_stmt->body, scopes, globals, functions, expected_return_type, function_name,
                         diagnostics, loop_depth, switch_depth,
                         defer_loop_base_depth, defer_switch_base_depth,
                         inside_defer_body,
                         max_message_send_args, message_send_context);
      scopes.pop_back();
      if (block_stmt->is_do_catch_scope) {
        for (const auto &clause : block_stmt->catch_clauses) {
          scopes.push_back({});
          if (clause.has_binding && !clause.binding_name.empty()) {
            scopes.back().emplace(clause.binding_name,
                                  MakeScalarSemanticType(ValueType::ObjCId));
          }
          ValidateStatements(clause.body, scopes, globals, functions,
                             expected_return_type, function_name, diagnostics,
                             loop_depth, switch_depth, defer_loop_base_depth,
                             defer_switch_base_depth, inside_defer_body,
                             max_message_send_args, message_send_context);
          scopes.pop_back();
        }
      }
      return;
    }
    case Stmt::Kind::Defer: {
      const BlockStmt *block_stmt = stmt->block_stmt.get();
      if (block_stmt == nullptr) {
        return;
      }
      scopes.push_back({});
      ValidateStatements(block_stmt->body, scopes, globals, functions,
                         expected_return_type, function_name, diagnostics,
                         loop_depth, switch_depth, loop_depth, switch_depth,
                         true, max_message_send_args, message_send_context);
      scopes.pop_back();
      return;
    }
    case Stmt::Kind::Break:
      if (inside_defer_body && loop_depth <= defer_loop_base_depth &&
          switch_depth <= defer_switch_base_depth) {
        diagnostics.push_back(MakeDiag(
            stmt->line,
            stmt->column,
            "O3S222",
            "control-flow misuse: 'break' may not exit a defer body"));
        return;
      }
      if (loop_depth <= 0 && switch_depth <= 0) {
        diagnostics.push_back(MakeDiag(stmt->line, stmt->column, "O3S212", "loop-control misuse: 'break' outside loop"));
      }
      return;
    case Stmt::Kind::Continue:
      if (inside_defer_body && loop_depth <= defer_loop_base_depth) {
        diagnostics.push_back(MakeDiag(
            stmt->line,
            stmt->column,
            "O3S222",
            "control-flow misuse: 'continue' may not exit a defer body"));
        return;
      }
      if (loop_depth <= 0) {
        diagnostics.push_back(
            MakeDiag(stmt->line, stmt->column, "O3S213", "loop-control misuse: 'continue' outside loop"));
      }
      return;
    case Stmt::Kind::Empty:
      return;
  }
}

static void ValidateStatements(const std::vector<std::unique_ptr<Stmt>> &statements,
                               std::vector<SemanticScope> &scopes,
                               const std::unordered_map<std::string, ValueType> &globals,
                               const std::unordered_map<std::string, FunctionInfo> &functions,
                               const SemanticTypeInfo &expected_return_type, const std::string &function_name,
                               std::vector<std::string> &diagnostics, int loop_depth, int switch_depth,
                               int defer_loop_base_depth, int defer_switch_base_depth,
                               bool inside_defer_body,
                               std::size_t max_message_send_args,
                               const Objc3MessageSendResolutionContext &message_send_context) {
  for (const auto &stmt : statements) {
    ValidateStatement(stmt.get(), scopes, globals, functions, expected_return_type, function_name, diagnostics,
                      loop_depth, switch_depth, defer_loop_base_depth,
                      defer_switch_base_depth, inside_defer_body,
                      max_message_send_args,
                      message_send_context);
  }
}

#include "sema/objc3_semantic_passes_type_system_semantic_model_summary.inc"

Objc3ErrorHandlingErrorSemanticModelSummary BuildErrorHandlingErrorSemanticModelSummary(
    const Objc3FrontendErrorHandlingErrorSourceClosureSummary &source_summary,
    const Objc3SemanticIntegrationSurface &surface) {
  Objc3ErrorHandlingErrorSemanticModelSummary summary;
  summary.function_throws_declaration_sites =
      source_summary.function_throws_declaration_sites;
  summary.method_throws_declaration_sites =
      source_summary.method_throws_declaration_sites;
  summary.throws_declaration_sites =
      summary.function_throws_declaration_sites +
      summary.method_throws_declaration_sites;
  summary.result_like_sites = surface.result_like_lowering_summary.result_like_sites;
  summary.result_success_sites =
      surface.result_like_lowering_summary.result_success_sites;
  summary.result_failure_sites =
      surface.result_like_lowering_summary.result_failure_sites;
  summary.result_branch_sites =
      surface.result_like_lowering_summary.result_branch_sites;
  summary.result_payload_sites =
      surface.result_like_lowering_summary.result_payload_sites;
  summary.ns_error_bridging_sites =
      surface.ns_error_bridging_summary.ns_error_bridging_sites;
  summary.ns_error_out_parameter_sites =
      surface.ns_error_bridging_summary.ns_error_out_parameter_sites;
  summary.ns_error_bridge_path_sites =
      surface.ns_error_bridging_summary.ns_error_bridge_path_sites;
  summary.objc_nserror_attribute_sites =
      source_summary.objc_nserror_attribute_sites;
