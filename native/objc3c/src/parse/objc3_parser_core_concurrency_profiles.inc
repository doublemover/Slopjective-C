// source-closure anchor: actor/isolation/sendable admission remains
// parser-owned symbol profiling on top of the existing token stream. This
// tranche does not claim dedicated actor/sendable/nonisolated grammar forms
// yet; it freezes the current source contract around actor-isolation
// declarations, hop markers, sendable markers, and non-sendable crossings.
static bool IsActorIsolationDeclSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("actor") != std::string::npos ||
         lowered.find("isolated") != std::string::npos ||
         lowered.find("isolation") != std::string::npos;
}

static bool IsActorHopSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("hop_to") != std::string::npos ||
         lowered.find("enqueue") != std::string::npos ||
         lowered.find("executor") != std::string::npos;
}

static bool IsSendableAnnotationSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("sendable") != std::string::npos ||
         lowered.find("sendability") != std::string::npos;
}

static bool IsNonSendableCrossingSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("non_sendable") != std::string::npos ||
         lowered.find("unsafe_sendable") != std::string::npos ||
         lowered.find("cross_actor") != std::string::npos;
}

struct Objc3ActorIsolationSendabilitySiteCounts {
  std::size_t actor_isolation_decl_sites = 0;
  std::size_t actor_hop_sites = 0;
  std::size_t sendable_annotation_sites = 0;
  std::size_t non_sendable_crossing_sites = 0;
};

static void CollectActorIsolationSendabilitySitesFromSymbol(
    const std::string &symbol,
    Objc3ActorIsolationSendabilitySiteCounts &counts) {
  if (IsActorIsolationDeclSymbol(symbol)) {
    counts.actor_isolation_decl_sites += 1u;
  }
  if (IsActorHopSymbol(symbol)) {
    counts.actor_hop_sites += 1u;
  }
  if (IsSendableAnnotationSymbol(symbol)) {
    counts.sendable_annotation_sites += 1u;
  }
  if (IsNonSendableCrossingSymbol(symbol)) {
    counts.non_sendable_crossing_sites += 1u;
  }
}

static void CollectActorIsolationSendabilityExprSites(
    const Expr *expr,
    Objc3ActorIsolationSendabilitySiteCounts &counts) {
  if (expr == nullptr) {
    return;
  }
  switch (expr->kind) {
  case Expr::Kind::Call:
    CollectActorIsolationSendabilitySitesFromSymbol(expr->ident, counts);
    for (const auto &arg : expr->args) {
      CollectActorIsolationSendabilityExprSites(arg.get(), counts);
    }
    return;
  case Expr::Kind::MessageSend:
    CollectActorIsolationSendabilitySitesFromSymbol(expr->selector, counts);
    CollectActorIsolationSendabilityExprSites(expr->receiver.get(), counts);
    for (const auto &arg : expr->args) {
      CollectActorIsolationSendabilityExprSites(arg.get(), counts);
    }
    return;
  case Expr::Kind::Binary:
    CollectActorIsolationSendabilityExprSites(expr->left.get(), counts);
    CollectActorIsolationSendabilityExprSites(expr->right.get(), counts);
    return;
  case Expr::Kind::Conditional:
    CollectActorIsolationSendabilityExprSites(expr->left.get(), counts);
    CollectActorIsolationSendabilityExprSites(expr->right.get(), counts);
    CollectActorIsolationSendabilityExprSites(expr->third.get(), counts);
    return;
  case Expr::Kind::BlockLiteral:
  case Expr::Kind::BoolLiteral:
  case Expr::Kind::Identifier:
  case Expr::Kind::NilLiteral:
  case Expr::Kind::Number:
  default:
    return;
  }
}

static void CollectActorIsolationSendabilityForClauseSites(
    const ForClause &clause,
    Objc3ActorIsolationSendabilitySiteCounts &counts) {
  CollectActorIsolationSendabilityExprSites(clause.value.get(), counts);
}

