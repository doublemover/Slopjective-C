static bool IsErrorDiagnosticSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("error") != std::string::npos ||
         lowered.find("diagnostic") != std::string::npos ||
         lowered.find("invalid") != std::string::npos ||
         lowered.find("missing") != std::string::npos ||
         lowered.find("unsupported") != std::string::npos;
}

static bool IsRecoveryAnchorSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("recover") != std::string::npos ||
         lowered.find("recovery") != std::string::npos ||
         lowered.find("resync") != std::string::npos ||
         lowered.find("anchor") != std::string::npos;
}

static bool IsRecoveryBoundarySymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("throws") != std::string::npos ||
         lowered.find("throw") != std::string::npos ||
         lowered.find("result") != std::string::npos ||
         lowered.find("nserror") != std::string::npos ||
         lowered.find("catch") != std::string::npos ||
         lowered.find("finally") != std::string::npos ||
         lowered.find("boundary") != std::string::npos ||
         lowered.find("cleanup") != std::string::npos;
}

static bool IsFailClosedDiagnosticSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("fatal") != std::string::npos ||
         lowered.find("panic") != std::string::npos ||
         lowered.find("trap") != std::string::npos ||
         lowered.find("abort") != std::string::npos ||
         lowered.find("fail_closed") != std::string::npos ||
         lowered.find("hard_error") != std::string::npos;
}

struct Objc3ErrorDiagnosticsRecoverySiteCounts {
  std::size_t diagnostic_emit_sites = 0;
  std::size_t recovery_anchor_sites = 0;
  std::size_t recovery_boundary_sites = 0;
  std::size_t fail_closed_diagnostic_sites = 0;
};

static void CollectErrorDiagnosticsRecoverySitesFromSymbol(
    const std::string &symbol,
    Objc3ErrorDiagnosticsRecoverySiteCounts &counts) {
  if (IsErrorDiagnosticSymbol(symbol)) {
    counts.diagnostic_emit_sites += 1u;
  }
  if (IsRecoveryAnchorSymbol(symbol)) {
    counts.recovery_anchor_sites += 1u;
  }
  if (IsRecoveryBoundarySymbol(symbol)) {
    counts.recovery_boundary_sites += 1u;
  }
  if (IsFailClosedDiagnosticSymbol(symbol)) {
    counts.fail_closed_diagnostic_sites += 1u;
  }
}

