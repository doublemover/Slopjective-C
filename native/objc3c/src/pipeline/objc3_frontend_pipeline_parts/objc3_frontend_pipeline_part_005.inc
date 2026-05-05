    const std::vector<Objc3LexToken> &tokens) {
  Objc3FrontendControlFlowControlFlowSourceClosureSummary summary;
  for (const auto &token : tokens) {
    if (token.kind == Objc3LexTokenKind::KwDefer) {
      ++summary.defer_keyword_sites;
    }
  }
  for (const auto &fn : program.functions) {
    for (const auto &stmt : fn.body) {
      CollectControlFlowControlFlowSourceClosureStmtSites(stmt.get(), summary);
    }
  }
  for (const auto &implementation : program.implementations) {
    for (const auto &method : implementation.methods) {
      for (const auto &stmt : method.body) {
        CollectControlFlowControlFlowSourceClosureStmtSites(stmt.get(), summary);
      }
    }
  }
  summary.guard_binding_source_supported = true;
  summary.guard_condition_list_source_supported = true;
  summary.switch_case_pattern_source_supported = true;
  summary.defer_statement_source_supported = true;
  summary.match_statement_source_supported = true;
  summary.match_wildcard_pattern_source_supported = true;
  summary.match_literal_pattern_source_supported = true;
  summary.match_binding_pattern_source_supported = true;
  summary.match_result_case_pattern_source_supported = true;
  summary.defer_keyword_reserved = true;
  summary.defer_fail_closed = false;
  summary.match_expression_fail_closed = true;
  summary.guarded_pattern_fail_closed = true;
  summary.type_test_pattern_fail_closed = true;
  summary.deterministic_handoff =
      summary.guard_binding_clause_sites >= summary.guard_binding_sites &&
      summary.guard_boolean_condition_sites +
              summary.guard_binding_sites >=
          summary.guard_binding_sites &&
      summary.switch_default_pattern_sites <=
          summary.switch_case_pattern_sites + summary.switch_default_pattern_sites &&
      summary.match_default_sites <=
          summary.match_statement_sites + summary.match_default_sites;
  summary.ready_for_semantic_expansion = summary.deterministic_handoff;
  summary.replay_key = BuildControlFlowControlFlowSourceClosureReplayKey(summary);
  return summary;
}

Objc3FrontendErrorHandlingErrorSourceClosureSummary
BuildErrorHandlingErrorSourceClosureSummary(const Objc3Program &program,
                                    const std::vector<Objc3LexToken> &tokens) {
  Objc3FrontendErrorHandlingErrorSourceClosureSummary summary;
  for (const auto &token : tokens) {
    if (token.kind == Objc3LexTokenKind::KwTry) {
      ++summary.try_keyword_sites;
    } else if (token.kind == Objc3LexTokenKind::KwThrow) {
      ++summary.throw_keyword_sites;
    } else if (token.kind == Objc3LexTokenKind::KwCatch) {
      ++summary.catch_keyword_sites;
    }
  }

  bool throws_profiles_normalized = true;
  bool result_profiles_normalized = true;
  bool ns_error_profiles_normalized = true;
  for (const auto &fn : program.functions) {
    if (fn.throws_declared) {
      ++summary.function_throws_declaration_sites;
    }
    throws_profiles_normalized =
        throws_profiles_normalized && fn.throws_declaration_profile_is_normalized;
    result_profiles_normalized =
        result_profiles_normalized && fn.result_like_profile_is_normalized;
    ns_error_profiles_normalized =
        ns_error_profiles_normalized && fn.ns_error_bridging_profile_is_normalized;
    summary.result_like_sites += fn.result_like_sites;
    summary.result_success_sites += fn.result_success_sites;
    summary.result_failure_sites += fn.result_failure_sites;
    summary.result_branch_sites += fn.result_branch_sites;
    summary.result_payload_sites += fn.result_payload_sites;
    summary.ns_error_bridging_sites += fn.ns_error_bridging_sites;
    summary.ns_error_out_parameter_sites += fn.ns_error_out_parameter_sites;
    summary.ns_error_bridge_path_sites += fn.ns_error_bridge_path_sites;
    summary.objc_nserror_attribute_sites += fn.objc_nserror_attribute_sites;
    summary.objc_status_code_attribute_sites +=
        fn.objc_status_code_attribute_sites;
    summary.status_code_success_clause_sites +=
        fn.status_code_success_clause_sites;
    summary.status_code_error_type_clause_sites +=
        fn.status_code_error_type_clause_sites;
    summary.status_code_mapping_clause_sites +=
        fn.status_code_mapping_clause_sites;
    throws_profiles_normalized =
        throws_profiles_normalized && fn.error_bridge_marker_profile_is_normalized;
  }
  for (const auto &implementation : program.implementations) {
    for (const auto &method : implementation.methods) {
      if (method.throws_declared) {
        ++summary.method_throws_declaration_sites;
      }
      throws_profiles_normalized =
          throws_profiles_normalized &&
          method.throws_declaration_profile_is_normalized;
      result_profiles_normalized =
          result_profiles_normalized && method.result_like_profile_is_normalized;
      ns_error_profiles_normalized =
          ns_error_profiles_normalized &&
          method.ns_error_bridging_profile_is_normalized;
      summary.result_like_sites += method.result_like_sites;
      summary.result_success_sites += method.result_success_sites;
      summary.result_failure_sites += method.result_failure_sites;
      summary.result_branch_sites += method.result_branch_sites;
      summary.result_payload_sites += method.result_payload_sites;
      summary.ns_error_bridging_sites += method.ns_error_bridging_sites;
      summary.ns_error_out_parameter_sites +=
          method.ns_error_out_parameter_sites;
      summary.ns_error_bridge_path_sites += method.ns_error_bridge_path_sites;
      summary.objc_nserror_attribute_sites +=
          method.objc_nserror_attribute_sites;
      summary.objc_status_code_attribute_sites +=
          method.objc_status_code_attribute_sites;
      summary.status_code_success_clause_sites +=
          method.status_code_success_clause_sites;
      summary.status_code_error_type_clause_sites +=
          method.status_code_error_type_clause_sites;
      summary.status_code_mapping_clause_sites +=
          method.status_code_mapping_clause_sites;
      throws_profiles_normalized =
          throws_profiles_normalized &&
          method.error_bridge_marker_profile_is_normalized;
    }
  }

  summary.throws_declaration_source_supported = true;
  summary.result_carrier_source_supported = true;
  summary.ns_error_bridging_source_supported = true;
  summary.error_bridge_marker_source_supported = true;
  summary.try_keyword_reserved = true;
  summary.throw_keyword_reserved = true;
  summary.catch_keyword_reserved = true;
  summary.try_fail_closed = true;
  summary.throw_fail_closed = true;
  summary.do_catch_fail_closed = true;
  summary.deterministic_handoff =
      throws_profiles_normalized && result_profiles_normalized &&
      ns_error_profiles_normalized &&
      summary.status_code_success_clause_sites <=
          summary.objc_status_code_attribute_sites &&
      summary.status_code_error_type_clause_sites <=
          summary.objc_status_code_attribute_sites &&
      summary.status_code_mapping_clause_sites <=
          summary.objc_status_code_attribute_sites &&
      summary.result_success_sites + summary.result_failure_sites <=
          summary.result_like_sites &&
      summary.ns_error_bridge_path_sites <=
          summary.ns_error_out_parameter_sites;
  summary.ready_for_semantic_expansion = summary.deterministic_handoff;
  summary.replay_key = BuildErrorHandlingErrorSourceClosureReplayKey(summary);
  return summary;
}

