#include "sema/objc3_semantic_passes_category_merge_candidate_helpers.inc"

#include "sema/objc3_semantic_passes_category_merge_surface_builder.inc"

#include "sema/objc3_semantic_passes_protocol_conformance_validation.inc"

#include "sema/objc3_semantic_passes_concrete_message_resolution.inc"

#include "sema/objc3_semantic_passes_method_lookup_override_conflicts.inc"

static bool IsCompatiblePropertyTypeMetadataSignature(const Objc3SemanticPropertyTypeMetadata &lhs,
                                                      const Objc3SemanticPropertyTypeMetadata &rhs) {
  // property-ivar executable semantics anchor:
  // runtime-meaningful property compatibility preserves declaration-level attribute,
  // accessor, and ownership semantics while excluding storage-local layout identities.
  return lhs.type == rhs.type &&
         lhs.is_vector == rhs.is_vector &&
         lhs.vector_base_spelling == rhs.vector_base_spelling &&
         lhs.vector_lane_count == rhs.vector_lane_count &&
         lhs.id_spelling == rhs.id_spelling &&
         lhs.class_spelling == rhs.class_spelling &&
         lhs.instancetype_spelling == rhs.instancetype_spelling &&
         lhs.is_readonly == rhs.is_readonly &&
         lhs.is_readwrite == rhs.is_readwrite &&
         lhs.is_atomic == rhs.is_atomic &&
         lhs.is_nonatomic == rhs.is_nonatomic &&
         lhs.is_copy == rhs.is_copy &&
         lhs.is_retain == rhs.is_retain &&
         lhs.is_strong == rhs.is_strong &&
         lhs.is_weak == rhs.is_weak &&
         lhs.is_unowned == rhs.is_unowned &&
         lhs.is_unsafe_unretained == rhs.is_unsafe_unretained &&
         lhs.is_assign == rhs.is_assign &&
         lhs.is_nullable == rhs.is_nullable &&
         lhs.is_nonnull == rhs.is_nonnull &&
         lhs.is_null_resettable == rhs.is_null_resettable &&
         lhs.is_class == rhs.is_class &&
         lhs.is_direct == rhs.is_direct &&
         lhs.has_getter == rhs.has_getter &&
         lhs.has_setter == rhs.has_setter &&
         lhs.getter_selector == rhs.getter_selector &&
         lhs.setter_selector == rhs.setter_selector &&
         lhs.ownership_lifetime_profile == rhs.ownership_lifetime_profile &&
         lhs.ownership_runtime_hook_profile == rhs.ownership_runtime_hook_profile &&
         lhs.property_attribute_profile == rhs.property_attribute_profile &&
         lhs.effective_getter_selector == rhs.effective_getter_selector &&
         lhs.effective_setter_available == rhs.effective_setter_available &&
         lhs.effective_setter_selector == rhs.effective_setter_selector &&
         lhs.accessor_ownership_profile == rhs.accessor_ownership_profile;
}

static const Objc3PropertyInfo *FindPropertyInImplementationInfo(
    const Objc3ImplementationInfo &implementation_info,
    const std::string &property_name) {
  const auto property_it = implementation_info.properties.find(property_name);
  if (property_it == implementation_info.properties.end()) {
    return nullptr;
  }
  return &property_it->second;
}

static const Objc3SemanticPropertyTypeMetadata *FindPropertyInImplementationMetadata(
    const Objc3SemanticImplementationTypeMetadata &metadata,
    const std::string &property_name) {
  for (const auto &property_metadata : metadata.properties_lexicographic) {
    if (property_metadata.name == property_name) {
      return &property_metadata;
    }
  }
  return nullptr;
}

static bool HasResolvedDefaultIvarBinding(const Objc3PropertyInfo &property) {
  return property.executable_synthesized_binding_kind == "implicit-ivar" &&
         !property.ivar_binding_symbol.empty() &&
         !property.executable_synthesized_binding_symbol.empty() &&
         !property.executable_ivar_layout_symbol.empty() &&
         property.executable_ivar_layout_alignment_bytes > 0u;
}

