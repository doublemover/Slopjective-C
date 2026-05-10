static void DiagnoseConcurrencyExecutorAffinityCompletion(
    const Objc3ConcurrencyTaskCallableLegalityProfile &profile,
    bool async_declared, bool executor_affinity_declared,
    const std::string &executor_affinity_kind, int line, int column,
    std::vector<std::string> &diagnostics) {
  const std::size_t task_related_sites =
      profile.task_creation_sites + profile.task_group_scope_sites +
      profile.task_group_add_task_sites + profile.task_group_wait_next_sites +
      profile.task_group_cancel_all_sites + profile.cancellation_check_sites +
      profile.cancellation_handler_sites;
  if (!async_declared || task_related_sites == 0u) {
    return;
  }
  if (!executor_affinity_declared) {
    diagnostics.push_back(MakeDiag(
        line, column, "O3S231",
        "task semantics failed: async task callables require objc_executor affinity for deterministic executor hops"));
  }
  if (executor_affinity_declared && executor_affinity_kind == "main" &&
      profile.detached_task_creation_sites > 0u) {
    diagnostics.push_back(MakeDiag(
        line, column, "O3S232",
        "task semantics failed: detached task creation cannot use the main executor affinity"));
  }
}

static bool IsActorInterfaceOwner(
    const std::unordered_map<std::string, const Objc3InterfaceDecl *> &actors,
    const std::string &name) {
  return actors.find(name) != actors.end();
}

struct Objc3ConcurrencyActorMethodBodyProfile {
  std::size_t replay_proof_sites = 0;
  std::size_t race_guard_sites = 0;
  std::size_t task_handoff_sites = 0;
  std::size_t actor_isolation_sites = 0;
  std::size_t actor_hop_sites = 0;
  std::size_t non_sendable_crossing_sites = 0;
  std::size_t escaping_block_literal_sites = 0;
};

static void CollectConcurrencyActorMethodBodyProfileFromSymbol(
    const std::string &symbol, Objc3ConcurrencyActorMethodBodyProfile &profile) {
  if (symbol.empty()) {
    return;
  }
  const std::string lowered = objc3c::support::LowercaseAscii(symbol);
  if (lowered.find("replay") != std::string::npos ||
      lowered.find("proof") != std::string::npos) {
    ++profile.replay_proof_sites;
  }
  if (lowered.find("race") != std::string::npos ||
      lowered.find("raceguard") != std::string::npos ||
      lowered.find("race_guard") != std::string::npos ||
      lowered.find("lock") != std::string::npos) {
    ++profile.race_guard_sites;
  }
  if (lowered.find("handoff") != std::string::npos ||
      lowered.find("await") != std::string::npos ||
      lowered.find("task") != std::string::npos) {
    ++profile.task_handoff_sites;
  }
  if (lowered.find("actor") != std::string::npos ||
      lowered.find("isolation") != std::string::npos ||
      lowered.find("isolated") != std::string::npos) {
    ++profile.actor_isolation_sites;
  }
  if (lowered.find("hop_to") != std::string::npos ||
      lowered.find("enqueue") != std::string::npos ||
      lowered.find("executor") != std::string::npos) {
    ++profile.actor_hop_sites;
  }
  if (lowered.find("non_sendable") != std::string::npos ||
      lowered.find("unsafe_sendable") != std::string::npos ||
      lowered.find("cross_actor") != std::string::npos) {
    ++profile.non_sendable_crossing_sites;
  }
}

static void CollectConcurrencyActorMethodBodyProfileStmt(
    const Stmt *stmt, Objc3ConcurrencyActorMethodBodyProfile &profile);

