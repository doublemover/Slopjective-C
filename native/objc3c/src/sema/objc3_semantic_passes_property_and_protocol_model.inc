           info.type == ValueType::ObjCObjectPtr;
  };
  const bool property_has_arc_runtime_owned_attribute =
      objc3c::support::HasArcRuntimeOwnedPropertyAttribute(
          property_has_attribute_named("strong"),
          property_has_attribute_named("retain"),
          property_has_attribute_named("copy"),
          property_has_attribute_named("weak"),
          property.property_attribute_profile);

  Objc3PropertyInfo info;
  info.type = property.type;
  info.canonical_type = BuildCanonicalSemanticTypeFromProperty(property);
  info.is_vector = property.vector_spelling;
  info.vector_base_spelling = property.vector_base_spelling;
  info.vector_lane_count = property.vector_lane_count;
  info.id_spelling = property.id_spelling;
  info.class_spelling = property.class_spelling;
  info.instancetype_spelling = property.instancetype_spelling;
  info.object_pointer_type_spelling = property.object_pointer_type_spelling;
  info.has_generic_suffix = property.has_generic_suffix;
  info.has_pointer_declarator = property.has_pointer_declarator;
  info.has_nullability_suffix = !property.nullability_suffix_tokens.empty();
  info.has_ownership_qualifier = property.has_ownership_qualifier;
  info.ownership_insert_retain = property.ownership_insert_retain;
  info.ownership_insert_release = property.ownership_insert_release;
  info.ownership_insert_autorelease = property.ownership_insert_autorelease;
  info.ownership_is_weak_reference = property.ownership_is_weak_reference;
  info.ownership_is_unowned_reference = property.ownership_is_unowned_reference;
  info.ownership_is_unowned_safe_reference = property.ownership_is_unowned_safe_reference;
  info.ownership_arc_diagnostic_candidate = property.ownership_arc_diagnostic_candidate;
  info.ownership_arc_fixit_available = property.ownership_arc_fixit_available;
  info.ownership_arc_diagnostic_profile = property.ownership_arc_diagnostic_profile;
  info.ownership_arc_fixit_hint = property.ownership_arc_fixit_hint;
  info.ownership_lifetime_profile = property.ownership_lifetime_profile;
  info.ownership_runtime_hook_profile = property.ownership_runtime_hook_profile;
  info.has_invalid_generic_suffix = HasInvalidGenericPropertyTypeSuffix(property);
  info.has_invalid_pointer_declarator = HasInvalidPointerPropertyTypeDeclarator(property);
  info.has_invalid_nullability_suffix = HasInvalidNullabilityPropertyTypeSuffix(property);
  info.has_invalid_ownership_qualifier = HasInvalidOwnershipQualifierPropertyTypeSuffix(property);
  info.has_invalid_type_suffix = HasInvalidPropertyTypeSuffix(property);
  info.attribute_entries = property.attributes.size();
  info.is_readonly = property.is_readonly;
  info.is_readwrite = property.is_readwrite;
  info.is_atomic = property.is_atomic;
  info.is_nonatomic = property.is_nonatomic;
  info.is_copy = property.is_copy;
  info.is_retain = property.is_retain;
  info.is_strong = property.is_strong;
  info.is_weak = property.is_weak;
  info.is_unowned = property.is_unowned;
  info.is_unsafe_unretained = property.is_unsafe_unretained;
  info.is_assign = property.is_assign;
  info.is_nullable = property.is_nullable;
  info.is_nonnull = property.is_nonnull;
  info.is_null_resettable = property.is_null_resettable;
  info.is_class = property.is_class;
  info.is_direct = property.is_direct;
  info.has_getter = property.has_getter;
  info.has_setter = property.has_setter;
  info.getter_selector = TrimAsciiWhitespace(property.getter_selector);
  info.setter_selector = TrimAsciiWhitespace(property.setter_selector);
  info.ivar_binding_symbol = property.ivar_binding_symbol;
  info.executable_synthesized_binding_kind =
      property.executable_synthesized_binding_kind;
  info.executable_synthesized_binding_symbol =
      property.executable_synthesized_binding_symbol;
  info.property_attribute_profile = property.property_attribute_profile;
  info.effective_getter_selector = property.effective_getter_selector;
  info.effective_setter_available = property.effective_setter_available;
  info.effective_setter_selector = property.effective_setter_selector;
  info.accessor_ownership_profile = property.accessor_ownership_profile;
  info.executable_ivar_layout_symbol = property.executable_ivar_layout_symbol;
  info.executable_ivar_layout_slot_index =
      property.executable_ivar_layout_slot_index;
  info.executable_ivar_layout_size_bytes =
      property.executable_ivar_layout_size_bytes;
  info.executable_ivar_layout_alignment_bytes =
      property.executable_ivar_layout_alignment_bytes;
  info.executable_ivar_layout_offset_bytes =
      property.executable_ivar_layout_offset_bytes;
  info.executable_ivar_layout_padding_bytes =
      property.executable_ivar_layout_padding_bytes;
  info.executable_ivar_layout_inherited_slot_count =
      property.executable_ivar_layout_inherited_slot_count;
  info.executable_ivar_layout_inherited_size_bytes =
      property.executable_ivar_layout_inherited_size_bytes;
  info.executable_ivar_layout_owner_size_bytes =
      property.executable_ivar_layout_owner_size_bytes;
  info.executable_ivar_init_order_index =
      property.executable_ivar_init_order_index;
  info.executable_ivar_destroy_order_index =
      property.executable_ivar_destroy_order_index;
  info.executable_ivar_layout_valid =
      property.executable_ivar_layout_valid;
  info.executable_ivar_layout_replay_key =
      property.executable_ivar_layout_replay_key;
  info.line = property.line;
  info.column = property.column;

  const bool arc_inferred_strong_object_property =
      arc_mode_enabled && !info.has_ownership_qualifier && !info.is_weak &&
      !info.is_unowned && !info.is_unsafe_unretained && !info.is_assign &&
      property_is_object_like(info);
  if (arc_inferred_strong_object_property) {
    // ARC inference/lifetime implementation anchor: under explicit
    // ARC mode, unqualified object properties in the supported runnable slice
    // now inherit a canonical strong-owned retain/release profile directly in
    // sema so lowering and replay accounting no longer depend on explicit
    // ownership spelling alone.
    info.has_ownership_qualifier = true;
    info.ownership_insert_retain = true;
    info.ownership_insert_release = true;
    info.ownership_insert_autorelease = false;
    info.ownership_is_weak_reference = false;
    info.ownership_is_unowned_reference = false;
    info.ownership_is_unowned_safe_reference = false;
    info.ownership_arc_diagnostic_candidate = false;
    info.ownership_arc_fixit_available = false;
    info.ownership_arc_diagnostic_profile.clear();
    info.ownership_arc_fixit_hint.clear();
    info.ownership_lifetime_profile = "strong-owned";
    info.ownership_runtime_hook_profile.clear();
  }

  if (arc_mode_enabled &&
      (property_is_object_like(info) ||
       property_has_arc_runtime_owned_attribute) &&
      info.ownership_lifetime_profile.empty()) {
    // ARC interaction-semantics anchor: attribute-only strong/weak
    // ARC properties now publish the same lifetime and synthesized-accessor
    // ownership profiles that later property/runtime lanes already consume for
    // explicitly qualified ownership spellings.
    // runnable-arc-runtime gate anchor: lane-E consumes this
    // canonical property ARC packet as part of the runnable ARC proof chain.
    // runnable-arc-closeout anchor: this canonical property ARC
    // packet remains a closeout-matrix row synchronized with the private
    // runtime-probe-backed property evidence.
    const objc3c::support::Objc3PropertyOwnershipProfileKind
        ownership_profile_kind =
            objc3c::support::ApplyPropertyOwnershipProfileRetiredRoute(
                info,
                info.is_weak || property_has_attribute_named("weak"),
                info.is_strong || property_has_attribute_named("strong"),
                info.is_retain || property_has_attribute_named("retain"),
                info.is_copy || property_has_attribute_named("copy"),
                true);
    if (ownership_profile_kind ==
        objc3c::support::Objc3PropertyOwnershipProfileKind::kWeak) {
      info.ownership_is_weak_reference = true;
    }
  }
  if (arc_mode_enabled &&
      (property_is_object_like(info) ||
       property_has_arc_runtime_owned_attribute) &&
      info.accessor_ownership_profile.empty() &&
      (!info.ownership_lifetime_profile.empty() ||
       !info.ownership_runtime_hook_profile.empty())) {
    // ARC interaction-semantics anchor: synthesized accessor
    // ownership packets now stay aligned with ARC-inferred or attribute-owned
    // property lifetime profiles instead of remaining blank for the same
    // executable storage model.
    objc3c::support::RebuildPropertyAccessorOwnershipProfile(info);
  }

  std::unordered_map<std::string, std::size_t> attribute_name_counts;
  for (const auto &attribute : property.attributes) {
    info.attribute_names_lexicographic.push_back(attribute.name);
    const std::size_t count = ++attribute_name_counts[attribute.name];
    bool invalid_attribute = false;
    const auto emit_invalid_attribute = [&](const std::string &message) {
      diagnostics.push_back(MakeDiag(attribute.line, attribute.column, "O3S206", message));
      invalid_attribute = true;
    };

    if (!IsKnownPropertyAttributeName(attribute.name)) {
      info.has_unknown_attribute = true;
      emit_invalid_attribute("type mismatch: unknown @property attribute '" + attribute.name + "' for property '" +
                             property.name + "' in " + owner_kind + " '" + owner_name + "'");
    }
    if (count > 1u) {
      info.has_duplicate_attribute = true;
      emit_invalid_attribute("type mismatch: duplicate @property attribute '" + attribute.name +
                             "' for property '" + property.name + "' in " + owner_kind + " '" + owner_name + "'");
    }
    if ((attribute.name != "getter" && attribute.name != "setter" &&
         attribute.name != "behavior") &&
        attribute.has_value) {
      emit_invalid_attribute("type mismatch: @property attribute '" + attribute.name +
                             "' must not specify a value for property '" + property.name + "' in " + owner_kind +
                             " '" + owner_name + "'");
    }
    if ((attribute.name == "getter" || attribute.name == "setter") &&
        (!attribute.has_value || TrimAsciiWhitespace(attribute.value).empty())) {
      emit_invalid_attribute("type mismatch: @property accessor attribute '" + attribute.name +
                             "' requires a selector value for property '" + property.name + "' in " + owner_kind +
                             " '" + owner_name + "'");
    }
    if (attribute.name == "behavior" &&
        (!attribute.has_value || TrimAsciiWhitespace(attribute.value).empty())) {
      emit_invalid_attribute("type mismatch: @property behavior attribute requires a non-empty value for property '" +
                             property.name + "' in " + owner_kind + " '" + owner_name + "'");
    }

    if (invalid_attribute) {
      ++info.invalid_attribute_entries;
    }
  }
  std::sort(info.attribute_names_lexicographic.begin(), info.attribute_names_lexicographic.end());

  const auto emit_property_contract_violation = [&](unsigned line, unsigned column, const std::string &message) {
    diagnostics.push_back(MakeDiag(line, column, "O3S206", message));
    ++info.property_contract_violations;
  };

  if (info.has_getter) {
    if (info.getter_selector.empty() || !IsValidPropertyGetterSelector(info.getter_selector)) {
      info.has_accessor_selector_contract_violation = true;
      emit_property_contract_violation(
          property.line,
          property.column,
          "type mismatch: invalid @property getter selector '" +
              (info.getter_selector.empty() ? std::string("<empty>") : info.getter_selector) + "' for property '" +
              property.name + "' in " + owner_kind + " '" + owner_name + "'");
    }
  }
  if (info.has_setter) {
    if (info.setter_selector.empty() || !IsValidPropertySetterSelector(info.setter_selector)) {
      info.has_accessor_selector_contract_violation = true;
      emit_property_contract_violation(
          property.line,
          property.column,
          "type mismatch: invalid @property setter selector '" +
              (info.setter_selector.empty() ? std::string("<empty>") : info.setter_selector) + "' for property '" +
              property.name + "' in " + owner_kind + " '" + owner_name + "'");
    }
  }
  if (info.is_readonly && info.is_readwrite) {
    info.has_readwrite_conflict = true;
    emit_property_contract_violation(
        property.line,
        property.column,
        "type mismatch: @property modifiers 'readonly' and 'readwrite' conflict for property '" + property.name +
            "' in " + owner_kind + " '" + owner_name + "'");
  }
  if (info.is_atomic && info.is_nonatomic) {
    info.has_atomicity_conflict = true;
    emit_property_contract_violation(
        property.line,
        property.column,
        "type mismatch: @property modifiers 'atomic' and 'nonatomic' conflict for property '" + property.name +
            "' in " + owner_kind + " '" + owner_name + "'");
  }
  const std::size_t ownership_modifiers =
      objc3c::support::CountRuntimeBackedPropertyOwnershipModifierSlots(
          info.is_copy, info.is_strong, info.is_retain, info.is_weak,
          info.is_unowned, info.is_assign, info.is_unsafe_unretained);
  if (ownership_modifiers > 1u) {
    info.has_ownership_conflict = true;
    emit_property_contract_violation(
        property.line,
        property.column,
        "type mismatch: @property ownership modifiers conflict for property '" + property.name + "' in " + owner_kind +
            " '" + owner_name + "'");
  }
  info.has_weak_unowned_conflict =
      property.has_weak_unowned_conflict ||
      (info.is_weak && (info.is_unowned || info.is_unsafe_unretained));
  if (info.has_weak_unowned_conflict) {
    emit_property_contract_violation(
        property.line,
        property.column,
        "type mismatch: @property ownership modifiers 'weak' and 'unowned' conflict for property '" + property.name +
            "' in " + owner_kind + " '" + owner_name + "'");
  }
  const bool supports_runtime_managed_ownership =
      objc3c::support::SupportsRuntimeManagedPropertyOwnership(
          info.is_vector, info.id_spelling, info.class_spelling,
          info.instancetype_spelling, info.object_pointer_type_spelling);
  const bool has_runtime_managed_ownership_modifier =
      objc3c::support::HasRuntimeManagedPropertyOwnershipModifier(
          info.is_copy, info.is_strong, info.is_retain, info.is_weak,
          info.is_unowned, info.is_unsafe_unretained);
  const bool has_explicit_runtime_backed_storage_modifier =
      objc3c::support::HasExplicitRuntimeBackedPropertyStorageModifier(
          info.is_copy, info.is_strong, info.is_retain, info.is_weak,
          info.is_unowned, info.is_assign, info.is_unsafe_unretained);
  // accessor/ownership legality anchor: lane-B remains the
  // fail-closed source of truth for unsupported scalar ownership modifiers and
  // atomic ownership-aware property combinations until runtime storage
  // semantics land.
  if (has_runtime_managed_ownership_modifier &&
      !supports_runtime_managed_ownership) {
    std::string ownership_modifier =
        objc3c::support::BuildRuntimeBackedPropertyStorageModifier(
            info.is_copy, info.is_strong, info.is_retain, info.is_weak,
            info.is_unowned, info.is_assign, info.is_unsafe_unretained);
    emit_property_contract_violation(
        property.line,
        property.column,
        "type mismatch: @property ownership modifier '" + ownership_modifier +
            "' requires an Objective-C object property for property '" +
            property.name + "' in " + owner_kind + " '" + owner_name + "'");
  }
  // runtime-backed storage ownership legality anchor: explicit
  // `__weak`, `__unsafe_unretained`, and `__strong` qualifiers now participate
  // in live property storage legality for runtime-backed Objective-C object
  // properties. Conflicting explicit storage modifiers fail before metadata
  // emission so later runtime lanes do not have to recover storage intent from
  // inconsistent source.
  // ARC semantic-rule freeze anchor: explicit ARC mode does not
  // relax property ownership conflicts, atomic ownership-aware storage, or
  // broader inference. Those forms remain deterministic semantic failures
  // until later ARC lifetime implementation issues land.
  if (supports_runtime_managed_ownership && info.has_ownership_qualifier &&
      !info.has_invalid_ownership_qualifier && ownership_modifiers <= 1u &&
      has_explicit_runtime_backed_storage_modifier) {
    const std::string explicit_modifier =
        objc3c::support::BuildRuntimeBackedPropertyStorageModifier(
            info.is_copy, info.is_strong, info.is_retain, info.is_weak,
            info.is_unowned, info.is_assign, info.is_unsafe_unretained);
    const auto emit_ownership_qualifier_storage_mismatch =
        [&](const std::string &expected_storage_model) {
          emit_property_contract_violation(
              OwnershipQualifierLine(property.ownership_qualifier_tokens,
                                     property.line),
              OwnershipQualifierColumn(property.ownership_qualifier_tokens,
                                       property.column),
              "type mismatch: property ownership qualifier '" +
                  property.ownership_qualifier_spelling +
                  "' conflicts with @property ownership modifier '" +
                  explicit_modifier + "' for property '" + property.name +
                  "' in " + owner_kind + " '" + owner_name +
                  "'; expected " + expected_storage_model);
        };
    if (property.ownership_qualifier_spelling == "__weak" &&
        explicit_modifier != "weak") {
      emit_ownership_qualifier_storage_mismatch(
          "@property modifier 'weak' or no explicit ownership modifier");
    } else if (property.ownership_qualifier_spelling == "__unsafe_unretained" &&
               explicit_modifier != "assign" && explicit_modifier != "unsafe_unretained") {
      emit_ownership_qualifier_storage_mismatch(
          "@property modifier 'assign', 'unsafe_unretained', or no explicit ownership modifier");
    } else if (property.ownership_qualifier_spelling == "__strong" &&
               explicit_modifier != "strong" && explicit_modifier != "retain" &&
               explicit_modifier != "copy") {
      emit_ownership_qualifier_storage_mismatch(
          "@property modifier 'strong', 'retain', 'copy', or no explicit ownership modifier");
    }
  }
  if (info.is_atomic && has_runtime_managed_ownership_modifier &&
      supports_runtime_managed_ownership) {
    emit_property_contract_violation(
        property.line,
        property.column,
        "type mismatch: atomic ownership-aware property '" + property.name +
            "' in " + owner_kind + " '" + owner_name +
            "' is unsupported until executable accessor storage semantics land");
  }
  if (info.is_nullable && info.is_nonnull) {
    emit_property_contract_violation(
        property.line,
        property.column,
        "type mismatch: @property nullability modifiers 'nullable' and 'nonnull' conflict for property '" +
            property.name + "' in " + owner_kind + " '" + owner_name + "'");
  }
  if (info.is_null_resettable && (info.is_nullable || info.is_nonnull)) {
    emit_property_contract_violation(
        property.line,
        property.column,
        "type mismatch: @property modifier 'null_resettable' conflicts with explicit nullable/nonnull for property '" +
            property.name + "' in " + owner_kind + " '" + owner_name + "'");
  }
  if (info.is_readonly && info.has_setter) {
    info.has_accessor_selector_contract_violation = true;
    emit_property_contract_violation(
        property.line,
        property.column,
        "type mismatch: readonly property '" + property.name + "' in " + owner_kind + " '" + owner_name +
            "' must not declare a setter modifier");
  }
  if (property.property_behavior_declared) {
    const std::string canonical_behavior =
        NormalizeMetaprogrammingSupportedPropertyBehaviorName(
            property.property_behavior_name);
    const auto emit_property_behavior_violation =
        [&](std::string_view code, const std::string &message) {
          diagnostics.push_back(MakeDiag(property.line, property.column,
                                         std::string(code), message));
          ++info.property_contract_violations;
        };
    if (canonical_behavior.empty()) {
      emit_property_behavior_violation(
          "O3S326",
          "unsupported property behavior '" +
              NormalizeMetaprogrammingRawPropertyBehaviorToken(
                  property.property_behavior_name) +
              "' for property '" + property.name + "' in " + owner_kind +
              " '" + owner_name + "'");
    } else {
      if (!property_is_object_like(info)) {
        emit_property_behavior_violation(
            "O3S327",
            "property behavior '" + canonical_behavior +
                "' requires an Objective-C object property for property '" +
                property.name + "' in " + owner_kind + " '" + owner_name +
                "'");
      }
      if (canonical_behavior == "Observed") {
        if (owner_kind == "protocol") {
          emit_property_behavior_violation(
              "O3S328",
              "observed property behavior requires a concrete interface or implementation property for property '" +
                  property.name + "' in " + owner_kind + " '" + owner_name +
                  "'");
        }
        if (info.is_readonly || !info.effective_setter_available) {
          emit_property_behavior_violation(
              "O3S329",
              "observed property behavior requires writable setter-visible storage for property '" +
                  property.name + "' in " + owner_kind + " '" + owner_name +
                  "'");
        }
      } else if (canonical_behavior == "Projected") {
        if (!info.is_readonly || info.has_setter ||
            info.effective_setter_available) {
          emit_property_behavior_violation(
              "O3S330",
              "projected property behavior requires a readonly getter-only property for property '" +
                  property.name + "' in " + owner_kind + " '" + owner_name +
                  "'");
        }
      }
    }
  }

  info.has_invalid_attribute_contract =
      info.has_unknown_attribute || info.has_duplicate_attribute || info.has_readwrite_conflict ||
      info.has_atomicity_conflict || info.has_ownership_conflict || info.has_accessor_selector_contract_violation ||
      info.invalid_attribute_entries > 0 || info.property_contract_violations > 0;
  return info;
}

