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
