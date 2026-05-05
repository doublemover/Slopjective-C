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

static std::string BuildBlockLiteralCaptureProfile(const std::vector<std::string> &capture_names_lexicographic) {
  if (capture_names_lexicographic.empty()) {
    return "block-captures:none";
  }
  std::ostringstream out;
  out << "block-captures:";
  for (std::size_t i = 0; i < capture_names_lexicographic.size(); ++i) {
    out << capture_names_lexicographic[i];
    if (i + 1u != capture_names_lexicographic.size()) {
      out << ",";
    }
  }
  return out.str();
}

struct ParsedBlockParameterSourceModel {
  std::string name;
  std::string type_spelling = "implicit-unspecified";
  bool explicit_type = false;
};

static std::vector<std::string> BuildSortedUniqueStrings(
    std::vector<std::string> values);

static std::string BuildBlockParameterSignatureEntry(
    const ParsedBlockParameterSourceModel &parameter) {
  std::ostringstream out;
  out << "name=" << parameter.name
      << ";type=" << parameter.type_spelling
      << ";explicit_type=" << (parameter.explicit_type ? "true" : "false");
  return out.str();
}

static std::vector<std::string> BuildBlockParameterSignatureEntriesLexicographic(
    const std::vector<ParsedBlockParameterSourceModel> &parameters) {
  std::vector<std::string> entries;
  entries.reserve(parameters.size());
  for (const auto &parameter : parameters) {
    entries.push_back(BuildBlockParameterSignatureEntry(parameter));
  }
  return BuildSortedUniqueStrings(std::move(entries));
}

static std::vector<ValueType> BuildBlockParameterTypesSourceOrder(
    const std::vector<ParsedBlockParameterSourceModel> &parameters) {
  std::vector<ValueType> types;
  types.reserve(parameters.size());
  for (const auto &parameter : parameters) {
    if (!parameter.explicit_type) {
      types.push_back(ValueType::Unknown);
      continue;
    }
    if (parameter.type_spelling == "i32") {
      types.push_back(ValueType::I32);
      continue;
    }
    if (parameter.type_spelling == "bool") {
      types.push_back(ValueType::Bool);
      continue;
    }
    if (parameter.type_spelling == "void") {
      types.push_back(ValueType::Void);
      continue;
    }
    types.push_back(ValueType::Unknown);
  }
  return types;
}

static std::string BuildBlockSignatureProfile(
    const std::vector<ParsedBlockParameterSourceModel> &parameters) {
  const std::size_t explicit_typed_parameter_count =
      static_cast<std::size_t>(std::count_if(
          parameters.begin(), parameters.end(),
          [](const ParsedBlockParameterSourceModel &parameter) {
            return parameter.explicit_type;
          }));
  const std::size_t implicit_parameter_count =
      parameters.size() - explicit_typed_parameter_count;
  std::ostringstream out;
  out << "block-signature:parameters=" << parameters.size()
      << ";explicit-typed=" << explicit_typed_parameter_count
      << ";implicit=" << implicit_parameter_count
      << ";return-surface=body-inferred";
  return out.str();
}

static std::string BuildBlockCaptureInventoryEntry(const std::string &capture_name) {
  return "name=" + capture_name +
         ";storage=by-value-readonly;byref=false;mutable=false";
}

static std::vector<std::string> BuildBlockCaptureInventoryEntriesLexicographic(
    const std::vector<std::string> &capture_names_lexicographic) {
  std::vector<std::string> entries;
  entries.reserve(capture_names_lexicographic.size());
  for (const auto &capture_name : capture_names_lexicographic) {
    entries.push_back(BuildBlockCaptureInventoryEntry(capture_name));
  }
  return BuildSortedUniqueStrings(std::move(entries));
}

static std::string BuildBlockCaptureInventoryProfile(
    std::size_t capture_count,
    std::size_t byvalue_readonly_capture_count) {
  std::ostringstream out;
  out << "block-capture-inventory:captures=" << capture_count
      << ";byvalue-readonly=" << byvalue_readonly_capture_count
      << ";byref=0;mutable=0";
  return out.str();
}

static std::string BuildBlockLiteralAbiLayoutProfile(std::size_t parameter_count,
                                                     std::size_t capture_count,
                                                     std::size_t body_statement_count) {
  std::ostringstream out;
  out << "block-abi-layout:invoke-arg-slots=" << parameter_count
      << ";capture-words=" << capture_count
      << ";body-statements=" << body_statement_count;
  return out.str();
}

