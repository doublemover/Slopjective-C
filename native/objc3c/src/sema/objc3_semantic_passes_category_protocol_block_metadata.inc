#include "sema/objc3_semantic_passes_category_merge_candidate_helpers.inc"

#include "sema/objc3_semantic_passes_category_merge_surface_builder.inc"

#include "sema/objc3_semantic_passes_protocol_conformance_validation.inc"

static Objc3ResolvedMessageSendMethod ResolveConcreteOwnerMethod(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &owner_name,
    const std::string &selector,
    bool is_class_method) {
  Objc3ResolvedMessageSendMethod resolution;
  resolution.owner_name = owner_name;
  resolution.is_class_method = is_class_method;

  const Objc3MethodInfo *merged_category_method =
      ResolveMethodInMergedCategorySurface(surface, owner_name, selector,
                                          is_class_method);
  if (merged_category_method != nullptr) {
    resolution.status = Objc3ResolvedMessageSendMethod::Status::Resolved;
    resolution.method = merged_category_method;
    return resolution;
  }

  const Objc3MethodInfo *interface_method =
      ResolveMethodInOwnerInterface(surface, owner_name, selector, is_class_method);
  const Objc3MethodInfo *implementation_method = ResolveMethodInOwnerImplementation(
      surface, owner_name, selector, is_class_method);

  if (interface_method == nullptr && implementation_method == nullptr) {
    bool missing_base = false;
    bool cycle_detected = false;
    const Objc3MethodInfo *super_method = FindSurfaceMethodInSuperChain(
        surface, owner_name, selector, is_class_method, missing_base,
        cycle_detected);
    if (cycle_detected || missing_base) {
      resolution.status =
          Objc3ResolvedMessageSendMethod::Status::AmbiguousConcreteSelector;
      resolution.missing_base = missing_base;
      resolution.cycle_detected = cycle_detected;
      return resolution;
    }
    if (super_method != nullptr) {
      resolution.status = Objc3ResolvedMessageSendMethod::Status::Resolved;
      resolution.method = super_method;
      return resolution;
    }
    resolution.status =
        Objc3ResolvedMessageSendMethod::Status::MissingConcreteSelector;
    return resolution;
  }
  if (interface_method != nullptr && implementation_method != nullptr) {
    if (!IsCompatibleMethodSignature(*interface_method, *implementation_method)) {
      resolution.status =
          Objc3ResolvedMessageSendMethod::Status::AmbiguousConcreteSelector;
      return resolution;
    }
    resolution.status = Objc3ResolvedMessageSendMethod::Status::Resolved;
    resolution.method = implementation_method;
    return resolution;
  }
  resolution.status = Objc3ResolvedMessageSendMethod::Status::Resolved;
  resolution.method =
      implementation_method != nullptr ? implementation_method : interface_method;
  return resolution;
}

static Expr::DispatchSurfaceKind ResolveNormalizedMessageSendDispatchSurfaceKind(
    const Expr &expr,
    const Objc3MessageSendResolutionContext &context) {
  Expr::DispatchSurfaceKind dispatch_kind = expr.dispatch_surface_kind;
  if (expr.dispatch_surface_is_normalized) {
    return dispatch_kind;
  }
  if (expr.receiver == nullptr) {
    return dispatch_kind;
  }
  if (expr.receiver->kind == Expr::Kind::NilLiteral) {
    return Expr::DispatchSurfaceKind::Instance;
  }
  if (expr.receiver->kind != Expr::Kind::Identifier) {
    return Expr::DispatchSurfaceKind::Dynamic;
  }
  if (context.inside_method && expr.receiver->ident == "super") {
    return Expr::DispatchSurfaceKind::Super;
  }
  if ((context.surface != nullptr &&
       context.surface->interfaces.find(expr.receiver->ident) !=
           context.surface->interfaces.end()) ||
      (context.surface != nullptr &&
       context.surface->implementations.find(expr.receiver->ident) !=
           context.surface->implementations.end())) {
    return Expr::DispatchSurfaceKind::Class;
  }
  if (context.inside_method && expr.receiver->ident == "self") {
    return context.is_class_method ? Expr::DispatchSurfaceKind::Class
                                   : Expr::DispatchSurfaceKind::Instance;
  }
  return Expr::DispatchSurfaceKind::Instance;
}