static bool HasResolvedDefaultIvarBinding(
    const Objc3SemanticPropertyTypeMetadata &property) {
  return property.executable_synthesized_binding_kind == "implicit-ivar" &&
         !property.ivar_binding_symbol.empty() &&
         !property.executable_synthesized_binding_symbol.empty() &&
         !property.executable_ivar_layout_symbol.empty() &&
         property.executable_ivar_layout_alignment_bytes > 0u;
}

static bool HasCompatibleResolvedDefaultIvarBinding(
    const Objc3PropertyInfo &interface_property,
    const Objc3PropertyInfo &implementation_property) {
  return interface_property.ivar_binding_symbol ==
             implementation_property.ivar_binding_symbol &&
         interface_property.executable_synthesized_binding_kind ==
             implementation_property.executable_synthesized_binding_kind &&
         interface_property.executable_synthesized_binding_symbol ==
             implementation_property.executable_synthesized_binding_symbol &&
         interface_property.executable_ivar_layout_symbol ==
             implementation_property.executable_ivar_layout_symbol &&
         interface_property.executable_ivar_layout_slot_index ==
             implementation_property.executable_ivar_layout_slot_index &&
         interface_property.executable_ivar_layout_size_bytes ==
             implementation_property.executable_ivar_layout_size_bytes &&
         interface_property.executable_ivar_layout_alignment_bytes ==
             implementation_property.executable_ivar_layout_alignment_bytes &&
         interface_property.executable_ivar_layout_offset_bytes ==
             implementation_property.executable_ivar_layout_offset_bytes &&
         interface_property.executable_ivar_layout_padding_bytes ==
             implementation_property.executable_ivar_layout_padding_bytes &&
         interface_property.executable_ivar_layout_inherited_slot_count ==
             implementation_property.executable_ivar_layout_inherited_slot_count &&
         interface_property.executable_ivar_layout_inherited_size_bytes ==
             implementation_property.executable_ivar_layout_inherited_size_bytes &&
         interface_property.executable_ivar_layout_owner_size_bytes ==
             implementation_property.executable_ivar_layout_owner_size_bytes &&
         interface_property.executable_ivar_init_order_index ==
             implementation_property.executable_ivar_init_order_index &&
         interface_property.executable_ivar_destroy_order_index ==
             implementation_property.executable_ivar_destroy_order_index &&
         interface_property.executable_ivar_layout_valid ==
             implementation_property.executable_ivar_layout_valid &&
         interface_property.executable_ivar_layout_replay_key ==
             implementation_property.executable_ivar_layout_replay_key;
}

static bool HasCompatibleResolvedDefaultIvarBinding(
    const Objc3SemanticPropertyTypeMetadata &interface_property,
    const Objc3SemanticPropertyTypeMetadata &implementation_property) {
  return interface_property.ivar_binding_symbol ==
             implementation_property.ivar_binding_symbol &&
         interface_property.executable_synthesized_binding_kind ==
             implementation_property.executable_synthesized_binding_kind &&
         interface_property.executable_synthesized_binding_symbol ==
             implementation_property.executable_synthesized_binding_symbol &&
         interface_property.executable_ivar_layout_symbol ==
             implementation_property.executable_ivar_layout_symbol &&
         interface_property.executable_ivar_layout_slot_index ==
             implementation_property.executable_ivar_layout_slot_index &&
         interface_property.executable_ivar_layout_size_bytes ==
             implementation_property.executable_ivar_layout_size_bytes &&
         interface_property.executable_ivar_layout_alignment_bytes ==
             implementation_property.executable_ivar_layout_alignment_bytes &&
         interface_property.executable_ivar_layout_offset_bytes ==
             implementation_property.executable_ivar_layout_offset_bytes &&
         interface_property.executable_ivar_layout_padding_bytes ==
             implementation_property.executable_ivar_layout_padding_bytes &&
         interface_property.executable_ivar_layout_inherited_slot_count ==
             implementation_property.executable_ivar_layout_inherited_slot_count &&
         interface_property.executable_ivar_layout_inherited_size_bytes ==
             implementation_property.executable_ivar_layout_inherited_size_bytes &&
         interface_property.executable_ivar_layout_owner_size_bytes ==
             implementation_property.executable_ivar_layout_owner_size_bytes &&
         interface_property.executable_ivar_init_order_index ==
             implementation_property.executable_ivar_init_order_index &&
         interface_property.executable_ivar_destroy_order_index ==
             implementation_property.executable_ivar_destroy_order_index &&
         interface_property.executable_ivar_layout_valid ==
             implementation_property.executable_ivar_layout_valid &&
         interface_property.executable_ivar_layout_replay_key ==
             implementation_property.executable_ivar_layout_replay_key;
}