static void CollectConcurrencyActorMethodBodyProfileExpr(
    const Expr *expr, Objc3ConcurrencyActorMethodBodyProfile &profile) {
  if (expr == nullptr) {
    return;
  }
  switch (expr->kind) {
  case Expr::Kind::Call:
    CollectConcurrencyActorMethodBodyProfileFromSymbol(expr->ident, profile);
    for (const auto &arg : expr->args) {
      CollectConcurrencyActorMethodBodyProfileExpr(arg.get(), profile);
    }
    return;
  case Expr::Kind::BlockLiteral:
    if (expr->block_storage_escape_to_heap ||
        expr->block_escape_shape_promotes_to_heap_candidate) {
      ++profile.escaping_block_literal_sites;
    }
    for (const auto &stmt : expr->block_body) {
      CollectConcurrencyActorMethodBodyProfileStmt(stmt.get(), profile);
    }
    return;
  case Expr::Kind::MessageSend:
    CollectConcurrencyActorMethodBodyProfileFromSymbol(expr->selector, profile);
    CollectConcurrencyActorMethodBodyProfileExpr(expr->receiver.get(), profile);
    for (const auto &arg : expr->args) {
      CollectConcurrencyActorMethodBodyProfileExpr(arg.get(), profile);
    }
    return;
  case Expr::Kind::Binary:
    CollectConcurrencyActorMethodBodyProfileExpr(expr->left.get(), profile);
    CollectConcurrencyActorMethodBodyProfileExpr(expr->right.get(), profile);
    return;
  case Expr::Kind::Conditional:
    CollectConcurrencyActorMethodBodyProfileExpr(expr->left.get(), profile);
    CollectConcurrencyActorMethodBodyProfileExpr(expr->right.get(), profile);
    CollectConcurrencyActorMethodBodyProfileExpr(expr->third.get(), profile);
    return;
  default:
    return;
  }
}

static void CollectConcurrencyActorMethodBodyProfileStmt(
    const Stmt *stmt, Objc3ConcurrencyActorMethodBodyProfile &profile) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
    if (stmt->let_stmt != nullptr) {
      CollectConcurrencyActorMethodBodyProfileExpr(stmt->let_stmt->value.get(),
                                             profile);
    }
    return;
  case Stmt::Kind::Assign:
    if (stmt->assign_stmt != nullptr) {
      CollectConcurrencyActorMethodBodyProfileExpr(stmt->assign_stmt->value.get(),
                                             profile);
    }
    return;
  case Stmt::Kind::Return:
    if (stmt->return_stmt != nullptr) {
      CollectConcurrencyActorMethodBodyProfileExpr(stmt->return_stmt->value.get(),
                                             profile);
    }
    return;
  case Stmt::Kind::If:
    if (stmt->if_stmt == nullptr) {
      return;
    }
    CollectConcurrencyActorMethodBodyProfileExpr(stmt->if_stmt->condition.get(),
                                           profile);
    for (const auto &then_stmt : stmt->if_stmt->then_body) {
      CollectConcurrencyActorMethodBodyProfileStmt(then_stmt.get(), profile);
    }
    for (const auto &else_stmt : stmt->if_stmt->else_body) {
      CollectConcurrencyActorMethodBodyProfileStmt(else_stmt.get(), profile);
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->do_while_stmt->body) {
      CollectConcurrencyActorMethodBodyProfileStmt(body_stmt.get(), profile);
    }
    CollectConcurrencyActorMethodBodyProfileExpr(
        stmt->do_while_stmt->condition.get(), profile);
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt == nullptr) {
      return;
    }
    CollectConcurrencyActorMethodBodyProfileExpr(stmt->for_stmt->init.value.get(),
                                           profile);
    CollectConcurrencyActorMethodBodyProfileExpr(stmt->for_stmt->condition.get(),
                                           profile);
    CollectConcurrencyActorMethodBodyProfileExpr(stmt->for_stmt->step.value.get(),
                                           profile);
    for (const auto &body_stmt : stmt->for_stmt->body) {
      CollectConcurrencyActorMethodBodyProfileStmt(body_stmt.get(), profile);
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt == nullptr) {
      return;
    }
    CollectConcurrencyActorMethodBodyProfileExpr(stmt->switch_stmt->condition.get(),
                                           profile);
    for (const auto &switch_case : stmt->switch_stmt->cases) {
      for (const auto &case_stmt : switch_case.body) {
        CollectConcurrencyActorMethodBodyProfileStmt(case_stmt.get(), profile);
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt == nullptr) {
      return;
    }
    CollectConcurrencyActorMethodBodyProfileExpr(stmt->while_stmt->condition.get(),
                                           profile);
    for (const auto &body_stmt : stmt->while_stmt->body) {
      CollectConcurrencyActorMethodBodyProfileStmt(body_stmt.get(), profile);
    }
    return;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    if (stmt->block_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->block_stmt->body) {
      CollectConcurrencyActorMethodBodyProfileStmt(body_stmt.get(), profile);
    }
    return;
  case Stmt::Kind::Expr:
    if (stmt->expr_stmt != nullptr) {
      CollectConcurrencyActorMethodBodyProfileExpr(stmt->expr_stmt->value.get(),
                                             profile);
    }
    return;
  default:
    return;
  }
}

