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

#include "sema/objc3_semantic_passes_property_and_protocol_model_property_diagnostics.inc"
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
