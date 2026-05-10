      auto body_scopes = scopes;
      body_scopes.push_back({});
      DiagnoseOwnershipResourceMoveStatements(do_while_stmt->body, body_scopes,
                                          diagnostics);
      MergeOwnershipResourceMoveState(scopes, body_scopes);
      DiagnoseOwnershipResourceMoveExpr(do_while_stmt->condition.get(), scopes,
                                    diagnostics);
      return;
    }
    case Stmt::Kind::For: {
      const ForStmt *for_stmt = stmt->for_stmt.get();
      if (for_stmt == nullptr) {
        return;
      }
      auto loop_scopes = scopes;
      loop_scopes.push_back({});
      DiagnoseOwnershipResourceMoveForClause(for_stmt->init, loop_scopes,
                                         diagnostics);
      DiagnoseOwnershipResourceMoveExpr(for_stmt->condition.get(), loop_scopes,
                                    diagnostics);
      DiagnoseOwnershipResourceMoveForClause(for_stmt->step, loop_scopes,
                                         diagnostics);
      loop_scopes.push_back({});
      DiagnoseOwnershipResourceMoveStatements(for_stmt->body, loop_scopes,
                                          diagnostics);
      MergeOwnershipResourceMoveState(scopes, loop_scopes);
      return;
    }
    case Stmt::Kind::Switch: {
      const SwitchStmt *switch_stmt = stmt->switch_stmt.get();
      if (switch_stmt == nullptr) {
        return;
      }
      DiagnoseOwnershipResourceMoveExpr(switch_stmt->condition.get(), scopes,
                                    diagnostics);
      for (const auto &case_stmt : switch_stmt->cases) {
        auto case_scopes = scopes;
        case_scopes.push_back({});
        DiagnoseOwnershipResourceMoveStatements(case_stmt.body, case_scopes,
                                            diagnostics);
        MergeOwnershipResourceMoveState(scopes, case_scopes);
      }
      return;
    }
    case Stmt::Kind::While: {
      const WhileStmt *while_stmt = stmt->while_stmt.get();
      if (while_stmt == nullptr) {
        return;
      }
      DiagnoseOwnershipResourceMoveExpr(while_stmt->condition.get(), scopes,
                                    diagnostics);
      auto body_scopes = scopes;
      body_scopes.push_back({});
      DiagnoseOwnershipResourceMoveStatements(while_stmt->body, body_scopes,
                                          diagnostics);
      MergeOwnershipResourceMoveState(scopes, body_scopes);
      return;
    }
    case Stmt::Kind::Block:
      if (stmt->block_stmt != nullptr) {
        auto block_scopes = scopes;
        block_scopes.push_back({});
        DiagnoseOwnershipResourceMoveStatements(stmt->block_stmt->body, block_scopes,
                                            diagnostics);
        MergeOwnershipResourceMoveState(scopes, block_scopes);
      }
      return;
    case Stmt::Kind::Defer:
      if (stmt->block_stmt != nullptr) {
        auto defer_scopes = scopes;
        defer_scopes.push_back({});
        DiagnoseOwnershipResourceMoveStatements(stmt->block_stmt->body, defer_scopes,
                                            diagnostics);
      }
      return;
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return;
  }
}

static void DiagnoseOwnershipResourceMoveStatements(
    const std::vector<std::unique_ptr<Stmt>> &statements,
    std::vector<OwnershipResourceMoveScope> &scopes,
    std::vector<std::string> &diagnostics) {
  for (const auto &stmt : statements) {
    DiagnoseOwnershipResourceMoveStatement(stmt.get(), scopes, diagnostics);
  }
}

static void DiagnoseOwnershipResourceMoveSemantics(
    const std::vector<std::unique_ptr<Stmt>> &statements,
    const std::vector<SemanticScope> &semantic_scopes,
    std::vector<std::string> &diagnostics) {
  std::vector<OwnershipResourceMoveScope> scopes;
  scopes.reserve(semantic_scopes.size());
  for (const auto &semantic_scope : semantic_scopes) {
    OwnershipResourceMoveScope scope;
    for (const auto &[name, _] : semantic_scope) {
      scope[name] = {};
    }
    scopes.push_back(std::move(scope));
  }
  DiagnoseOwnershipResourceMoveStatements(statements, scopes, diagnostics);
}

struct OwnershipResourceMoveSiteProfile {
  std::size_t cleanup_owned_local_sites = 0;
  std::size_t resource_move_capture_sites = 0;
};