static std::string BuildBlockLiteralAbiDescriptorSymbol(unsigned line,
                                                        unsigned column,
                                                        std::size_t parameter_count,
                                                        std::size_t capture_count) {
  std::ostringstream out;
  out << "__objc3_block_desc_" << line << "_" << column
      << "_p" << parameter_count
      << "_c" << capture_count;
  return out.str();
}

static std::string BuildBlockLiteralInvokeTrampolineSymbol(unsigned line,
                                                           unsigned column,
                                                           std::size_t parameter_count,
                                                           std::size_t capture_count) {
  std::ostringstream out;
  out << "__objc3_block_invoke_" << line << "_" << column
      << "_p" << parameter_count
      << "_c" << capture_count;
  return out.str();
}

static std::vector<std::string> BuildBlockInvokeSurfaceEntriesLexicographic(
    const Expr &block) {
  std::vector<std::string> entries;
  entries.push_back("descriptor=" + block.block_abi_descriptor_symbol);
  entries.push_back("invoke=" + block.block_invoke_trampoline_symbol);
  return BuildSortedUniqueStrings(std::move(entries));
}

static std::string BuildBlockInvokeSurfaceProfile(const Expr &block) {
  std::ostringstream out;
  out << "block-invoke-surface:invoke-arg-slots="
      << block.block_abi_invoke_argument_slots
      << ";capture-words=" << block.block_abi_capture_word_count
      << ";descriptor-symbol=" << block.block_abi_descriptor_symbol
      << ";invoke-symbol=" << block.block_invoke_trampoline_symbol
      << ";return-surface=body-inferred";
  return out.str();
}

static std::string BuildBlockSourceModelReplayKey(const Expr &block) {
  std::ostringstream out;
  out << "signature_entries="
      << block.block_parameter_signature_entries_lexicographic.size()
      << ";explicit_typed_parameters="
      << block.block_explicit_typed_parameter_count
      << ";capture_inventory_entries="
      << block.block_capture_inventory_entries_lexicographic.size()
      << ";byvalue_readonly_captures="
      << block.block_byvalue_readonly_capture_count
      << ";invoke_surface_entries="
      << block.block_invoke_surface_entries_lexicographic.size()
      << ";deterministic="
      << (block.block_source_model_is_normalized ? "true" : "false")
      << ";lane_contract="
      << Expr::kObjc3ExecutableBlockSourceModelLaneContract;
  return out.str();
}

static std::string BuildBlockStorageEscapeProfile(std::size_t mutable_capture_count,
                                                  std::size_t byref_slot_count,
                                                  bool escape_to_heap,
                                                  std::size_t body_statement_count) {
  std::ostringstream out;
  out << "block-storage:mutable-captures=" << mutable_capture_count
      << ";byref-slots=" << byref_slot_count
      << ";escape=" << (escape_to_heap ? "heap" : "stack")
      << ";body-statements=" << body_statement_count;
  return out.str();
}

static std::string BuildBlockStorageByrefLayoutSymbol(unsigned line,
                                                      unsigned column,
                                                      std::size_t mutable_capture_count,
                                                      std::size_t byref_slot_count,
                                                      bool escape_to_heap) {
  std::ostringstream out;
  out << "__objc3_block_byref_layout_" << line << "_" << column
      << "_m" << mutable_capture_count
      << "_b" << byref_slot_count
      << "_" << (escape_to_heap ? "heap" : "stack");
  return out.str();
}

static std::string BuildBlockCopyDisposeProfile(std::size_t mutable_capture_count,
                                                std::size_t byref_slot_count,
                                                bool escape_to_heap,
                                                std::size_t body_statement_count) {
  std::ostringstream out;
  out << "block-copy-dispose:copy-helper=" << (mutable_capture_count > 0u ? "enabled" : "elided")
      << ";dispose-helper=" << (byref_slot_count > 0u ? "enabled" : "elided")
      << ";escape=" << (escape_to_heap ? "heap" : "stack")
      << ";body-statements=" << body_statement_count;
  return out.str();
}

static std::string BuildBlockCopyHelperSymbol(unsigned line,
                                              unsigned column,
                                              std::size_t mutable_capture_count,
                                              std::size_t byref_slot_count,
                                              bool escape_to_heap) {
  std::ostringstream out;
  out << "__objc3_block_copy_helper_" << line << "_" << column
      << "_m" << mutable_capture_count
      << "_b" << byref_slot_count
      << "_" << (escape_to_heap ? "heap" : "stack");
  return out.str();
}