Objc3FrontendConcurrencyAsyncSourceClosureSummary
BuildConcurrencyAsyncSourceClosureSummary(const Objc3Program &program,
                                    const std::vector<Objc3LexToken> &tokens) {
  Objc3FrontendConcurrencyAsyncSourceClosureSummary summary;
  bool async_profiles_normalized = true;
  bool await_profiles_normalized = true;

  for (const auto &token : tokens) {
    if (token.kind == Objc3LexTokenKind::KwAsync) {
      ++summary.async_keyword_sites;
    } else if (token.kind == Objc3LexTokenKind::KwAwait) {
      ++summary.await_keyword_sites;
    }
  }

  for (const auto &fn : program.functions) {
    if (fn.async_declared) {
      ++summary.async_function_sites;
    }
    if (fn.executor_affinity_declared) {
      ++summary.executor_attribute_sites;
      if (fn.executor_affinity_kind == "main") {
        ++summary.executor_main_sites;
      } else if (fn.executor_affinity_kind == "global") {
        ++summary.executor_global_sites;
      } else if (fn.executor_affinity_named) {
        ++summary.executor_named_sites;
      }
    }
    async_profiles_normalized =
        async_profiles_normalized && fn.async_continuation_profile_is_normalized;
    await_profiles_normalized =
        await_profiles_normalized && fn.await_suspension_profile_is_normalized;
    for (const auto &stmt : fn.body) {
      CollectConcurrencyAsyncSourceClosureStmtSites(stmt.get(), summary);
    }
  }

  for (const auto &interface_decl : program.interfaces) {
    for (const auto &method : interface_decl.methods) {
      if (method.async_declared) {
        ++summary.async_method_sites;
      }
      if (method.executor_affinity_declared) {
        ++summary.executor_attribute_sites;
        if (method.executor_affinity_kind == "main") {
          ++summary.executor_main_sites;
        } else if (method.executor_affinity_kind == "global") {
          ++summary.executor_global_sites;
        } else if (method.executor_affinity_named) {
          ++summary.executor_named_sites;
        }
      }
      async_profiles_normalized =
          async_profiles_normalized &&
          method.async_continuation_profile_is_normalized;
      await_profiles_normalized =
          await_profiles_normalized &&
          method.await_suspension_profile_is_normalized;
    }
  }

  for (const auto &implementation : program.implementations) {
    for (const auto &method : implementation.methods) {
      if (method.async_declared) {
        ++summary.async_method_sites;
      }
      if (method.executor_affinity_declared) {
        ++summary.executor_attribute_sites;
        if (method.executor_affinity_kind == "main") {
          ++summary.executor_main_sites;
        } else if (method.executor_affinity_kind == "global") {
          ++summary.executor_global_sites;
        } else if (method.executor_affinity_named) {
          ++summary.executor_named_sites;
        }
      }
      async_profiles_normalized =
          async_profiles_normalized &&
          method.async_continuation_profile_is_normalized;
      await_profiles_normalized =
          await_profiles_normalized &&
          method.await_suspension_profile_is_normalized;
      for (const auto &stmt : method.body) {
        CollectConcurrencyAsyncSourceClosureStmtSites(stmt.get(), summary);
      }
    }
  }

  summary.async_function_source_supported = true;
  summary.async_method_source_supported = true;
  summary.await_expression_source_supported = true;
  summary.executor_attribute_source_supported = true;
  summary.deterministic_handoff =
      async_profiles_normalized && await_profiles_normalized &&
      summary.async_function_sites <= summary.async_keyword_sites &&
      summary.await_expression_sites <= summary.await_keyword_sites &&
      summary.executor_main_sites + summary.executor_global_sites +
              summary.executor_named_sites <=
          summary.executor_attribute_sites;
  summary.ready_for_semantic_expansion = summary.deterministic_handoff;
  summary.replay_key = BuildConcurrencyAsyncSourceClosureReplayKey(summary);
  return summary;
}

Objc3FrontendConcurrencyActorMemberIsolationSourceClosureSummary
BuildConcurrencyActorMemberIsolationSourceClosureSummary(const Objc3Program &program) {
  Objc3FrontendConcurrencyActorMemberIsolationSourceClosureSummary summary;

  for (const auto &interface_decl : program.interfaces) {
    if (!interface_decl.is_actor) {
      continue;
    }
    ++summary.actor_interface_sites;
    summary.actor_property_sites += interface_decl.properties.size();
    summary.actor_method_sites += interface_decl.methods.size();
    summary.actor_member_metadata_sites +=
        interface_decl.properties.size() + interface_decl.methods.size();
    for (const auto &method : interface_decl.methods) {
      if (method.objc_nonisolated_declared) {
        ++summary.objc_nonisolated_annotation_sites;
      }
      if (method.executor_affinity_declared) {
        ++summary.actor_member_executor_annotation_sites;
      }
      if (method.async_declared) {
        ++summary.actor_async_method_sites;
      }
    }
  }

  summary.actor_declaration_source_supported = true;
  summary.actor_member_source_supported = true;
  summary.isolation_annotation_source_supported = true;
  summary.actor_metadata_surface_supported = true;
  summary.deterministic_handoff =
      summary.objc_nonisolated_annotation_sites <= summary.actor_method_sites &&
      summary.actor_member_executor_annotation_sites <= summary.actor_method_sites &&
      summary.actor_async_method_sites <= summary.actor_method_sites &&
      summary.actor_member_metadata_sites ==
          summary.actor_method_sites + summary.actor_property_sites;
  summary.ready_for_semantic_expansion = summary.deterministic_handoff;
  summary.replay_key =
      BuildConcurrencyActorMemberIsolationSourceClosureReplayKey(summary);
  return summary;
}