static void CollectOwnershipResourceMoveExprSites(const Expr *expr,
                                              OwnershipResourceMoveSiteProfile &profile);

static void CollectOwnershipResourceMoveStmtSites(const Stmt *stmt,
                                              OwnershipResourceMoveSiteProfile &profile) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
    case Stmt::Kind::Let:
      if (stmt->let_stmt != nullptr) {
        if (IsOwnershipCleanupOwnedLocal(*stmt->let_stmt)) {
          ++profile.cleanup_owned_local_sites;
        }
        CollectOwnershipResourceMoveExprSites(stmt->let_stmt->value.get(), profile);
      }
      return;
    case Stmt::Kind::Assign:
      if (stmt->assign_stmt != nullptr) {
        CollectOwnershipResourceMoveExprSites(stmt->assign_stmt->value.get(), profile);
      }
      return;
    case Stmt::Kind::Return:
      if (stmt->return_stmt != nullptr) {
        CollectOwnershipResourceMoveExprSites(stmt->return_stmt->value.get(), profile);
      }
      return;
    case Stmt::Kind::If:
      if (stmt->if_stmt != nullptr) {
        CollectOwnershipResourceMoveExprSites(stmt->if_stmt->condition.get(), profile);
        for (const auto &body_stmt : stmt->if_stmt->then_body) {
          CollectOwnershipResourceMoveStmtSites(body_stmt.get(), profile);
        }
        for (const auto &body_stmt : stmt->if_stmt->else_body) {
          CollectOwnershipResourceMoveStmtSites(body_stmt.get(), profile);
        }
      }
      return;
    case Stmt::Kind::DoWhile:
      if (stmt->do_while_stmt != nullptr) {
        for (const auto &body_stmt : stmt->do_while_stmt->body) {
          CollectOwnershipResourceMoveStmtSites(body_stmt.get(), profile);
        }
        CollectOwnershipResourceMoveExprSites(stmt->do_while_stmt->condition.get(), profile);
      }
      return;
    case Stmt::Kind::For:
      if (stmt->for_stmt != nullptr) {
        CollectOwnershipResourceMoveExprSites(stmt->for_stmt->init.value.get(), profile);
        CollectOwnershipResourceMoveExprSites(stmt->for_stmt->condition.get(), profile);
        CollectOwnershipResourceMoveExprSites(stmt->for_stmt->step.value.get(), profile);
        for (const auto &body_stmt : stmt->for_stmt->body) {
          CollectOwnershipResourceMoveStmtSites(body_stmt.get(), profile);
        }
      }
      return;
    case Stmt::Kind::Switch:
      if (stmt->switch_stmt != nullptr) {
        CollectOwnershipResourceMoveExprSites(stmt->switch_stmt->condition.get(), profile);
        for (const auto &case_stmt : stmt->switch_stmt->cases) {
          for (const auto &body_stmt : case_stmt.body) {
            CollectOwnershipResourceMoveStmtSites(body_stmt.get(), profile);
          }
        }
      }
      return;
    case Stmt::Kind::While:
      if (stmt->while_stmt != nullptr) {
        CollectOwnershipResourceMoveExprSites(stmt->while_stmt->condition.get(), profile);
        for (const auto &body_stmt : stmt->while_stmt->body) {
          CollectOwnershipResourceMoveStmtSites(body_stmt.get(), profile);
        }
      }
      return;
    case Stmt::Kind::Block:
    case Stmt::Kind::Defer:
      if (stmt->block_stmt != nullptr) {
        for (const auto &body_stmt : stmt->block_stmt->body) {
          CollectOwnershipResourceMoveStmtSites(body_stmt.get(), profile);
        }
      }
      return;
    case Stmt::Kind::Expr:
      if (stmt->expr_stmt != nullptr) {
        CollectOwnershipResourceMoveExprSites(stmt->expr_stmt->value.get(), profile);
      }
      return;
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return;
  }
}