struct PropertyAccessorSelectorOwnerState {
  std::string property_name;
  bool owner_marked_conflict = false;
};

static void FinalizePropertyInvalidAttributeContract(Objc3PropertyInfo &property) {
  property.has_invalid_attribute_contract =
      property.has_unknown_attribute || property.has_duplicate_attribute ||
      property.has_readwrite_conflict || property.has_atomicity_conflict ||
      property.has_ownership_conflict ||
      property.has_accessor_selector_contract_violation ||
      property.invalid_attribute_entries > 0 ||
      property.property_contract_violations > 0;
}

static void EmitPropertyContractViolation(Objc3PropertyInfo &property,
                                          unsigned line,
                                          unsigned column,
                                          const std::string &message,
                                          std::vector<std::string> &diagnostics,
                                          bool accessor_selector_violation) {
  if (accessor_selector_violation) {
    property.has_accessor_selector_contract_violation = true;
  }
  diagnostics.push_back(MakeDiag(line, column, "O3S206", message));
  ++property.property_contract_violations;
  FinalizePropertyInvalidAttributeContract(property);
}

static bool EmitImplementationPropertyReflectionAccessorCompatibilityDiagnostics(
    const Objc3PropertyInfo &interface_property,
    Objc3PropertyInfo &implementation_property,
    const std::string &property_name,
    const std::string &implementation_name,
    std::vector<std::string> &diagnostics) {
  bool emitted = false;
  const auto emit = [&](const std::string &message,
                        bool accessor_selector_violation) {
    EmitPropertyContractViolation(implementation_property,
                                  implementation_property.line,
                                  implementation_property.column,
                                  message,
                                  diagnostics,
                                  accessor_selector_violation);
    emitted = true;
  };

  const std::string implementation_label =
      "implementation '" + implementation_name + "'";

  const bool getter_profile_mismatch =
      interface_property.getter_selector != implementation_property.getter_selector ||
      interface_property.effective_getter_selector !=
          implementation_property.effective_getter_selector;
  if (getter_profile_mismatch) {
    emit("type mismatch: effective getter selector profile for property '" +
             property_name + "' in " + implementation_label +
             " drifted from the interface declaration",
         true);
  }

  const bool setter_profile_mismatch =
      interface_property.has_setter != implementation_property.has_setter ||
      interface_property.setter_selector != implementation_property.setter_selector ||
      interface_property.effective_setter_available !=
          implementation_property.effective_setter_available ||
      interface_property.effective_setter_selector !=
          implementation_property.effective_setter_selector;
  if (setter_profile_mismatch) {
    emit("type mismatch: effective setter selector profile for property '" +
             property_name + "' in " + implementation_label +
             " drifted from the interface declaration",
         true);
  }

  const bool reflection_profile_mismatch =
      interface_property.property_attribute_profile !=
          implementation_property.property_attribute_profile ||
      interface_property.ownership_lifetime_profile !=
          implementation_property.ownership_lifetime_profile ||
      interface_property.ownership_runtime_hook_profile !=
          implementation_property.ownership_runtime_hook_profile ||
      interface_property.accessor_ownership_profile !=
          implementation_property.accessor_ownership_profile;
  if (reflection_profile_mismatch) {
    emit("type mismatch: reflected property attribute and ownership profile for property '" +
             property_name + "' in " + implementation_label +
             " drifted from the interface declaration",
         false);
  }

  return emitted;
}

