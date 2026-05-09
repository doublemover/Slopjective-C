static bool IsTaskRuntimeCancellationProfileNormalized(
    std::size_t task_runtime_interop_sites,
    std::size_t runtime_hook_sites,
    std::size_t cancellation_check_sites,
    std::size_t cancellation_handler_sites,
    std::size_t suspension_point_sites,
    std::size_t cancellation_propagation_sites,
    std::size_t normalized_sites,
    std::size_t gate_blocked_sites,
    std::size_t contract_violation_sites) {
  if (runtime_hook_sites > task_runtime_interop_sites ||
      cancellation_check_sites > task_runtime_interop_sites ||
      cancellation_handler_sites > task_runtime_interop_sites ||
      suspension_point_sites > task_runtime_interop_sites ||
      cancellation_propagation_sites > cancellation_check_sites ||
      normalized_sites > task_runtime_interop_sites ||
      gate_blocked_sites > task_runtime_interop_sites ||
      contract_violation_sites > task_runtime_interop_sites) {
    return false;
  }
  if (normalized_sites + gate_blocked_sites != task_runtime_interop_sites) {
    return false;
  }
  return contract_violation_sites == 0u;
}

static Objc3TaskRuntimeCancellationProfile BuildTaskRuntimeCancellationProfileFromCounts(
    std::size_t runtime_hook_sites,
    std::size_t cancellation_check_sites,
    std::size_t cancellation_handler_sites,
    std::size_t suspension_point_sites) {
  Objc3TaskRuntimeCancellationProfile profile;
  profile.runtime_hook_sites = runtime_hook_sites;
  profile.cancellation_check_sites = cancellation_check_sites;
  profile.cancellation_handler_sites = cancellation_handler_sites;
  profile.suspension_point_sites = suspension_point_sites;
  profile.cancellation_propagation_sites =
      std::min(profile.cancellation_handler_sites, profile.cancellation_check_sites);
  profile.task_runtime_interop_sites = profile.runtime_hook_sites;
  if (profile.task_runtime_interop_sites >
      std::numeric_limits<std::size_t>::max() - profile.cancellation_check_sites) {
    profile.task_runtime_interop_sites = std::numeric_limits<std::size_t>::max();
    profile.contract_violation_sites += 1u;
  } else {
    profile.task_runtime_interop_sites += profile.cancellation_check_sites;
  }
  if (profile.task_runtime_interop_sites >
      std::numeric_limits<std::size_t>::max() - profile.suspension_point_sites) {
    profile.task_runtime_interop_sites = std::numeric_limits<std::size_t>::max();
    profile.contract_violation_sites += 1u;
  } else {
    profile.task_runtime_interop_sites += profile.suspension_point_sites;
  }
  profile.gate_blocked_sites = profile.cancellation_propagation_sites;
  if (profile.gate_blocked_sites > profile.task_runtime_interop_sites) {
    profile.gate_blocked_sites = profile.task_runtime_interop_sites;
    profile.contract_violation_sites += 1u;
  }
  profile.normalized_sites =
      profile.task_runtime_interop_sites - profile.gate_blocked_sites;
  if (profile.runtime_hook_sites > profile.task_runtime_interop_sites ||
      profile.cancellation_check_sites > profile.task_runtime_interop_sites ||
      profile.cancellation_handler_sites > profile.task_runtime_interop_sites ||
      profile.suspension_point_sites > profile.task_runtime_interop_sites ||
      profile.cancellation_propagation_sites > profile.cancellation_check_sites ||
      profile.normalized_sites > profile.task_runtime_interop_sites ||
      profile.gate_blocked_sites > profile.task_runtime_interop_sites ||
      profile.contract_violation_sites > profile.task_runtime_interop_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.normalized_sites + profile.gate_blocked_sites !=
      profile.task_runtime_interop_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.contract_violation_sites > profile.task_runtime_interop_sites) {
    profile.contract_violation_sites = profile.task_runtime_interop_sites;
  }
  profile.deterministic_task_runtime_cancellation_handoff =
      profile.contract_violation_sites == 0u;
  return profile;
}