Objc3FrontendConcurrencyTaskGroupCancellationSourceClosureSummary
BuildConcurrencyTaskGroupCancellationSourceClosureSummary(
    const Objc3Program &program) {
  Objc3FrontendConcurrencyTaskGroupCancellationSourceClosureSummary summary;

  for (const auto &fn : program.functions) {
    if (fn.async_declared) {
      ++summary.async_callable_sites;
    }
    if (fn.executor_affinity_declared) {
      ++summary.executor_attribute_sites;
    }
    for (const auto &stmt : fn.body) {
      CollectConcurrencyTaskGroupCancellationStmtSites(stmt.get(), summary);
    }
  }

  for (const auto &implementation : program.implementations) {
    for (const auto &method : implementation.methods) {
      if (method.async_declared) {
        ++summary.async_callable_sites;
      }
      if (method.executor_affinity_declared) {
        ++summary.executor_attribute_sites;
      }
      for (const auto &stmt : method.body) {
        CollectConcurrencyTaskGroupCancellationStmtSites(stmt.get(), summary);
      }
    }
  }

  summary.task_creation_source_supported = true;
  summary.task_group_source_supported = true;
  summary.cancellation_source_supported = true;
  summary.deterministic_handoff =
      summary.task_group_add_task_sites <=
          summary.task_group_scope_sites + summary.task_group_add_task_sites +
              summary.task_group_wait_next_sites +
              summary.task_group_cancel_all_sites &&
      summary.task_group_wait_next_sites <=
          summary.task_group_scope_sites + summary.task_group_add_task_sites +
              summary.task_group_wait_next_sites +
              summary.task_group_cancel_all_sites &&
      summary.task_group_cancel_all_sites <=
          summary.task_group_scope_sites + summary.task_group_add_task_sites +
              summary.task_group_wait_next_sites +
              summary.task_group_cancel_all_sites &&
      summary.cancellation_handler_sites <= summary.cancellation_check_sites + 1u;
  summary.ready_for_semantic_expansion = summary.deterministic_handoff;
  summary.replay_key =
      BuildConcurrencyTaskGroupCancellationSourceClosureReplayKey(summary);
  return summary;
}

static void CollectOwnershipSystemExtensionStmtSites(
    const Stmt *stmt,
    Objc3FrontendOwnershipSystemExtensionSourceClosureSummary &summary);
static void CollectOwnershipCleanupResourceCaptureStmtSites(
    const Stmt *stmt,
    Objc3FrontendOwnershipCleanupResourceCaptureSourceCompletionSummary &summary);

static void CollectOwnershipSystemExtensionExprSites(
    const Expr *expr,
    Objc3FrontendOwnershipSystemExtensionSourceClosureSummary &summary) {
  if (expr == nullptr) {
    return;
  }
  switch (expr->kind) {
  case Expr::Kind::BlockLiteral:
    if (expr->block_has_explicit_capture_list) {
      ++summary.explicit_capture_list_sites;
      summary.explicit_capture_item_sites += expr->block_explicit_capture_count;
      summary.explicit_capture_weak_sites += expr->block_explicit_capture_weak_count;
      summary.explicit_capture_unowned_sites += expr->block_explicit_capture_unowned_count;
      summary.explicit_capture_move_sites += expr->block_explicit_capture_move_count;
      summary.explicit_capture_plain_sites += expr->block_explicit_capture_plain_count;
    }
    for (const auto &stmt : expr->block_body) {
      if (stmt != nullptr) {
        CollectOwnershipSystemExtensionStmtSites(stmt.get(), summary);
      }
    }
    return;
  case Expr::Kind::Call:
  case Expr::Kind::MessageSend:
    CollectOwnershipSystemExtensionExprSites(expr->receiver.get(), summary);
    CollectOwnershipSystemExtensionExprSites(expr->left.get(), summary);
    CollectOwnershipSystemExtensionExprSites(expr->right.get(), summary);
    CollectOwnershipSystemExtensionExprSites(expr->third.get(), summary);
    for (const auto &arg : expr->args) {
      CollectOwnershipSystemExtensionExprSites(arg.get(), summary);
    }
    return;
  case Expr::Kind::Binary:
  case Expr::Kind::Conditional:
    CollectOwnershipSystemExtensionExprSites(expr->left.get(), summary);
    CollectOwnershipSystemExtensionExprSites(expr->right.get(), summary);
    CollectOwnershipSystemExtensionExprSites(expr->third.get(), summary);
    return;
  default:
    return;
  }
}

static void CollectOwnershipSystemExtensionStmtSites(
    const Stmt *stmt,
    Objc3FrontendOwnershipSystemExtensionSourceClosureSummary &summary) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
    if (stmt->let_stmt != nullptr) {
      if (stmt->let_stmt->resource_attribute_declared) {
        ++summary.resource_attribute_sites;
        if (!stmt->let_stmt->resource_close_symbol.empty()) {
          ++summary.resource_close_clause_sites;
        }
        if (!stmt->let_stmt->resource_invalid_expression.empty()) {
          ++summary.resource_invalid_clause_sites;
        }
      }
      CollectOwnershipSystemExtensionExprSites(stmt->let_stmt->value.get(), summary);
    }
    return;
  case Stmt::Kind::Assign:
    if (stmt->assign_stmt != nullptr) {
      CollectOwnershipSystemExtensionExprSites(stmt->assign_stmt->value.get(), summary);
    }
    return;
  case Stmt::Kind::Return:
    if (stmt->return_stmt != nullptr) {
      CollectOwnershipSystemExtensionExprSites(stmt->return_stmt->value.get(), summary);
    }
    return;
  case Stmt::Kind::If:
    if (stmt->if_stmt != nullptr) {
      CollectOwnershipSystemExtensionExprSites(stmt->if_stmt->condition.get(), summary);
      for (const auto &body_stmt : stmt->if_stmt->then_body) {
        CollectOwnershipSystemExtensionStmtSites(body_stmt.get(), summary);
      }
      for (const auto &body_stmt : stmt->if_stmt->else_body) {
        CollectOwnershipSystemExtensionStmtSites(body_stmt.get(), summary);
      }
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt != nullptr) {
      for (const auto &body_stmt : stmt->do_while_stmt->body) {
        CollectOwnershipSystemExtensionStmtSites(body_stmt.get(), summary);
      }
      CollectOwnershipSystemExtensionExprSites(stmt->do_while_stmt->condition.get(), summary);
    }
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt != nullptr) {
      CollectOwnershipSystemExtensionExprSites(stmt->for_stmt->init.value.get(), summary);
      CollectOwnershipSystemExtensionExprSites(stmt->for_stmt->condition.get(), summary);
      CollectOwnershipSystemExtensionExprSites(stmt->for_stmt->step.value.get(), summary);
      for (const auto &body_stmt : stmt->for_stmt->body) {
        CollectOwnershipSystemExtensionStmtSites(body_stmt.get(), summary);
      }
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt != nullptr) {
      CollectOwnershipSystemExtensionExprSites(stmt->switch_stmt->condition.get(), summary);
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        for (const auto &body_stmt : switch_case.body) {
          CollectOwnershipSystemExtensionStmtSites(body_stmt.get(), summary);
        }
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt != nullptr) {
      CollectOwnershipSystemExtensionExprSites(stmt->while_stmt->condition.get(), summary);
      for (const auto &body_stmt : stmt->while_stmt->body) {
        CollectOwnershipSystemExtensionStmtSites(body_stmt.get(), summary);
      }
    }
    return;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    if (stmt->block_stmt != nullptr) {
      for (const auto &body_stmt : stmt->block_stmt->body) {
        CollectOwnershipSystemExtensionStmtSites(body_stmt.get(), summary);
      }
    }
    return;
  case Stmt::Kind::Expr:
    if (stmt->expr_stmt != nullptr) {
      CollectOwnershipSystemExtensionExprSites(stmt->expr_stmt->value.get(), summary);
    }
    return;
  default:
    return;
  }
}

