  return profile;
}

static Objc3NSErrorBridgingProfile BuildNSErrorBridgingProfileFromFunction(const FunctionDecl &fn) {
  return BuildNSErrorBridgingProfileFromParameters(fn.params, CountFailableCallSitesInBody(fn.body));
}

static Objc3NSErrorBridgingProfile BuildNSErrorBridgingProfileFromOpaqueBody(const Objc3MethodDecl &method) {
  std::size_t raw_failable_call_sites = 0;
  if (method.has_body) {
    for (const auto &param : method.params) {
      if (IsNSErrorOutParameterSite(param)) {
        raw_failable_call_sites = 1u;
        break;
      }
    }
  }
  return BuildNSErrorBridgingProfileFromParameters(method.params, raw_failable_call_sites);
}

static bool IsUnsafeOwnershipQualifierSpelling(const std::string &spelling) {
  return spelling == "__unsafe_unretained";
}

static std::size_t CountRawPointerTypeSites(const std::vector<FuncParam> &params,
                                            bool has_return_pointer_declarator) {
  std::size_t sites = has_return_pointer_declarator ? 1u : 0u;
  for (const auto &param : params) {
    if (param.has_pointer_declarator) {
      sites += 1u;
    }
  }
  return sites;
}

static std::size_t CountUnsafeKeywordSites(const std::vector<FuncParam> &params,
                                           const std::string &return_ownership_qualifier_spelling) {
  std::size_t sites = IsUnsafeOwnershipQualifierSpelling(return_ownership_qualifier_spelling) ? 1u : 0u;
  for (const auto &param : params) {
    if (IsUnsafeOwnershipQualifierSpelling(param.ownership_qualifier_spelling)) {
      sites += 1u;
    }
  }
  return sites;
}

static bool IsPointerArithmeticMutationOperator(const std::string &op) {
  return op == "+=" || op == "-=" || op == "++" || op == "--";
}