static void OverlayAuthoritativeDefaultIvarBinding(
    Objc3PropertyInfo &implementation_property,
    const Objc3PropertyInfo &interface_property) {
  implementation_property.ivar_binding_symbol =
      interface_property.ivar_binding_symbol;
  implementation_property.executable_synthesized_binding_kind =
      interface_property.executable_synthesized_binding_kind;
  implementation_property.executable_synthesized_binding_symbol =
      interface_property.executable_synthesized_binding_symbol;
  implementation_property.executable_ivar_layout_symbol =
      interface_property.executable_ivar_layout_symbol;
  implementation_property.executable_ivar_layout_slot_index =
      interface_property.executable_ivar_layout_slot_index;
  implementation_property.executable_ivar_layout_size_bytes =
      interface_property.executable_ivar_layout_size_bytes;
  implementation_property.executable_ivar_layout_alignment_bytes =
      interface_property.executable_ivar_layout_alignment_bytes;
  implementation_property.executable_ivar_layout_offset_bytes =
      interface_property.executable_ivar_layout_offset_bytes;
  implementation_property.executable_ivar_layout_padding_bytes =
      interface_property.executable_ivar_layout_padding_bytes;
  implementation_property.executable_ivar_layout_inherited_slot_count =
      interface_property.executable_ivar_layout_inherited_slot_count;
  implementation_property.executable_ivar_layout_inherited_size_bytes =
      interface_property.executable_ivar_layout_inherited_size_bytes;
  implementation_property.executable_ivar_layout_owner_size_bytes =
      interface_property.executable_ivar_layout_owner_size_bytes;
  implementation_property.executable_ivar_init_order_index =
      interface_property.executable_ivar_init_order_index;
  implementation_property.executable_ivar_destroy_order_index =
      interface_property.executable_ivar_destroy_order_index;
  implementation_property.executable_ivar_layout_valid =
      interface_property.executable_ivar_layout_valid;
  implementation_property.executable_ivar_layout_replay_key =
      interface_property.executable_ivar_layout_replay_key;
}