static void ValidatePropertyAccessorSelectorUniqueness(
    Objc3PropertyInfo &property,
    const std::string &property_name,
    const std::string &container_name,
    const std::string &container_label,
    std::unordered_map<std::string, Objc3PropertyInfo> &properties,
    std::unordered_map<std::string, PropertyAccessorSelectorOwnerState>
        &getter_owner_states,
    std::unordered_map<std::string, PropertyAccessorSelectorOwnerState>
        &setter_owner_states,
    std::vector<std::string> &diagnostics) {
  const auto validate_selector =
      [&](const std::string &selector,
          const char *accessor_kind,
          std::unordered_map<std::string, PropertyAccessorSelectorOwnerState>
              &owner_states) {
        if (selector.empty()) {
          return;
        }

        auto owner_it = owner_states.find(selector);
        if (owner_it == owner_states.end()) {
          owner_states.emplace(selector,
                               PropertyAccessorSelectorOwnerState{
                                   property_name,
                                   false,
                               });
          return;
        }
        if (owner_it->second.property_name == property_name) {
          return;
        }

        const std::string message =
            "type mismatch: duplicate effective " +
            std::string(accessor_kind) + " selector '" + selector +
            "' for properties '" + owner_it->second.property_name + "' and '" +
            property_name + "' in " + container_label + " '" + container_name +
            "'";

        auto previous_property_it = properties.find(owner_it->second.property_name);
        if (previous_property_it != properties.end() &&
            !owner_it->second.owner_marked_conflict) {
          EmitPropertyContractViolation(previous_property_it->second,
                                        previous_property_it->second.line,
                                        previous_property_it->second.column,
                                        message,
                                        diagnostics,
                                        true);
          owner_it->second.owner_marked_conflict = true;
        }

        EmitPropertyContractViolation(property,
                                      property.line,
                                      property.column,
                                      message,
                                      diagnostics,
                                      true);
      };

  if (IsValidPropertyGetterSelector(property.effective_getter_selector)) {
    validate_selector(property.effective_getter_selector,
                      "getter",
                      getter_owner_states);
  }
  if (property.effective_setter_available &&
      IsValidPropertySetterSelector(property.effective_setter_selector)) {
    validate_selector(property.effective_setter_selector,
                      "setter",
                      setter_owner_states);
  }
}