static Objc3ResolvedMessageSendMethod ResolveConcreteMessageSendMethod(
    const Expr &expr,
    const Objc3MessageSendResolutionContext &context) {
  Objc3ResolvedMessageSendMethod resolution;
  if (context.surface == nullptr || expr.receiver == nullptr) {
    return resolution;
  }

  const Expr::DispatchSurfaceKind dispatch_kind =
      ResolveNormalizedMessageSendDispatchSurfaceKind(expr, context);

  switch (dispatch_kind) {
    case Expr::DispatchSurfaceKind::Super: {
      resolution.owner_name = context.current_super_name;
      resolution.is_class_method = context.is_class_method;
      if (context.current_implementation_name.empty() ||
          context.current_super_name.empty()) {
        resolution.status =
            Objc3ResolvedMessageSendMethod::Status::MissingConcreteSelector;
        return resolution;
      }
      bool missing_base = false;
      bool cycle_detected = false;
      const Objc3MethodInfo *super_method = FindSurfaceMethodInSuperChain(
          *context.surface, context.current_implementation_name, expr.selector,
          context.is_class_method, missing_base, cycle_detected);
      if (cycle_detected || missing_base) {
        resolution.status =
            Objc3ResolvedMessageSendMethod::Status::AmbiguousConcreteSelector;
        resolution.missing_base = missing_base;
        resolution.cycle_detected = cycle_detected;
        return resolution;
      }
      if (super_method == nullptr) {
        resolution.status =
            Objc3ResolvedMessageSendMethod::Status::MissingConcreteSelector;
        return resolution;
      }
      resolution.status = Objc3ResolvedMessageSendMethod::Status::Resolved;
      resolution.method = super_method;
      return resolution;
    }
    case Expr::DispatchSurfaceKind::Class: {
      std::string owner_name;
      if (expr.receiver->kind == Expr::Kind::Identifier &&
          expr.receiver->ident == "self" && context.inside_method &&
          !context.current_implementation_name.empty()) {
        owner_name = context.current_implementation_name;
      } else if (expr.receiver->kind == Expr::Kind::Identifier) {
        owner_name = expr.receiver->ident;
      }
      if (owner_name.empty()) {
        return resolution;
      }
      return ResolveConcreteOwnerMethod(*context.surface, owner_name,
                                        expr.selector, true);
    }
    case Expr::DispatchSurfaceKind::Instance: {
      if (!(expr.receiver->kind == Expr::Kind::Identifier &&
            expr.receiver->ident == "self" && context.inside_method &&
            !context.is_class_method &&
            !context.current_implementation_name.empty())) {
        return resolution;
      }
      return ResolveConcreteOwnerMethod(*context.surface,
                                        context.current_implementation_name,
                                        expr.selector, false);
    }
    case Expr::DispatchSurfaceKind::Direct:
    case Expr::DispatchSurfaceKind::Dynamic:
    case Expr::DispatchSurfaceKind::Unclassified:
      return resolution;
  }
  return resolution;
}