Objc3FrontendOwnershipSystemExtensionSourceClosureSummary
BuildOwnershipSystemExtensionSourceClosureSummary(const Objc3Program &program) {
  Objc3FrontendOwnershipSystemExtensionSourceClosureSummary summary;

  auto count_borrowed_in_params = [&summary](const std::vector<FuncParam> &params) {
    for (const auto &param : params) {
      if (param.borrowed_pointer_qualified) {
        ++summary.borrowed_pointer_sites;
      }
    }
  };

  for (const auto &fn : program.functions) {
    count_borrowed_in_params(fn.params);
    if (fn.return_borrowed_pointer_qualified) {
      ++summary.borrowed_pointer_sites;
    }
    if (fn.objc_returns_borrowed_declared) {
      ++summary.returns_borrowed_attribute_sites;
    }
    for (const auto &stmt : fn.body) {
      CollectOwnershipSystemExtensionStmtSites(stmt.get(), summary);
    }
  }

  auto count_method = [&summary, &count_borrowed_in_params](const Objc3MethodDecl &method) {
    count_borrowed_in_params(method.params);
    if (method.return_borrowed_pointer_qualified) {
      ++summary.borrowed_pointer_sites;
    }
    if (method.objc_returns_borrowed_declared) {
      ++summary.returns_borrowed_attribute_sites;
    }
  };

  for (const auto &interface_decl : program.interfaces) {
    for (const auto &method : interface_decl.methods) {
      count_method(method);
    }
  }
  for (const auto &implementation : program.implementations) {
    for (const auto &method : implementation.methods) {
      count_method(method);
      for (const auto &stmt : method.body) {
        CollectOwnershipSystemExtensionStmtSites(stmt.get(), summary);
      }
    }
  }

  summary.resource_attribute_source_supported = true;
  summary.borrowed_pointer_source_supported = true;
  summary.returns_borrowed_source_supported = true;
  summary.explicit_capture_list_source_supported = true;
  summary.deterministic_handoff =
      summary.resource_close_clause_sites <= summary.resource_attribute_sites &&
      summary.resource_invalid_clause_sites <= summary.resource_attribute_sites &&
      summary.explicit_capture_weak_sites + summary.explicit_capture_unowned_sites +
              summary.explicit_capture_move_sites + summary.explicit_capture_plain_sites <=
          summary.explicit_capture_item_sites;
  summary.ready_for_semantic_expansion = summary.deterministic_handoff;
  summary.replay_key = BuildOwnershipSystemExtensionSourceClosureReplayKey(summary);
  return summary;
}

static void CollectOwnershipCleanupResourceCaptureExprSites(
    const Expr *expr,
    Objc3FrontendOwnershipCleanupResourceCaptureSourceCompletionSummary &summary) {
  if (expr == nullptr) {
    return;
  }
  switch (expr->kind) {
  case Expr::Kind::BlockLiteral:
    if (expr->block_has_explicit_capture_list) {
      ++summary.explicit_capture_list_sites;
      summary.explicit_capture_item_sites += expr->block_explicit_capture_count;
      summary.explicit_capture_weak_sites += expr->block_explicit_capture_weak_count;
      summary.explicit_capture_unowned_sites += expr->block_explicit_capture_unowned_count;
      summary.explicit_capture_move_sites += expr->block_explicit_capture_move_count;
      summary.explicit_capture_plain_sites += expr->block_explicit_capture_plain_count;
    }
    for (const auto &stmt : expr->block_body) {
      if (stmt != nullptr) {
        CollectOwnershipCleanupResourceCaptureStmtSites(stmt.get(), summary);
      }
    }
    return;
  case Expr::Kind::Call:
  case Expr::Kind::MessageSend:
    CollectOwnershipCleanupResourceCaptureExprSites(expr->receiver.get(), summary);
    CollectOwnershipCleanupResourceCaptureExprSites(expr->left.get(), summary);
    CollectOwnershipCleanupResourceCaptureExprSites(expr->right.get(), summary);
    CollectOwnershipCleanupResourceCaptureExprSites(expr->third.get(), summary);
    for (const auto &arg : expr->args) {
      CollectOwnershipCleanupResourceCaptureExprSites(arg.get(), summary);
    }
    return;
  case Expr::Kind::Binary:
  case Expr::Kind::Conditional:
    CollectOwnershipCleanupResourceCaptureExprSites(expr->left.get(), summary);
    CollectOwnershipCleanupResourceCaptureExprSites(expr->right.get(), summary);
    CollectOwnershipCleanupResourceCaptureExprSites(expr->third.get(), summary);
    return;
  default:
    return;
  }
}

static void CollectOwnershipCleanupResourceCaptureStmtSites(
    const Stmt *stmt,
    Objc3FrontendOwnershipCleanupResourceCaptureSourceCompletionSummary &summary) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
    if (stmt->let_stmt != nullptr) {
      if (stmt->let_stmt->cleanup_attribute_declared) {
        ++summary.cleanup_attribute_sites;
        if (stmt->let_stmt->cleanup_sugar_declared) {
          ++summary.cleanup_sugar_sites;
        }
      }
      if (stmt->let_stmt->resource_attribute_declared) {
        ++summary.resource_attribute_sites;
        if (stmt->let_stmt->resource_sugar_declared) {
          ++summary.resource_sugar_sites;
        }
        if (!stmt->let_stmt->resource_close_symbol.empty()) {
          ++summary.resource_close_clause_sites;
        }
        if (!stmt->let_stmt->resource_invalid_expression.empty()) {
          ++summary.resource_invalid_clause_sites;
        }
      }
      CollectOwnershipCleanupResourceCaptureExprSites(stmt->let_stmt->value.get(),
                                                  summary);
    }
    return;
  case Stmt::Kind::Assign:
    if (stmt->assign_stmt != nullptr) {
      CollectOwnershipCleanupResourceCaptureExprSites(stmt->assign_stmt->value.get(),
                                                  summary);
    }
    return;
  case Stmt::Kind::Return:
    if (stmt->return_stmt != nullptr) {
      CollectOwnershipCleanupResourceCaptureExprSites(stmt->return_stmt->value.get(),
                                                  summary);
    }
    return;
  case Stmt::Kind::If:
    if (stmt->if_stmt != nullptr) {
      CollectOwnershipCleanupResourceCaptureExprSites(stmt->if_stmt->condition.get(),
                                                  summary);
      for (const auto &body_stmt : stmt->if_stmt->then_body) {
        CollectOwnershipCleanupResourceCaptureStmtSites(body_stmt.get(), summary);
      }
      for (const auto &body_stmt : stmt->if_stmt->else_body) {
        CollectOwnershipCleanupResourceCaptureStmtSites(body_stmt.get(), summary);
      }
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt != nullptr) {
      for (const auto &body_stmt : stmt->do_while_stmt->body) {
        CollectOwnershipCleanupResourceCaptureStmtSites(body_stmt.get(), summary);
      }
      CollectOwnershipCleanupResourceCaptureExprSites(
          stmt->do_while_stmt->condition.get(), summary);
    }
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt != nullptr) {
      CollectOwnershipCleanupResourceCaptureExprSites(stmt->for_stmt->init.value.get(),
                                                  summary);
      CollectOwnershipCleanupResourceCaptureExprSites(
          stmt->for_stmt->condition.get(), summary);
      CollectOwnershipCleanupResourceCaptureExprSites(stmt->for_stmt->step.value.get(),
                                                  summary);
      for (const auto &body_stmt : stmt->for_stmt->body) {
        CollectOwnershipCleanupResourceCaptureStmtSites(body_stmt.get(), summary);
      }
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt != nullptr) {
      CollectOwnershipCleanupResourceCaptureExprSites(
          stmt->switch_stmt->condition.get(), summary);
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        for (const auto &body_stmt : switch_case.body) {
          CollectOwnershipCleanupResourceCaptureStmtSites(body_stmt.get(), summary);
        }
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt != nullptr) {
      CollectOwnershipCleanupResourceCaptureExprSites(
          stmt->while_stmt->condition.get(), summary);
      for (const auto &body_stmt : stmt->while_stmt->body) {
        CollectOwnershipCleanupResourceCaptureStmtSites(body_stmt.get(), summary);
      }
    }
    return;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    if (stmt->block_stmt != nullptr) {
      for (const auto &body_stmt : stmt->block_stmt->body) {
        CollectOwnershipCleanupResourceCaptureStmtSites(body_stmt.get(), summary);
      }
    }
    return;
  case Stmt::Kind::Expr:
    if (stmt->expr_stmt != nullptr) {
      CollectOwnershipCleanupResourceCaptureExprSites(stmt->expr_stmt->value.get(),
                                                  summary);
    }
    return;
  default:
    return;
  }
}