static bool IsCompatibleCanonicalSemanticType(
    const Objc3SemanticCanonicalType &lhs,
    const Objc3SemanticCanonicalType &rhs) {
  return lhs.value_type == rhs.value_type &&
         lhs.kind == rhs.kind &&
         lhs.nullability == rhs.nullability &&
         lhs.ownership == rhs.ownership &&
         lhs.is_vector == rhs.is_vector &&
         lhs.vector_base_spelling == rhs.vector_base_spelling &&
         lhs.vector_lane_count == rhs.vector_lane_count &&
         lhs.has_pointer_declarator == rhs.has_pointer_declarator &&
         lhs.pointer_declarator_depth == rhs.pointer_declarator_depth &&
         lhs.object_pointer_type_name == rhs.object_pointer_type_name &&
         lhs.has_protocol_composition == rhs.has_protocol_composition &&
         lhs.protocol_composition_lexicographic ==
             rhs.protocol_composition_lexicographic &&
         lhs.has_generic_suffix == rhs.has_generic_suffix &&
         lhs.generic_arguments_source_order ==
             rhs.generic_arguments_source_order &&
         lhs.generic_arguments_lexicographic ==
             rhs.generic_arguments_lexicographic &&
         lhs.has_invalid_type_suffix == rhs.has_invalid_type_suffix &&
         lhs.deterministic == rhs.deterministic &&
         lhs.canonical_spelling == rhs.canonical_spelling &&
         lhs.replay_key == rhs.replay_key;
}