static Objc3MethodLookupOverrideConflictSummary BuildMethodLookupOverrideConflictSummaryFromIntegrationSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  Objc3MethodLookupOverrideConflictSummary summary;
  std::unordered_set<std::string> unresolved_units;

  for (const auto &implementation_entry : surface.implementations) {
    const std::string &implementation_name = implementation_entry.first;
    const Objc3ImplementationInfo &implementation_info = implementation_entry.second;
    summary.method_lookup_sites += implementation_info.methods.size();

    const auto interface_it = surface.interfaces.find(implementation_name);
    if (interface_it == surface.interfaces.end()) {
      unresolved_units.insert("impl:" + implementation_name);
      summary.method_lookup_misses += implementation_info.methods.size();
      continue;
    }

    for (const auto &method_entry : implementation_info.methods) {
      const auto interface_method_it = interface_it->second.methods.find(method_entry.first);
      if (interface_method_it == interface_it->second.methods.end()) {
        ++summary.method_lookup_misses;
      } else {
        ++summary.method_lookup_hits;
      }
    }
  }

  for (const auto &interface_entry : surface.interfaces) {
    const std::string &interface_name = interface_entry.first;
    const Objc3InterfaceInfo &interface_info = interface_entry.second;
    if (interface_info.super_name.empty()) {
      continue;
    }
    for (const auto &method_entry : interface_info.methods) {
      ++summary.override_lookup_sites;
      // anchor: this superclass walk and signature check remain the
      // canonical override-legality source that the executable metadata graph
      // replays with `method-to-overridden-method` owner edges.
      bool missing_base = false;
      bool cycle_detected = false;
      const Objc3MethodInfo *base_method =
          FindSurfaceMethodInSuperChain(surface, interface_name,
                                       method_entry.first,
                                       method_entry.second.is_class_method,
                                       missing_base, cycle_detected);
      if (cycle_detected) {
        summary.deterministic = false;
      }
      if (missing_base) {
        unresolved_units.insert("iface:" + interface_name);
        ++summary.override_lookup_misses;
        continue;
      }
      if (base_method == nullptr) {
        ++summary.override_lookup_misses;
        continue;
      }
      ++summary.override_lookup_hits;
      if (!IsCompatibleMethodSignature(*base_method, method_entry.second)) {
        ++summary.override_conflicts;
      }
    }
  }

  summary.unresolved_base_interfaces = unresolved_units.size();
  summary.deterministic =
      summary.deterministic &&
      summary.method_lookup_hits <= summary.method_lookup_sites &&
      summary.method_lookup_hits + summary.method_lookup_misses == summary.method_lookup_sites &&
      summary.override_lookup_hits <= summary.override_lookup_sites &&
      summary.override_lookup_hits + summary.override_lookup_misses == summary.override_lookup_sites &&
      summary.override_conflicts <= summary.override_lookup_hits;
  return summary;
}

static bool IsCompatibleMethodTypeMetadataSignature(const Objc3SemanticMethodTypeMetadata &lhs,
                                                    const Objc3SemanticMethodTypeMetadata &rhs) {
  if (lhs.arity != rhs.arity || lhs.return_type != rhs.return_type ||
      lhs.return_is_vector != rhs.return_is_vector ||
      lhs.is_class_method != rhs.is_class_method) {
    return false;
  }
  if (lhs.return_is_vector &&
      (lhs.return_vector_base_spelling != rhs.return_vector_base_spelling ||
       lhs.return_vector_lane_count != rhs.return_vector_lane_count)) {
    return false;
  }
  if (lhs.param_types.size() != rhs.param_types.size() ||
      lhs.param_is_vector.size() != rhs.param_is_vector.size() ||
      lhs.param_vector_base_spelling.size() != rhs.param_vector_base_spelling.size() ||
      lhs.param_vector_lane_count.size() != rhs.param_vector_lane_count.size()) {
    return false;
  }
  for (std::size_t i = 0; i < lhs.param_types.size(); ++i) {
    if (lhs.param_types[i] != rhs.param_types[i] ||
        lhs.param_is_vector[i] != rhs.param_is_vector[i]) {
      return false;
    }
    if (lhs.param_is_vector[i] &&
        (lhs.param_vector_base_spelling[i] != rhs.param_vector_base_spelling[i] ||
         lhs.param_vector_lane_count[i] != rhs.param_vector_lane_count[i])) {
      return false;
    }
  }
  return true;
}

static const Objc3SemanticMethodTypeMetadata *FindMethodInInterfaceMetadata(
    const Objc3SemanticInterfaceTypeMetadata &metadata,
    const std::string &selector) {
  for (const auto &method_metadata : metadata.methods_lexicographic) {
    if (method_metadata.selector == selector) {
      return &method_metadata;
    }
  }
  return nullptr;
}