static Objc3TaskRuntimeCancellationProfile BuildTaskRuntimeCancellationProfileFromFunction(
    const FunctionDecl &fn) {
  const Objc3TaskRuntimeCancellationSiteCounts counts =
      CountTaskRuntimeCancellationSitesInBody(fn.body);
  return BuildTaskRuntimeCancellationProfileFromCounts(
      counts.runtime_hook_sites,
      counts.cancellation_check_sites,
      counts.cancellation_handler_sites,
      counts.suspension_point_sites);
}

static Objc3TaskRuntimeCancellationProfile BuildTaskRuntimeCancellationProfileFromOpaqueBody(
    const Objc3MethodDecl &method) {
  Objc3TaskRuntimeCancellationSiteCounts counts;
  if (method.has_body) {
    CollectTaskRuntimeCancellationSitesFromSymbol(method.selector, counts);
  }
  return BuildTaskRuntimeCancellationProfileFromCounts(
      counts.runtime_hook_sites,
      counts.cancellation_check_sites,
      counts.cancellation_handler_sites,
      counts.suspension_point_sites);
}

static bool IsConcurrencyReplaySymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("replay") != std::string::npos ||
         lowered.find("resume") != std::string::npos ||
         lowered.find("retry") != std::string::npos;
}

static bool IsReplayProofSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("proof") != std::string::npos ||
         lowered.find("deterministic") != std::string::npos ||
         lowered.find("stable") != std::string::npos;
}

static bool IsRaceGuardSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("race") != std::string::npos ||
         lowered.find("raceguard") != std::string::npos ||
         lowered.find("race_guard") != std::string::npos ||
         lowered.find("replayguard") != std::string::npos ||
         lowered.find("replay_guard") != std::string::npos ||
         lowered.find("lock") != std::string::npos;
}

static bool IsTaskHandoffSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("handoff") != std::string::npos ||
         lowered.find("await") != std::string::npos ||
         lowered.find("task") != std::string::npos;
}

static bool IsActorIsolationSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("actor") != std::string::npos ||
         lowered.find("isolation") != std::string::npos ||
         lowered.find("isolated") != std::string::npos;
}

struct Objc3ConcurrencyReplayRaceGuardSiteCounts {
  std::size_t replay_proof_sites = 0;
  std::size_t race_guard_sites = 0;
  std::size_t task_handoff_sites = 0;
  std::size_t actor_isolation_sites = 0;
};

static void CollectConcurrencyReplayRaceGuardSitesFromSymbol(
    const std::string &symbol,
    Objc3ConcurrencyReplayRaceGuardSiteCounts &counts) {
  if (IsReplayProofSymbol(symbol)) {
    counts.replay_proof_sites += 1u;
  }
  if (IsRaceGuardSymbol(symbol)) {
    counts.race_guard_sites += 1u;
  }
  if (IsTaskHandoffSymbol(symbol)) {
    counts.task_handoff_sites += 1u;
  }
  if (IsActorIsolationSymbol(symbol)) {
    counts.actor_isolation_sites += 1u;
  }
  if (IsConcurrencyReplaySymbol(symbol) && counts.replay_proof_sites == 0u) {
    counts.replay_proof_sites += 1u;
  }
}