Objc3FrontendOwnershipCleanupResourceCaptureSourceCompletionSummary
BuildOwnershipCleanupResourceCaptureSourceCompletionSummary(
    const Objc3Program &program) {
  Objc3FrontendOwnershipCleanupResourceCaptureSourceCompletionSummary summary;

  for (const auto &fn : program.functions) {
    for (const auto &stmt : fn.body) {
      CollectOwnershipCleanupResourceCaptureStmtSites(stmt.get(), summary);
    }
  }
  for (const auto &implementation : program.implementations) {
    for (const auto &method : implementation.methods) {
      for (const auto &stmt : method.body) {
        CollectOwnershipCleanupResourceCaptureStmtSites(stmt.get(), summary);
      }
    }
  }

  summary.cleanup_attribute_source_supported = true;
  summary.resource_sugar_source_supported = true;
  summary.explicit_capture_list_source_supported = true;
  summary.deterministic_handoff =
      summary.cleanup_sugar_sites <= summary.cleanup_attribute_sites &&
      summary.resource_sugar_sites <= summary.resource_attribute_sites &&
      summary.resource_close_clause_sites <= summary.resource_attribute_sites &&
      summary.resource_invalid_clause_sites <= summary.resource_attribute_sites &&
      summary.explicit_capture_weak_sites + summary.explicit_capture_unowned_sites +
              summary.explicit_capture_move_sites + summary.explicit_capture_plain_sites <=
          summary.explicit_capture_item_sites;
  summary.ready_for_semantic_expansion = summary.deterministic_handoff;
  summary.replay_key =
      BuildOwnershipCleanupResourceCaptureSourceCompletionReplayKey(summary);
  return summary;
}

Objc3FrontendOwnershipRetainableCFamilySourceCompletionSummary
BuildOwnershipRetainableCFamilySourceCompletionSummary(
    const Objc3Program &program) {
  Objc3FrontendOwnershipRetainableCFamilySourceCompletionSummary summary;

  const auto accumulate_callable =
      [&summary](const auto &decl) {
        for (const std::string &attribute_name :
             decl.retainable_c_family_callable_attributes) {
          if (attribute_name == "objc_family_retain") {
            ++summary.family_retain_sites;
          } else if (attribute_name == "objc_family_release") {
            ++summary.family_release_sites;
          } else if (attribute_name == "objc_family_autorelease") {
            ++summary.family_autorelease_sites;
          } else if (attribute_name == "os_returns_retained" ||
                     attribute_name == "cf_returns_retained" ||
                     attribute_name == "ns_returns_retained") {
            ++summary.compatibility_returns_retained_sites;
          } else if (attribute_name == "os_returns_not_retained" ||
                     attribute_name == "cf_returns_not_retained" ||
                     attribute_name == "ns_returns_not_retained") {
            ++summary.compatibility_returns_not_retained_sites;
          } else if (attribute_name == "os_consumed" ||
                     attribute_name == "cf_consumed" ||
                     attribute_name == "ns_consumed") {
            ++summary.compatibility_consumed_sites;
          }
        }
      };

  for (const auto &fn : program.functions) {
    accumulate_callable(fn);
  }
  for (const auto &interface_decl : program.interfaces) {
    for (const auto &method : interface_decl.methods) {
      accumulate_callable(method);
    }
  }
  for (const auto &implementation : program.implementations) {
    for (const auto &method : implementation.methods) {
      accumulate_callable(method);
    }
  }

  summary.callable_annotation_source_supported = true;
  summary.compatibility_alias_source_supported = true;
  summary.deterministic_handoff = true;
  summary.ready_for_semantic_expansion = true;
  summary.replay_key =
      BuildOwnershipRetainableCFamilySourceCompletionReplayKey(summary);
  return summary;
}

Objc3FrontendDispatchDispatchIntentSourceClosureSummary
BuildDispatchDispatchIntentSourceClosureSummary(const Objc3Program &program) {
  Objc3FrontendDispatchDispatchIntentSourceClosureSummary summary;

  const auto accumulate_callable = [&summary](const auto &decl) {
    if (decl.objc_direct_declared) {
      ++summary.direct_callable_sites;
    }
    if (decl.objc_final_declared) {
      ++summary.final_callable_sites;
    }
    if (decl.objc_dynamic_declared) {
      ++summary.dynamic_callable_sites;
    }
  };

  for (const auto &fn : program.functions) {
    accumulate_callable(fn);
  }
  for (const auto &interface_decl : program.interfaces) {
    if (interface_decl.objc_direct_members_declared) {
      ++summary.direct_members_container_sites;
    }
    if (interface_decl.objc_final_declared) {
      ++summary.final_container_sites;
    }
    if (interface_decl.objc_sealed_declared) {
      ++summary.sealed_container_sites;
    }
    if (interface_decl.is_actor) {
      ++summary.actor_container_sites;
    }
    for (const auto &method : interface_decl.methods) {
      accumulate_callable(method);
    }
  }
  for (const auto &implementation : program.implementations) {
    for (const auto &method : implementation.methods) {
      accumulate_callable(method);
    }
  }

  summary.callable_annotation_source_supported = true;
  summary.container_annotation_source_supported = true;
  summary.deterministic_handoff = true;
  summary.ready_for_semantic_expansion = true;
  summary.replay_key = BuildDispatchDispatchIntentSourceClosureReplayKey(summary);
  return summary;
}

