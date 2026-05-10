static Objc3MessageSendSelectorLoweringSiteMetadata BuildMessageSendSelectorLoweringSiteMetadata(const Expr &expr) {
  Objc3MessageSendSelectorLoweringSiteMetadata metadata;
  metadata.selector = expr.selector;
  metadata.selector_lowering_symbol =
      expr.selector_lowering_symbol.empty() ? expr.selector : expr.selector_lowering_symbol;
  metadata.argument_count = expr.args.size();
  const Expr::MessageSendForm resolved_form = ResolveMessageSendForm(expr);
  metadata.unary_form = resolved_form == Expr::MessageSendForm::Unary;
  metadata.keyword_form = resolved_form == Expr::MessageSendForm::Keyword;
  metadata.line = expr.line;
  metadata.column = expr.column;

  if (!expr.selector_lowering_pieces.empty()) {
    metadata.selector_piece_count = expr.selector_lowering_pieces.size();
    for (const Expr::MessageSendSelectorPiece &piece : expr.selector_lowering_pieces) {
      if (piece.has_argument) {
        ++metadata.selector_argument_piece_count;
      }
    }
  } else if (metadata.keyword_form) {
    const std::string selector_symbol =
        metadata.selector_lowering_symbol.empty() ? metadata.selector : metadata.selector_lowering_symbol;
    metadata.selector_piece_count = CountSelectorKeywordPieces(selector_symbol);
    metadata.selector_argument_piece_count = metadata.selector_piece_count;
  }

  metadata.selector_lowering_is_normalized =
      expr.selector_lowering_is_normalized ||
      (!metadata.selector.empty() && !metadata.selector_lowering_symbol.empty() &&
       metadata.selector == metadata.selector_lowering_symbol);
  metadata.receiver_is_nil_literal = expr.receiver != nullptr && expr.receiver->kind == Expr::Kind::NilLiteral;
  const objc3c::support::Objc3NilReceiverProfile nil_receiver_profile =
      objc3c::support::BuildNilReceiverProfile(
          metadata.receiver_is_nil_literal,
          expr.nil_receiver_semantics_is_normalized,
          expr.nil_receiver_semantics_enabled, expr.nil_receiver_foldable,
          expr.nil_receiver_requires_runtime_dispatch);
  metadata.nil_receiver_semantics_enabled =
      nil_receiver_profile.semantics_enabled;
  metadata.nil_receiver_foldable = nil_receiver_profile.foldable;
  metadata.nil_receiver_requires_runtime_dispatch =
      nil_receiver_profile.requires_runtime_dispatch;
  metadata.nil_receiver_semantics_is_normalized =
      nil_receiver_profile.normalized;
  metadata.runtime_link_host_link_required =
      expr.runtime_link_host_link_is_normalized ? expr.runtime_link_host_link_required
                                                : metadata.nil_receiver_requires_runtime_dispatch;
  metadata.runtime_link_host_link_elided =
      expr.runtime_link_host_link_is_normalized ? expr.runtime_link_host_link_elided
                                                : !metadata.runtime_link_host_link_required;
  metadata.runtime_link_host_link_runtime_dispatch_arg_slots = expr.dispatch_abi_runtime_arg_slots;
  metadata.runtime_link_host_link_declaration_parameter_count =
      expr.runtime_link_host_link_is_normalized
          ? static_cast<std::size_t>(expr.runtime_link_host_link_declaration_parameter_count)
          : metadata.runtime_link_host_link_runtime_dispatch_arg_slots + 2u;
  metadata.runtime_dispatch_bridge_symbol = expr.runtime_dispatch_bridge_symbol.empty()
                                                ? kObjc3RuntimeLinkHostLinkDefaultDispatchSymbol
                                                : expr.runtime_dispatch_bridge_symbol;
  metadata.runtime_link_host_link_symbol = expr.runtime_link_host_link_symbol;
  metadata.runtime_link_host_link_is_normalized =
      expr.runtime_link_host_link_is_normalized ||
      (metadata.runtime_link_host_link_required == metadata.nil_receiver_requires_runtime_dispatch &&
       metadata.runtime_link_host_link_elided == !metadata.runtime_link_host_link_required &&
       metadata.runtime_link_host_link_declaration_parameter_count ==
           metadata.runtime_link_host_link_runtime_dispatch_arg_slots + 2u &&
       !metadata.runtime_dispatch_bridge_symbol.empty());
  metadata.receiver_is_super_identifier =
      expr.receiver != nullptr && expr.receiver->kind == Expr::Kind::Identifier && expr.receiver->ident == "super";
  const objc3c::support::Objc3SuperDispatchProfile super_dispatch_profile =
      objc3c::support::BuildSuperDispatchProfile(
          metadata.receiver_is_super_identifier,
          expr.super_dispatch_semantics_is_normalized,
          expr.super_dispatch_enabled,
          expr.super_dispatch_requires_class_context);
  metadata.super_dispatch_enabled = super_dispatch_profile.enabled;
  metadata.super_dispatch_requires_class_context =
      super_dispatch_profile.requires_class_context;
  metadata.super_dispatch_semantics_is_normalized =
      super_dispatch_profile.normalized;
  const objc3c::support::Objc3MethodFamilyProfile method_family_profile =
      objc3c::support::BuildMethodFamilyProfile(
          metadata.selector, expr.method_family_semantics_is_normalized,
          expr.method_family_name, expr.method_family_returns_retained_result,
          expr.method_family_returns_related_result);
  metadata.method_family_name = method_family_profile.name;
  metadata.method_family_returns_retained_result =
      method_family_profile.returns_retained_result;
  metadata.method_family_returns_related_result =
      method_family_profile.returns_related_result;
  metadata.method_family_semantics_is_normalized =
      method_family_profile.normalized;
  return metadata;
}

