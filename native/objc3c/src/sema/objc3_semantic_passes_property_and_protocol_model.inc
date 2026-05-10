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

#include "sema/objc3_semantic_passes_property_accessor_contracts.inc"

#include "sema/objc3_semantic_passes_property_and_protocol_model_method_metadata.inc"

#include "sema/objc3_semantic_passes_protocol_requirement_model.inc"

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