static void CollectConcurrencyReplayRaceGuardExprSites(
    const Expr *expr,
    Objc3ConcurrencyReplayRaceGuardSiteCounts &counts) {
  if (expr == nullptr) {
    return;
  }
  switch (expr->kind) {
  case Expr::Kind::Call:
    CollectConcurrencyReplayRaceGuardSitesFromSymbol(expr->ident, counts);
    for (const auto &arg : expr->args) {
      CollectConcurrencyReplayRaceGuardExprSites(arg.get(), counts);
    }
    return;
  case Expr::Kind::MessageSend:
    CollectConcurrencyReplayRaceGuardSitesFromSymbol(expr->selector, counts);
    CollectConcurrencyReplayRaceGuardExprSites(expr->receiver.get(), counts);
    for (const auto &arg : expr->args) {
      CollectConcurrencyReplayRaceGuardExprSites(arg.get(), counts);
    }
    return;
  case Expr::Kind::Binary:
    CollectConcurrencyReplayRaceGuardExprSites(expr->left.get(), counts);
    CollectConcurrencyReplayRaceGuardExprSites(expr->right.get(), counts);
    return;
  case Expr::Kind::Conditional:
    CollectConcurrencyReplayRaceGuardExprSites(expr->left.get(), counts);
    CollectConcurrencyReplayRaceGuardExprSites(expr->right.get(), counts);
    CollectConcurrencyReplayRaceGuardExprSites(expr->third.get(), counts);
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

static void CollectConcurrencyReplayRaceGuardForClauseSites(
    const ForClause &clause,
    Objc3ConcurrencyReplayRaceGuardSiteCounts &counts) {
  CollectConcurrencyReplayRaceGuardExprSites(clause.value.get(), counts);
}

static void CollectConcurrencyReplayRaceGuardStmtSites(
    const Stmt *stmt,
    Objc3ConcurrencyReplayRaceGuardSiteCounts &counts) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
    if (stmt->let_stmt != nullptr) {
      CollectConcurrencyReplayRaceGuardExprSites(stmt->let_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::Assign:
    if (stmt->assign_stmt != nullptr) {
      CollectConcurrencyReplayRaceGuardExprSites(stmt->assign_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::Return:
    if (stmt->return_stmt != nullptr) {
      CollectConcurrencyReplayRaceGuardExprSites(stmt->return_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::If:
    if (stmt->if_stmt == nullptr) {
      return;
    }
    CollectConcurrencyReplayRaceGuardExprSites(stmt->if_stmt->condition.get(), counts);
    for (const auto &then_stmt : stmt->if_stmt->then_body) {
      CollectConcurrencyReplayRaceGuardStmtSites(then_stmt.get(), counts);
    }
    for (const auto &else_stmt : stmt->if_stmt->else_body) {
      CollectConcurrencyReplayRaceGuardStmtSites(else_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->do_while_stmt->body) {
      CollectConcurrencyReplayRaceGuardStmtSites(body_stmt.get(), counts);
    }
    CollectConcurrencyReplayRaceGuardExprSites(
        stmt->do_while_stmt->condition.get(),
        counts);
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt == nullptr) {
      return;
    }
    CollectConcurrencyReplayRaceGuardForClauseSites(stmt->for_stmt->init, counts);
    CollectConcurrencyReplayRaceGuardExprSites(stmt->for_stmt->condition.get(), counts);
    CollectConcurrencyReplayRaceGuardForClauseSites(stmt->for_stmt->step, counts);
    for (const auto &body_stmt : stmt->for_stmt->body) {
      CollectConcurrencyReplayRaceGuardStmtSites(body_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt == nullptr) {
      return;
    }
    CollectConcurrencyReplayRaceGuardExprSites(stmt->switch_stmt->condition.get(), counts);
    for (const auto &switch_case : stmt->switch_stmt->cases) {
      for (const auto &case_stmt : switch_case.body) {
        CollectConcurrencyReplayRaceGuardStmtSites(case_stmt.get(), counts);
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt == nullptr) {
      return;
    }
    CollectConcurrencyReplayRaceGuardExprSites(stmt->while_stmt->condition.get(), counts);
    for (const auto &body_stmt : stmt->while_stmt->body) {
      CollectConcurrencyReplayRaceGuardStmtSites(body_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    if (stmt->block_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->block_stmt->body) {
      CollectConcurrencyReplayRaceGuardStmtSites(body_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::Expr:
    if (stmt->expr_stmt != nullptr) {
      CollectConcurrencyReplayRaceGuardExprSites(stmt->expr_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  }
}

static Objc3ConcurrencyReplayRaceGuardSiteCounts CountConcurrencyReplayRaceGuardSitesInBody(
    const std::vector<std::unique_ptr<Stmt>> &body) {
  Objc3ConcurrencyReplayRaceGuardSiteCounts counts;
  for (const auto &stmt : body) {
    CollectConcurrencyReplayRaceGuardStmtSites(stmt.get(), counts);
  }
  return counts;
}

struct Objc3ConcurrencyReplayRaceGuardProfile {
  std::size_t concurrency_replay_race_guard_sites = 0;
  std::size_t concurrency_replay_sites = 0;
  std::size_t replay_proof_sites = 0;
  std::size_t race_guard_sites = 0;
  std::size_t task_handoff_sites = 0;
  std::size_t actor_isolation_sites = 0;
  std::size_t deterministic_schedule_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic_concurrency_replay_race_guard_handoff = false;
};

static std::string BuildConcurrencyReplayRaceGuardProfile(
    std::size_t concurrency_replay_race_guard_sites,
    std::size_t concurrency_replay_sites,
    std::size_t replay_proof_sites,
    std::size_t race_guard_sites,
    std::size_t task_handoff_sites,
    std::size_t actor_isolation_sites,
    std::size_t deterministic_schedule_sites,
    std::size_t guard_blocked_sites,
    std::size_t contract_violation_sites,
    bool deterministic_concurrency_replay_race_guard_handoff) {
  std::ostringstream out;
  out << "concurrency-replay-race-guard:concurrency_replay_race_guard_sites="
      << concurrency_replay_race_guard_sites
      << ";concurrency_replay_sites=" << concurrency_replay_sites
      << ";replay_proof_sites=" << replay_proof_sites
      << ";race_guard_sites=" << race_guard_sites
      << ";task_handoff_sites=" << task_handoff_sites
      << ";actor_isolation_sites=" << actor_isolation_sites
      << ";deterministic_schedule_sites=" << deterministic_schedule_sites
      << ";guard_blocked_sites=" << guard_blocked_sites
      << ";contract_violation_sites=" << contract_violation_sites
      << ";deterministic_concurrency_replay_race_guard_handoff="
      << (deterministic_concurrency_replay_race_guard_handoff ? "true" : "false");
  return out.str();
}

static bool IsConcurrencyReplayRaceGuardProfileNormalized(
    std::size_t concurrency_replay_race_guard_sites,
    std::size_t concurrency_replay_sites,
    std::size_t replay_proof_sites,
    std::size_t race_guard_sites,
    std::size_t task_handoff_sites,
    std::size_t actor_isolation_sites,
    std::size_t deterministic_schedule_sites,
    std::size_t guard_blocked_sites,
    std::size_t contract_violation_sites) {
  if (concurrency_replay_race_guard_sites != concurrency_replay_sites ||
      replay_proof_sites > concurrency_replay_sites ||
      race_guard_sites > concurrency_replay_sites ||
      task_handoff_sites > concurrency_replay_sites ||
      actor_isolation_sites > concurrency_replay_sites ||
      deterministic_schedule_sites > concurrency_replay_sites ||
      guard_blocked_sites > concurrency_replay_sites ||
      contract_violation_sites > concurrency_replay_sites) {
    return false;
  }
  if (deterministic_schedule_sites + guard_blocked_sites != concurrency_replay_sites) {
    return false;
  }
  return contract_violation_sites == 0u;
}

static Objc3ConcurrencyReplayRaceGuardProfile BuildConcurrencyReplayRaceGuardProfileFromCounts(
    std::size_t replay_proof_sites,
    std::size_t race_guard_sites,
    std::size_t task_handoff_sites,
    std::size_t actor_isolation_sites) {
  Objc3ConcurrencyReplayRaceGuardProfile profile;
  profile.replay_proof_sites = replay_proof_sites;
  profile.race_guard_sites = race_guard_sites;
  profile.task_handoff_sites = task_handoff_sites;
  profile.actor_isolation_sites = actor_isolation_sites;
  profile.concurrency_replay_sites =
      std::max({profile.replay_proof_sites,
                profile.race_guard_sites,
                profile.task_handoff_sites,
                profile.actor_isolation_sites});
  profile.concurrency_replay_race_guard_sites = profile.concurrency_replay_sites;
  profile.guard_blocked_sites =
      std::min(profile.concurrency_replay_sites, profile.race_guard_sites / 2u);
  profile.deterministic_schedule_sites =
      profile.concurrency_replay_sites - profile.guard_blocked_sites;
  if (profile.concurrency_replay_race_guard_sites != profile.concurrency_replay_sites ||
      profile.replay_proof_sites > profile.concurrency_replay_sites ||
      profile.race_guard_sites > profile.concurrency_replay_sites ||
      profile.task_handoff_sites > profile.concurrency_replay_sites ||
      profile.actor_isolation_sites > profile.concurrency_replay_sites ||
      profile.deterministic_schedule_sites > profile.concurrency_replay_sites ||
      profile.guard_blocked_sites > profile.concurrency_replay_sites ||
      profile.contract_violation_sites > profile.concurrency_replay_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.deterministic_schedule_sites + profile.guard_blocked_sites !=
      profile.concurrency_replay_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.contract_violation_sites > profile.concurrency_replay_sites) {
    profile.contract_violation_sites = profile.concurrency_replay_sites;
  }
  profile.deterministic_concurrency_replay_race_guard_handoff =
      profile.contract_violation_sites == 0u;
  return profile;
}

static Objc3ConcurrencyReplayRaceGuardProfile BuildConcurrencyReplayRaceGuardProfileFromFunction(
    const FunctionDecl &fn) {
  const Objc3ConcurrencyReplayRaceGuardSiteCounts counts =
      CountConcurrencyReplayRaceGuardSitesInBody(fn.body);
  return BuildConcurrencyReplayRaceGuardProfileFromCounts(
      counts.replay_proof_sites,
      counts.race_guard_sites,
      counts.task_handoff_sites,
      counts.actor_isolation_sites);
}

static Objc3ConcurrencyReplayRaceGuardProfile BuildConcurrencyReplayRaceGuardProfileFromOpaqueBody(
    const Objc3MethodDecl &method) {
  Objc3ConcurrencyReplayRaceGuardSiteCounts counts;
  if (method.has_body) {
    CollectConcurrencyReplayRaceGuardSitesFromSymbol(method.selector, counts);
  }
  return BuildConcurrencyReplayRaceGuardProfileFromCounts(
      counts.replay_proof_sites,
      counts.race_guard_sites,
      counts.task_handoff_sites,
      counts.actor_isolation_sites);
}

static std::string BuildProtocolQualifiedObjectTypeProfile(
    bool object_pointer_type_spelling,
    bool has_generic_suffix,
    bool generic_suffix_terminated,
    bool has_pointer_declarator,
    const std::string &generic_suffix_text) {
  const bool protocol_composition_valid =
      !has_generic_suffix || (generic_suffix_terminated && object_pointer_type_spelling);
  std::ostringstream out;
  out << "protocol-qualified-object-type:object-pointer="
      << (object_pointer_type_spelling ? "true" : "false")
      << ";has-protocol-composition=" << (has_generic_suffix ? "true" : "false")
      << ";terminated=" << (generic_suffix_terminated ? "true" : "false")
      << ";pointer-declarator=" << (has_pointer_declarator ? "true" : "false")
      << ";composition-bytes=" << generic_suffix_text.size()
      << ";composition-valid=" << (protocol_composition_valid ? "true" : "false");
  return out.str();
}

static bool IsProtocolQualifiedObjectTypeProfileNormalized(
    bool object_pointer_type_spelling,
    bool has_generic_suffix,
    bool generic_suffix_terminated) {
  if (!has_generic_suffix) {
    return true;
  }
  return generic_suffix_terminated && object_pointer_type_spelling;
}

static bool IsOwnershipQualifierSpelling(const std::string &text) {
  return text == "__strong" || text == "__weak" || text == "__autoreleasing" ||
         text == "__unsafe_unretained";
}

static std::string BuildOwnershipQualifierSymbol(const std::string &spelling, bool is_return_type) {
  if (spelling.empty()) {
    return "";
  }
  return std::string(is_return_type ? "return-ownership-qualifier:" : "ownership-qualifier:") + spelling;
}

struct Objc3OwnershipOperationProfile {
  bool insert_retain = false;
  bool insert_release = false;
  bool insert_autorelease = false;
  std::string profile;
};

struct Objc3WeakUnownedLifetimeProfile {
  bool is_weak_reference = false;
  bool is_unowned_reference = false;
  bool is_unowned_safe_reference = false;
  std::string lifetime_profile;
  std::string runtime_hook_profile;
};

struct Objc3ArcDiagnosticFixitProfile {
  bool diagnostic_candidate = false;
  bool fixit_available = false;
  std::string diagnostic_profile;
  std::string fixit_hint;
};

static Objc3OwnershipOperationProfile BuildParamOwnershipOperationProfile(const std::string &spelling) {
  Objc3OwnershipOperationProfile profile;
  if (spelling == "__strong") {
    profile.insert_retain = true;
    profile.insert_release = true;
    profile.profile = "param-retain-release";
  } else if (spelling == "__weak") {
    profile.profile = "param-weak-side-table";
  } else if (spelling == "__autoreleasing") {
    profile.insert_autorelease = true;
    profile.profile = "param-autorelease-bridge";
  } else if (spelling == "__unsafe_unretained") {
    profile.profile = "param-unsafe-unretained";
  }
  return profile;
}

static Objc3OwnershipOperationProfile BuildReturnOwnershipOperationProfile(const std::string &spelling) {
  Objc3OwnershipOperationProfile profile;
  if (spelling == "__strong") {
    profile.insert_retain = true;
    profile.insert_release = true;
    profile.profile = "return-retain-release-transfer";
  } else if (spelling == "__weak") {
    profile.profile = "return-weak-load";
  } else if (spelling == "__autoreleasing") {
    profile.insert_autorelease = true;
    profile.profile = "return-autorelease-transfer";
  } else if (spelling == "__unsafe_unretained") {
    profile.profile = "return-unsafe-unretained";
  }
  return profile;
}

static Objc3WeakUnownedLifetimeProfile BuildWeakUnownedLifetimeProfile(const std::string &spelling,
                                                                       bool prefer_safe_unowned) {
  Objc3WeakUnownedLifetimeProfile profile;
  if (spelling == "__weak") {
    profile.is_weak_reference = true;
    profile.lifetime_profile = "weak";
    profile.runtime_hook_profile = "objc-weak-side-table";
  } else if (spelling == "__unsafe_unretained") {
    profile.is_unowned_reference = true;
    profile.is_unowned_safe_reference = prefer_safe_unowned;
    profile.lifetime_profile = prefer_safe_unowned ? "unowned-safe" : "unowned-unsafe";
    profile.runtime_hook_profile = prefer_safe_unowned ? "objc-unowned-safe-guard"
                                                       : "objc-unowned-unsafe-direct";
  } else if (spelling == "__strong") {
    profile.lifetime_profile = "strong-owned";
  } else if (spelling == "__autoreleasing") {
    profile.lifetime_profile = "autoreleasing";
  }
  return profile;
}

static Objc3WeakUnownedLifetimeProfile BuildPropertyWeakUnownedLifetimeProfile(
    const Objc3PropertyDecl &property) {
  if (property.is_weak) {
    return BuildWeakUnownedLifetimeProfile("__weak", false);
  }
  if (property.is_unowned) {
    return BuildWeakUnownedLifetimeProfile("__unsafe_unretained", true);
  }
  if (property.is_unsafe_unretained) {
    return BuildWeakUnownedLifetimeProfile("__unsafe_unretained", false);
  }
  if (!property.ownership_qualifier_spelling.empty()) {
    return BuildWeakUnownedLifetimeProfile(property.ownership_qualifier_spelling, false);
  }
  if (property.is_assign) {
    return BuildWeakUnownedLifetimeProfile("__unsafe_unretained", false);
  }
  return Objc3WeakUnownedLifetimeProfile{};
}

static Objc3ArcDiagnosticFixitProfile BuildArcDiagnosticFixitProfile(const std::string &spelling,
                                                                     bool is_return_type,
                                                                     bool is_property_type,
                                                                     bool weak_unowned_conflict) {
  Objc3ArcDiagnosticFixitProfile profile;
  if (weak_unowned_conflict) {
    profile.diagnostic_candidate = true;
    profile.fixit_available = true;
    profile.diagnostic_profile = "arc-weak-unowned-conflict";
    profile.fixit_hint = "remove-weak-or-unowned-attribute";
    return profile;
  }

  if (spelling == "__unsafe_unretained") {
    profile.diagnostic_candidate = true;
    profile.fixit_available = true;
    profile.diagnostic_profile = is_return_type ? "arc-return-unsafe-unretained" : "arc-unsafe-unretained";
    profile.fixit_hint = is_property_type ? "replace-with-weak-or-strong-attribute"
                                          : "replace-with-__weak-or-__strong";
    return profile;
  }

  if (spelling == "__autoreleasing") {
    profile.diagnostic_candidate = true;
    profile.fixit_available = true;
    profile.diagnostic_profile = is_return_type ? "arc-return-autoreleasing-transfer" : "arc-autoreleasing-misuse";
    profile.fixit_hint = is_return_type ? "replace-return-qualifier-with-__strong"
                                        : "replace-with-__strong-or-out-parameter";
    return profile;
  }

  if (is_return_type && spelling == "__weak") {
    profile.diagnostic_candidate = true;
    profile.fixit_available = true;
    profile.diagnostic_profile = "arc-return-weak-escape";
    profile.fixit_hint = "replace-return-qualifier-with-__strong";
  }
  return profile;
}

static std::vector<std::string> BuildSortedUniqueStrings(std::vector<std::string> values) {
  std::sort(values.begin(), values.end());
  values.erase(std::unique(values.begin(), values.end()), values.end());
  return values;
}

static bool IsSortedUniqueStrings(const std::vector<std::string> &values) {
  return std::adjacent_find(values.begin(), values.end()) == values.end() &&
         std::is_sorted(values.begin(), values.end());
}

class Objc3Parser {
 public:
  explicit Objc3Parser(const std::vector<Token> &tokens) : tokens_(tokens) {}

  using BlockLiteralSourceUseKind = Objc3BlockLiteralSourceUseKind;

  struct ScopedBlockLiteralSourceUse {
    explicit ScopedBlockLiteralSourceUse(std::vector<BlockLiteralSourceUseKind> &stack,
                                         BlockLiteralSourceUseKind kind)
        : stack_(stack) {
      stack_.push_back(kind);
    }

    ~ScopedBlockLiteralSourceUse() { stack_.pop_back(); }

   private:
    std::vector<BlockLiteralSourceUseKind> &stack_;
  };

  Objc3ParsedProgram Parse() {
    Objc3ParsedProgram program = ast_builder_.BeginProgram();
    while (!At(TokenKind::Eof)) {
      if (Match(TokenKind::KwModule)) {
        ParseModule(program);
      } else if (Match(TokenKind::KwLet)) {
        auto decl = ParseGlobalLet();
        if (decl != nullptr) {
          ast_builder_.AddGlobalDecl(program, std::move(*decl));
        }
      } else if (Match(TokenKind::KwAtInterface)) {
        auto decl = ParseObjcInterfaceDecl();
        if (decl != nullptr) {
          ast_builder_.AddInterfaceDecl(program, std::move(*decl));
        }
      } else if (Match(TokenKind::KwAtImplementation)) {
        auto decl = ParseObjcImplementationDecl();
        if (decl != nullptr) {
          ast_builder_.AddImplementationDecl(program, std::move(*decl));
        }
      } else if (Match(TokenKind::KwAtProtocol)) {
        auto decl = ParseObjcProtocolDecl();
        if (decl != nullptr) {
          ast_builder_.AddProtocolDecl(program, std::move(*decl));
        }
      } else if (IsObjc3ActorClassDeclarationLead(tokens_, index_)) {
        // source-surface anchor: `actor class` stays a contextual
        // parser form rather than a dedicated lexer keyword while lane-A
        // completes actor-member frontend admission.
        const Token actor_token = Advance();
        Advance();
        auto decl = ParseObjcActorInterfaceDecl(actor_token);
        if (decl != nullptr) {
          ast_builder_.AddInterfaceDecl(program, std::move(*decl));
        }
      } else if (AtIdentifierText("__attribute__")) {
        // source-completion anchor: prefixed Part 9 container
        // attributes are parser-owned wrappers around the existing interface /
        // actor container forms rather than a widened top-level grammar family.
        Objc3InterfaceDecl pending_container_attributes;
        if (!ParseOptionalContainerDispatchAttributes(
                pending_container_attributes)) {
          SynchronizeTopLevel();
          continue;
        }
        if (Match(TokenKind::KwAtInterface)) {
          auto decl = ParseObjcInterfaceDecl(&pending_container_attributes);
          if (decl != nullptr) {
            ast_builder_.AddInterfaceDecl(program, std::move(*decl));
          }
        } else if (IsObjc3ActorClassDeclarationLead(tokens_, index_)) {
          const Token actor_token = Advance();
          Advance();
          auto decl = ParseObjcActorInterfaceDecl(actor_token,
                                                  &pending_container_attributes);
          if (decl != nullptr) {
            ast_builder_.AddInterfaceDecl(program, std::move(*decl));
          }
        } else {
          const Token &token = Peek();
          diagnostics_.push_back(BuildObjc3UnsupportedTopLevelDiagnostic(token));
          SynchronizeTopLevel();
        }
      } else if (IsObjc3TopLevelFunctionQualifierLead(Peek().kind)) {
        ParseTopLevelFunctionDecl(program);
      } else if (AtRemovedOptionalTemplateAlias()) {
        RejectRemovedOptionalTemplateAlias(Peek());
        SynchronizeTopLevel();
      } else if (AtCStyleTopLevelFunctionDeclStart()) {
        ParseTopLevelCStyleFunctionDecl(program);
      } else {
        const Token &token = Peek();
        diagnostics_.push_back(BuildObjc3UnsupportedTopLevelDiagnostic(token));
        SynchronizeTopLevel();
      }
    }
    FinalizeObjcProgramPropertyIvarLayoutClosure(program);
    ast_builder_.SetDraftSyntaxSurfaceSummary(
        program, BuildObjc3DraftSyntaxSurfaceSummary(program));
    return program;
  }

  std::vector<std::string> TakeDiagnostics() { return diagnostics_; }

 private:
  bool At(TokenKind kind) const { return AtTokenKind(tokens_, index_, kind); }

  const Token &Peek() const { return PeekToken(tokens_, index_); }

  const Token &Previous() const { return PreviousToken(tokens_, index_); }

  const Token &Advance() { return AdvanceToken(tokens_, index_); }

  bool Match(TokenKind kind) { return MatchTokenKind(tokens_, index_, kind); }

  bool AtRemovedOptionalTemplateAlias() const {
    return IsObjc3RemovedOptionalTemplateAliasLead(tokens_, index_);
  }

  bool RejectRemovedOptionalTemplateAlias(const Token &token) {
    diagnostics_.push_back(BuildObjc3RemovedOptionalTemplateAliasDiagnostic(token));
    return false;
  }

  bool AtCStyleTypeLeadToken() const {
    return IsObjc3CStyleTypeLeadToken(tokens_, index_);
  }

  bool AtCStyleTopLevelFunctionDeclStart() const {
    return IsObjc3CStyleFunctionDeclarationStart(tokens_, index_);
  }