static void CollectMessageSendSelectorLoweringSiteMetadataFromExpr(
    const Expr *expr, std::vector<Objc3MessageSendSelectorLoweringSiteMetadata> &sites) {
  if (expr == nullptr) {
    return;
  }

  if (expr->kind == Expr::Kind::MessageSend) {
    sites.push_back(BuildMessageSendSelectorLoweringSiteMetadata(*expr));
    CollectMessageSendSelectorLoweringSiteMetadataFromExpr(expr->receiver.get(), sites);
    for (const auto &arg : expr->args) {
      CollectMessageSendSelectorLoweringSiteMetadataFromExpr(arg.get(), sites);
    }
    return;
  }

  if (expr->kind == Expr::Kind::Binary) {
    CollectMessageSendSelectorLoweringSiteMetadataFromExpr(expr->left.get(), sites);
    CollectMessageSendSelectorLoweringSiteMetadataFromExpr(expr->right.get(), sites);
    return;
  }

  if (expr->kind == Expr::Kind::Conditional) {
    CollectMessageSendSelectorLoweringSiteMetadataFromExpr(expr->left.get(), sites);
    CollectMessageSendSelectorLoweringSiteMetadataFromExpr(expr->right.get(), sites);
    CollectMessageSendSelectorLoweringSiteMetadataFromExpr(expr->third.get(), sites);
    return;
  }

  if (expr->kind == Expr::Kind::Call) {
    for (const auto &arg : expr->args) {
      CollectMessageSendSelectorLoweringSiteMetadataFromExpr(arg.get(), sites);
    }
  }
}

static void CollectMessageSendSelectorLoweringSiteMetadataFromStatements(
    const std::vector<std::unique_ptr<Stmt>> &statements,
    std::vector<Objc3MessageSendSelectorLoweringSiteMetadata> &sites);

static void CollectMessageSendSelectorLoweringSiteMetadataFromForClause(
    const ForClause &clause,
    std::vector<Objc3MessageSendSelectorLoweringSiteMetadata> &sites) {
  CollectMessageSendSelectorLoweringSiteMetadataFromExpr(clause.value.get(), sites);
}