static Objc3PropertySynthesisIvarBindingSummary BuildPropertySynthesisIvarBindingSummaryFromIntegrationSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  Objc3PropertySynthesisIvarBindingSummary summary;
  for (const auto &implementation_entry : surface.implementations) {
    const auto interface_it = surface.interfaces.find(implementation_entry.first);
    if (interface_it == surface.interfaces.end()) {
      continue;
    }

    for (const auto &interface_property_entry : interface_it->second.properties) {
      ++summary.property_synthesis_sites;
      ++summary.property_synthesis_default_ivar_bindings;
      ++summary.interface_owned_property_synthesis_sites;
      ++summary.ivar_binding_sites;
      const Objc3PropertyInfo &interface_property = interface_property_entry.second;
      const Objc3PropertyInfo *implementation_property =
          FindPropertyInImplementationInfo(implementation_entry.second,
                                           interface_property_entry.first);
      if (implementation_property != nullptr) {
        ++summary.implementation_property_redeclaration_sites;
      }

      if (!HasResolvedDefaultIvarBinding(interface_property)) {
        ++summary.ivar_binding_missing;
        continue;
      }

      if (implementation_property != nullptr &&
          (!IsCompatiblePropertySignature(interface_property,
                                         *implementation_property) ||
           !HasResolvedDefaultIvarBinding(*implementation_property) ||
           !HasCompatibleResolvedDefaultIvarBinding(interface_property,
                                                   *implementation_property))) {
        ++summary.ivar_binding_conflicts;
      } else {
        ++summary.ivar_binding_resolved;
      }
    }
  }

  summary.deterministic =
      summary.property_synthesis_explicit_ivar_bindings <= summary.property_synthesis_sites &&
      summary.property_synthesis_default_ivar_bindings <= summary.property_synthesis_sites &&
      summary.property_synthesis_explicit_ivar_bindings +
              summary.property_synthesis_default_ivar_bindings ==
          summary.property_synthesis_sites &&
      summary.interface_owned_property_synthesis_sites ==
          summary.property_synthesis_sites &&
      summary.implementation_property_redeclaration_sites <=
          summary.property_synthesis_sites &&
      summary.ivar_binding_sites == summary.property_synthesis_sites &&
      summary.ivar_binding_resolved <= summary.ivar_binding_sites &&
      summary.ivar_binding_missing <= summary.ivar_binding_sites &&
      summary.ivar_binding_conflicts <= summary.ivar_binding_sites &&
      summary.ivar_binding_resolved + summary.ivar_binding_missing + summary.ivar_binding_conflicts ==
          summary.ivar_binding_sites;
  return summary;
}

// export-legality anchor: class implementation property synthesis
// plus ivar-binding preconditions remain the canonical sema summary that later
// runtime metadata export milestones consume without source-shape revalidation.
// binary-boundary anchor: lane-D packages this exact sema-owned
// property/ivar summary into the runtime metadata envelope so later binary
// consumers stay aligned with lowering admission and replay keys.
// semantic-closure gate anchor: lane-E treats this sema-owned
// property/ivar summary as one required section-emission prerequisite beside
// A003 graph closure, C003 replay projection, and D002 packaging.
// corpus-sync anchor: the integrated corpus gate replays this
// property/ivar summary directly from manifest output on the real runner path.
static Objc3PropertySynthesisIvarBindingSummary BuildPropertySynthesisIvarBindingSummaryFromTypeMetadataHandoff(
    const Objc3SemanticTypeMetadataHandoff &handoff) {
  Objc3PropertySynthesisIvarBindingSummary summary;
  std::unordered_map<std::string, const Objc3SemanticInterfaceTypeMetadata *> interfaces_by_name;
  interfaces_by_name.reserve(handoff.interfaces_lexicographic.size());
  for (const auto &interface_metadata : handoff.interfaces_lexicographic) {
    interfaces_by_name.emplace(interface_metadata.name, &interface_metadata);
  }

  for (const auto &implementation_metadata : handoff.implementations_lexicographic) {
    const auto interface_it = interfaces_by_name.find(implementation_metadata.name);
    if (interface_it == interfaces_by_name.end()) {
      continue;
    }

    for (const auto &interface_property : interface_it->second->properties_lexicographic) {
      ++summary.property_synthesis_sites;
      ++summary.property_synthesis_default_ivar_bindings;
      ++summary.interface_owned_property_synthesis_sites;
      ++summary.ivar_binding_sites;
      const Objc3SemanticPropertyTypeMetadata *implementation_property =
          FindPropertyInImplementationMetadata(implementation_metadata,
                                              interface_property.name);
      if (implementation_property != nullptr) {
        ++summary.implementation_property_redeclaration_sites;
      }

      if (!HasResolvedDefaultIvarBinding(interface_property)) {
        ++summary.ivar_binding_missing;
        continue;
      }

      if (implementation_property != nullptr &&
          (!IsCompatiblePropertyTypeMetadataSignature(interface_property,
                                                     *implementation_property) ||
           !HasResolvedDefaultIvarBinding(*implementation_property) ||
           !HasCompatibleResolvedDefaultIvarBinding(interface_property,
                                                   *implementation_property))) {
        ++summary.ivar_binding_conflicts;
      } else {
        ++summary.ivar_binding_resolved;
      }
    }
  }

  summary.deterministic =
      summary.property_synthesis_explicit_ivar_bindings <= summary.property_synthesis_sites &&
      summary.property_synthesis_default_ivar_bindings <= summary.property_synthesis_sites &&
      summary.property_synthesis_explicit_ivar_bindings +
              summary.property_synthesis_default_ivar_bindings ==
          summary.property_synthesis_sites &&
      summary.interface_owned_property_synthesis_sites ==
          summary.property_synthesis_sites &&
      summary.implementation_property_redeclaration_sites <=
          summary.property_synthesis_sites &&
      summary.ivar_binding_sites == summary.property_synthesis_sites &&
      summary.ivar_binding_resolved <= summary.ivar_binding_sites &&
      summary.ivar_binding_missing <= summary.ivar_binding_sites &&
      summary.ivar_binding_conflicts <= summary.ivar_binding_sites &&
      summary.ivar_binding_resolved + summary.ivar_binding_missing + summary.ivar_binding_conflicts ==
          summary.ivar_binding_sites;
  return summary;
}