static void CollectOwnershipResourceMoveExprSites(const Expr *expr,
                                              OwnershipResourceMoveSiteProfile &profile) {
  if (expr == nullptr) {
    return;
  }
  if (expr->kind == Expr::Kind::BlockLiteral) {
    for (const auto &item : expr->block_explicit_capture_items_source_order) {
      if (item.mode == "move") {
        ++profile.resource_move_capture_sites;
      }
    }
  }
  CollectOwnershipResourceMoveExprSites(expr->receiver.get(), profile);
  CollectOwnershipResourceMoveExprSites(expr->left.get(), profile);
  CollectOwnershipResourceMoveExprSites(expr->right.get(), profile);
  CollectOwnershipResourceMoveExprSites(expr->third.get(), profile);
  for (const auto &arg : expr->args) {
    CollectOwnershipResourceMoveExprSites(arg.get(), profile);
  }
}

#include "sema/objc3_semantic_passes_borrowed_ownership_diagnostics.inc"

#include "sema/objc3_semantic_passes_ownership_site_profiles.inc"

static const Objc3PropertyInfo *FindTypedKeyPathPropertyOnOwner(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &owner_name,
    const std::string &property_name) {
  auto merge_it = surface.category_merge_surfaces.find(owner_name);
  if (merge_it != surface.category_merge_surfaces.end()) {
    auto property_it = merge_it->second.merged_properties.find(property_name);
    if (property_it != merge_it->second.merged_properties.end()) {
      return &property_it->second;
    }
  }
  auto interface_it = surface.interfaces.find(owner_name);
  if (interface_it != surface.interfaces.end()) {
    auto property_it = interface_it->second.properties.find(property_name);
    if (property_it != interface_it->second.properties.end()) {
      return &property_it->second;
    }
  }
  auto implementation_it = surface.implementations.find(owner_name);
  if (implementation_it != surface.implementations.end()) {
    auto property_it = implementation_it->second.properties.find(property_name);
    if (property_it != implementation_it->second.properties.end()) {
      return &property_it->second;
    }
  }
  return nullptr;
}

static bool ResolveTypedKeyPathClassRootOwner(
    const std::string &root_name,
    const Objc3MessageSendResolutionContext &context,
    std::string &owner_name) {
  if (context.surface == nullptr || root_name.empty()) {
    return false;
  }
  if (context.surface->interfaces.find(root_name) !=
      context.surface->interfaces.end()) {
    owner_name = root_name;
    return true;
  }
  if (context.surface->implementations.find(root_name) !=
      context.surface->implementations.end()) {
    owner_name = root_name;
    return true;
  }
  return false;
}

static bool TryResolveTypedKeyPathValidatedOwner(
    const Expr &expr,
    const std::vector<SemanticScope> &scopes,
    const std::unordered_map<std::string, ValueType> &globals,
    const std::unordered_map<std::string, FunctionInfo> &functions,
    const Objc3MessageSendResolutionContext &context,
    std::string &owner_name,
    bool &is_class_root) {
  owner_name.clear();
  is_class_root = false;
  if (expr.kind != Expr::Kind::Identifier ||
      !expr.typed_keypath_literal_enabled) {
    return false;
  }
  if (expr.typed_keypath_root_is_self) {
    if (context.inside_method &&
        !context.current_implementation_name.empty()) {
      owner_name = context.current_implementation_name;
    }
    return true;
  }

  const SemanticTypeInfo local_type =
      ScopeLookupType(scopes, expr.typed_keypath_root_name);
  if (!IsUnknownSemanticType(local_type)) {
    return IsObjCReferenceSemanticType(local_type);
  }

  auto global_it = globals.find(expr.typed_keypath_root_name);
  if (global_it != globals.end()) {
    if (global_it->second == ValueType::ObjCClass &&
        ResolveTypedKeyPathClassRootOwner(expr.typed_keypath_root_name, context,
                                          owner_name)) {
      is_class_root = true;
      return true;
    }
    return IsObjCReferenceSemanticType(
        MakeSemanticTypeFromGlobal(global_it->second));
  }

  if (functions.find(expr.typed_keypath_root_name) != functions.end()) {
    return false;
  }

  if (ResolveTypedKeyPathClassRootOwner(expr.typed_keypath_root_name, context,
                                        owner_name)) {
    is_class_root = true;
    return true;
  }

  return false;
}

static Objc3ResolvedMessageSendMethod ResolveConcreteMessageSendMethod(
    const Expr &expr,
    const Objc3MessageSendResolutionContext &context);
static Objc3ResolvedMessageSendMethod ResolveConcreteOwnerMethod(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &owner_name,
    const std::string &selector,
    bool is_class_method);
static Objc3ResolvedMessageSendPropertyAccessor
ResolveConcreteOwnerPropertyAccessor(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &owner_name,
    const std::string &selector,
    bool is_class_property);