Objc3FrontendDispatchDispatchIntentSourceCompletionSummary
BuildDispatchDispatchIntentSourceCompletionSummary(const Objc3Program &program) {
  Objc3FrontendDispatchDispatchIntentSourceCompletionSummary summary;

  std::unordered_map<std::string, bool> direct_members_by_container;
  for (const auto &interface_decl : program.interfaces) {
    if (interface_decl.prefixed_dispatch_control_attributes_declared) {
      ++summary.prefixed_container_attribute_sites;
    }
    if (interface_decl.objc_direct_members_declared) {
      ++summary.direct_members_container_sites;
    }
    if (interface_decl.objc_final_declared) {
      ++summary.final_container_sites;
    }
    if (interface_decl.objc_sealed_declared) {
      ++summary.sealed_container_sites;
    }
    direct_members_by_container.emplace(interface_decl.name,
                                        interface_decl.objc_direct_members_declared);

    for (const auto &method : interface_decl.methods) {
      const bool effective_direct =
          method.objc_direct_declared ||
          (interface_decl.objc_direct_members_declared &&
           !method.objc_dynamic_declared);
      if (effective_direct) {
        ++summary.effective_direct_member_sites;
      }
      if (interface_decl.objc_direct_members_declared &&
          !method.objc_direct_declared && !method.objc_dynamic_declared) {
        ++summary.direct_members_defaulted_method_sites;
      }
      if (interface_decl.objc_direct_members_declared &&
          method.objc_dynamic_declared) {
        ++summary.direct_members_dynamic_opt_out_sites;
      }
    }
  }

  for (const auto &implementation : program.implementations) {
    if (implementation.has_category) {
      continue;
    }
    const auto found = direct_members_by_container.find(implementation.name);
    const bool direct_members_enabled =
        found != direct_members_by_container.end() && found->second;
    for (const auto &method : implementation.methods) {
      const bool effective_direct =
          method.objc_direct_declared ||
          (direct_members_enabled && !method.objc_dynamic_declared);
      if (effective_direct) {
        ++summary.effective_direct_member_sites;
      }
      if (direct_members_enabled && !method.objc_direct_declared &&
          !method.objc_dynamic_declared) {
        ++summary.direct_members_defaulted_method_sites;
      }
      if (direct_members_enabled && method.objc_dynamic_declared) {
        ++summary.direct_members_dynamic_opt_out_sites;
      }
    }
  }

  summary.prefixed_attribute_source_supported = true;
  summary.defaulting_source_supported = true;
  summary.deterministic_handoff =
      summary.direct_members_defaulted_method_sites +
              summary.direct_members_dynamic_opt_out_sites <=
          summary.effective_direct_member_sites +
              summary.direct_members_dynamic_opt_out_sites;
  summary.ready_for_semantic_expansion = summary.deterministic_handoff;
  summary.replay_key =
      BuildDispatchDispatchIntentSourceCompletionReplayKey(summary);
  return summary;
}

Objc3FrontendMetaprogrammingMetaprogrammingSourceClosureSummary
BuildMetaprogrammingMetaprogrammingSourceClosureSummary(const Objc3Program &program) {
  Objc3FrontendMetaprogrammingMetaprogrammingSourceClosureSummary summary;

  for (const auto &fn : program.functions) {
    if (fn.objc_macro_declared) {
      ++summary.macro_marker_sites;
    }
  }
  for (const auto &interface_decl : program.interfaces) {
    if (interface_decl.objc_derive_declared) {
      ++summary.derive_marker_sites;
    }
    for (const auto &property : interface_decl.properties) {
      if (property.property_behavior_declared) {
        ++summary.property_behavior_sites;
      }
    }
    for (const auto &method : interface_decl.methods) {
      if (method.objc_macro_declared) {
        ++summary.macro_marker_sites;
      }
    }
  }
  for (const auto &protocol_decl : program.protocols) {
    for (const auto &property : protocol_decl.properties) {
      if (property.property_behavior_declared) {
        ++summary.property_behavior_sites;
      }
    }
    for (const auto &method : protocol_decl.methods) {
      if (method.objc_macro_declared) {
        ++summary.macro_marker_sites;
      }
    }
  }
  for (const auto &implementation : program.implementations) {
    for (const auto &property : implementation.properties) {
      if (property.property_behavior_declared) {
        ++summary.property_behavior_sites;
      }
    }
    for (const auto &method : implementation.methods) {
      if (method.objc_macro_declared) {
        ++summary.macro_marker_sites;
      }
    }
  }

  summary.derive_marker_source_supported = true;
  summary.macro_marker_source_supported = true;
  summary.property_behavior_source_supported = true;
  summary.deterministic_handoff = true;
  summary.ready_for_semantic_expansion = true;
  summary.replay_key =
      BuildMetaprogrammingMetaprogrammingSourceClosureReplayKey(summary);
  return summary;
}

Objc3FrontendMetaprogrammingMacroPackageProvenanceSourceCompletionSummary
BuildMetaprogrammingMacroPackageProvenanceSourceCompletionSummary(
    const Objc3Program &program) {
  Objc3FrontendMetaprogrammingMacroPackageProvenanceSourceCompletionSummary summary;

  const auto accumulate_callable = [&summary](const auto &decl) {
    if (decl.objc_macro_declared) {
      ++summary.macro_marker_sites;
    }
    if (decl.objc_macro_package_declared) {
      ++summary.macro_package_sites;
    }
    if (decl.objc_macro_provenance_declared) {
      ++summary.macro_provenance_sites;
    }
    if (decl.objc_macro_cache_key_declared) {
      ++summary.macro_cache_key_sites;
    }
    if (decl.objc_macro_sandbox_declared) {
      ++summary.macro_sandbox_policy_sites;
    }
    if (decl.objc_macro_declared && decl.objc_macro_package_declared &&
        decl.objc_macro_provenance_declared) {
      ++summary.expansion_visible_macro_sites;
    }
  };

  for (const auto &fn : program.functions) {
    accumulate_callable(fn);
  }
  for (const auto &interface_decl : program.interfaces) {
    for (const auto &method : interface_decl.methods) {
      accumulate_callable(method);
    }
  }
  for (const auto &protocol_decl : program.protocols) {
    for (const auto &method : protocol_decl.methods) {
      accumulate_callable(method);
    }
  }
  for (const auto &implementation : program.implementations) {
    for (const auto &method : implementation.methods) {
      accumulate_callable(method);
    }
  }

  summary.macro_package_source_supported = true;
  summary.macro_provenance_source_supported = true;
  summary.macro_cache_key_source_supported = true;
  summary.macro_sandbox_policy_source_supported = true;
  summary.expansion_visible_source_supported = true;
  summary.deterministic_handoff =
      summary.expansion_visible_macro_sites <= summary.macro_marker_sites &&
      summary.expansion_visible_macro_sites <= summary.macro_package_sites &&
      summary.expansion_visible_macro_sites <= summary.macro_provenance_sites;
  summary.ready_for_semantic_expansion = summary.deterministic_handoff;
  summary.replay_key =
      BuildMetaprogrammingMacroPackageProvenanceSourceCompletionReplayKey(summary);
  return summary;
}