static std::size_t CountSelectorKeywordPieces(const std::string &selector_symbol) {
  return static_cast<std::size_t>(std::count(selector_symbol.begin(), selector_symbol.end(), ':'));
}

static Expr::MessageSendForm ResolveMessageSendForm(const Expr &expr) {
  if (expr.message_send_form == Expr::MessageSendForm::Unary ||
      expr.message_send_form == Expr::MessageSendForm::Keyword) {
    return expr.message_send_form;
  }
  return expr.args.empty() ? Expr::MessageSendForm::Unary : Expr::MessageSendForm::Keyword;
}

static Objc3BlockLiteralCaptureSiteMetadata BuildBlockLiteralCaptureSiteMetadata(const Expr &expr) {
  Objc3BlockLiteralCaptureSiteMetadata metadata;
  metadata.parameter_count = expr.block_parameter_count;
  metadata.capture_count = expr.block_capture_count;
  metadata.body_statement_count = expr.block_body_statement_count;
  metadata.capture_set_deterministic = expr.block_capture_set_deterministic;
  metadata.literal_is_normalized = expr.block_literal_is_normalized;
  metadata.capture_profile = expr.block_capture_profile;
  metadata.line = expr.line;
  metadata.column = expr.column;
  metadata.has_count_mismatch =
      expr.block_parameter_names_lexicographic.size() != metadata.parameter_count ||
      expr.block_capture_names_lexicographic.size() != metadata.capture_count ||
      !IsSortedUniqueStrings(expr.block_parameter_names_lexicographic) ||
      !IsSortedUniqueStrings(expr.block_capture_names_lexicographic);
  return metadata;
}

static void CollectBlockLiteralCaptureSiteMetadataFromExpr(const Expr *expr,
                                                           std::vector<Objc3BlockLiteralCaptureSiteMetadata> &sites) {
  if (expr == nullptr) {
    return;
  }

  if (expr->kind == Expr::Kind::BlockLiteral) {
    sites.push_back(BuildBlockLiteralCaptureSiteMetadata(*expr));
  }

  CollectBlockLiteralCaptureSiteMetadataFromExpr(expr->receiver.get(), sites);
  CollectBlockLiteralCaptureSiteMetadataFromExpr(expr->left.get(), sites);
  CollectBlockLiteralCaptureSiteMetadataFromExpr(expr->right.get(), sites);
  CollectBlockLiteralCaptureSiteMetadataFromExpr(expr->third.get(), sites);
  for (const auto &arg : expr->args) {
    CollectBlockLiteralCaptureSiteMetadataFromExpr(arg.get(), sites);
  }
}