static void CollectErrorDiagnosticsRecoveryExprSites(
    const Expr *expr,
    Objc3ErrorDiagnosticsRecoverySiteCounts &counts) {
  if (expr == nullptr) {
    return;
  }
  switch (expr->kind) {
  case Expr::Kind::Call:
    CollectErrorDiagnosticsRecoverySitesFromSymbol(expr->ident, counts);
    for (const auto &arg : expr->args) {
      CollectErrorDiagnosticsRecoveryExprSites(arg.get(), counts);
    }
    return;
  case Expr::Kind::MessageSend:
    CollectErrorDiagnosticsRecoverySitesFromSymbol(expr->selector, counts);
    CollectErrorDiagnosticsRecoveryExprSites(expr->receiver.get(), counts);
    for (const auto &arg : expr->args) {
      CollectErrorDiagnosticsRecoveryExprSites(arg.get(), counts);
    }
    return;
  case Expr::Kind::Binary:
    CollectErrorDiagnosticsRecoveryExprSites(expr->left.get(), counts);
    CollectErrorDiagnosticsRecoveryExprSites(expr->right.get(), counts);
    return;
  case Expr::Kind::Conditional:
    CollectErrorDiagnosticsRecoveryExprSites(expr->left.get(), counts);
    CollectErrorDiagnosticsRecoveryExprSites(expr->right.get(), counts);
    CollectErrorDiagnosticsRecoveryExprSites(expr->third.get(), counts);
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

static void CollectErrorDiagnosticsRecoveryForClauseSites(
    const ForClause &clause,
    Objc3ErrorDiagnosticsRecoverySiteCounts &counts) {
  CollectErrorDiagnosticsRecoveryExprSites(clause.value.get(), counts);
}

static void CollectErrorDiagnosticsRecoveryStmtSites(
    const Stmt *stmt,
    Objc3ErrorDiagnosticsRecoverySiteCounts &counts) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
    if (stmt->let_stmt != nullptr) {
      CollectErrorDiagnosticsRecoveryExprSites(stmt->let_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::Assign:
    if (stmt->assign_stmt != nullptr) {
      CollectErrorDiagnosticsRecoveryExprSites(stmt->assign_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::Return:
    if (stmt->return_stmt != nullptr) {
      CollectErrorDiagnosticsRecoveryExprSites(stmt->return_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::If:
    if (stmt->if_stmt == nullptr) {
      return;
    }
    CollectErrorDiagnosticsRecoveryExprSites(stmt->if_stmt->condition.get(), counts);
    for (const auto &then_stmt : stmt->if_stmt->then_body) {
      CollectErrorDiagnosticsRecoveryStmtSites(then_stmt.get(), counts);
    }
    for (const auto &else_stmt : stmt->if_stmt->else_body) {
      CollectErrorDiagnosticsRecoveryStmtSites(else_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->do_while_stmt->body) {
      CollectErrorDiagnosticsRecoveryStmtSites(body_stmt.get(), counts);
    }
    CollectErrorDiagnosticsRecoveryExprSites(stmt->do_while_stmt->condition.get(), counts);
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt == nullptr) {
      return;
    }
    CollectErrorDiagnosticsRecoveryForClauseSites(stmt->for_stmt->init, counts);
    CollectErrorDiagnosticsRecoveryExprSites(stmt->for_stmt->condition.get(), counts);
    CollectErrorDiagnosticsRecoveryForClauseSites(stmt->for_stmt->step, counts);
    for (const auto &body_stmt : stmt->for_stmt->body) {
      CollectErrorDiagnosticsRecoveryStmtSites(body_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt == nullptr) {
      return;
    }
    CollectErrorDiagnosticsRecoveryExprSites(stmt->switch_stmt->condition.get(), counts);
    for (const auto &switch_case : stmt->switch_stmt->cases) {
      for (const auto &case_stmt : switch_case.body) {
        CollectErrorDiagnosticsRecoveryStmtSites(case_stmt.get(), counts);
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt == nullptr) {
      return;
    }
    CollectErrorDiagnosticsRecoveryExprSites(stmt->while_stmt->condition.get(), counts);
    for (const auto &body_stmt : stmt->while_stmt->body) {
      CollectErrorDiagnosticsRecoveryStmtSites(body_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    if (stmt->block_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->block_stmt->body) {
      CollectErrorDiagnosticsRecoveryStmtSites(body_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::Expr:
    if (stmt->expr_stmt != nullptr) {
      CollectErrorDiagnosticsRecoveryExprSites(stmt->expr_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  }
}

static Objc3ErrorDiagnosticsRecoverySiteCounts CountErrorDiagnosticsRecoverySitesInBody(
    const std::vector<std::unique_ptr<Stmt>> &body) {
  Objc3ErrorDiagnosticsRecoverySiteCounts counts;
  for (const auto &stmt : body) {
    CollectErrorDiagnosticsRecoveryStmtSites(stmt.get(), counts);
  }
  return counts;
}

struct Objc3ErrorDiagnosticsRecoveryProfile {
  std::size_t error_diagnostics_recovery_sites = 0;
  std::size_t diagnostic_emit_sites = 0;
  std::size_t recovery_anchor_sites = 0;
  std::size_t recovery_boundary_sites = 0;
  std::size_t fail_closed_diagnostic_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t gate_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic_error_diagnostics_recovery_handoff = false;
};

static bool TryAddErrorDiagnosticsRecoverySiteCounts(
    std::size_t lhs, std::size_t rhs, std::size_t &out) {
  if (lhs > std::numeric_limits<std::size_t>::max() - rhs) {
    return false;
  }
  out = lhs + rhs;
  return true;
}

static std::string BuildErrorDiagnosticsRecoveryProfile(
    std::size_t error_diagnostics_recovery_sites,
    std::size_t diagnostic_emit_sites,
    std::size_t recovery_anchor_sites,
    std::size_t recovery_boundary_sites,
    std::size_t fail_closed_diagnostic_sites,
    std::size_t normalized_sites,
    std::size_t gate_blocked_sites,
    std::size_t contract_violation_sites,
    bool deterministic_error_diagnostics_recovery_handoff) {
  std::ostringstream out;
  out << "error-diagnostics-recovery:error_diagnostics_recovery_sites="
      << error_diagnostics_recovery_sites
      << ";diagnostic_emit_sites=" << diagnostic_emit_sites
      << ";recovery_anchor_sites=" << recovery_anchor_sites
      << ";recovery_boundary_sites=" << recovery_boundary_sites
      << ";fail_closed_diagnostic_sites=" << fail_closed_diagnostic_sites
      << ";normalized_sites=" << normalized_sites
      << ";gate_blocked_sites=" << gate_blocked_sites
      << ";contract_violation_sites=" << contract_violation_sites
      << ";deterministic_error_diagnostics_recovery_handoff="
      << (deterministic_error_diagnostics_recovery_handoff ? "true" : "false");
  return out.str();
}

static bool IsErrorDiagnosticsRecoveryProfileNormalized(
    std::size_t error_diagnostics_recovery_sites,
    std::size_t diagnostic_emit_sites,
    std::size_t recovery_anchor_sites,
    std::size_t recovery_boundary_sites,
    std::size_t fail_closed_diagnostic_sites,
    std::size_t normalized_sites,
    std::size_t gate_blocked_sites,
    std::size_t contract_violation_sites) {
  if (diagnostic_emit_sites > error_diagnostics_recovery_sites ||
      recovery_anchor_sites > error_diagnostics_recovery_sites ||
      recovery_boundary_sites > error_diagnostics_recovery_sites ||
      fail_closed_diagnostic_sites > error_diagnostics_recovery_sites ||
      normalized_sites > error_diagnostics_recovery_sites ||
      gate_blocked_sites > error_diagnostics_recovery_sites ||
      contract_violation_sites > error_diagnostics_recovery_sites) {
    return false;
  }
  std::size_t normalized_total = 0u;
  if (!TryAddErrorDiagnosticsRecoverySiteCounts(
          normalized_sites, gate_blocked_sites, normalized_total)) {
    return false;
  }
  if (normalized_total != error_diagnostics_recovery_sites) {
    return false;
  }
  return contract_violation_sites == 0u;
}

static Objc3ErrorDiagnosticsRecoveryProfile BuildErrorDiagnosticsRecoveryProfileFromCounts(
    std::size_t diagnostic_emit_sites,
    std::size_t recovery_anchor_sites,
    std::size_t recovery_boundary_sites,
    std::size_t fail_closed_diagnostic_sites) {
  Objc3ErrorDiagnosticsRecoveryProfile profile;
  profile.diagnostic_emit_sites = diagnostic_emit_sites;
  profile.recovery_anchor_sites = recovery_anchor_sites;
  profile.recovery_boundary_sites = recovery_boundary_sites;
  profile.fail_closed_diagnostic_sites = fail_closed_diagnostic_sites;
  profile.error_diagnostics_recovery_sites = profile.diagnostic_emit_sites;
  std::size_t summed_sites = 0u;
  if (!TryAddErrorDiagnosticsRecoverySiteCounts(
          profile.error_diagnostics_recovery_sites,
          profile.recovery_anchor_sites,
          summed_sites)) {
    profile.error_diagnostics_recovery_sites = std::numeric_limits<std::size_t>::max();
    profile.contract_violation_sites += 1u;
  } else {
    profile.error_diagnostics_recovery_sites = summed_sites;
  }
  if (!TryAddErrorDiagnosticsRecoverySiteCounts(
          profile.error_diagnostics_recovery_sites,
          profile.recovery_boundary_sites,
          summed_sites)) {
    profile.error_diagnostics_recovery_sites = std::numeric_limits<std::size_t>::max();
    profile.contract_violation_sites += 1u;
  } else {
    profile.error_diagnostics_recovery_sites = summed_sites;
  }
  if (!TryAddErrorDiagnosticsRecoverySiteCounts(
          profile.error_diagnostics_recovery_sites,
          profile.fail_closed_diagnostic_sites,
          summed_sites)) {
    profile.error_diagnostics_recovery_sites = std::numeric_limits<std::size_t>::max();
    profile.contract_violation_sites += 1u;
  } else {
    profile.error_diagnostics_recovery_sites = summed_sites;
  }

  const bool gate_open =
      profile.diagnostic_emit_sites > 0u && profile.recovery_anchor_sites > 0u;
  std::size_t blocked_total = profile.recovery_boundary_sites;
  if (!TryAddErrorDiagnosticsRecoverySiteCounts(
          blocked_total,
          profile.fail_closed_diagnostic_sites,
          blocked_total)) {
    blocked_total = std::numeric_limits<std::size_t>::max();
    profile.contract_violation_sites += 1u;
  }
  profile.gate_blocked_sites = gate_open
                                   ? 0u
                                   : std::min(profile.error_diagnostics_recovery_sites, blocked_total);
  profile.normalized_sites =
      profile.error_diagnostics_recovery_sites - profile.gate_blocked_sites;

  if (profile.diagnostic_emit_sites > profile.error_diagnostics_recovery_sites ||
      profile.recovery_anchor_sites > profile.error_diagnostics_recovery_sites ||
      profile.recovery_boundary_sites > profile.error_diagnostics_recovery_sites ||
      profile.fail_closed_diagnostic_sites > profile.error_diagnostics_recovery_sites ||
      profile.normalized_sites > profile.error_diagnostics_recovery_sites ||
      profile.gate_blocked_sites > profile.error_diagnostics_recovery_sites ||
      profile.contract_violation_sites > profile.error_diagnostics_recovery_sites) {
    profile.contract_violation_sites += 1u;
  }
  std::size_t normalized_total = 0u;
  if (!TryAddErrorDiagnosticsRecoverySiteCounts(
          profile.normalized_sites,
          profile.gate_blocked_sites,
          normalized_total) ||
      normalized_total != profile.error_diagnostics_recovery_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (gate_open && profile.gate_blocked_sites != 0u) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.contract_violation_sites > profile.error_diagnostics_recovery_sites) {
    profile.contract_violation_sites = profile.error_diagnostics_recovery_sites;
  }
  profile.deterministic_error_diagnostics_recovery_handoff =
      profile.contract_violation_sites == 0u;
  return profile;
}

static Objc3ErrorDiagnosticsRecoveryProfile BuildErrorDiagnosticsRecoveryProfileFromFunction(
    const FunctionDecl &fn) {
  const Objc3ErrorDiagnosticsRecoverySiteCounts counts =
      CountErrorDiagnosticsRecoverySitesInBody(fn.body);
  return BuildErrorDiagnosticsRecoveryProfileFromCounts(
      counts.diagnostic_emit_sites,
      counts.recovery_anchor_sites,
      counts.recovery_boundary_sites,
      counts.fail_closed_diagnostic_sites);
}

static Objc3ErrorDiagnosticsRecoveryProfile BuildErrorDiagnosticsRecoveryProfileFromOpaqueBody(
    const Objc3MethodDecl &method) {
  Objc3ErrorDiagnosticsRecoverySiteCounts counts;
  if (method.has_body) {
    CollectErrorDiagnosticsRecoverySitesFromSymbol(method.selector, counts);
  }
  return BuildErrorDiagnosticsRecoveryProfileFromCounts(
      counts.diagnostic_emit_sites,
      counts.recovery_anchor_sites,
      counts.recovery_boundary_sites,