Objc3FrontendMetaprogrammingPropertyBehaviorSourceCompletionSummary
BuildMetaprogrammingPropertyBehaviorSourceCompletionSummary(const Objc3Program &program) {
  Objc3FrontendMetaprogrammingPropertyBehaviorSourceCompletionSummary summary;

  const auto accumulate_property = [&summary](const Objc3PropertyDecl &property,
                                              const char *owner_kind) {
    if (!property.property_behavior_declared) {
      return;
    }
    ++summary.property_behavior_sites;
    const std::string kind(owner_kind);
    if (kind == "interface") {
      ++summary.interface_property_behavior_sites;
    } else if (kind == "implementation") {
      ++summary.implementation_property_behavior_sites;
    } else if (kind == "protocol") {
      ++summary.protocol_property_behavior_sites;
    }
    if (property.executable_synthesized_binding_kind == "implicit-ivar" &&
        !property.executable_synthesized_binding_symbol.empty()) {
      ++summary.synthesized_binding_visible_sites;
    }
    if (!property.effective_getter_selector.empty()) {
      ++summary.synthesized_getter_visible_sites;
    }
    if (property.effective_setter_available &&
        !property.effective_setter_selector.empty()) {
      ++summary.synthesized_setter_visible_sites;
    }
  };

  for (const auto &interface_decl : program.interfaces) {
    for (const auto &property : interface_decl.properties) {
      accumulate_property(property, "interface");
    }
  }
  for (const auto &protocol_decl : program.protocols) {
    for (const auto &property : protocol_decl.properties) {
      accumulate_property(property, "protocol");
    }
  }
  for (const auto &implementation : program.implementations) {
    for (const auto &property : implementation.properties) {
      accumulate_property(property, "implementation");
    }
  }

  summary.property_behavior_source_supported = true;
  summary.synthesized_declaration_visibility_supported = true;
  summary.deterministic_handoff =
      summary.interface_property_behavior_sites +
              summary.implementation_property_behavior_sites +
              summary.protocol_property_behavior_sites ==
          summary.property_behavior_sites;
  summary.ready_for_semantic_expansion = summary.deterministic_handoff;
  summary.replay_key =
      BuildMetaprogrammingPropertyBehaviorSourceCompletionReplayKey(summary);
  return summary;
}

Objc3FrontendInteropForeignImportSourceClosureSummary
BuildInteropForeignImportSourceClosureSummary(const Objc3Program &program) {
  Objc3FrontendInteropForeignImportSourceClosureSummary summary;

  const auto accumulate_callable = [&summary](const auto &decl) {
    if (decl.objc_foreign_declared) {
      ++summary.foreign_callable_sites;
      ++summary.interop_annotation_sites;
      if constexpr (requires { decl.is_prototype; }) {
        if (decl.is_prototype) {
          ++summary.extern_foreign_callable_sites;
        }
      } else if constexpr (requires { decl.has_body; }) {
        if (!decl.has_body) {
          ++summary.extern_foreign_callable_sites;
        }
      }
    }
    if (decl.objc_import_module_declared) {
      ++summary.import_module_annotation_sites;
      ++summary.interop_annotation_sites;
      if (!decl.objc_import_module_name.empty()) {
        ++summary.imported_module_name_sites;
      }
    }
    if (decl.objc_export_header_declared) {
      ++summary.export_header_annotation_sites;
      ++summary.interop_annotation_sites;
      if (!decl.objc_export_header_name.empty()) {
        ++summary.export_header_name_sites;
      }
    }
    if (decl.objc_mixed_image_declared) {
      ++summary.mixed_image_annotation_sites;
      ++summary.interop_annotation_sites;
      if (!decl.objc_mixed_image_name.empty()) {
        ++summary.mixed_image_name_sites;
      }
    }
    if (decl.objc_package_entry_declared) {
      ++summary.package_entry_annotation_sites;
      ++summary.interop_annotation_sites;
      if (!decl.objc_package_entry_name.empty()) {
        ++summary.package_entry_name_sites;
      }
    }
  };

  for (const auto &fn : program.functions) {
    accumulate_callable(fn);
  }
  for (const auto &interface_decl : program.interfaces) {
    for (const auto &method : interface_decl.methods) {
      accumulate_callable(method);
    }
  }
  for (const auto &protocol_decl : program.protocols) {
    for (const auto &method : protocol_decl.methods) {
      accumulate_callable(method);
    }
  }
  for (const auto &implementation : program.implementations) {
    for (const auto &method : implementation.methods) {
      accumulate_callable(method);
    }
  }

  summary.foreign_declaration_source_supported = true;
  summary.imported_surface_source_supported = true;
  summary.interop_annotation_source_supported = true;
  summary.deterministic_handoff =
      summary.extern_foreign_callable_sites <= summary.foreign_callable_sites &&
      summary.imported_module_name_sites <= summary.import_module_annotation_sites &&
      summary.export_header_name_sites <= summary.export_header_annotation_sites &&
      summary.mixed_image_name_sites <= summary.mixed_image_annotation_sites &&
      summary.package_entry_name_sites <= summary.package_entry_annotation_sites &&
      summary.interop_annotation_sites ==
          summary.foreign_callable_sites + summary.import_module_annotation_sites +
              summary.export_header_annotation_sites +
              summary.mixed_image_annotation_sites +
              summary.package_entry_annotation_sites;
  summary.ready_for_semantic_expansion = summary.deterministic_handoff;
  summary.replay_key = BuildInteropForeignImportSourceClosureReplayKey(summary);
  return summary;
}

Objc3FrontendInteropCppSwiftInteropAnnotationSourceCompletionSummary
BuildInteropCppSwiftInteropAnnotationSourceCompletionSummary(
    const Objc3Program &program) {
  Objc3FrontendInteropCppSwiftInteropAnnotationSourceCompletionSummary summary;

  const auto accumulate_callable = [&summary](const auto &decl) {
    if (decl.objc_swift_name_declared) {
      ++summary.swift_name_annotation_sites;
      ++summary.interop_metadata_annotation_sites;
      if (!decl.objc_swift_name.empty()) {
        ++summary.named_annotation_payload_sites;
      }
    }
    if (decl.objc_swift_private_declared) {
      ++summary.swift_private_annotation_sites;
      ++summary.interop_metadata_annotation_sites;
    }
    if (decl.objc_cxx_name_declared) {
      ++summary.cpp_name_annotation_sites;
      ++summary.interop_metadata_annotation_sites;
      if (!decl.objc_cxx_name.empty()) {
        ++summary.named_annotation_payload_sites;
      }
    }
    if (decl.objc_header_name_declared) {
      ++summary.header_name_annotation_sites;
      ++summary.interop_metadata_annotation_sites;
      if (!decl.objc_header_name.empty()) {
        ++summary.named_annotation_payload_sites;
      }
    }
    if (decl.objc_abi_align_declared) {
      ++summary.abi_alignment_annotation_sites;
      ++summary.interop_metadata_annotation_sites;
    }
    if (decl.objc_foreign_type_declared) {
      ++summary.foreign_type_annotation_sites;
      ++summary.interop_metadata_annotation_sites;
      if (!decl.objc_foreign_type_name.empty()) {
        ++summary.named_annotation_payload_sites;
      }
    }
  };

  for (const auto &fn : program.functions) {
    accumulate_callable(fn);
  }
  for (const auto &interface_decl : program.interfaces) {
    for (const auto &method : interface_decl.methods) {
      accumulate_callable(method);
    }
  }
  for (const auto &protocol_decl : program.protocols) {
    for (const auto &method : protocol_decl.methods) {
      accumulate_callable(method);
    }
  }
  for (const auto &implementation : program.implementations) {
    for (const auto &method : implementation.methods) {
      accumulate_callable(method);
    }
  }

  summary.swift_annotation_source_supported = true;
  summary.cpp_annotation_source_supported = true;
  summary.interop_metadata_source_supported = true;
  summary.deterministic_handoff =
      summary.named_annotation_payload_sites ==
          summary.swift_name_annotation_sites +
              summary.cpp_name_annotation_sites +
              summary.header_name_annotation_sites +
              summary.foreign_type_annotation_sites &&
      summary.interop_metadata_annotation_sites ==
          summary.swift_name_annotation_sites +
              summary.swift_private_annotation_sites +
              summary.cpp_name_annotation_sites +
              summary.header_name_annotation_sites +
              summary.abi_alignment_annotation_sites +
              summary.foreign_type_annotation_sites;
  summary.ready_for_semantic_expansion = summary.deterministic_handoff;
  summary.replay_key =
      BuildInteropCppSwiftInteropAnnotationSourceCompletionReplayKey(summary);
  return summary;
}