static void CollectBlockLiteralCaptureSiteMetadataFromStatements(
    const std::vector<std::unique_ptr<Stmt>> &statements,
    std::vector<Objc3BlockLiteralCaptureSiteMetadata> &sites);

static void CollectBlockLiteralCaptureSiteMetadataFromForClause(
    const ForClause &clause,
    std::vector<Objc3BlockLiteralCaptureSiteMetadata> &sites) {
  CollectBlockLiteralCaptureSiteMetadataFromExpr(clause.value.get(), sites);
}

static void CollectBlockLiteralCaptureSiteMetadataFromStatement(
    const Stmt *stmt, std::vector<Objc3BlockLiteralCaptureSiteMetadata> &sites) {
  if (stmt == nullptr) {
    return;
  }

  switch (stmt->kind) {
    case Stmt::Kind::Let:
      CollectBlockLiteralCaptureSiteMetadataFromExpr(stmt->let_stmt->value.get(), sites);
      return;
    case Stmt::Kind::Assign:
      CollectBlockLiteralCaptureSiteMetadataFromExpr(stmt->assign_stmt->value.get(), sites);
      return;
    case Stmt::Kind::Return:
      CollectBlockLiteralCaptureSiteMetadataFromExpr(stmt->return_stmt->value.get(), sites);
      return;
    case Stmt::Kind::If:
      CollectBlockLiteralCaptureSiteMetadataFromExpr(stmt->if_stmt->condition.get(), sites);
      CollectBlockLiteralCaptureSiteMetadataFromStatements(stmt->if_stmt->then_body, sites);
      CollectBlockLiteralCaptureSiteMetadataFromStatements(stmt->if_stmt->else_body, sites);
      return;
    case Stmt::Kind::DoWhile:
      CollectBlockLiteralCaptureSiteMetadataFromStatements(stmt->do_while_stmt->body, sites);
      CollectBlockLiteralCaptureSiteMetadataFromExpr(stmt->do_while_stmt->condition.get(), sites);
      return;
    case Stmt::Kind::For:
      CollectBlockLiteralCaptureSiteMetadataFromForClause(stmt->for_stmt->init, sites);
      CollectBlockLiteralCaptureSiteMetadataFromExpr(stmt->for_stmt->condition.get(), sites);
      CollectBlockLiteralCaptureSiteMetadataFromForClause(stmt->for_stmt->step, sites);
      CollectBlockLiteralCaptureSiteMetadataFromStatements(stmt->for_stmt->body, sites);
      return;
    case Stmt::Kind::Switch:
      CollectBlockLiteralCaptureSiteMetadataFromExpr(stmt->switch_stmt->condition.get(), sites);
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        CollectBlockLiteralCaptureSiteMetadataFromStatements(switch_case.body, sites);
      }
      return;
    case Stmt::Kind::While:
      CollectBlockLiteralCaptureSiteMetadataFromExpr(stmt->while_stmt->condition.get(), sites);
      CollectBlockLiteralCaptureSiteMetadataFromStatements(stmt->while_stmt->body, sites);
      return;
    case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
      CollectBlockLiteralCaptureSiteMetadataFromStatements(stmt->block_stmt->body, sites);
      return;
    case Stmt::Kind::Expr:
      CollectBlockLiteralCaptureSiteMetadataFromExpr(stmt->expr_stmt->value.get(), sites);
      return;
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return;
  }
}