static const Objc3SemanticMethodTypeMetadata *FindHandoffMethodInSuperChain(
    const std::unordered_map<std::string, const Objc3SemanticInterfaceTypeMetadata *> &interfaces_by_name,
    const Objc3SemanticInterfaceTypeMetadata &metadata,
    const std::string &selector,
    bool &missing_base,
    bool &cycle_detected) {
  missing_base = false;
  cycle_detected = false;
  std::string next_super = metadata.super_name;
  std::unordered_set<std::string> visited;
  while (!next_super.empty()) {
    if (!visited.insert(next_super).second) {
      cycle_detected = true;
      return nullptr;
    }
    const auto super_it = interfaces_by_name.find(next_super);
    if (super_it == interfaces_by_name.end()) {
      missing_base = true;
      return nullptr;
    }
    const Objc3SemanticMethodTypeMetadata *method =
        FindMethodInInterfaceMetadata(*super_it->second, selector);
    if (method != nullptr) {
      return method;
    }
    next_super = super_it->second->super_name;
  }
  return nullptr;
}

static Objc3MethodLookupOverrideConflictSummary BuildMethodLookupOverrideConflictSummaryFromTypeMetadataHandoff(
    const Objc3SemanticTypeMetadataHandoff &handoff) {
  Objc3MethodLookupOverrideConflictSummary summary;
  std::unordered_map<std::string, const Objc3SemanticInterfaceTypeMetadata *> interfaces_by_name;
  interfaces_by_name.reserve(handoff.interfaces_lexicographic.size());
  for (const auto &metadata : handoff.interfaces_lexicographic) {
    interfaces_by_name.emplace(metadata.name, &metadata);
  }
  std::unordered_set<std::string> unresolved_units;

  for (const auto &implementation_metadata : handoff.implementations_lexicographic) {
    summary.method_lookup_sites += implementation_metadata.methods_lexicographic.size();
    const auto interface_it = interfaces_by_name.find(implementation_metadata.name);
    if (interface_it == interfaces_by_name.end()) {
      unresolved_units.insert("impl:" + implementation_metadata.name);
      summary.method_lookup_misses += implementation_metadata.methods_lexicographic.size();
      continue;
    }
    for (const auto &method_metadata : implementation_metadata.methods_lexicographic) {
      const Objc3SemanticMethodTypeMetadata *interface_method =
          FindMethodInInterfaceMetadata(*interface_it->second, method_metadata.selector);
      if (interface_method == nullptr) {
        ++summary.method_lookup_misses;
      } else {
        ++summary.method_lookup_hits;
      }
    }
  }

  for (const auto &interface_metadata : handoff.interfaces_lexicographic) {
    if (interface_metadata.super_name.empty()) {
      continue;
    }
    for (const auto &method_metadata : interface_metadata.methods_lexicographic) {
      ++summary.override_lookup_sites;
      bool missing_base = false;
      bool cycle_detected = false;
      const Objc3SemanticMethodTypeMetadata *base_method =
          FindHandoffMethodInSuperChain(interfaces_by_name,
                                        interface_metadata,
                                        method_metadata.selector,
                                        missing_base,
                                        cycle_detected);
      if (cycle_detected) {
        summary.deterministic = false;
      }
      if (missing_base) {
        unresolved_units.insert("iface:" + interface_metadata.name);
        ++summary.override_lookup_misses;
        continue;
      }
      if (base_method == nullptr) {
        ++summary.override_lookup_misses;
        continue;
      }
      ++summary.override_lookup_hits;
      if (!IsCompatibleMethodTypeMetadataSignature(*base_method, method_metadata)) {
        ++summary.override_conflicts;
      }
    }
  }

  summary.unresolved_base_interfaces = unresolved_units.size();
  summary.deterministic =
      summary.deterministic &&
      summary.method_lookup_hits <= summary.method_lookup_sites &&
      summary.method_lookup_hits + summary.method_lookup_misses == summary.method_lookup_sites &&
      summary.override_lookup_hits <= summary.override_lookup_sites &&
      summary.override_lookup_hits + summary.override_lookup_misses == summary.override_lookup_sites &&
      summary.override_conflicts <= summary.override_lookup_hits;
  return summary;
}

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