static void CollectPointerArithmeticExprSites(const Expr *expr, std::size_t &sites) {
  if (expr == nullptr) {
    return;
  }

  switch (expr->kind) {
  case Expr::Kind::Binary:
    if (expr->op == "+" || expr->op == "-") {
      sites += 1u;
    }
    CollectPointerArithmeticExprSites(expr->left.get(), sites);
    CollectPointerArithmeticExprSites(expr->right.get(), sites);
    return;
  case Expr::Kind::Conditional:
    CollectPointerArithmeticExprSites(expr->left.get(), sites);
    CollectPointerArithmeticExprSites(expr->right.get(), sites);
    CollectPointerArithmeticExprSites(expr->third.get(), sites);
    return;
  case Expr::Kind::Call:
    for (const auto &arg : expr->args) {
      CollectPointerArithmeticExprSites(arg.get(), sites);
    }
    return;
  case Expr::Kind::MessageSend:
    CollectPointerArithmeticExprSites(expr->receiver.get(), sites);
    for (const auto &arg : expr->args) {
      CollectPointerArithmeticExprSites(arg.get(), sites);
    }
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

static void CollectPointerArithmeticForClauseSites(const ForClause &clause, std::size_t &sites) {
  if (clause.kind == ForClause::Kind::Assign && IsPointerArithmeticMutationOperator(clause.op)) {
    sites += 1u;
  }
  CollectPointerArithmeticExprSites(clause.value.get(), sites);
}

static void CollectPointerArithmeticStmtSites(const Stmt *stmt, std::size_t &sites) {
  if (stmt == nullptr) {
    return;
  }

  switch (stmt->kind) {
  case Stmt::Kind::Let:
    if (stmt->let_stmt != nullptr) {
      CollectPointerArithmeticExprSites(stmt->let_stmt->value.get(), sites);
    }
    return;
  case Stmt::Kind::Assign:
    if (stmt->assign_stmt != nullptr) {
      if (IsPointerArithmeticMutationOperator(stmt->assign_stmt->op)) {
        sites += 1u;
      }
      CollectPointerArithmeticExprSites(stmt->assign_stmt->value.get(), sites);
    }
    return;
  case Stmt::Kind::Return:
    if (stmt->return_stmt != nullptr) {
      CollectPointerArithmeticExprSites(stmt->return_stmt->value.get(), sites);
    }
    return;
  case Stmt::Kind::If:
    if (stmt->if_stmt == nullptr) {
      return;
    }
    CollectPointerArithmeticExprSites(stmt->if_stmt->condition.get(), sites);
    for (const auto &then_stmt : stmt->if_stmt->then_body) {
      CollectPointerArithmeticStmtSites(then_stmt.get(), sites);
    }
    for (const auto &else_stmt : stmt->if_stmt->else_body) {
      CollectPointerArithmeticStmtSites(else_stmt.get(), sites);
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->do_while_stmt->body) {
      CollectPointerArithmeticStmtSites(body_stmt.get(), sites);
    }
    CollectPointerArithmeticExprSites(stmt->do_while_stmt->condition.get(), sites);
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt == nullptr) {
      return;
    }
    CollectPointerArithmeticForClauseSites(stmt->for_stmt->init, sites);
    CollectPointerArithmeticExprSites(stmt->for_stmt->condition.get(), sites);
    CollectPointerArithmeticForClauseSites(stmt->for_stmt->step, sites);
    for (const auto &body_stmt : stmt->for_stmt->body) {
      CollectPointerArithmeticStmtSites(body_stmt.get(), sites);
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt == nullptr) {
      return;
    }
    CollectPointerArithmeticExprSites(stmt->switch_stmt->condition.get(), sites);
    for (const auto &switch_case : stmt->switch_stmt->cases) {
      for (const auto &case_stmt : switch_case.body) {
        CollectPointerArithmeticStmtSites(case_stmt.get(), sites);
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt == nullptr) {
      return;
    }
    CollectPointerArithmeticExprSites(stmt->while_stmt->condition.get(), sites);
    for (const auto &body_stmt : stmt->while_stmt->body) {
      CollectPointerArithmeticStmtSites(body_stmt.get(), sites);
    }
    return;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    if (stmt->block_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->block_stmt->body) {
      CollectPointerArithmeticStmtSites(body_stmt.get(), sites);
    }
    return;
  case Stmt::Kind::Expr:
    if (stmt->expr_stmt != nullptr) {
      CollectPointerArithmeticExprSites(stmt->expr_stmt->value.get(), sites);
    }
    return;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  }
}

static std::size_t CountPointerArithmeticSitesInBody(const std::vector<std::unique_ptr<Stmt>> &body) {
  std::size_t sites = 0;
  for (const auto &stmt : body) {
    CollectPointerArithmeticStmtSites(stmt.get(), sites);
  }
  return sites;
}

struct Objc3UnsafePointerExtensionProfile {
  std::size_t unsafe_pointer_extension_sites = 0;
  std::size_t unsafe_keyword_sites = 0;
  std::size_t pointer_arithmetic_sites = 0;
  std::size_t raw_pointer_type_sites = 0;
  std::size_t unsafe_operation_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t gate_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic_unsafe_pointer_extension_handoff = false;
};

static std::string BuildUnsafePointerExtensionProfile(
    std::size_t unsafe_pointer_extension_sites,
    std::size_t unsafe_keyword_sites,
    std::size_t pointer_arithmetic_sites,
    std::size_t raw_pointer_type_sites,
    std::size_t unsafe_operation_sites,
    std::size_t normalized_sites,
    std::size_t gate_blocked_sites,
    std::size_t contract_violation_sites,
    bool deterministic_unsafe_pointer_extension_handoff) {
  std::ostringstream out;
  out << "unsafe-pointer-extension:unsafe_pointer_extension_sites="
      << unsafe_pointer_extension_sites
      << ";unsafe_keyword_sites=" << unsafe_keyword_sites
      << ";pointer_arithmetic_sites=" << pointer_arithmetic_sites
      << ";raw_pointer_type_sites=" << raw_pointer_type_sites
      << ";unsafe_operation_sites=" << unsafe_operation_sites
      << ";normalized_sites=" << normalized_sites
      << ";gate_blocked_sites=" << gate_blocked_sites
      << ";contract_violation_sites=" << contract_violation_sites
      << ";deterministic_unsafe_pointer_extension_handoff="
      << (deterministic_unsafe_pointer_extension_handoff ? "true" : "false");
  return out.str();
}

static bool IsUnsafePointerExtensionProfileNormalized(
    std::size_t unsafe_pointer_extension_sites,
    std::size_t unsafe_keyword_sites,
    std::size_t pointer_arithmetic_sites,
    std::size_t raw_pointer_type_sites,
    std::size_t unsafe_operation_sites,
    std::size_t normalized_sites,
    std::size_t gate_blocked_sites,
    std::size_t contract_violation_sites) {
  if (unsafe_keyword_sites > unsafe_pointer_extension_sites ||
      pointer_arithmetic_sites > unsafe_pointer_extension_sites ||
      raw_pointer_type_sites > unsafe_pointer_extension_sites ||
      unsafe_operation_sites > unsafe_pointer_extension_sites ||
      normalized_sites > unsafe_pointer_extension_sites ||
      gate_blocked_sites > unsafe_pointer_extension_sites) {
    return false;
  }
  if (normalized_sites + gate_blocked_sites != unsafe_pointer_extension_sites) {
    return false;
  }
  return contract_violation_sites == 0u;
}

static Objc3UnsafePointerExtensionProfile BuildUnsafePointerExtensionProfileFromCounts(
    std::size_t unsafe_keyword_sites,
    std::size_t pointer_arithmetic_sites,
    std::size_t raw_pointer_type_sites) {
  Objc3UnsafePointerExtensionProfile profile;
  profile.unsafe_keyword_sites = unsafe_keyword_sites;
  profile.pointer_arithmetic_sites = pointer_arithmetic_sites;
  profile.raw_pointer_type_sites = raw_pointer_type_sites;
  profile.unsafe_operation_sites = pointer_arithmetic_sites + raw_pointer_type_sites;
  profile.unsafe_pointer_extension_sites =
      profile.unsafe_keyword_sites + profile.pointer_arithmetic_sites + profile.raw_pointer_type_sites;

  const bool gate_open = unsafe_keyword_sites > 0u;
  profile.gate_blocked_sites =
      gate_open ? 0u : (profile.pointer_arithmetic_sites + profile.raw_pointer_type_sites);
  profile.normalized_sites = profile.unsafe_pointer_extension_sites - profile.gate_blocked_sites;

  if (profile.unsafe_keyword_sites > profile.unsafe_pointer_extension_sites ||
      profile.pointer_arithmetic_sites > profile.unsafe_pointer_extension_sites ||
      profile.raw_pointer_type_sites > profile.unsafe_pointer_extension_sites ||
      profile.unsafe_operation_sites > profile.unsafe_pointer_extension_sites ||
      profile.normalized_sites > profile.unsafe_pointer_extension_sites ||
      profile.gate_blocked_sites > profile.unsafe_pointer_extension_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.normalized_sites + profile.gate_blocked_sites != profile.unsafe_pointer_extension_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (!gate_open && profile.normalized_sites != profile.unsafe_keyword_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (gate_open && profile.gate_blocked_sites != 0u) {
    profile.contract_violation_sites += 1u;
  }

  profile.deterministic_unsafe_pointer_extension_handoff =
      profile.contract_violation_sites == 0u;
  return profile;
}

static Objc3UnsafePointerExtensionProfile BuildUnsafePointerExtensionProfileFromFunction(
    const FunctionDecl &fn) {
  const std::size_t unsafe_keyword_sites =
      CountUnsafeKeywordSites(fn.params, fn.return_ownership_qualifier_spelling);
  const std::size_t raw_pointer_type_sites =
      CountRawPointerTypeSites(fn.params, fn.has_return_pointer_declarator);
  const std::size_t pointer_arithmetic_sites = CountPointerArithmeticSitesInBody(fn.body);
  return BuildUnsafePointerExtensionProfileFromCounts(
      unsafe_keyword_sites, pointer_arithmetic_sites, raw_pointer_type_sites);
}

static Objc3UnsafePointerExtensionProfile BuildUnsafePointerExtensionProfileFromOpaqueBody(
    const Objc3MethodDecl &method) {
  const std::size_t unsafe_keyword_sites =
      CountUnsafeKeywordSites(method.params, method.return_ownership_qualifier_spelling);
  const std::size_t raw_pointer_type_sites =
      CountRawPointerTypeSites(method.params, method.has_return_pointer_declarator);
  const std::size_t pointer_arithmetic_sites =
      method.has_body && raw_pointer_type_sites > 0u ? 1u : 0u;
  return BuildUnsafePointerExtensionProfileFromCounts(
      unsafe_keyword_sites, pointer_arithmetic_sites, raw_pointer_type_sites);
}

static bool IsInlineAsmCallSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered == "asm" || lowered == "__asm" || lowered == "__asm__" ||
         lowered.rfind("asm_", 0) == 0 || lowered.rfind("__asm_", 0) == 0 ||
         lowered.find("inline_asm") != std::string::npos;
}

static bool IsIntrinsicCallSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.rfind("__builtin_", 0) == 0 || lowered.rfind("llvm.", 0) == 0 ||
         lowered.rfind("llvm_", 0) == 0 || lowered.find("intrinsic") != std::string::npos;
}

static bool IsPrivilegedIntrinsicCallSymbol(const std::string &symbol) {
  if (!IsIntrinsicCallSymbol(symbol)) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("privileged") != std::string::npos ||
         lowered.find("unsafe") != std::string::npos ||
         lowered.find("syscall") != std::string::npos ||
         lowered.rfind("__builtin_ia32_", 0) == 0 ||
         lowered.rfind("__builtin_arm_", 0) == 0;
}

struct Objc3InlineAsmIntrinsicSiteCounts {
  std::size_t inline_asm_sites = 0;
  std::size_t intrinsic_sites = 0;
  std::size_t governed_intrinsic_sites = 0;
  std::size_t privileged_intrinsic_sites = 0;
};

static void CollectInlineAsmIntrinsicSitesFromSymbol(
    const std::string &symbol,
    Objc3InlineAsmIntrinsicSiteCounts &counts) {
  if (IsInlineAsmCallSymbol(symbol)) {
    counts.inline_asm_sites += 1u;
  }
  if (IsIntrinsicCallSymbol(symbol)) {
    counts.intrinsic_sites += 1u;
    counts.governed_intrinsic_sites += 1u;
    if (IsPrivilegedIntrinsicCallSymbol(symbol)) {
      counts.privileged_intrinsic_sites += 1u;
    }
  }
}

static void CollectInlineAsmIntrinsicExprSites(
    const Expr *expr,
    Objc3InlineAsmIntrinsicSiteCounts &counts) {
  if (expr == nullptr) {
    return;
  }
  switch (expr->kind) {
  case Expr::Kind::Call:
    CollectInlineAsmIntrinsicSitesFromSymbol(expr->ident, counts);
    for (const auto &arg : expr->args) {
      CollectInlineAsmIntrinsicExprSites(arg.get(), counts);
    }
    return;
  case Expr::Kind::MessageSend:
    CollectInlineAsmIntrinsicSitesFromSymbol(expr->selector, counts);
    CollectInlineAsmIntrinsicExprSites(expr->receiver.get(), counts);
    for (const auto &arg : expr->args) {
      CollectInlineAsmIntrinsicExprSites(arg.get(), counts);
    }
    return;
  case Expr::Kind::Binary:
    CollectInlineAsmIntrinsicExprSites(expr->left.get(), counts);
    CollectInlineAsmIntrinsicExprSites(expr->right.get(), counts);
    return;
  case Expr::Kind::Conditional:
    CollectInlineAsmIntrinsicExprSites(expr->left.get(), counts);
    CollectInlineAsmIntrinsicExprSites(expr->right.get(), counts);
    CollectInlineAsmIntrinsicExprSites(expr->third.get(), counts);
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

static void CollectInlineAsmIntrinsicForClauseSites(
    const ForClause &clause,
    Objc3InlineAsmIntrinsicSiteCounts &counts) {
  CollectInlineAsmIntrinsicExprSites(clause.value.get(), counts);
}

static void CollectInlineAsmIntrinsicStmtSites(
    const Stmt *stmt,
    Objc3InlineAsmIntrinsicSiteCounts &counts) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
    if (stmt->let_stmt != nullptr) {
      CollectInlineAsmIntrinsicExprSites(stmt->let_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::Assign:
    if (stmt->assign_stmt != nullptr) {
      CollectInlineAsmIntrinsicExprSites(stmt->assign_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::Return:
    if (stmt->return_stmt != nullptr) {
      CollectInlineAsmIntrinsicExprSites(stmt->return_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::If:
    if (stmt->if_stmt == nullptr) {
      return;
    }
    CollectInlineAsmIntrinsicExprSites(stmt->if_stmt->condition.get(), counts);
    for (const auto &then_stmt : stmt->if_stmt->then_body) {
      CollectInlineAsmIntrinsicStmtSites(then_stmt.get(), counts);
    }
    for (const auto &else_stmt : stmt->if_stmt->else_body) {
      CollectInlineAsmIntrinsicStmtSites(else_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->do_while_stmt->body) {
      CollectInlineAsmIntrinsicStmtSites(body_stmt.get(), counts);
    }
    CollectInlineAsmIntrinsicExprSites(stmt->do_while_stmt->condition.get(), counts);
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt == nullptr) {
      return;
    }
    CollectInlineAsmIntrinsicForClauseSites(stmt->for_stmt->init, counts);
    CollectInlineAsmIntrinsicExprSites(stmt->for_stmt->condition.get(), counts);
    CollectInlineAsmIntrinsicForClauseSites(stmt->for_stmt->step, counts);
    for (const auto &body_stmt : stmt->for_stmt->body) {
      CollectInlineAsmIntrinsicStmtSites(body_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt == nullptr) {
      return;
    }
    CollectInlineAsmIntrinsicExprSites(stmt->switch_stmt->condition.get(), counts);
    for (const auto &switch_case : stmt->switch_stmt->cases) {
      for (const auto &case_stmt : switch_case.body) {
        CollectInlineAsmIntrinsicStmtSites(case_stmt.get(), counts);
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt == nullptr) {
      return;
    }
    CollectInlineAsmIntrinsicExprSites(stmt->while_stmt->condition.get(), counts);
    for (const auto &body_stmt : stmt->while_stmt->body) {
      CollectInlineAsmIntrinsicStmtSites(body_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    if (stmt->block_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->block_stmt->body) {
      CollectInlineAsmIntrinsicStmtSites(body_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::Expr:
    if (stmt->expr_stmt != nullptr) {
      CollectInlineAsmIntrinsicExprSites(stmt->expr_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  }
}

static Objc3InlineAsmIntrinsicSiteCounts CountInlineAsmIntrinsicSitesInBody(
    const std::vector<std::unique_ptr<Stmt>> &body) {
  Objc3InlineAsmIntrinsicSiteCounts counts;
  for (const auto &stmt : body) {
    CollectInlineAsmIntrinsicStmtSites(stmt.get(), counts);
  }
  return counts;
}

struct Objc3InlineAsmIntrinsicGovernanceProfile {
  std::size_t inline_asm_intrinsic_sites = 0;
  std::size_t inline_asm_sites = 0;
  std::size_t intrinsic_sites = 0;
  std::size_t governed_intrinsic_sites = 0;
  std::size_t privileged_intrinsic_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t gate_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic_inline_asm_intrinsic_governance_handoff = false;
};

static std::string BuildInlineAsmIntrinsicGovernanceProfile(
    std::size_t inline_asm_intrinsic_sites,
    std::size_t inline_asm_sites,
    std::size_t intrinsic_sites,
    std::size_t governed_intrinsic_sites,
    std::size_t privileged_intrinsic_sites,
    std::size_t normalized_sites,
    std::size_t gate_blocked_sites,
    std::size_t contract_violation_sites,
    bool deterministic_inline_asm_intrinsic_governance_handoff) {
  std::ostringstream out;
  out << "inline-asm-intrinsic-governance:inline_asm_intrinsic_sites="
      << inline_asm_intrinsic_sites
      << ";inline_asm_sites=" << inline_asm_sites
      << ";intrinsic_sites=" << intrinsic_sites
      << ";governed_intrinsic_sites=" << governed_intrinsic_sites
      << ";privileged_intrinsic_sites=" << privileged_intrinsic_sites
      << ";normalized_sites=" << normalized_sites
      << ";gate_blocked_sites=" << gate_blocked_sites
      << ";contract_violation_sites=" << contract_violation_sites
      << ";deterministic_inline_asm_intrinsic_governance_handoff="
      << (deterministic_inline_asm_intrinsic_governance_handoff ? "true" : "false");
  return out.str();
}

static bool IsInlineAsmIntrinsicGovernanceProfileNormalized(
    std::size_t inline_asm_intrinsic_sites,
    std::size_t inline_asm_sites,
    std::size_t intrinsic_sites,
    std::size_t governed_intrinsic_sites,
    std::size_t privileged_intrinsic_sites,
    std::size_t normalized_sites,
    std::size_t gate_blocked_sites,
    std::size_t contract_violation_sites) {
  if (inline_asm_sites > inline_asm_intrinsic_sites ||
      intrinsic_sites > inline_asm_intrinsic_sites ||
      governed_intrinsic_sites > intrinsic_sites ||
      privileged_intrinsic_sites > governed_intrinsic_sites ||
      normalized_sites > inline_asm_intrinsic_sites ||
      gate_blocked_sites > inline_asm_intrinsic_sites ||
      contract_violation_sites > inline_asm_intrinsic_sites) {
    return false;
  }
  if (normalized_sites + gate_blocked_sites != inline_asm_intrinsic_sites) {
    return false;
  }
  return contract_violation_sites == 0u;
}

static Objc3InlineAsmIntrinsicGovernanceProfile BuildInlineAsmIntrinsicGovernanceProfileFromCounts(
    std::size_t inline_asm_sites,
    std::size_t intrinsic_sites,
    std::size_t governed_intrinsic_sites,
    std::size_t privileged_intrinsic_sites) {
  Objc3InlineAsmIntrinsicGovernanceProfile profile;
  profile.inline_asm_sites = inline_asm_sites;
  profile.intrinsic_sites = intrinsic_sites;
  profile.governed_intrinsic_sites = governed_intrinsic_sites;
  profile.privileged_intrinsic_sites = privileged_intrinsic_sites;
  profile.inline_asm_intrinsic_sites = profile.inline_asm_sites + profile.intrinsic_sites;
  profile.gate_blocked_sites = profile.privileged_intrinsic_sites;
  if (profile.gate_blocked_sites > profile.inline_asm_intrinsic_sites) {
    profile.normalized_sites = 0u;
  } else {
    profile.normalized_sites =
        profile.inline_asm_intrinsic_sites - profile.gate_blocked_sites;
  }

  if (profile.inline_asm_sites > profile.inline_asm_intrinsic_sites ||
      profile.intrinsic_sites > profile.inline_asm_intrinsic_sites ||
      profile.governed_intrinsic_sites > profile.intrinsic_sites ||
      profile.privileged_intrinsic_sites > profile.governed_intrinsic_sites ||
      profile.normalized_sites > profile.inline_asm_intrinsic_sites ||
      profile.gate_blocked_sites > profile.inline_asm_intrinsic_sites ||
      profile.contract_violation_sites > profile.inline_asm_intrinsic_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.normalized_sites + profile.gate_blocked_sites != profile.inline_asm_intrinsic_sites) {
    profile.contract_violation_sites += 1u;
  }

  profile.deterministic_inline_asm_intrinsic_governance_handoff =
      profile.contract_violation_sites == 0u;
  return profile;
}

static Objc3InlineAsmIntrinsicGovernanceProfile BuildInlineAsmIntrinsicGovernanceProfileFromFunction(
    const FunctionDecl &fn) {
  const Objc3InlineAsmIntrinsicSiteCounts counts = CountInlineAsmIntrinsicSitesInBody(fn.body);
  return BuildInlineAsmIntrinsicGovernanceProfileFromCounts(
      counts.inline_asm_sites,
      counts.intrinsic_sites,
      counts.governed_intrinsic_sites,
      counts.privileged_intrinsic_sites);
}

static Objc3InlineAsmIntrinsicGovernanceProfile BuildInlineAsmIntrinsicGovernanceProfileFromOpaqueBody(
    const Objc3MethodDecl &method) {
  Objc3InlineAsmIntrinsicSiteCounts counts;
  if (method.has_body) {
    CollectInlineAsmIntrinsicSitesFromSymbol(method.selector, counts);
  }
  return BuildInlineAsmIntrinsicGovernanceProfileFromCounts(
      counts.inline_asm_sites,
      counts.intrinsic_sites,
      counts.governed_intrinsic_sites,
      counts.privileged_intrinsic_sites);
}

static bool IsExceptionalExitSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("throw") != std::string::npos ||
         lowered.find("exception") != std::string::npos ||
         lowered.find("unwind") != std::string::npos ||
         lowered.find("raise") != std::string::npos;
}

static bool IsCleanupActionSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("cleanup") != std::string::npos ||
         lowered.find("defer") != std::string::npos ||
         lowered.find("finally") != std::string::npos ||
         lowered.find("release") != std::string::npos;
}

static bool IsCleanupScopeSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("cleanup_scope") != std::string::npos ||
         lowered.find("scope_guard") != std::string::npos ||
         lowered.find("defer_scope") != std::string::npos ||
         lowered.find("guard_scope") != std::string::npos;
}

static bool IsCleanupResumeSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("resume") != std::string::npos ||
         lowered.find("rethrow") != std::string::npos ||
         lowered.find("continue_unwind") != std::string::npos;
}

struct Objc3UnwindCleanupSiteCounts {
  std::size_t exceptional_exit_sites = 0;
  std::size_t cleanup_action_sites = 0;
  std::size_t cleanup_scope_sites = 0;
  std::size_t cleanup_resume_sites = 0;
};

static void CollectUnwindCleanupSitesFromSymbol(
    const std::string &symbol,
    Objc3UnwindCleanupSiteCounts &counts) {
  if (IsExceptionalExitSymbol(symbol)) {
    counts.exceptional_exit_sites += 1u;
  }
  if (IsCleanupActionSymbol(symbol)) {
    counts.cleanup_action_sites += 1u;
  }
  if (IsCleanupScopeSymbol(symbol)) {
    counts.cleanup_scope_sites += 1u;
  }
  if (IsCleanupResumeSymbol(symbol)) {
    counts.cleanup_resume_sites += 1u;
  }
}

static void CollectUnwindCleanupExprSites(
    const Expr *expr,
    Objc3UnwindCleanupSiteCounts &counts) {
  if (expr == nullptr) {
    return;
  }
  switch (expr->kind) {
  case Expr::Kind::Call:
    CollectUnwindCleanupSitesFromSymbol(expr->ident, counts);
    for (const auto &arg : expr->args) {
      CollectUnwindCleanupExprSites(arg.get(), counts);
    }
    return;
  case Expr::Kind::MessageSend:
    CollectUnwindCleanupSitesFromSymbol(expr->selector, counts);
    CollectUnwindCleanupExprSites(expr->receiver.get(), counts);
    for (const auto &arg : expr->args) {
      CollectUnwindCleanupExprSites(arg.get(), counts);
    }
    return;
  case Expr::Kind::Binary:
    CollectUnwindCleanupExprSites(expr->left.get(), counts);
    CollectUnwindCleanupExprSites(expr->right.get(), counts);
    return;
  case Expr::Kind::Conditional:
    CollectUnwindCleanupExprSites(expr->left.get(), counts);
    CollectUnwindCleanupExprSites(expr->right.get(), counts);
    CollectUnwindCleanupExprSites(expr->third.get(), counts);
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

static void CollectUnwindCleanupForClauseSites(
    const ForClause &clause,
    Objc3UnwindCleanupSiteCounts &counts) {
  CollectUnwindCleanupExprSites(clause.value.get(), counts);
}

static void CollectUnwindCleanupStmtSites(
    const Stmt *stmt,
    Objc3UnwindCleanupSiteCounts &counts) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
    if (stmt->let_stmt != nullptr) {
      CollectUnwindCleanupExprSites(stmt->let_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::Assign:
    if (stmt->assign_stmt != nullptr) {
      CollectUnwindCleanupExprSites(stmt->assign_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::Return:
    if (stmt->return_stmt != nullptr) {
      CollectUnwindCleanupExprSites(stmt->return_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::If:
    if (stmt->if_stmt == nullptr) {
      return;
    }
    CollectUnwindCleanupExprSites(stmt->if_stmt->condition.get(), counts);
    for (const auto &then_stmt : stmt->if_stmt->then_body) {
      CollectUnwindCleanupStmtSites(then_stmt.get(), counts);
    }
    for (const auto &else_stmt : stmt->if_stmt->else_body) {
      CollectUnwindCleanupStmtSites(else_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->do_while_stmt->body) {
      CollectUnwindCleanupStmtSites(body_stmt.get(), counts);
    }
    CollectUnwindCleanupExprSites(stmt->do_while_stmt->condition.get(), counts);
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt == nullptr) {
      return;
    }
    CollectUnwindCleanupForClauseSites(stmt->for_stmt->init, counts);
    CollectUnwindCleanupExprSites(stmt->for_stmt->condition.get(), counts);
    CollectUnwindCleanupForClauseSites(stmt->for_stmt->step, counts);
    for (const auto &body_stmt : stmt->for_stmt->body) {
      CollectUnwindCleanupStmtSites(body_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt == nullptr) {
      return;
    }
    CollectUnwindCleanupExprSites(stmt->switch_stmt->condition.get(), counts);
    for (const auto &switch_case : stmt->switch_stmt->cases) {
      for (const auto &case_stmt : switch_case.body) {
        CollectUnwindCleanupStmtSites(case_stmt.get(), counts);
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt == nullptr) {
      return;
    }
    CollectUnwindCleanupExprSites(stmt->while_stmt->condition.get(), counts);
    for (const auto &body_stmt : stmt->while_stmt->body) {
      CollectUnwindCleanupStmtSites(body_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    if (stmt->block_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->block_stmt->body) {
      CollectUnwindCleanupStmtSites(body_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::Expr:
    if (stmt->expr_stmt != nullptr) {
      CollectUnwindCleanupExprSites(stmt->expr_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  }
}

static Objc3UnwindCleanupSiteCounts CountUnwindCleanupSitesInBody(
    const std::vector<std::unique_ptr<Stmt>> &body) {
  Objc3UnwindCleanupSiteCounts counts;
  for (const auto &stmt : body) {
    CollectUnwindCleanupStmtSites(stmt.get(), counts);
  }
  return counts;
}

struct Objc3UnwindCleanupProfile {
  std::size_t unwind_cleanup_sites = 0;
  std::size_t exceptional_exit_sites = 0;
  std::size_t cleanup_action_sites = 0;
  std::size_t cleanup_scope_sites = 0;
  std::size_t cleanup_resume_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t fail_closed_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic_unwind_cleanup_handoff = false;
};

static std::string BuildUnwindCleanupProfile(
    std::size_t unwind_cleanup_sites,
    std::size_t exceptional_exit_sites,
    std::size_t cleanup_action_sites,
    std::size_t cleanup_scope_sites,
    std::size_t cleanup_resume_sites,
    std::size_t normalized_sites,
    std::size_t fail_closed_sites,
    std::size_t contract_violation_sites,
    bool deterministic_unwind_cleanup_handoff) {
  std::ostringstream out;
  out << "unwind-cleanup:unwind_cleanup_sites=" << unwind_cleanup_sites
      << ";exceptional_exit_sites=" << exceptional_exit_sites
      << ";cleanup_action_sites=" << cleanup_action_sites
      << ";cleanup_scope_sites=" << cleanup_scope_sites
      << ";cleanup_resume_sites=" << cleanup_resume_sites
      << ";normalized_sites=" << normalized_sites
      << ";fail_closed_sites=" << fail_closed_sites
      << ";contract_violation_sites=" << contract_violation_sites
      << ";deterministic_unwind_cleanup_handoff="
      << (deterministic_unwind_cleanup_handoff ? "true" : "false");
  return out.str();
}

static bool IsUnwindCleanupProfileNormalized(
    std::size_t unwind_cleanup_sites,
    std::size_t exceptional_exit_sites,
    std::size_t cleanup_action_sites,
    std::size_t cleanup_scope_sites,
    std::size_t cleanup_resume_sites,
    std::size_t normalized_sites,
    std::size_t fail_closed_sites,
    std::size_t contract_violation_sites) {
  if (exceptional_exit_sites > unwind_cleanup_sites ||
      cleanup_action_sites > unwind_cleanup_sites ||
      cleanup_scope_sites > unwind_cleanup_sites ||
      cleanup_resume_sites > unwind_cleanup_sites ||
      normalized_sites > unwind_cleanup_sites ||
      fail_closed_sites > unwind_cleanup_sites ||
      contract_violation_sites > unwind_cleanup_sites) {
    return false;
  }
  if (normalized_sites + fail_closed_sites != unwind_cleanup_sites) {
    return false;
  }
  return contract_violation_sites == 0u;
}

static Objc3UnwindCleanupProfile BuildUnwindCleanupProfileFromCounts(
    std::size_t exceptional_exit_sites,
    std::size_t cleanup_action_sites,
    std::size_t cleanup_scope_sites,
    std::size_t cleanup_resume_sites) {
  Objc3UnwindCleanupProfile profile;
  profile.exceptional_exit_sites = exceptional_exit_sites;
  profile.cleanup_action_sites = cleanup_action_sites;
  profile.cleanup_scope_sites = cleanup_scope_sites;
  profile.cleanup_resume_sites = cleanup_resume_sites;
  profile.unwind_cleanup_sites = profile.exceptional_exit_sites;
  if (profile.unwind_cleanup_sites >
      std::numeric_limits<std::size_t>::max() - profile.cleanup_action_sites) {
    profile.unwind_cleanup_sites = std::numeric_limits<std::size_t>::max();
    profile.contract_violation_sites += 1u;
  } else {
    profile.unwind_cleanup_sites += profile.cleanup_action_sites;
  }
  if (profile.unwind_cleanup_sites >
      std::numeric_limits<std::size_t>::max() - profile.cleanup_scope_sites) {
    profile.unwind_cleanup_sites = std::numeric_limits<std::size_t>::max();
    profile.contract_violation_sites += 1u;
  } else {
    profile.unwind_cleanup_sites += profile.cleanup_scope_sites;
  }
  profile.fail_closed_sites =
      std::min(profile.unwind_cleanup_sites, profile.cleanup_resume_sites);
  profile.normalized_sites = profile.unwind_cleanup_sites - profile.fail_closed_sites;
  if (profile.exceptional_exit_sites > profile.unwind_cleanup_sites ||
      profile.cleanup_action_sites > profile.unwind_cleanup_sites ||
      profile.cleanup_scope_sites > profile.unwind_cleanup_sites ||
      profile.cleanup_resume_sites > profile.unwind_cleanup_sites ||
      profile.normalized_sites > profile.unwind_cleanup_sites ||
      profile.fail_closed_sites > profile.unwind_cleanup_sites ||
      profile.contract_violation_sites > profile.unwind_cleanup_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.normalized_sites + profile.fail_closed_sites != profile.unwind_cleanup_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.contract_violation_sites > profile.unwind_cleanup_sites) {
    profile.contract_violation_sites = profile.unwind_cleanup_sites;
  }
  profile.deterministic_unwind_cleanup_handoff =
      profile.contract_violation_sites == 0u;
  return profile;
}

static Objc3UnwindCleanupProfile BuildUnwindCleanupProfileFromFunction(
    const FunctionDecl &fn) {
  const Objc3UnwindCleanupSiteCounts counts =
      CountUnwindCleanupSitesInBody(fn.body);
  return BuildUnwindCleanupProfileFromCounts(
      counts.exceptional_exit_sites,
      counts.cleanup_action_sites,
      counts.cleanup_scope_sites,
      counts.cleanup_resume_sites);
}

static Objc3UnwindCleanupProfile BuildUnwindCleanupProfileFromOpaqueBody(
    const Objc3MethodDecl &method) {
  Objc3UnwindCleanupSiteCounts counts;
  if (method.has_body) {
    CollectUnwindCleanupSitesFromSymbol(method.selector, counts);
  }
  return BuildUnwindCleanupProfileFromCounts(
      counts.exceptional_exit_sites,
      counts.cleanup_action_sites,
      counts.cleanup_scope_sites,
      counts.cleanup_resume_sites);
}

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
         lowered.find("fallback") != std::string::npos ||
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