static void CollectBlockLiteralCaptureSiteMetadataFromStatements(
    const std::vector<std::unique_ptr<Stmt>> &statements,
    std::vector<Objc3BlockLiteralCaptureSiteMetadata> &sites) {
  for (const auto &statement : statements) {
    CollectBlockLiteralCaptureSiteMetadataFromStatement(statement.get(), sites);
  }
}

static bool IsBlockLiteralCaptureSiteMetadataLess(const Objc3BlockLiteralCaptureSiteMetadata &lhs,
                                                  const Objc3BlockLiteralCaptureSiteMetadata &rhs) {
  if (lhs.capture_profile != rhs.capture_profile) {
    return lhs.capture_profile < rhs.capture_profile;
  }
  if (lhs.parameter_count != rhs.parameter_count) {
    return lhs.parameter_count < rhs.parameter_count;
  }
  if (lhs.capture_count != rhs.capture_count) {
    return lhs.capture_count < rhs.capture_count;
  }
  if (lhs.body_statement_count != rhs.body_statement_count) {
    return lhs.body_statement_count < rhs.body_statement_count;
  }
  if (lhs.capture_set_deterministic != rhs.capture_set_deterministic) {
    return lhs.capture_set_deterministic < rhs.capture_set_deterministic;
  }
  if (lhs.literal_is_normalized != rhs.literal_is_normalized) {
    return lhs.literal_is_normalized < rhs.literal_is_normalized;
  }
  if (lhs.has_count_mismatch != rhs.has_count_mismatch) {
    return lhs.has_count_mismatch < rhs.has_count_mismatch;
  }
  if (lhs.line != rhs.line) {
    return lhs.line < rhs.line;
  }
  return lhs.column < rhs.column;
}

static std::vector<Objc3BlockLiteralCaptureSiteMetadata>
BuildBlockLiteralCaptureSiteMetadataLexicographic(const Objc3Program &ast) {
  std::vector<Objc3BlockLiteralCaptureSiteMetadata> sites;
  for (const auto &global : ast.globals) {
    CollectBlockLiteralCaptureSiteMetadataFromExpr(global.value.get(), sites);
  }
  for (const auto &fn : ast.functions) {
    CollectBlockLiteralCaptureSiteMetadataFromStatements(fn.body, sites);
  }
  std::sort(sites.begin(), sites.end(), IsBlockLiteralCaptureSiteMetadataLess);
  return sites;
}

static Objc3BlockLiteralCaptureSemanticsSummary BuildBlockLiteralCaptureSemanticsSummaryFromSites(
    const std::vector<Objc3BlockLiteralCaptureSiteMetadata> &sites) {
  Objc3BlockLiteralCaptureSemanticsSummary summary;
  summary.block_literal_sites = sites.size();
  for (const auto &site : sites) {
    summary.block_parameter_entries += site.parameter_count;
    summary.block_capture_entries += site.capture_count;
    summary.block_body_statement_entries += site.body_statement_count;
    if (site.capture_count == 0u) {
      ++summary.block_empty_capture_sites;
    }
    if (!site.capture_set_deterministic) {
      ++summary.block_nondeterministic_capture_sites;
    }
    if (!site.literal_is_normalized) {
      ++summary.block_non_normalized_sites;
    }
    const bool profile_missing = site.capture_count > 0u && site.capture_profile.empty();
    const bool site_contract_violation = site.has_count_mismatch || !site.capture_set_deterministic ||
                                         !site.literal_is_normalized || profile_missing;
    if (site_contract_violation) {
      ++summary.contract_violation_sites;
    }
  }
  summary.deterministic =
      summary.contract_violation_sites == 0u &&
      summary.block_empty_capture_sites <= summary.block_literal_sites &&
      summary.block_nondeterministic_capture_sites <= summary.block_literal_sites &&
      summary.block_non_normalized_sites <= summary.block_literal_sites &&
      summary.contract_violation_sites <= summary.block_literal_sites;
  return summary;
}