static Expr::DispatchSurfaceKind ResolveNormalizedMessageSendDispatchSurfaceKind(
    const Expr &expr,
    const Objc3MessageSendResolutionContext &context);

struct ProtocolCompositionInfo {
  bool has_protocol_composition = false;
  std::vector<std::string> names_lexicographic;
  bool has_invalid_protocol_composition = false;
};

static std::string NormalizeMetaprogrammingRawDeriveToken(std::string token) {
  token = TrimAsciiWhitespace(token);
  if (token.size() >= 2u &&
      ((token.front() == '"' && token.back() == '"') ||
       (token.front() == '\'' && token.back() == '\''))) {
    token = token.substr(1, token.size() - 2u);
  }
  return token;
}

static std::string NormalizeMetaprogrammingRawMacroToken(std::string token) {
  token = TrimAsciiWhitespace(token);
  if (token.size() >= 2u &&
      ((token.front() == '"' && token.back() == '"') ||
       (token.front() == '\'' && token.back() == '\''))) {
    token = token.substr(1, token.size() - 2u);
  }
  return token;
}

static std::string NormalizeMetaprogrammingRawPropertyBehaviorToken(std::string token) {
  token = TrimAsciiWhitespace(token);
  if (token.size() >= 2u &&
      ((token.front() == '"' && token.back() == '"') ||
       (token.front() == '\'' && token.back() == '\''))) {
    token = token.substr(1, token.size() - 2u);
  }
  return token;
}

static std::string BuildMetaprogrammingLowercaseDeriveToken(std::string token) {
  token = NormalizeMetaprogrammingRawDeriveToken(std::move(token));
  return objc3c::support::LowercaseAscii(token);
}

static std::string NormalizeMetaprogrammingSupportedDeriveName(
    const std::string &derive_name) {
  const std::string lowered = BuildMetaprogrammingLowercaseDeriveToken(derive_name);
  if (lowered == "equatable" || lowered == "equality") {
    return "Equality";
  }
  if (lowered == "hash") {
    return "Hash";
  }
  if (lowered == "debugdescription") {
    return "DebugDescription";
  }
  return "";
}

static std::string BuildMetaprogrammingLowercasePropertyBehaviorToken(std::string token) {
  token = NormalizeMetaprogrammingRawPropertyBehaviorToken(std::move(token));
  return objc3c::support::LowercaseAscii(token);
}

static std::string NormalizeMetaprogrammingSupportedPropertyBehaviorName(
    const std::string &behavior_name) {
  const std::string lowered =
      BuildMetaprogrammingLowercasePropertyBehaviorToken(behavior_name);
  if (lowered == "observed") {
    return "Observed";
  }
  if (lowered == "projected") {
    return "Projected";
  }
  return "";
}

static std::string BuildMetaprogrammingLowercaseMacroToken(std::string token) {
  token = NormalizeMetaprogrammingRawMacroToken(std::move(token));
  return objc3c::support::LowercaseAscii(token);
}

static bool IsSupportedMetaprogrammingMacroPackageName(const std::string &package_name) {
  const std::string lowered = BuildMetaprogrammingLowercaseMacroToken(package_name);
  return lowered == "std.metaprogramming" ||
         lowered.rfind("std.metaprogramming.", 0) == 0u;
}

static bool IsSupportedMetaprogrammingMacroProvenanceToken(
    const std::string &provenance_name) {
  const std::string normalized =
      NormalizeMetaprogrammingRawMacroToken(provenance_name);
  const std::string lowered = BuildMetaprogrammingLowercaseMacroToken(provenance_name);
  if (normalized != lowered) {
    return false;
  }
  constexpr std::string_view prefix = "sha256:";
  if (lowered.rfind(prefix, 0) != 0u) {
    return false;
  }
  const std::string_view digest = std::string_view(lowered).substr(prefix.size());
  if (digest.size() < 6u || digest.size() > 64u) {
    return false;
  }
  return std::all_of(digest.begin(), digest.end(), [](char c) {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
  });
}

static std::string BuildMetaprogrammingDerivedSelectorForInventoryRow(
    const std::string &canonical_derive_name) {
  if (canonical_derive_name == "Equality") {
    return "isEqual:";
  }
  if (canonical_derive_name == "Hash") {
    return "hash";
  }
  if (canonical_derive_name == "DebugDescription") {