static void CollectMessageSendSelectorLoweringSiteMetadataFromStatement(
    const Stmt *stmt, std::vector<Objc3MessageSendSelectorLoweringSiteMetadata> &sites) {
  if (stmt == nullptr) {
    return;
  }

  switch (stmt->kind) {
    case Stmt::Kind::Let:
      CollectMessageSendSelectorLoweringSiteMetadataFromExpr(stmt->let_stmt->value.get(), sites);
      return;
    case Stmt::Kind::Assign:
      CollectMessageSendSelectorLoweringSiteMetadataFromExpr(stmt->assign_stmt->value.get(), sites);
      return;
    case Stmt::Kind::Return:
      CollectMessageSendSelectorLoweringSiteMetadataFromExpr(stmt->return_stmt->value.get(), sites);
      return;
    case Stmt::Kind::If:
      CollectMessageSendSelectorLoweringSiteMetadataFromExpr(stmt->if_stmt->condition.get(), sites);
      CollectMessageSendSelectorLoweringSiteMetadataFromStatements(stmt->if_stmt->then_body, sites);
      CollectMessageSendSelectorLoweringSiteMetadataFromStatements(stmt->if_stmt->else_body, sites);
      return;
    case Stmt::Kind::DoWhile:
      CollectMessageSendSelectorLoweringSiteMetadataFromStatements(stmt->do_while_stmt->body, sites);
      CollectMessageSendSelectorLoweringSiteMetadataFromExpr(stmt->do_while_stmt->condition.get(), sites);
      return;
    case Stmt::Kind::For:
      CollectMessageSendSelectorLoweringSiteMetadataFromForClause(stmt->for_stmt->init, sites);
      CollectMessageSendSelectorLoweringSiteMetadataFromExpr(stmt->for_stmt->condition.get(), sites);
      CollectMessageSendSelectorLoweringSiteMetadataFromForClause(stmt->for_stmt->step, sites);
      CollectMessageSendSelectorLoweringSiteMetadataFromStatements(stmt->for_stmt->body, sites);
      return;
    case Stmt::Kind::Switch:
      CollectMessageSendSelectorLoweringSiteMetadataFromExpr(stmt->switch_stmt->condition.get(), sites);
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        CollectMessageSendSelectorLoweringSiteMetadataFromStatements(switch_case.body, sites);
      }
      return;
    case Stmt::Kind::While:
      CollectMessageSendSelectorLoweringSiteMetadataFromExpr(stmt->while_stmt->condition.get(), sites);
      CollectMessageSendSelectorLoweringSiteMetadataFromStatements(stmt->while_stmt->body, sites);
      return;
    case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
      CollectMessageSendSelectorLoweringSiteMetadataFromStatements(stmt->block_stmt->body, sites);
      return;
    case Stmt::Kind::Expr:
      CollectMessageSendSelectorLoweringSiteMetadataFromExpr(stmt->expr_stmt->value.get(), sites);
      return;
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return;
  }
}

static void CollectMessageSendSelectorLoweringSiteMetadataFromStatements(
    const std::vector<std::unique_ptr<Stmt>> &statements,
    std::vector<Objc3MessageSendSelectorLoweringSiteMetadata> &sites) {
  for (const auto &statement : statements) {
    CollectMessageSendSelectorLoweringSiteMetadataFromStatement(statement.get(), sites);
  }
}