static Objc3BlockLiteralCaptureSemanticsSummary BuildBlockLiteralCaptureSemanticsSummaryFromIntegrationSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  return BuildBlockLiteralCaptureSemanticsSummaryFromSites(surface.block_literal_capture_sites_lexicographic);
}

static Objc3BlockLiteralCaptureSemanticsSummary BuildBlockLiteralCaptureSemanticsSummaryFromTypeMetadataHandoff(
    const Objc3SemanticTypeMetadataHandoff &handoff) {
  return BuildBlockLiteralCaptureSemanticsSummaryFromSites(handoff.block_literal_capture_sites_lexicographic);
}

static Objc3BlockAbiInvokeTrampolineSiteMetadata BuildBlockAbiInvokeTrampolineSiteMetadata(const Expr &expr) {
  Objc3BlockAbiInvokeTrampolineSiteMetadata metadata;
  metadata.invoke_argument_slots = expr.block_abi_invoke_argument_slots;
  metadata.capture_word_count = expr.block_abi_capture_word_count;
  metadata.parameter_count = expr.block_parameter_count;
  metadata.capture_count = expr.block_capture_count;
  metadata.body_statement_count = expr.block_body_statement_count;
  metadata.has_invoke_trampoline = expr.block_abi_has_invoke_trampoline;
  metadata.layout_is_normalized = expr.block_abi_layout_is_normalized;
  metadata.layout_profile = expr.block_abi_layout_profile;
  metadata.descriptor_symbol = expr.block_abi_descriptor_symbol;
  metadata.invoke_trampoline_symbol = expr.block_invoke_trampoline_symbol;
  metadata.line = expr.line;
  metadata.column = expr.column;
  metadata.has_count_mismatch =
      metadata.invoke_argument_slots != metadata.parameter_count ||
      metadata.capture_word_count != metadata.capture_count ||
      !IsSortedUniqueStrings(expr.block_parameter_names_lexicographic) ||
      !IsSortedUniqueStrings(expr.block_capture_names_lexicographic);
  return metadata;
}

static void CollectBlockAbiInvokeTrampolineSiteMetadataFromExpr(
    const Expr *expr,
    std::vector<Objc3BlockAbiInvokeTrampolineSiteMetadata> &sites) {
  if (expr == nullptr) {
    return;
  }

  if (expr->kind == Expr::Kind::BlockLiteral) {
    sites.push_back(BuildBlockAbiInvokeTrampolineSiteMetadata(*expr));
  }

  CollectBlockAbiInvokeTrampolineSiteMetadataFromExpr(expr->receiver.get(), sites);
  CollectBlockAbiInvokeTrampolineSiteMetadataFromExpr(expr->left.get(), sites);
  CollectBlockAbiInvokeTrampolineSiteMetadataFromExpr(expr->right.get(), sites);
  CollectBlockAbiInvokeTrampolineSiteMetadataFromExpr(expr->third.get(), sites);
  for (const auto &arg : expr->args) {
    CollectBlockAbiInvokeTrampolineSiteMetadataFromExpr(arg.get(), sites);
  }
}

static void CollectBlockAbiInvokeTrampolineSiteMetadataFromStatements(
    const std::vector<std::unique_ptr<Stmt>> &statements,
    std::vector<Objc3BlockAbiInvokeTrampolineSiteMetadata> &sites);

static void CollectBlockAbiInvokeTrampolineSiteMetadataFromForClause(
    const ForClause &clause,
    std::vector<Objc3BlockAbiInvokeTrampolineSiteMetadata> &sites) {
  CollectBlockAbiInvokeTrampolineSiteMetadataFromExpr(clause.value.get(), sites);
}

static void CollectBlockAbiInvokeTrampolineSiteMetadataFromStatement(
    const Stmt *stmt,
    std::vector<Objc3BlockAbiInvokeTrampolineSiteMetadata> &sites) {
  if (stmt == nullptr) {
    return;
  }