static Objc3ConcurrencyActorMethodBodyProfile BuildConcurrencyActorMethodBodyProfile(
    const Objc3MethodDecl &method) {
  Objc3ConcurrencyActorMethodBodyProfile profile;
  for (const auto &stmt : method.body) {
    CollectConcurrencyActorMethodBodyProfileStmt(stmt.get(), profile);
  }
  if (profile.task_handoff_sites == 0u) {
    profile.replay_proof_sites = 0u;
    profile.race_guard_sites = 0u;
    profile.actor_isolation_sites = 0u;
  }
  return profile;
}

static void DiagnoseConcurrencyActorMethodIsolationRules(
    const Objc3MethodDecl &method, bool owner_is_actor, unsigned line,
    unsigned column, std::vector<std::string> &diagnostics) {
  if (method.objc_nonisolated_declared && !owner_is_actor) {
    diagnostics.push_back(MakeDiag(
        line, column, "O3S286",
        "actor isolation semantics failed: objc_nonisolated is only valid on actor methods"));
  }
  if (!owner_is_actor) {
    return;
  }
  const Objc3ConcurrencyActorMethodBodyProfile body_profile =
      BuildConcurrencyActorMethodBodyProfile(method);
  if (method.objc_nonisolated_declared && method.async_declared) {
    diagnostics.push_back(MakeDiag(
        line, column, "O3S287",
        "actor isolation semantics failed: objc_nonisolated actor methods cannot also be async"));
  }
  if (method.objc_nonisolated_declared && method.executor_affinity_declared) {
    diagnostics.push_back(MakeDiag(
        line, column, "O3S288",
        "actor isolation semantics failed: objc_nonisolated actor methods cannot declare objc_executor affinity"));
  }
  if (body_profile.actor_hop_sites > 0u && !method.async_declared) {
    diagnostics.push_back(MakeDiag(
        line, column, "O3S289",
        "actor isolation semantics failed: actor hop sites require an async actor method"));
  }
  if (body_profile.non_sendable_crossing_sites > 0u) {
    diagnostics.push_back(MakeDiag(
        line, column, "O3S290",
        "actor sendability semantics failed: non-sendable cross-actor crossings remain unsupported in actor methods"));
  }
}

static void DiagnoseConcurrencyActorRaceHazardRules(
    const Objc3MethodDecl &method, bool owner_is_actor, unsigned line,
    unsigned column, std::vector<std::string> &diagnostics) {
  if (!owner_is_actor) {
    return;
  }
  const Objc3ConcurrencyActorMethodBodyProfile body_profile =
      BuildConcurrencyActorMethodBodyProfile(method);
  if (body_profile.task_handoff_sites == 0u) {
    return;
  }
  if (body_profile.race_guard_sites == 0u) {
    diagnostics.push_back(MakeDiag(
        line, column, "O3S291",
        "actor race-hazard semantics failed: task handoff in actor methods requires a race guard site"));
  }
  if (body_profile.replay_proof_sites == 0u) {
    diagnostics.push_back(MakeDiag(
        line, column, "O3S292",
        "actor race-hazard semantics failed: task handoff in actor methods requires replay proof coverage"));
  }
  if (body_profile.actor_isolation_sites == 0u) {
    diagnostics.push_back(MakeDiag(
        line, column, "O3S293",
        "actor race-hazard semantics failed: task handoff in actor methods requires actor isolation coverage"));
  }
  if (body_profile.escaping_block_literal_sites > 0u) {
    diagnostics.push_back(MakeDiag(
        line, column, "O3S294",
        "actor escape semantics failed: escaping block literals remain unsupported in actor methods that perform task handoff"));
  }
}