static void CollectActorIsolationSendabilityStmtSites(
    const Stmt *stmt,
    Objc3ActorIsolationSendabilitySiteCounts &counts) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
    if (stmt->let_stmt != nullptr) {
      CollectActorIsolationSendabilityExprSites(stmt->let_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::Assign:
    if (stmt->assign_stmt != nullptr) {
      CollectActorIsolationSendabilityExprSites(stmt->assign_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::Return:
    if (stmt->return_stmt != nullptr) {
      CollectActorIsolationSendabilityExprSites(stmt->return_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::If:
    if (stmt->if_stmt == nullptr) {
      return;
    }
    CollectActorIsolationSendabilityExprSites(stmt->if_stmt->condition.get(), counts);
    for (const auto &then_stmt : stmt->if_stmt->then_body) {
      CollectActorIsolationSendabilityStmtSites(then_stmt.get(), counts);
    }
    for (const auto &else_stmt : stmt->if_stmt->else_body) {
      CollectActorIsolationSendabilityStmtSites(else_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->do_while_stmt->body) {
      CollectActorIsolationSendabilityStmtSites(body_stmt.get(), counts);
    }
    CollectActorIsolationSendabilityExprSites(
        stmt->do_while_stmt->condition.get(),
        counts);
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt == nullptr) {
      return;
    }
    CollectActorIsolationSendabilityForClauseSites(stmt->for_stmt->init, counts);
    CollectActorIsolationSendabilityExprSites(stmt->for_stmt->condition.get(), counts);
    CollectActorIsolationSendabilityForClauseSites(stmt->for_stmt->step, counts);
    for (const auto &body_stmt : stmt->for_stmt->body) {
      CollectActorIsolationSendabilityStmtSites(body_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt == nullptr) {
      return;
    }
    CollectActorIsolationSendabilityExprSites(stmt->switch_stmt->condition.get(), counts);
    for (const auto &switch_case : stmt->switch_stmt->cases) {
      for (const auto &case_stmt : switch_case.body) {
        CollectActorIsolationSendabilityStmtSites(case_stmt.get(), counts);
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt == nullptr) {
      return;
    }
    CollectActorIsolationSendabilityExprSites(stmt->while_stmt->condition.get(), counts);
    for (const auto &body_stmt : stmt->while_stmt->body) {
      CollectActorIsolationSendabilityStmtSites(body_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    if (stmt->block_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->block_stmt->body) {
      CollectActorIsolationSendabilityStmtSites(body_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::Expr:
    if (stmt->expr_stmt != nullptr) {
      CollectActorIsolationSendabilityExprSites(stmt->expr_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  }
}

static Objc3ActorIsolationSendabilitySiteCounts CountActorIsolationSendabilitySitesInBody(
    const std::vector<std::unique_ptr<Stmt>> &body) {
  Objc3ActorIsolationSendabilitySiteCounts counts;
  for (const auto &stmt : body) {
    CollectActorIsolationSendabilityStmtSites(stmt.get(), counts);
  }
  return counts;
}

struct Objc3ActorIsolationSendabilityProfile {
  std::size_t actor_isolation_sendability_sites = 0;
  std::size_t actor_isolation_decl_sites = 0;
  std::size_t actor_hop_sites = 0;
  std::size_t sendable_annotation_sites = 0;
  std::size_t non_sendable_crossing_sites = 0;
  std::size_t isolation_boundary_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t gate_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic_actor_isolation_sendability_handoff = false;
};

static std::string BuildActorIsolationSendabilityProfile(
    std::size_t actor_isolation_sendability_sites,
    std::size_t actor_isolation_decl_sites,
    std::size_t actor_hop_sites,
    std::size_t sendable_annotation_sites,
    std::size_t non_sendable_crossing_sites,
    std::size_t isolation_boundary_sites,
    std::size_t normalized_sites,
    std::size_t gate_blocked_sites,
    std::size_t contract_violation_sites,
    bool deterministic_actor_isolation_sendability_handoff) {
  std::ostringstream out;
  out << "actor-isolation-sendability:actor_isolation_sendability_sites="
      << actor_isolation_sendability_sites
      << ";actor_isolation_decl_sites=" << actor_isolation_decl_sites
      << ";actor_hop_sites=" << actor_hop_sites
      << ";sendable_annotation_sites=" << sendable_annotation_sites
      << ";non_sendable_crossing_sites=" << non_sendable_crossing_sites
      << ";isolation_boundary_sites=" << isolation_boundary_sites
      << ";normalized_sites=" << normalized_sites
      << ";gate_blocked_sites=" << gate_blocked_sites
      << ";contract_violation_sites=" << contract_violation_sites
      << ";deterministic_actor_isolation_sendability_handoff="
      << (deterministic_actor_isolation_sendability_handoff ? "true" : "false");
  return out.str();
}

static bool IsActorIsolationSendabilityProfileNormalized(
    std::size_t actor_isolation_sendability_sites,
    std::size_t actor_isolation_decl_sites,
    std::size_t actor_hop_sites,
    std::size_t sendable_annotation_sites,
    std::size_t non_sendable_crossing_sites,
    std::size_t isolation_boundary_sites,
    std::size_t normalized_sites,
    std::size_t gate_blocked_sites,
    std::size_t contract_violation_sites) {
  if (actor_isolation_decl_sites > actor_isolation_sendability_sites ||
      actor_hop_sites > actor_isolation_sendability_sites ||
      sendable_annotation_sites > actor_isolation_sendability_sites ||
      non_sendable_crossing_sites > actor_isolation_sendability_sites ||
      isolation_boundary_sites > actor_isolation_sendability_sites ||
      normalized_sites > actor_isolation_sendability_sites ||
      gate_blocked_sites > actor_isolation_sendability_sites ||
      contract_violation_sites > actor_isolation_sendability_sites) {
    return false;
  }
  if (normalized_sites + gate_blocked_sites != actor_isolation_sendability_sites) {
    return false;
  }
  return contract_violation_sites == 0u;
}

static Objc3ActorIsolationSendabilityProfile BuildActorIsolationSendabilityProfileFromCounts(
    std::size_t actor_isolation_decl_sites,
    std::size_t actor_hop_sites,
    std::size_t sendable_annotation_sites,
    std::size_t non_sendable_crossing_sites) {
  Objc3ActorIsolationSendabilityProfile profile;
  profile.actor_isolation_decl_sites = actor_isolation_decl_sites;
  profile.actor_hop_sites = actor_hop_sites;
  profile.sendable_annotation_sites = sendable_annotation_sites;
  profile.non_sendable_crossing_sites = non_sendable_crossing_sites;
  profile.actor_isolation_sendability_sites = profile.actor_isolation_decl_sites;
  if (profile.actor_isolation_sendability_sites >
      std::numeric_limits<std::size_t>::max() - profile.actor_hop_sites) {
    profile.actor_isolation_sendability_sites = std::numeric_limits<std::size_t>::max();
    profile.contract_violation_sites += 1u;
  } else {
    profile.actor_isolation_sendability_sites += profile.actor_hop_sites;
  }
  if (profile.actor_isolation_sendability_sites >
      std::numeric_limits<std::size_t>::max() - profile.sendable_annotation_sites) {
    profile.actor_isolation_sendability_sites = std::numeric_limits<std::size_t>::max();
    profile.contract_violation_sites += 1u;
  } else {
    profile.actor_isolation_sendability_sites += profile.sendable_annotation_sites;
  }
  profile.isolation_boundary_sites =
      std::min(profile.actor_isolation_decl_sites, profile.actor_hop_sites);
  profile.gate_blocked_sites =
      std::min(profile.actor_isolation_sendability_sites, profile.non_sendable_crossing_sites);
  profile.normalized_sites =
      profile.actor_isolation_sendability_sites - profile.gate_blocked_sites;
  if (profile.actor_isolation_decl_sites > profile.actor_isolation_sendability_sites ||
      profile.actor_hop_sites > profile.actor_isolation_sendability_sites ||
      profile.sendable_annotation_sites > profile.actor_isolation_sendability_sites ||
      profile.non_sendable_crossing_sites > profile.actor_isolation_sendability_sites ||
      profile.isolation_boundary_sites > profile.actor_isolation_sendability_sites ||
      profile.normalized_sites > profile.actor_isolation_sendability_sites ||
      profile.gate_blocked_sites > profile.actor_isolation_sendability_sites ||
      profile.contract_violation_sites > profile.actor_isolation_sendability_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.normalized_sites + profile.gate_blocked_sites !=
      profile.actor_isolation_sendability_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.contract_violation_sites > profile.actor_isolation_sendability_sites) {
    profile.contract_violation_sites = profile.actor_isolation_sendability_sites;
  }
  profile.deterministic_actor_isolation_sendability_handoff =
      profile.contract_violation_sites == 0u;
  return profile;
}

static Objc3ActorIsolationSendabilityProfile BuildActorIsolationSendabilityProfileFromFunction(
    const FunctionDecl &fn) {
  const Objc3ActorIsolationSendabilitySiteCounts counts =
      CountActorIsolationSendabilitySitesInBody(fn.body);
  return BuildActorIsolationSendabilityProfileFromCounts(
      counts.actor_isolation_decl_sites,
      counts.actor_hop_sites,
      counts.sendable_annotation_sites,
      counts.non_sendable_crossing_sites);
}

static Objc3ActorIsolationSendabilityProfile BuildActorIsolationSendabilityProfileFromOpaqueBody(
    const Objc3MethodDecl &method) {
  Objc3ActorIsolationSendabilitySiteCounts counts;
  if (method.has_body) {
    CollectActorIsolationSendabilitySitesFromSymbol(method.selector, counts);
  }
  return BuildActorIsolationSendabilityProfileFromCounts(
      counts.actor_isolation_decl_sites,
      counts.actor_hop_sites,
      counts.sendable_annotation_sites,
      counts.non_sendable_crossing_sites);
}

static bool IsTaskRuntimeHookSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  // source-closure anchor: task/executor source ownership remains a
  // deterministic parser-side symbol-profile contract instead of dedicated
  // contextual keywords. Later runtime lanes may widen runnable semantics, but
  // they must continue to preserve this identifier-driven source boundary.
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("task_runtime") != std::string::npos ||
         lowered.find("task_group") != std::string::npos ||
         lowered.find("group_task") != std::string::npos ||
         lowered.find("task_spawn") != std::string::npos ||
         lowered.find("spawn_task") != std::string::npos ||
         lowered.find("detached_task") != std::string::npos ||
         lowered.find("task_detach") != std::string::npos ||
         lowered.find("runtime_task") != std::string::npos ||
         lowered.find("executor") != std::string::npos;
}

static bool IsCancellationCheckSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("cancel") != std::string::npos ||
         lowered.find("is_cancelled") != std::string::npos ||
         lowered.find("cancellation") != std::string::npos;
}

static bool IsCancellationHandlerSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("cancel_handler") != std::string::npos ||
         lowered.find("with_cancellation_handler") != std::string::npos ||
         lowered.find("on_cancel") != std::string::npos;
}

static bool IsSuspensionPointSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("await") != std::string::npos ||
         lowered.find("wait_next") != std::string::npos ||
         lowered.find("suspend") != std::string::npos ||
         lowered.find("yield") != std::string::npos;
}

struct Objc3TaskRuntimeCancellationSiteCounts {
  std::size_t runtime_hook_sites = 0;
  std::size_t cancellation_check_sites = 0;
  std::size_t cancellation_handler_sites = 0;
  std::size_t suspension_point_sites = 0;
};

static void CollectTaskRuntimeCancellationSitesFromSymbol(
    const std::string &symbol,
    Objc3TaskRuntimeCancellationSiteCounts &counts) {
  if (IsTaskRuntimeHookSymbol(symbol)) {
    counts.runtime_hook_sites += 1u;
  }
  if (IsCancellationCheckSymbol(symbol)) {
    counts.cancellation_check_sites += 1u;
  }
  if (IsCancellationHandlerSymbol(symbol)) {
    counts.cancellation_handler_sites += 1u;
  }
  if (IsSuspensionPointSymbol(symbol)) {
    counts.suspension_point_sites += 1u;
  }
}

static void CollectTaskRuntimeCancellationExprSites(
    const Expr *expr,
    Objc3TaskRuntimeCancellationSiteCounts &counts) {
  if (expr == nullptr) {
    return;
  }
  switch (expr->kind) {
  case Expr::Kind::Call:
    CollectTaskRuntimeCancellationSitesFromSymbol(expr->ident, counts);
    for (const auto &arg : expr->args) {
      CollectTaskRuntimeCancellationExprSites(arg.get(), counts);
    }
    return;
  case Expr::Kind::MessageSend:
    CollectTaskRuntimeCancellationSitesFromSymbol(expr->selector, counts);
    CollectTaskRuntimeCancellationExprSites(expr->receiver.get(), counts);
    for (const auto &arg : expr->args) {
      CollectTaskRuntimeCancellationExprSites(arg.get(), counts);
    }
    return;
  case Expr::Kind::Binary:
    CollectTaskRuntimeCancellationExprSites(expr->left.get(), counts);
    CollectTaskRuntimeCancellationExprSites(expr->right.get(), counts);
    return;
  case Expr::Kind::Conditional:
    CollectTaskRuntimeCancellationExprSites(expr->left.get(), counts);
    CollectTaskRuntimeCancellationExprSites(expr->right.get(), counts);
    CollectTaskRuntimeCancellationExprSites(expr->third.get(), counts);
    return;
  case Expr::Kind::BlockLiteral:
  case Expr::Kind::BoolLiteral:
  case Expr::Kind::Identifier:
  case Expr::Kind::NilLiteral:
  case Expr::Kind::Number:
  default:
    return;
  }
}

static void CollectTaskRuntimeCancellationForClauseSites(
    const ForClause &clause,
    Objc3TaskRuntimeCancellationSiteCounts &counts) {
  CollectTaskRuntimeCancellationExprSites(clause.value.get(), counts);
}

static void CollectTaskRuntimeCancellationStmtSites(
    const Stmt *stmt,
    Objc3TaskRuntimeCancellationSiteCounts &counts) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
    if (stmt->let_stmt != nullptr) {
      CollectTaskRuntimeCancellationExprSites(stmt->let_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::Assign:
    if (stmt->assign_stmt != nullptr) {
      CollectTaskRuntimeCancellationExprSites(stmt->assign_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::Return:
    if (stmt->return_stmt != nullptr) {
      CollectTaskRuntimeCancellationExprSites(stmt->return_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::If:
    if (stmt->if_stmt == nullptr) {
      return;
    }
    CollectTaskRuntimeCancellationExprSites(stmt->if_stmt->condition.get(), counts);
    for (const auto &then_stmt : stmt->if_stmt->then_body) {
      CollectTaskRuntimeCancellationStmtSites(then_stmt.get(), counts);
    }
    for (const auto &else_stmt : stmt->if_stmt->else_body) {
      CollectTaskRuntimeCancellationStmtSites(else_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->do_while_stmt->body) {
      CollectTaskRuntimeCancellationStmtSites(body_stmt.get(), counts);
    }
    CollectTaskRuntimeCancellationExprSites(
        stmt->do_while_stmt->condition.get(),
        counts);
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt == nullptr) {
      return;
    }
    CollectTaskRuntimeCancellationForClauseSites(stmt->for_stmt->init, counts);
    CollectTaskRuntimeCancellationExprSites(stmt->for_stmt->condition.get(), counts);
    CollectTaskRuntimeCancellationForClauseSites(stmt->for_stmt->step, counts);
    for (const auto &body_stmt : stmt->for_stmt->body) {
      CollectTaskRuntimeCancellationStmtSites(body_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt == nullptr) {
      return;
    }
    CollectTaskRuntimeCancellationExprSites(stmt->switch_stmt->condition.get(), counts);
    for (const auto &switch_case : stmt->switch_stmt->cases) {
      for (const auto &case_stmt : switch_case.body) {
        CollectTaskRuntimeCancellationStmtSites(case_stmt.get(), counts);
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt == nullptr) {
      return;
    }
    CollectTaskRuntimeCancellationExprSites(stmt->while_stmt->condition.get(), counts);
    for (const auto &body_stmt : stmt->while_stmt->body) {
      CollectTaskRuntimeCancellationStmtSites(body_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    if (stmt->block_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->block_stmt->body) {
      CollectTaskRuntimeCancellationStmtSites(body_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::Expr:
    if (stmt->expr_stmt != nullptr) {
      CollectTaskRuntimeCancellationExprSites(stmt->expr_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  }
}

static Objc3TaskRuntimeCancellationSiteCounts CountTaskRuntimeCancellationSitesInBody(
    const std::vector<std::unique_ptr<Stmt>> &body) {
  Objc3TaskRuntimeCancellationSiteCounts counts;
  for (const auto &stmt : body) {
    CollectTaskRuntimeCancellationStmtSites(stmt.get(), counts);
  }
  return counts;
}

struct Objc3TaskRuntimeCancellationProfile {
  std::size_t task_runtime_interop_sites = 0;
  std::size_t runtime_hook_sites = 0;
  std::size_t cancellation_check_sites = 0;
  std::size_t cancellation_handler_sites = 0;
  std::size_t suspension_point_sites = 0;
  std::size_t cancellation_propagation_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t gate_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic_task_runtime_cancellation_handoff = false;
};

static std::string BuildTaskRuntimeCancellationProfile(
    std::size_t task_runtime_interop_sites,
    std::size_t runtime_hook_sites,
    std::size_t cancellation_check_sites,
    std::size_t cancellation_handler_sites,
    std::size_t suspension_point_sites,
    std::size_t cancellation_propagation_sites,
    std::size_t normalized_sites,
    std::size_t gate_blocked_sites,
    std::size_t contract_violation_sites,
    bool deterministic_task_runtime_cancellation_handoff) {
  std::ostringstream out;
  out << "task-runtime-cancellation:task_runtime_interop_sites="
      << task_runtime_interop_sites
      << ";runtime_hook_sites=" << runtime_hook_sites
      << ";cancellation_check_sites=" << cancellation_check_sites
      << ";cancellation_handler_sites=" << cancellation_handler_sites
      << ";suspension_point_sites=" << suspension_point_sites
      << ";cancellation_propagation_sites=" << cancellation_propagation_sites
      << ";normalized_sites=" << normalized_sites
      << ";gate_blocked_sites=" << gate_blocked_sites
      << ";contract_violation_sites=" << contract_violation_sites
      << ";deterministic_task_runtime_cancellation_handoff="
      << (deterministic_task_runtime_cancellation_handoff ? "true" : "false");
  return out.str();
}