static bool IsMessageSendSelectorLoweringSiteMetadataLess(
    const Objc3MessageSendSelectorLoweringSiteMetadata &lhs,
    const Objc3MessageSendSelectorLoweringSiteMetadata &rhs) {
  if (lhs.selector != rhs.selector) {
    return lhs.selector < rhs.selector;
  }
  if (lhs.selector_lowering_symbol != rhs.selector_lowering_symbol) {
    return lhs.selector_lowering_symbol < rhs.selector_lowering_symbol;
  }
  if (lhs.argument_count != rhs.argument_count) {
    return lhs.argument_count < rhs.argument_count;
  }
  if (lhs.selector_piece_count != rhs.selector_piece_count) {
    return lhs.selector_piece_count < rhs.selector_piece_count;
  }
  if (lhs.selector_argument_piece_count != rhs.selector_argument_piece_count) {
    return lhs.selector_argument_piece_count < rhs.selector_argument_piece_count;
  }
  if (lhs.unary_form != rhs.unary_form) {
    return lhs.unary_form < rhs.unary_form;
  }
  if (lhs.keyword_form != rhs.keyword_form) {
    return lhs.keyword_form < rhs.keyword_form;
  }
  if (lhs.selector_lowering_is_normalized != rhs.selector_lowering_is_normalized) {
    return lhs.selector_lowering_is_normalized < rhs.selector_lowering_is_normalized;
  }
  if (lhs.receiver_is_nil_literal != rhs.receiver_is_nil_literal) {
    return lhs.receiver_is_nil_literal < rhs.receiver_is_nil_literal;
  }
  if (lhs.nil_receiver_semantics_enabled != rhs.nil_receiver_semantics_enabled) {
    return lhs.nil_receiver_semantics_enabled < rhs.nil_receiver_semantics_enabled;
  }
  if (lhs.nil_receiver_foldable != rhs.nil_receiver_foldable) {
    return lhs.nil_receiver_foldable < rhs.nil_receiver_foldable;
  }
  if (lhs.nil_receiver_requires_runtime_dispatch != rhs.nil_receiver_requires_runtime_dispatch) {
    return lhs.nil_receiver_requires_runtime_dispatch < rhs.nil_receiver_requires_runtime_dispatch;
  }
  if (lhs.nil_receiver_semantics_is_normalized != rhs.nil_receiver_semantics_is_normalized) {
    return lhs.nil_receiver_semantics_is_normalized < rhs.nil_receiver_semantics_is_normalized;
  }
  if (lhs.runtime_link_host_link_required != rhs.runtime_link_host_link_required) {
    return lhs.runtime_link_host_link_required < rhs.runtime_link_host_link_required;
  }
  if (lhs.runtime_link_host_link_elided != rhs.runtime_link_host_link_elided) {
    return lhs.runtime_link_host_link_elided < rhs.runtime_link_host_link_elided;
  }
  if (lhs.runtime_link_host_link_runtime_dispatch_arg_slots != rhs.runtime_link_host_link_runtime_dispatch_arg_slots) {
    return lhs.runtime_link_host_link_runtime_dispatch_arg_slots <
           rhs.runtime_link_host_link_runtime_dispatch_arg_slots;
  }
  if (lhs.runtime_link_host_link_declaration_parameter_count !=
      rhs.runtime_link_host_link_declaration_parameter_count) {
    return lhs.runtime_link_host_link_declaration_parameter_count <
           rhs.runtime_link_host_link_declaration_parameter_count;
  }
  if (lhs.runtime_dispatch_bridge_symbol != rhs.runtime_dispatch_bridge_symbol) {
    return lhs.runtime_dispatch_bridge_symbol < rhs.runtime_dispatch_bridge_symbol;
  }
  if (lhs.runtime_link_host_link_symbol != rhs.runtime_link_host_link_symbol) {
    return lhs.runtime_link_host_link_symbol < rhs.runtime_link_host_link_symbol;
  }
  if (lhs.runtime_link_host_link_is_normalized != rhs.runtime_link_host_link_is_normalized) {
    return lhs.runtime_link_host_link_is_normalized < rhs.runtime_link_host_link_is_normalized;
  }
  if (lhs.receiver_is_super_identifier != rhs.receiver_is_super_identifier) {
    return lhs.receiver_is_super_identifier < rhs.receiver_is_super_identifier;
  }
  if (lhs.super_dispatch_enabled != rhs.super_dispatch_enabled) {
    return lhs.super_dispatch_enabled < rhs.super_dispatch_enabled;
  }
  if (lhs.super_dispatch_requires_class_context != rhs.super_dispatch_requires_class_context) {
    return lhs.super_dispatch_requires_class_context < rhs.super_dispatch_requires_class_context;
  }
  if (lhs.super_dispatch_semantics_is_normalized != rhs.super_dispatch_semantics_is_normalized) {
    return lhs.super_dispatch_semantics_is_normalized < rhs.super_dispatch_semantics_is_normalized;
  }
  if (lhs.method_family_name != rhs.method_family_name) {
    return lhs.method_family_name < rhs.method_family_name;
  }
  if (lhs.method_family_returns_retained_result != rhs.method_family_returns_retained_result) {
    return lhs.method_family_returns_retained_result < rhs.method_family_returns_retained_result;
  }
  if (lhs.method_family_returns_related_result != rhs.method_family_returns_related_result) {
    return lhs.method_family_returns_related_result < rhs.method_family_returns_related_result;
  }
  if (lhs.method_family_semantics_is_normalized != rhs.method_family_semantics_is_normalized) {
    return lhs.method_family_semantics_is_normalized < rhs.method_family_semantics_is_normalized;
  }
  if (lhs.line != rhs.line) {
    return lhs.line < rhs.line;
  }
  return lhs.column < rhs.column;
}

static std::vector<Objc3MessageSendSelectorLoweringSiteMetadata>
BuildMessageSendSelectorLoweringSiteMetadataLexicographic(const Objc3Program &ast) {
  std::vector<Objc3MessageSendSelectorLoweringSiteMetadata> sites;
  for (const auto &global : ast.globals) {
    CollectMessageSendSelectorLoweringSiteMetadataFromExpr(global.value.get(), sites);
  }
  for (const auto &fn : ast.functions) {
    CollectMessageSendSelectorLoweringSiteMetadataFromStatements(fn.body, sites);
  }
  // method-family/runtime evidence anchor: implementation method
  // bodies must contribute selector-lowering sites so super/dynamic legality
  // and runtime-link host-link summaries stay aligned with sema parity.
  for (const auto &implementation_decl : ast.implementations) {
    for (const auto &method : implementation_decl.methods) {
      if (!method.has_body) {
        continue;
      }
      CollectMessageSendSelectorLoweringSiteMetadataFromStatements(method.body,
                                                                   sites);
    }
  }
  std::sort(sites.begin(), sites.end(), IsMessageSendSelectorLoweringSiteMetadataLess);
  return sites;
}
