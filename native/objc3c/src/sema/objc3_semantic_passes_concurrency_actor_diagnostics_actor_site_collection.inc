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