static std::string BuildBlockDisposeHelperSymbol(unsigned line,
                                                 unsigned column,
                                                 std::size_t mutable_capture_count,
                                                 std::size_t byref_slot_count,
                                                 bool escape_to_heap) {
  std::ostringstream out;
  out << "__objc3_block_dispose_helper_" << line << "_" << column
      << "_m" << mutable_capture_count
      << "_b" << byref_slot_count
      << "_" << (escape_to_heap ? "heap" : "stack");
  return out.str();
}

static std::size_t BuildBlockDeterminismPerfBaselineWeight(std::size_t parameter_count,
                                                           std::size_t capture_count,
                                                           std::size_t body_statement_count,
                                                           bool copy_helper_required,
                                                           bool dispose_helper_required) {
  std::size_t weight = parameter_count * 2u + capture_count * 8u + body_statement_count * 4u;
  if (copy_helper_required) {
    weight += 6u;
  }
  if (dispose_helper_required) {
    weight += 6u;
  }
  return weight;
}

static std::string BuildBlockDeterminismPerfBaselineProfile(std::size_t parameter_count,
                                                            std::size_t capture_count,
                                                            std::size_t body_statement_count,
                                                            bool copy_helper_required,
                                                            bool dispose_helper_required,
                                                            bool deterministic_capture_set,
                                                            bool copy_dispose_profile_is_normalized,
                                                            std::size_t baseline_weight) {
  const char *tier = baseline_weight <= 24u ? "light" : (baseline_weight <= 64u ? "medium" : "heavy");
  std::ostringstream out;
  out << "block-det-perf-baseline:params=" << parameter_count
      << ";captures=" << capture_count
      << ";body-statements=" << body_statement_count
      << ";copy-helper=" << (copy_helper_required ? "enabled" : "elided")
      << ";dispose-helper=" << (dispose_helper_required ? "enabled" : "elided")
      << ";deterministic-captures=" << (deterministic_capture_set ? "true" : "false")
      << ";normalized=" << (copy_dispose_profile_is_normalized ? "true" : "false")
      << ";weight=" << baseline_weight
      << ";tier=" << tier;
  return out.str();
}

static std::vector<std::string> BuildScopePathLexicographic(std::string owner_symbol,
                                                             std::string entry_symbol) {
  std::vector<std::string> path;
  if (!owner_symbol.empty()) {
    path.push_back(std::move(owner_symbol));
  }
  if (!entry_symbol.empty()) {
    path.push_back(std::move(entry_symbol));
  }
  std::sort(path.begin(), path.end());
  path.erase(std::unique(path.begin(), path.end()), path.end());
  return path;
}

// freeze: semantic-link symbols remain the canonical owner identities
// for executable metadata source graph nodes until semantic closure lands.
static std::string BuildObjcContainerScopeOwner(const std::string &container_kind,
                                                const std::string &name,
                                                bool has_category,
                                                const std::string &category_name) {
  std::string owner = container_kind + ":" + name;
  if (has_category) {
    owner += "(" + category_name + ")";
  }
  return owner;
}

static std::string BuildObjcMethodScopePathSymbol(const Objc3MethodDecl &method) {
  // executable method-body binding anchor: parser preserves one
  // canonical method owner identity per executable declaration so lane-C can
  // attach method-list entries to concrete LLVM body symbols without
  // rediscovering selector ownership from syntax.
  // executable realization-record expansion anchor: parser still
  // stops at canonical bundle/object/attachment identities only. Realization
  // record serialization remains a downstream lowering responsibility.
  // class-realization-runtime freeze anchor: parser does not decide
  // runtime class realization, metaclass graph walking, category attachment,
  // or protocol-aware negative lookup behavior. It only preserves the source
  // identities that later sema/lowering/runtime stages consume.
  // metaclass-graph-root-class anchor: parser also keeps empty
  // superclass spellings explicit so runtime can distinguish root-class
  // baselines from broken superclass closure without reinterpreting syntax.
  // category-attachment-protocol-conformance anchor: parser keeps
  // direct class/category adopted-protocol spellings stable so runtime queries
  // can consume emitted protocol refs instead of reconstructing conformance
  // from diagnostics or manifests.
  // canonical-runnable-object-sample anchor: parser also preserves
  // nested bracketed object-sample sends like [[Widget alloc] init] without
  // rewriting receiver shape, so later sema/runtime stages can admit the
  // runtime-owned builtin alloc/new/init boundary truthfully.
  // class-protocol-category conformance gate anchor: lane-E consumes
  // the A003/B004/C003/D004 evidence chain and therefore still depends on this
  // parser-owned syntax preservation instead of any gate-specific rewrite.
  // runnable class-protocol-category execution-matrix anchor: the
  // live matrix still depends on parser-stable superclass sends and
  // class/category surface spelling rather than any execution-matrix-specific
  // syntax reconstruction.
  return (method.is_class_method ? "class_method:" : "instance_method:") + method.selector;
}