Objc3FrontendToolingDiagnosticsMigratorSourceInventorySummary
BuildToolingDiagnosticsMigratorSourceInventorySummary(
    const Objc3FrontendMigrationHints &migration_hints,
    const Objc3FrontendErrorHandlingErrorSourceClosureSummary &error_handling_summary,
    const Objc3FrontendConcurrencyAsyncSourceClosureSummary &concurrency_async_summary,
    const Objc3FrontendConcurrencyActorMemberIsolationSourceClosureSummary
        &concurrency_actor_summary,
    const Objc3FrontendConcurrencyTaskGroupCancellationSourceClosureSummary
        &concurrency_task_summary,
    const Objc3FrontendOwnershipSystemExtensionSourceClosureSummary &ownership_summary,
    const Objc3FrontendOwnershipCleanupResourceCaptureSourceCompletionSummary
        &ownership_cleanup_summary,
    const Objc3FrontendOwnershipRetainableCFamilySourceCompletionSummary
        &ownership_family_summary,
    const Objc3FrontendDispatchDispatchIntentSourceClosureSummary
        &dispatch_closure_summary,
    const Objc3FrontendDispatchDispatchIntentSourceCompletionSummary
        &dispatch_completion_summary,
    const Objc3FrontendMetaprogrammingMetaprogrammingSourceClosureSummary
        &metaprogramming_closure_summary,
    const Objc3FrontendMetaprogrammingMacroPackageProvenanceSourceCompletionSummary
        &metaprogramming_macro_summary,
    const Objc3FrontendMetaprogrammingPropertyBehaviorSourceCompletionSummary
        &metaprogramming_property_summary,
    const Objc3FrontendInteropForeignImportSourceClosureSummary
        &interop_closure_summary,
    const Objc3FrontendInteropCppSwiftInteropAnnotationSourceCompletionSummary
        &interop_completion_summary) {
  Objc3FrontendToolingDiagnosticsMigratorSourceInventorySummary summary;

  summary.advanced_feature_family_count = 6;
  summary.dependency_surface_count = summary.dependency_contract_ids.size();
  summary.aggregated_source_only_claim_count =
      error_handling_summary.source_only_claim_ids.size() +
      concurrency_async_summary.source_only_claim_ids.size() +
      concurrency_actor_summary.source_only_claim_ids.size() +
      concurrency_task_summary.source_only_claim_ids.size() +
      ownership_summary.source_only_claim_ids.size() +
      ownership_cleanup_summary.source_only_claim_ids.size() +
      ownership_family_summary.source_only_claim_ids.size() +
      dispatch_closure_summary.source_only_claim_ids.size() +
      dispatch_completion_summary.source_only_claim_ids.size() +
      metaprogramming_closure_summary.source_only_claim_ids.size() +
      metaprogramming_macro_summary.source_only_claim_ids.size() +
      metaprogramming_property_summary.source_only_claim_ids.size() +
      interop_closure_summary.source_only_claim_ids.size() +
      interop_completion_summary.source_only_claim_ids.size();
  summary.fail_closed_construct_count =
      error_handling_summary.fail_closed_construct_ids.size();
  summary.error_surface_sites =
      error_handling_summary.function_throws_declaration_sites +
      error_handling_summary.method_throws_declaration_sites +
      error_handling_summary.result_like_sites + error_handling_summary.result_payload_sites +
      error_handling_summary.ns_error_bridging_sites +
      error_handling_summary.ns_error_out_parameter_sites +
      error_handling_summary.ns_error_bridge_path_sites +
      error_handling_summary.objc_nserror_attribute_sites +
      error_handling_summary.objc_status_code_attribute_sites +
      error_handling_summary.try_keyword_sites + error_handling_summary.throw_keyword_sites +
      error_handling_summary.catch_keyword_sites;
  summary.concurrency_surface_sites =
      concurrency_async_summary.async_keyword_sites +
      concurrency_async_summary.await_keyword_sites +
      concurrency_async_summary.executor_attribute_sites +
      concurrency_actor_summary.actor_interface_sites +
      concurrency_actor_summary.actor_method_sites +
      concurrency_actor_summary.actor_property_sites +
      concurrency_actor_summary.objc_nonisolated_annotation_sites +
      concurrency_actor_summary.actor_member_executor_annotation_sites +
      concurrency_task_summary.task_creation_sites +
      concurrency_task_summary.task_group_scope_sites +
      concurrency_task_summary.task_group_add_task_sites +
      concurrency_task_summary.task_group_wait_next_sites +
      concurrency_task_summary.task_group_cancel_all_sites +
      concurrency_task_summary.cancellation_check_sites +
      concurrency_task_summary.cancellation_handler_sites;
  summary.system_surface_sites =
      ownership_summary.resource_attribute_sites +
      ownership_summary.resource_close_clause_sites +
      ownership_summary.borrowed_pointer_sites +
      ownership_summary.returns_borrowed_attribute_sites +
      ownership_summary.explicit_capture_list_sites +
      ownership_summary.explicit_capture_item_sites +
      ownership_cleanup_summary.cleanup_attribute_sites +
      ownership_cleanup_summary.cleanup_sugar_sites +
      ownership_cleanup_summary.resource_sugar_sites +
      ownership_family_summary.family_retain_sites +
      ownership_family_summary.family_release_sites +
      ownership_family_summary.family_autorelease_sites +
      ownership_family_summary.compatibility_returns_retained_sites +
      ownership_family_summary.compatibility_returns_not_retained_sites +
      ownership_family_summary.compatibility_consumed_sites;
  summary.dispatch_surface_sites =
      dispatch_closure_summary.direct_callable_sites +
      dispatch_closure_summary.final_callable_sites +
      dispatch_closure_summary.dynamic_callable_sites +
      dispatch_closure_summary.direct_members_container_sites +
      dispatch_closure_summary.final_container_sites +
      dispatch_closure_summary.sealed_container_sites +
      dispatch_completion_summary.prefixed_container_attribute_sites +
      dispatch_completion_summary.effective_direct_member_sites +
      dispatch_completion_summary.direct_members_defaulted_method_sites +
      dispatch_completion_summary.direct_members_dynamic_opt_out_sites;