static bool IsCompatiblePropertySignature(const Objc3PropertyInfo &lhs, const Objc3PropertyInfo &rhs) {
  // property-ivar source-model completion anchor:
  // property attribute/accessor ownership/layout fields belong to declaration compatibility
  // only when they describe the shared declaration surface rather than storage-local layout symbols.
  // accessor/layout lowering freeze anchor: sema remains the
  // authority for the declaration-level property attribute/accessor/layout
  // compatibility packet and the canonical synthesized binding/layout
  // identities that lane-C publishes. IR/object emission must not recompute
  // those identities or invent executable accessor bodies.
  // ivar offset/layout emission anchor: sema owns the canonical slot
  // index, size, and alignment identities that lane-C converts into emitted
  // offset globals and per-owner layout tables. Lowering may derive byte
  // offsets from those identities, but it must not reclassify compatibility or
  // invent new layout shapes.
  return lhs.type == rhs.type &&
         IsCompatibleCanonicalSemanticType(lhs.canonical_type,
                                           rhs.canonical_type) &&
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

// Legacy extraction anchor retained for contract tests:
// BuildMethodInfo(const Objc3MethodDecl &method)
static Objc3MethodInfo BuildMethodInfo(const Objc3MethodDecl &method,
                                       const MethodSelectorNormalizationContractInfo &selector_contract,
                                       bool arc_mode_enabled,
                                       const std::unordered_set<std::string> *generic_interface_names = nullptr) {
  Objc3MethodInfo info;
  info.selector_normalized = selector_contract.normalized_selector;
  info.selector_piece_count = selector_contract.selector_piece_count;
  info.selector_parameter_piece_count = selector_contract.selector_parameter_piece_count;
  info.selector_contract_normalized = selector_contract.selector_contract_normalized;
  info.selector_had_pieceless_form = selector_contract.selector_had_pieceless_form;
  info.selector_has_spelling_mismatch = selector_contract.selector_has_spelling_mismatch;
  info.selector_has_arity_mismatch = selector_contract.selector_has_arity_mismatch;
  info.selector_has_parameter_linkage_mismatch = selector_contract.selector_has_parameter_linkage_mismatch;
  info.selector_has_normalization_flag_mismatch = selector_contract.selector_has_normalization_flag_mismatch;
  info.selector_has_missing_piece_keyword = selector_contract.selector_has_missing_piece_keyword;
  info.arity = method.params.size();
  info.param_types.reserve(method.params.size());
  info.param_canonical_types.reserve(method.params.size());
  info.param_is_vector.reserve(method.params.size());
  info.param_vector_base_spelling.reserve(method.params.size());
  info.param_vector_lane_count.reserve(method.params.size());
  info.param_has_generic_suffix.reserve(method.params.size());
  info.param_has_pointer_declarator.reserve(method.params.size());
  info.param_has_nullability_suffix.reserve(method.params.size());
  info.param_has_ownership_qualifier.reserve(method.params.size());
  info.param_id_spelling.reserve(method.params.size());
  info.param_class_spelling.reserve(method.params.size());
  info.param_instancetype_spelling.reserve(method.params.size());
  info.param_object_pointer_type_spelling.reserve(method.params.size());
  info.param_has_invalid_generic_suffix.reserve(method.params.size());
  info.param_has_invalid_pointer_declarator.reserve(method.params.size());
  info.param_has_invalid_nullability_suffix.reserve(method.params.size());
  info.param_has_invalid_ownership_qualifier.reserve(method.params.size());
  info.param_has_invalid_type_suffix.reserve(method.params.size());
  info.param_ownership_insert_retain.reserve(method.params.size());
  info.param_ownership_insert_release.reserve(method.params.size());
  info.param_ownership_insert_autorelease.reserve(method.params.size());
  info.param_ownership_is_weak_reference.reserve(method.params.size());
  info.param_ownership_is_unowned_reference.reserve(method.params.size());
  info.param_ownership_is_unowned_safe_reference.reserve(method.params.size());
  info.param_ownership_arc_diagnostic_candidate.reserve(method.params.size());
  info.param_ownership_arc_fixit_available.reserve(method.params.size());
  info.param_ownership_arc_diagnostic_profile.reserve(method.params.size());
  info.param_ownership_arc_fixit_hint.reserve(method.params.size());
  info.param_has_protocol_composition.reserve(method.params.size());
  info.param_protocol_composition_lexicographic.reserve(method.params.size());
  info.param_has_invalid_protocol_composition.reserve(method.params.size());
  for (const auto &param : method.params) {
    const ProtocolCompositionInfo protocol_composition =
        BuildProtocolCompositionInfoFromParam(param, generic_interface_names);
    info.param_types.push_back(param.type);
    info.param_canonical_types.push_back(BuildCanonicalSemanticTypeFromParam(param));
    info.param_is_vector.push_back(param.vector_spelling);
    info.param_vector_base_spelling.push_back(param.vector_base_spelling);
    info.param_vector_lane_count.push_back(param.vector_lane_count);
    info.param_has_generic_suffix.push_back(param.has_generic_suffix);
    info.param_has_pointer_declarator.push_back(param.has_pointer_declarator);
    info.param_has_nullability_suffix.push_back(!param.nullability_suffix_tokens.empty());
    info.param_has_ownership_qualifier.push_back(param.has_ownership_qualifier);
    info.param_id_spelling.push_back(param.id_spelling);
    info.param_class_spelling.push_back(param.class_spelling);
    info.param_instancetype_spelling.push_back(param.instancetype_spelling);
    info.param_object_pointer_type_spelling.push_back(param.object_pointer_type_spelling);
    info.param_has_invalid_generic_suffix.push_back(HasInvalidGenericParamTypeSuffix(param));
    info.param_has_invalid_pointer_declarator.push_back(HasInvalidPointerParamTypeDeclarator(param));
    info.param_has_invalid_nullability_suffix.push_back(HasInvalidNullabilityParamTypeSuffix(param));
    info.param_has_invalid_ownership_qualifier.push_back(HasInvalidOwnershipQualifierParamTypeSuffix(param));
    info.param_has_invalid_type_suffix.push_back(HasInvalidParamTypeSuffix(param));
    info.param_ownership_insert_retain.push_back(param.ownership_insert_retain);
    info.param_ownership_insert_release.push_back(param.ownership_insert_release);
    info.param_ownership_insert_autorelease.push_back(param.ownership_insert_autorelease);
    info.param_ownership_is_weak_reference.push_back(param.ownership_is_weak_reference);
    info.param_ownership_is_unowned_reference.push_back(param.ownership_is_unowned_reference);
    info.param_ownership_is_unowned_safe_reference.push_back(param.ownership_is_unowned_safe_reference);
    info.param_ownership_arc_diagnostic_candidate.push_back(param.ownership_arc_diagnostic_candidate);
    info.param_ownership_arc_fixit_available.push_back(param.ownership_arc_fixit_available);
    info.param_ownership_arc_diagnostic_profile.push_back(param.ownership_arc_diagnostic_profile);
    info.param_ownership_arc_fixit_hint.push_back(param.ownership_arc_fixit_hint);
    info.param_has_protocol_composition.push_back(protocol_composition.has_protocol_composition);
    info.param_protocol_composition_lexicographic.push_back(protocol_composition.names_lexicographic);
    info.param_has_invalid_protocol_composition.push_back(protocol_composition.has_invalid_protocol_composition);
    const std::size_t param_index = info.param_has_ownership_qualifier.size() - 1u;
    const bool arc_inferred_object_param =
        arc_mode_enabled && !info.param_has_ownership_qualifier[param_index] &&
        (info.param_id_spelling[param_index] ||
         info.param_instancetype_spelling[param_index] ||
         info.param_object_pointer_type_spelling[param_index]);
    if (arc_inferred_object_param) {
      // ARC inference/lifetime implementation anchor: under
      // explicit ARC mode, unqualified object parameters now synthesize a
      // canonical strong-owned retain/release profile for the supported
      // runnable slice.
      info.param_has_ownership_qualifier[param_index] = true;
      info.param_ownership_insert_retain[param_index] = true;
      info.param_ownership_insert_release[param_index] = true;
      info.param_ownership_insert_autorelease[param_index] = false;
      info.param_ownership_is_weak_reference[param_index] = false;
      info.param_ownership_is_unowned_reference[param_index] = false;
      info.param_ownership_is_unowned_safe_reference[param_index] = false;
      info.param_ownership_arc_diagnostic_candidate[param_index] = false;
      info.param_ownership_arc_fixit_available[param_index] = false;
      info.param_ownership_arc_diagnostic_profile[param_index].clear();
      info.param_ownership_arc_fixit_hint[param_index].clear();
    }
  }
  const ProtocolCompositionInfo return_protocol_composition =
      BuildProtocolCompositionInfoFromMethodReturn(method, generic_interface_names);
  info.return_has_generic_suffix = method.has_return_generic_suffix;
  info.return_has_pointer_declarator = method.has_return_pointer_declarator;
  info.return_has_nullability_suffix = !method.return_nullability_suffix_tokens.empty();
  info.return_has_ownership_qualifier = method.has_return_ownership_qualifier;
  info.return_id_spelling = method.return_id_spelling;
  info.return_class_spelling = method.return_class_spelling;
  info.return_instancetype_spelling = method.return_instancetype_spelling;
  info.return_object_pointer_type_spelling = method.return_object_pointer_type_spelling;
  info.return_has_invalid_generic_suffix = HasInvalidGenericReturnTypeSuffix(method);
  info.return_has_invalid_pointer_declarator = HasInvalidPointerReturnTypeDeclarator(method);
  info.return_has_invalid_nullability_suffix = HasInvalidNullabilityReturnTypeSuffix(method);
  info.return_has_invalid_ownership_qualifier = HasInvalidOwnershipQualifierReturnTypeSuffix(method);
  info.return_has_invalid_type_suffix = info.return_has_invalid_generic_suffix ||
                                        info.return_has_invalid_pointer_declarator ||
                                        info.return_has_invalid_nullability_suffix ||
                                        info.return_has_invalid_ownership_qualifier;
  info.return_ownership_insert_retain = method.return_ownership_insert_retain;
  info.return_ownership_insert_release = method.return_ownership_insert_release;
  info.return_ownership_insert_autorelease = method.return_ownership_insert_autorelease;
  info.return_ownership_is_weak_reference = method.return_ownership_is_weak_reference;
  info.return_ownership_is_unowned_reference = method.return_ownership_is_unowned_reference;
  info.return_ownership_is_unowned_safe_reference = method.return_ownership_is_unowned_safe_reference;
  info.return_ownership_arc_diagnostic_candidate = method.return_ownership_arc_diagnostic_candidate;
  info.return_ownership_arc_fixit_available = method.return_ownership_arc_fixit_available;
  info.return_ownership_arc_diagnostic_profile = method.return_ownership_arc_diagnostic_profile;
  info.return_ownership_arc_fixit_hint = method.return_ownership_arc_fixit_hint;
  info.return_type = method.return_type;
  info.return_canonical_type = BuildCanonicalSemanticTypeFromMethodReturn(method);
  info.return_is_vector = method.return_vector_spelling;
  info.return_vector_base_spelling = method.return_vector_base_spelling;
  info.return_vector_lane_count = method.return_vector_lane_count;
  info.return_has_protocol_composition = return_protocol_composition.has_protocol_composition;
  info.return_protocol_composition_lexicographic = return_protocol_composition.names_lexicographic;
  info.return_has_invalid_protocol_composition = return_protocol_composition.has_invalid_protocol_composition;
  const bool arc_inferred_object_return =
      arc_mode_enabled && !info.return_has_ownership_qualifier &&
      (info.return_id_spelling || info.return_instancetype_spelling ||
       info.return_object_pointer_type_spelling);
  if (arc_inferred_object_return) {
    // ARC inference/lifetime implementation anchor: under explicit
    // ARC mode, unqualified object returns now synthesize a canonical
    // strong-owned retain/release profile for the supported runnable slice.
    info.return_has_ownership_qualifier = true;
    info.return_ownership_insert_retain = true;
    info.return_ownership_insert_release = true;
    info.return_ownership_insert_autorelease = false;
    info.return_ownership_is_weak_reference = false;
    info.return_ownership_is_unowned_reference = false;
    info.return_ownership_is_unowned_safe_reference = false;
    info.return_ownership_arc_diagnostic_candidate = false;
    info.return_ownership_arc_fixit_available = false;
    info.return_ownership_arc_diagnostic_profile.clear();
    info.return_ownership_arc_fixit_hint.clear();
  }
  info.async_continuation_profile_is_normalized =
      method.async_continuation_profile_is_normalized;
  info.deterministic_async_continuation_handoff =
      method.deterministic_async_continuation_handoff;
  info.async_continuation_sites = method.async_continuation_sites;
  info.async_keyword_sites = method.async_keyword_sites;
  info.async_function_sites = method.async_function_sites;
  info.continuation_allocation_sites = method.continuation_allocation_sites;
  info.continuation_resume_sites = method.continuation_resume_sites;
  info.continuation_suspend_sites = method.continuation_suspend_sites;
  info.async_state_machine_sites = method.async_state_machine_sites;
  info.async_continuation_normalized_sites =
      method.async_continuation_normalized_sites;
  info.async_continuation_gate_blocked_sites =
      method.async_continuation_gate_blocked_sites;
  info.async_continuation_contract_violation_sites =
      method.async_continuation_contract_violation_sites;
  info.actor_isolation_sendability_profile_is_normalized =
      method.actor_isolation_sendability_profile_is_normalized;
  info.deterministic_actor_isolation_sendability_handoff =
      method.deterministic_actor_isolation_sendability_handoff;
  info.actor_isolation_sendability_sites =
      method.actor_isolation_sendability_sites;
  info.actor_isolation_decl_sites = method.actor_isolation_decl_sites;
  info.actor_hop_sites = method.actor_hop_sites;
  info.sendable_annotation_sites = method.sendable_annotation_sites;
  info.non_sendable_crossing_sites = method.non_sendable_crossing_sites;
  info.isolation_boundary_sites = method.isolation_boundary_sites;
  info.actor_isolation_sendability_normalized_sites =
      method.actor_isolation_sendability_normalized_sites;
  info.actor_isolation_sendability_gate_blocked_sites =
      method.actor_isolation_sendability_gate_blocked_sites;
  info.actor_isolation_sendability_contract_violation_sites =
      method.actor_isolation_sendability_contract_violation_sites;
  info.task_runtime_cancellation_profile_is_normalized =
      method.task_runtime_cancellation_profile_is_normalized;
  info.deterministic_task_runtime_cancellation_handoff =
      method.deterministic_task_runtime_cancellation_handoff;
  info.task_runtime_interop_sites = method.task_runtime_interop_sites;
  info.runtime_hook_sites = method.runtime_hook_sites;
  info.cancellation_check_sites = method.cancellation_check_sites;
  info.cancellation_handler_sites = method.cancellation_handler_sites;
  info.suspension_point_sites = method.suspension_point_sites;
  info.cancellation_propagation_sites = method.cancellation_propagation_sites;
  info.task_runtime_cancellation_normalized_sites =
      method.task_runtime_cancellation_normalized_sites;
  info.task_runtime_cancellation_gate_blocked_sites =
      method.task_runtime_cancellation_gate_blocked_sites;
  info.task_runtime_cancellation_contract_violation_sites =
      method.task_runtime_cancellation_contract_violation_sites;
  info.concurrency_replay_race_guard_profile_is_normalized =
      method.concurrency_replay_race_guard_profile_is_normalized;
  info.deterministic_concurrency_replay_race_guard_handoff =
      method.deterministic_concurrency_replay_race_guard_handoff;
  info.concurrency_replay_race_guard_sites =
      method.concurrency_replay_race_guard_sites;
  info.concurrency_replay_sites = method.concurrency_replay_sites;
  info.replay_proof_sites = method.replay_proof_sites;
  info.race_guard_sites = method.race_guard_sites;
  info.task_handoff_sites = method.task_handoff_sites;
  info.actor_isolation_sites = method.actor_isolation_sites;
  info.deterministic_schedule_sites = method.deterministic_schedule_sites;
  info.concurrency_replay_guard_blocked_sites =
      method.concurrency_replay_guard_blocked_sites;
  info.concurrency_replay_contract_violation_sites =
      method.concurrency_replay_contract_violation_sites;
  info.ns_error_bridging_profile_is_normalized =
      method.ns_error_bridging_profile_is_normalized;
  info.deterministic_ns_error_bridging_lowering_handoff =
      method.deterministic_ns_error_bridging_lowering_handoff;
  info.ns_error_bridging_sites = method.ns_error_bridging_sites;
  info.ns_error_parameter_sites = method.ns_error_parameter_sites;
  info.ns_error_out_parameter_sites = method.ns_error_out_parameter_sites;
  info.ns_error_bridge_path_sites = method.ns_error_bridge_path_sites;
  info.failable_call_sites = method.failable_call_sites;
  info.ns_error_bridging_normalized_sites =
      method.ns_error_bridging_normalized_sites;
  info.ns_error_bridge_boundary_sites = method.ns_error_bridge_boundary_sites;
  info.ns_error_bridging_contract_violation_sites =
      method.ns_error_bridging_contract_violation_sites;
  info.objc_direct_declared = method.objc_direct_declared;
  info.objc_final_declared = method.objc_final_declared;
  info.objc_dynamic_declared = method.objc_dynamic_declared;
  info.is_class_method = method.is_class_method;
  info.has_definition = method.has_body;
  return info;
}

struct Objc3ProtocolRequirementInfo {
  std::string selector;
  Objc3MethodInfo method;
  std::string protocol_name;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3ProtocolPropertyRequirementInfo {
  std::string property_name;
  Objc3PropertyInfo property;
  std::string protocol_name;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3ProtocolSemanticDefinition {
  std::string name;
  bool is_forward_declaration = false;
  std::vector<std::string> inherited_protocols_lexicographic;
  std::unordered_map<std::string, Objc3ProtocolPropertyRequirementInfo>
      required_properties_by_name;
  std::unordered_map<std::string, Objc3ProtocolPropertyRequirementInfo>
      optional_properties_by_name;
  std::unordered_map<std::string, Objc3ProtocolRequirementInfo>
      required_methods_by_key;
  std::unordered_map<std::string, Objc3ProtocolRequirementInfo>
      optional_methods_by_key;
};

struct Objc3InterfaceGenericParameterDefinition {
  std::string name;
  std::string variance_spelling;
  std::vector<std::string> constraint_protocols_lexicographic;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3InterfaceGenericDefinition {
  std::string name;
  std::vector<Objc3InterfaceGenericParameterDefinition> parameters;
};

enum class Objc3ProtocolRequirementClosureStatus {
  Ready,
  MissingProtocol,
  ForwardDeclaration,
  InheritanceCycle,
  IncompatibleInheritedRequirement,
};

struct Objc3ProtocolRequirementClosure {
  Objc3ProtocolRequirementClosureStatus status =
      Objc3ProtocolRequirementClosureStatus::Ready;
  std::string failing_protocol_name;
  std::string conflicting_member_name;
  bool conflicting_member_is_method = false;
  bool conflicting_selector_is_class_method = false;
  std::unordered_map<std::string, const Objc3ProtocolPropertyRequirementInfo *>
      required_properties_by_name;
  std::unordered_map<std::string, const Objc3ProtocolPropertyRequirementInfo *>
      optional_properties_by_name;
  std::unordered_map<std::string, const Objc3ProtocolRequirementInfo *>
      required_methods_by_key;
  std::unordered_map<std::string, const Objc3ProtocolRequirementInfo *>
      optional_methods_by_key;
};

struct Objc3ProtocolQualifiedMessageRequirementResolution {
  enum class Status {
    NotProtocolQualified,
    MissingRequirementClosure,
    MissingSelector,
    AmbiguousSelector,
    Resolved,
  };

  Status status = Status::NotProtocolQualified;
  const Objc3ProtocolRequirementInfo *requirement = nullptr;
  const Objc3ProtocolPropertyRequirementInfo *property_requirement = nullptr;
  bool property_requirement_is_setter = false;
  std::string protocol_name;
  std::string failing_protocol_name;
};

static bool CollectProtocolRequirementClosure(
    const std::unordered_map<std::string, Objc3ProtocolSemanticDefinition>
        &definitions,
    const std::string &protocol_name,
    std::unordered_set<std::string> &active_protocols,
    Objc3ProtocolRequirementClosure &closure);

static Objc3ProtocolQualifiedMessageRequirementResolution
ResolveProtocolQualifiedMessageRequirement(
    const Objc3SemanticCanonicalType &receiver_type,
    const std::string &selector,
    bool is_class_method,
    const Objc3MessageSendResolutionContext &context);

static std::string BuildProtocolRequirementKey(const std::string &selector,
                                               bool is_class_method) {
  return std::string(is_class_method ? "+" : "-") + selector;
}

static std::string FormatMethodSelectorForDiagnostic(const std::string &selector,
                                                     bool is_class_method) {
  return std::string(is_class_method ? "+" : "-") + selector;
}

static std::string BuildCategoryOwnerIdentity(const std::string &class_name,
                                              const std::string &category_name) {
  return "category:" + class_name + "(" + category_name + ")";
}

struct Objc3CategoryMergeOrderEntry {
  std::string class_name;
  std::string category_name;
  std::string owner_identity;
  std::string container_name;
  unsigned line = 1;
  unsigned column = 1;
};

static std::string FormatObjcContainerName(const std::string &name,
                                           bool has_category,
                                           const std::string &category_name) {
  return has_category ? name + "(" + category_name + ")" : name;
}

static std::unordered_map<std::string, Objc3ProtocolSemanticDefinition>
BuildProtocolSemanticDefinitions(const Objc3Program &ast,
                                 bool arc_mode_enabled,
                                 std::vector<std::string> &diagnostics) {
  std::unordered_map<std::string, Objc3ProtocolSemanticDefinition> definitions;
  for (const auto &protocol_decl : ast.protocols) {
    constexpr const char *kContainerLabel = "protocol";
    const std::string container_name = protocol_decl.name;
    auto definition_it = definitions.find(protocol_decl.name);
    if (definition_it == definitions.end()) {
      Objc3ProtocolSemanticDefinition definition;
      definition.name = protocol_decl.name;
      definition.is_forward_declaration = protocol_decl.is_forward_declaration;
      definition.inherited_protocols_lexicographic =
          BuildSortedUniqueStringsLocal(protocol_decl.inherited_protocols);
      definition_it =
          definitions.emplace(protocol_decl.name, std::move(definition)).first;
    } else if (!definition_it->second.is_forward_declaration &&
               !protocol_decl.is_forward_declaration) {
      diagnostics.push_back(MakeDiag(protocol_decl.line, protocol_decl.column,
                                     "O3S200",
                                     "duplicate protocol '" + protocol_decl.name +
                                         "'"));
      continue;
    } else if (definition_it->second.is_forward_declaration &&
               !protocol_decl.is_forward_declaration) {
      definition_it->second.is_forward_declaration = false;
      definition_it->second.inherited_protocols_lexicographic =
          BuildSortedUniqueStringsLocal(protocol_decl.inherited_protocols);
      definition_it->second.required_methods_by_key.clear();
      definition_it->second.optional_methods_by_key.clear();
      definition_it->second.required_properties_by_name.clear();
      definition_it->second.optional_properties_by_name.clear();
    } else {
      continue;
    }

    if (protocol_decl.is_forward_declaration) {
      continue;
    }

    std::unordered_map<std::string, Objc3PropertyInfo> protocol_properties;
    std::unordered_map<std::string, PropertyAccessorSelectorOwnerState>
        getter_owner_states;
    std::unordered_map<std::string, PropertyAccessorSelectorOwnerState>
        setter_owner_states;
    for (const auto &property_decl : protocol_decl.properties) {
      ValidatePropertyTypeSuffixes(property_decl, container_name, kContainerLabel,
                                   diagnostics);
      Objc3PropertyInfo property_info =
          BuildPropertyInfo(property_decl, container_name, kContainerLabel,
                            arc_mode_enabled,
                            diagnostics);
      const auto property_insert =
          protocol_properties.emplace(property_decl.name, std::move(property_info));
      if (!property_insert.second) {
        diagnostics.push_back(
            MakeDiag(property_decl.line, property_decl.column, "O3S200",
                     "duplicate protocol property '" + property_decl.name +
                         "' in protocol '" + protocol_decl.name + "'"));
        continue;
      }
      ValidatePropertyAccessorSelectorUniqueness(
          property_insert.first->second,
          property_decl.name,
          container_name,
          kContainerLabel,
          protocol_properties,
          getter_owner_states,
          setter_owner_states,
          diagnostics);
      Objc3ProtocolPropertyRequirementInfo requirement;
      requirement.property_name = property_decl.name;
      requirement.property = property_insert.first->second;
      requirement.protocol_name = protocol_decl.name;
      requirement.line = property_decl.line;
      requirement.column = property_decl.column;
      if (property_decl.protocol_requirement_kind ==
          Objc3ProtocolRequirementKind::Optional) {
        definition_it->second.optional_properties_by_name.emplace(
            requirement.property_name, std::move(requirement));
        continue;
      }
      definition_it->second.required_properties_by_name.emplace(
          requirement.property_name, std::move(requirement));
    }

    for (const auto &method_decl : protocol_decl.methods) {
      const MethodSelectorNormalizationContractInfo selector_contract =
          BuildMethodSelectorNormalizationContractInfo(method_decl);
      ValidateMethodSelectorNormalizationContract(method_decl, container_name,
                                                 kContainerLabel,
                                                 selector_contract, diagnostics);
      ValidateMethodReturnTypeSuffixes(method_decl, container_name,
                                       kContainerLabel, diagnostics);
      ValidateMethodParameterTypeSuffixes(method_decl, container_name,
                                          kContainerLabel, diagnostics);
      if (method_decl.objc_macro_declared ||
          method_decl.objc_macro_package_declared ||
          method_decl.objc_macro_provenance_declared) {
        diagnostics.push_back(MakeDiag(
            method_decl.line, method_decl.column, "O3S325",
            "macro expansion is currently limited to free functions; " +
                std::string(kContainerLabel) + " selector in '" +
                container_name + "' is not sandbox-admitted"));
      }
      if (method_decl.has_body) {
        diagnostics.push_back(MakeDiag(
            method_decl.line, method_decl.column, "O3S206",
            "type mismatch: protocol selector '" +
                selector_contract.normalized_selector + "' in '" +
                protocol_decl.name + "' must not define a body"));
      }

      const std::string requirement_key = BuildProtocolRequirementKey(
          selector_contract.normalized_selector, method_decl.is_class_method);
      if (definition_it->second.required_methods_by_key.find(requirement_key) !=
              definition_it->second.required_methods_by_key.end() ||
          definition_it->second.optional_methods_by_key.find(requirement_key) !=
              definition_it->second.optional_methods_by_key.end()) {
        diagnostics.push_back(MakeDiag(
            method_decl.line, method_decl.column, "O3S200",
            "duplicate protocol selector '" +
                FormatMethodSelectorForDiagnostic(
                    selector_contract.normalized_selector,
                    method_decl.is_class_method) +
                "' in protocol '" + protocol_decl.name + "'"));
        continue;
      }
      Objc3ProtocolRequirementInfo requirement;
      requirement.selector = selector_contract.normalized_selector;
      requirement.method =
          BuildMethodInfo(method_decl, selector_contract, arc_mode_enabled);
      requirement.protocol_name = protocol_decl.name;
      requirement.line = method_decl.line;
      requirement.column = method_decl.column;
      if (method_decl.protocol_requirement_kind ==
          Objc3ProtocolRequirementKind::Optional) {
        definition_it->second.optional_methods_by_key.emplace(requirement_key,
                                                              std::move(requirement));
        continue;
      }
      definition_it->second.required_methods_by_key.emplace(requirement_key,
                                                            std::move(requirement));
    }
  }
  return definitions;
}

static bool ProtocolSatisfiesConstraint(
    const std::unordered_map<std::string, Objc3ProtocolSemanticDefinition>
        &protocol_definitions,
    const std::string &candidate_protocol,
    const std::string &required_protocol,
    std::unordered_set<std::string> &active_protocols) {
  if (candidate_protocol == required_protocol) {
    return true;
  }
  if (!active_protocols.insert(candidate_protocol).second) {
    return false;
  }
  const auto definition_it = protocol_definitions.find(candidate_protocol);
  if (definition_it == protocol_definitions.end()) {
    active_protocols.erase(candidate_protocol);
    return false;
  }
  for (const auto &inherited :
       definition_it->second.inherited_protocols_lexicographic) {
    if (ProtocolSatisfiesConstraint(protocol_definitions, inherited,
                                    required_protocol, active_protocols)) {
      active_protocols.erase(candidate_protocol);
      return true;
    }
  }
  active_protocols.erase(candidate_protocol);
  return false;
}

static std::unordered_map<std::string, Objc3InterfaceGenericDefinition>
BuildInterfaceGenericDefinitions(
    const Objc3Program &ast,
    const std::unordered_map<std::string, Objc3ProtocolSemanticDefinition>
        &protocol_definitions,
    std::vector<std::string> &diagnostics) {
  std::unordered_map<std::string, Objc3InterfaceGenericDefinition> definitions;
  for (const auto &interface_decl : ast.interfaces) {
    if (interface_decl.has_category || interface_decl.generic_params.empty()) {
      continue;
    }
    Objc3InterfaceGenericDefinition definition;
    definition.name = interface_decl.name;
    std::unordered_set<std::string> seen_names;
    for (const auto &param_decl : interface_decl.generic_params) {
      if (!seen_names.insert(param_decl.name).second) {
        diagnostics.push_back(MakeDiag(
            param_decl.line, param_decl.column, "O3S200",
            "duplicate generic parameter '" + param_decl.name +
                "' in interface '" + interface_decl.name + "'"));
        continue;
      }
      Objc3InterfaceGenericParameterDefinition param;
      param.name = param_decl.name;
      param.variance_spelling = param_decl.variance_spelling;
      param.line = param_decl.line;
      param.column = param_decl.column;
      if (param_decl.has_constraint) {
        if (param_decl.constraint_type_name != "id" ||
            !param_decl.has_constraint_generic_suffix) {
          diagnostics.push_back(MakeDiag(
              param_decl.constraint_line, param_decl.constraint_column,
              "O3S206",
              "type mismatch: generic parameter '" + param_decl.name +
                  "' in interface '" + interface_decl.name +
                  "' requires an id<Protocol> constraint"));
        } else {
          const ProtocolCompositionParseResult parsed =
              ParseProtocolCompositionSuffixText(
                  param_decl.constraint_generic_suffix_text);
          if (!parsed.IsValid()) {
            diagnostics.push_back(MakeDiag(
                param_decl.constraint_line, param_decl.constraint_column,
                "O3S206",
                "type mismatch: generic parameter '" + param_decl.name +
                    "' in interface '" + interface_decl.name +
                    "' has an invalid protocol constraint"));
          } else {
            param.constraint_protocols_lexicographic =
                parsed.names_lexicographic;
            for (const auto &protocol_name :
                 param.constraint_protocols_lexicographic) {
              if (protocol_definitions.find(protocol_name) ==
                  protocol_definitions.end()) {
                diagnostics.push_back(MakeDiag(
                    param_decl.constraint_line, param_decl.constraint_column,
                    "O3S206",
                    "type mismatch: generic parameter '" + param_decl.name +
                        "' in interface '" + interface_decl.name +
                        "' references unknown constraint protocol '" +
                        protocol_name + "'"));
              }
            }
          }
        }
      }
      definition.parameters.push_back(std::move(param));
    }
    definitions[definition.name] = std::move(definition);
  }
  return definitions;
}

static std::string GenericArgumentObjectTypeName(const std::string &argument) {
  const std::string trimmed = TrimAsciiWhitespace(argument);
  if (trimmed.empty()) {
    return "";
  }
  if (trimmed.rfind("id<", 0) == 0u || trimmed == "id") {
    return "";
  }
  std::size_t end = 0;
  while (end < trimmed.size()) {
    const unsigned char c = static_cast<unsigned char>(trimmed[end]);
    if (!(std::isalnum(c) != 0 || c == '_' || c == ':')) {
      break;
    }
    ++end;
  }
  return trimmed.substr(0, end);
}

static bool ExtractGenericArgumentSpecialization(
    const std::string &argument, std::string &object_name,
    std::string &generic_suffix_text) {
  const std::string trimmed = TrimAsciiWhitespace(argument);
  object_name = GenericArgumentObjectTypeName(trimmed);
  generic_suffix_text.clear();
  if (object_name.empty()) {