static std::string BuildObjcPropertyScopePathSymbol(const Objc3PropertyDecl &property) {
  return "property:" + property.name;
}

static std::string BuildObjcPropertySynthesisSymbol(const Objc3PropertyDecl &property) {
  return "property_synthesis:" + property.name;
}

static std::string BuildObjcIvarBindingSymbol(const Objc3PropertyDecl &property) {
  return "ivar_binding:_" + property.name;
}

static std::string BuildObjcTypecheckParamFamilySymbol(const FuncParam &param) {
  if (param.id_spelling) {
    return "id";
  }
  if (param.class_spelling) {
    return "Class";
  }
  if (param.sel_spelling) {
    return "SEL";
  }
  if (param.object_pointer_type_spelling) {
    return "object-pointer:" + param.object_pointer_type_name;
  }
  return "";
}

static std::string BuildObjcTypecheckReturnFamilySymbol(const FunctionDecl &fn) {
  if (fn.return_id_spelling) {
    return "id";
  }
  if (fn.return_class_spelling) {
    return "Class";
  }
  if (fn.return_sel_spelling) {
    return "SEL";
  }
  if (fn.return_object_pointer_type_spelling) {
    return "object-pointer:" + fn.return_object_pointer_type_name;
  }
  return "";
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

static std::vector<std::string> BuildProtocolSemanticLinkTargetsLexicographic(
    const std::vector<std::string> &protocol_names) {
  // completion: protocol inheritance/adoption targets remain on the
  // canonical protocol: owner surface so executable metadata graph edges stay
  // deterministic across protocol and category export packets.
  // freeze: the same canonical protocol: owner identities remain the
  // semantic-consistency boundary inputs for inheritance and attachment rules.
  std::vector<std::string> targets;
  targets.reserve(protocol_names.size());
  for (const auto &name : protocol_names) {
    if (!name.empty()) {
      targets.push_back("protocol:" + name);
    }
  }
  return BuildSortedUniqueStrings(std::move(targets));
}

static std::string BuildObjcCategorySemanticLinkSymbol(const std::string &owner_name,
                                                       const std::string &category_name) {
  // completeness: interface/implementation semantic-link symbols stay
  // canonical declaration node identities while runtime class/metaclass graph
  // nodes derive a separate class:/metaclass: owner surface.
  // completion: category export packets keep category:Class(Category)
  // as the canonical category node owner identity while declaration/member
  // nodes continue to reference interface:/implementation: semantic-link owners.
  // lowering-handoff anchor: category semantic-link symbols remain
  // the parser-owned canonical owner identities that typed lowering handoff
  // packets consume without reparsing declaration containers.
  // typed-lowering anchor: typed metadata graph handoff packets keep
  // these parser-owned semantic-link owner identities in the published
  // lowering schema so downstream lowering never reparses declaration
  // containers to recover owner identity.
  // debug-projection anchor: the lane-C manifest/IR inspection matrix
  // replays these same parser-owned owner identities before runtime section
  // emission lands, so inspection never depends on reparsing declaration text.
  // runtime-ingest packaging anchor: lane-D packages this same
  // parser-owned owner identity surface through one manifest transport
  // contract, so later runtime ingest never reconstructs owner identity from
  // raw syntax.
  // semantic-closure gate anchor: lane-E freezes this parser-owned
  // owner identity surface together with B004 legality, C003 debug
  // projection, and D002 packaging before the next runtime step section emission begins.
  // corpus-sync anchor: representative class/category/property/ivar
  // runner probes continue to derive owner identity from this same parser
  // surface on the real integrated path.
  return "category:" + owner_name + "(" + category_name + ")";
}

static std::string BuildObjcMethodLookupSymbol(const Objc3MethodDecl &method) {
  return (method.is_class_method ? "class_lookup:" : "instance_lookup:") + method.selector;
}

static std::string BuildObjcMethodOverrideLookupSymbol(const Objc3MethodDecl &method) {
  return (method.is_class_method ? "class_override:" : "instance_override:") + method.selector;
}

static std::string BuildObjcMethodConflictLookupSymbol(const Objc3MethodDecl &method) {
  return (method.is_class_method ? "class_conflict:" : "instance_conflict:") + method.selector;
}

static std::vector<std::string> BuildObjcMethodLookupSymbolsLexicographic(
    const std::vector<Objc3MethodDecl> &methods) {
  std::vector<std::string> symbols;
  symbols.reserve(methods.size());
  for (const auto &method : methods) {
    if (!method.method_lookup_symbol.empty()) {
      symbols.push_back(method.method_lookup_symbol);
    }
  }
  return BuildSortedUniqueStrings(std::move(symbols));
}

static std::vector<std::string> BuildObjcMethodOverrideLookupSymbolsLexicographic(
    const std::vector<Objc3MethodDecl> &methods) {
  std::vector<std::string> symbols;
  symbols.reserve(methods.size());
  for (const auto &method : methods) {
    if (!method.override_lookup_symbol.empty()) {
      symbols.push_back(method.override_lookup_symbol);
    }
  }
  return BuildSortedUniqueStrings(std::move(symbols));
}

static std::vector<std::string> BuildObjcMethodConflictLookupSymbolsLexicographic(
    const std::vector<Objc3MethodDecl> &methods) {
  std::vector<std::string> symbols;
  symbols.reserve(methods.size());
  for (const auto &method : methods) {
    if (!method.conflict_lookup_symbol.empty()) {
      symbols.push_back(method.conflict_lookup_symbol);
    }
  }
  return BuildSortedUniqueStrings(std::move(symbols));
}

static std::vector<std::string> BuildObjcPropertySynthesisSymbolsLexicographic(
    const std::vector<Objc3PropertyDecl> &properties) {
  std::vector<std::string> symbols;
  symbols.reserve(properties.size());
  for (const auto &property : properties) {
    if (!property.property_synthesis_symbol.empty()) {
      symbols.push_back(property.property_synthesis_symbol);
    }
  }
  return BuildSortedUniqueStrings(std::move(symbols));
}

static std::vector<std::string> BuildObjcIvarBindingSymbolsLexicographic(
    const std::vector<Objc3PropertyDecl> &properties) {
  std::vector<std::string> symbols;
  symbols.reserve(properties.size());
  for (const auto &property : properties) {
    if (!property.ivar_binding_symbol.empty()) {
      symbols.push_back(property.ivar_binding_symbol);
    }
  }
  return BuildSortedUniqueStrings(std::move(symbols));
}

class Objc3Parser {
 public:
  explicit Objc3Parser(const std::vector<Token> &tokens) : tokens_(tokens) {}

  enum class BlockLiteralSourceUseKind {
    ExpressionSite,
    GlobalInitializer,
    LocalBindingInitializer,
    AssignmentValue,
    ReturnValue,
    CallArgument,
    MessageArgument,
  };

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
      } else if (AtIdentifierText("actor") &&
                 AtIdentifierTextOffset(1, "class")) {
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
        } else if (AtIdentifierText("actor") &&
                   AtIdentifierTextOffset(1, "class")) {
          const Token actor_token = Advance();
          Advance();
          auto decl = ParseObjcActorInterfaceDecl(actor_token,
                                                  &pending_container_attributes);
          if (decl != nullptr) {
            ast_builder_.AddInterfaceDecl(program, std::move(*decl));
          }
        } else {
          const Token &token = Peek();
          diagnostics_.push_back(MakeDiag(
              token.line, token.column, "O3P100",
              "unsupported Objective-C 3 statement"));
          SynchronizeTopLevel();
        }
      } else if (At(TokenKind::KwPure) || At(TokenKind::KwExtern) || At(TokenKind::KwAsync) ||
                 At(TokenKind::KwFn)) {
        ParseTopLevelFunctionDecl(program);
      } else if (AtCStyleTopLevelFunctionDeclStart()) {
        ParseTopLevelCompatFunctionDecl(program);
      } else {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P100",
                                        "unsupported Objective-C 3 statement"));
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
  bool At(TokenKind kind) const { return tokens_[index_].kind == kind; }

  const Token &Peek() const { return tokens_[index_]; }

  const Token &Previous() const { return tokens_[index_ - 1]; }

  const Token &Advance() {
    if (!At(TokenKind::Eof)) {
      ++index_;
    }
    return Previous();
  }

  bool Match(TokenKind kind) {
    if (At(kind)) {
      Advance();
      return true;
    }
    return false;
  }

  static bool IsCompatBuiltinTypeSpelling(const std::string &spelling) {
    return spelling == "int" || spelling == "bool" || spelling == "BOOL" || spelling == "void" ||
           spelling == "id" || spelling == "Class" || spelling == "SEL" || spelling == "Protocol" ||
           spelling == "instancetype" || spelling == "NSInteger" || spelling == "NSUInteger";
  }

  bool AtCStyleTypeLeadToken() const {
    if (At(TokenKind::KwI32) || At(TokenKind::KwBool) || At(TokenKind::KwBOOL) || At(TokenKind::KwVoid) ||
        At(TokenKind::KwId) || At(TokenKind::KwClass) || At(TokenKind::KwSEL) || At(TokenKind::KwProtocol) ||
        At(TokenKind::KwInstancetype) || At(TokenKind::KwNSInteger) || At(TokenKind::KwNSUInteger)) {
      return true;
    }
    return At(TokenKind::Identifier) && IsCompatBuiltinTypeSpelling(Peek().text);
  }

  bool AtCStyleTopLevelFunctionDeclStart() const {
    if (!AtCStyleTypeLeadToken()) {
      return false;
    }
    std::size_t cursor = index_ + 1;
    while (cursor < tokens_.size() && tokens_[cursor].kind == TokenKind::Star) {
      ++cursor;
    }
    if (cursor >= tokens_.size() || tokens_[cursor].kind != TokenKind::Identifier) {
      return false;
    }
    ++cursor;
    return cursor < tokens_.size() && tokens_[cursor].kind == TokenKind::LParen;
  }

  bool ParseCStyleCompatType(ValueType &type, bool allow_void, bool &id_spelling, bool &class_spelling,
                             bool &sel_spelling, bool &instancetype_spelling, bool &object_pointer_spelling,
                             std::string &object_pointer_name, unsigned &pointer_depth, unsigned diag_line,
                             unsigned diag_column, const char *diagnostic_context) {
    type = ValueType::Unknown;
    id_spelling = false;
    class_spelling = false;
    sel_spelling = false;
    instancetype_spelling = false;
    object_pointer_spelling = false;
    object_pointer_name.clear();
    pointer_depth = 0;

    const auto parse_builtin_type = [&](const std::string &spelling) -> bool {
      if (spelling == "int" || spelling == "NSInteger" || spelling == "NSUInteger") {
        type = ValueType::I32;
        return true;
      }
      if (spelling == "bool" || spelling == "BOOL") {
        type = ValueType::Bool;
        return true;
      }
      if (spelling == "void") {
        type = ValueType::Void;
        return true;
      }
      if (spelling == "id") {
        type = ValueType::ObjCId;
        id_spelling = true;
        return true;
      }
      if (spelling == "Class") {
        type = ValueType::ObjCClass;
        class_spelling = true;
        return true;
      }
      if (spelling == "SEL") {
        type = ValueType::ObjCSel;
        sel_spelling = true;
        return true;
      }
      if (spelling == "Protocol") {
        type = ValueType::ObjCProtocol;
        return true;
      }
      if (spelling == "instancetype") {
        type = ValueType::ObjCInstancetype;
        instancetype_spelling = true;
        return true;
      }
      return false;
    };

    if (Match(TokenKind::KwI32)) {
      type = ValueType::I32;
    } else if (Match(TokenKind::KwBool) || Match(TokenKind::KwBOOL)) {
      type = ValueType::Bool;
    } else if (Match(TokenKind::KwVoid)) {
      type = ValueType::Void;
    } else if (Match(TokenKind::KwId)) {
      type = ValueType::ObjCId;
      id_spelling = true;
    } else if (Match(TokenKind::KwClass)) {
      type = ValueType::ObjCClass;
      class_spelling = true;
    } else if (Match(TokenKind::KwSEL)) {
      type = ValueType::ObjCSel;
      sel_spelling = true;
    } else if (Match(TokenKind::KwProtocol)) {
      type = ValueType::ObjCProtocol;
