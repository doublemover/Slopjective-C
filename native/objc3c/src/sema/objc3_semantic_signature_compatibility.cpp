#include "sema/objc3_semantic_signature_compatibility.h"

#include "sema/objc3_typed_throws_effect_contract.h"

bool AreEquivalentProtocolCompositions(
    bool lhs_has_composition, const std::vector<std::string> &lhs_names,
    bool rhs_has_composition, const std::vector<std::string> &rhs_names) {
  if (lhs_has_composition != rhs_has_composition) {
    return false;
  }
  if (!lhs_has_composition) {
    return true;
  }
  return lhs_names == rhs_names;
}

bool IsCompatibleCanonicalSemanticType(
    const Objc3SemanticCanonicalType &lhs,
    const Objc3SemanticCanonicalType &rhs) {
  return lhs.value_type == rhs.value_type && lhs.kind == rhs.kind &&
         lhs.nullability == rhs.nullability && lhs.ownership == rhs.ownership &&
         lhs.is_vector == rhs.is_vector &&
         lhs.vector_base_spelling == rhs.vector_base_spelling &&
         lhs.vector_lane_count == rhs.vector_lane_count &&
         lhs.has_pointer_declarator == rhs.has_pointer_declarator &&
         lhs.pointer_declarator_depth == rhs.pointer_declarator_depth &&
         lhs.object_pointer_type_name == rhs.object_pointer_type_name &&
         lhs.object_type_facts_authoritative ==
             rhs.object_type_facts_authoritative &&
         lhs.is_objc_object_reference == rhs.is_objc_object_reference &&
         lhs.is_objc_id_object == rhs.is_objc_id_object &&
         lhs.is_objc_class_object == rhs.is_objc_class_object &&
         lhs.is_objc_metaclass_object == rhs.is_objc_metaclass_object &&
         lhs.is_objc_protocol_object == rhs.is_objc_protocol_object &&
         lhs.is_objc_instancetype_object == rhs.is_objc_instancetype_object &&
         lhs.is_objc_named_object_pointer ==
             rhs.is_objc_named_object_pointer &&
         lhs.has_protocol_composition == rhs.has_protocol_composition &&
         lhs.protocol_composition_lexicographic ==
             rhs.protocol_composition_lexicographic &&
         lhs.protocol_composition_facts_authoritative ==
             rhs.protocol_composition_facts_authoritative &&
         lhs.has_invalid_protocol_composition ==
             rhs.has_invalid_protocol_composition &&
         lhs.has_generic_suffix == rhs.has_generic_suffix &&
         lhs.generic_arguments_source_order == rhs.generic_arguments_source_order &&
         lhs.generic_arguments_lexicographic ==
             rhs.generic_arguments_lexicographic &&
         lhs.generic_arguments_facts_authoritative ==
             rhs.generic_arguments_facts_authoritative &&
         lhs.has_invalid_generic_suffix == rhs.has_invalid_generic_suffix &&
         lhs.has_explicit_nullability == rhs.has_explicit_nullability &&
         lhs.nullability_facts_authoritative ==
             rhs.nullability_facts_authoritative &&
         lhs.has_invalid_nullability_suffix ==
             rhs.has_invalid_nullability_suffix &&
         lhs.is_value_optional == rhs.is_value_optional &&
         lhs.value_optional_payload_type_spelling ==
             rhs.value_optional_payload_type_spelling &&
         lhs.value_optional_payload_value_type ==
             rhs.value_optional_payload_value_type &&
         lhs.value_optional_runtime_execution_supported ==
             rhs.value_optional_runtime_execution_supported &&
         lhs.value_optional_lowering_supported ==
             rhs.value_optional_lowering_supported &&
         lhs.has_invalid_type_suffix == rhs.has_invalid_type_suffix &&
         lhs.deterministic == rhs.deterministic &&
         lhs.canonical_spelling == rhs.canonical_spelling &&
         lhs.replay_key == rhs.replay_key;
}

bool IsCompatiblePropertySignature(const Objc3PropertyInfo &lhs,
                                   const Objc3PropertyInfo &rhs) {
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
         lhs.ownership_runtime_hook_profile ==
             rhs.ownership_runtime_hook_profile &&
         lhs.property_attribute_profile == rhs.property_attribute_profile &&
         lhs.property_behavior_declared == rhs.property_behavior_declared &&
         lhs.property_behavior_name == rhs.property_behavior_name &&
         lhs.effective_getter_selector == rhs.effective_getter_selector &&
         lhs.effective_setter_available == rhs.effective_setter_available &&
         lhs.effective_setter_selector == rhs.effective_setter_selector &&
         lhs.accessor_ownership_profile == rhs.accessor_ownership_profile;
}

bool IsCompatibleMethodSignature(const Objc3MethodInfo &lhs,
                                 const Objc3MethodInfo &rhs) {
  if (lhs.arity != rhs.arity || lhs.return_type != rhs.return_type ||
      lhs.return_is_vector != rhs.return_is_vector ||
      lhs.is_class_method != rhs.is_class_method ||
      !Objc3TypedThrowsCallableEffectsCompatible(
          lhs.throws_declared,
          lhs.typed_throws_declared,
          lhs.typed_throws_error_type_spelling,
          lhs.typed_throws_abi_lowering_ready,
          rhs.throws_declared,
          rhs.typed_throws_declared,
          rhs.typed_throws_error_type_spelling,
          rhs.typed_throws_abi_lowering_ready) ||
      lhs.typed_throws_effect_signature_key !=
          rhs.typed_throws_effect_signature_key ||
      lhs.typed_throws_callable_compatibility_policy !=
          rhs.typed_throws_callable_compatibility_policy ||
      lhs.generic_parameter_names_source_order !=
          rhs.generic_parameter_names_source_order ||
      lhs.generic_parameter_variance_source_order !=
          rhs.generic_parameter_variance_source_order ||
      lhs.generic_parameter_constraints_lexicographic !=
          rhs.generic_parameter_constraints_lexicographic ||
      lhs.generic_callable_signature_replay_key !=
          rhs.generic_callable_signature_replay_key ||
      lhs.generic_callable_reification_policy !=
          rhs.generic_callable_reification_policy ||
      lhs.generic_callable_mangling_policy_id !=
          rhs.generic_callable_mangling_policy_id ||
      lhs.generic_callable_contract_deterministic !=
          rhs.generic_callable_contract_deterministic ||
      !lhs.generic_callable_contract_deterministic ||
      !IsCompatibleCanonicalSemanticType(lhs.return_canonical_type,
                                         rhs.return_canonical_type) ||
      lhs.return_has_ownership_qualifier !=
          rhs.return_has_ownership_qualifier ||
      lhs.return_ownership_insert_retain != rhs.return_ownership_insert_retain ||
      lhs.return_ownership_insert_release !=
          rhs.return_ownership_insert_release ||
      lhs.return_ownership_insert_autorelease !=
          rhs.return_ownership_insert_autorelease ||
      lhs.return_ownership_is_weak_reference !=
          rhs.return_ownership_is_weak_reference ||
      lhs.return_ownership_is_unowned_reference !=
          rhs.return_ownership_is_unowned_reference ||
      lhs.return_ownership_is_unowned_safe_reference !=
          rhs.return_ownership_is_unowned_safe_reference ||
      lhs.return_ownership_arc_diagnostic_candidate !=
          rhs.return_ownership_arc_diagnostic_candidate ||
      lhs.return_ownership_arc_fixit_available !=
          rhs.return_ownership_arc_fixit_available ||
      lhs.return_ownership_arc_diagnostic_profile !=
          rhs.return_ownership_arc_diagnostic_profile ||
      lhs.return_ownership_arc_fixit_hint !=
          rhs.return_ownership_arc_fixit_hint) {
    return false;
  }
  if (lhs.return_is_vector &&
      (lhs.return_vector_base_spelling != rhs.return_vector_base_spelling ||
       lhs.return_vector_lane_count != rhs.return_vector_lane_count)) {
    return false;
  }
  if (lhs.param_canonical_types.size() != lhs.arity ||
      rhs.param_canonical_types.size() != rhs.arity ||
      lhs.param_has_ownership_qualifier.size() != lhs.arity ||
      rhs.param_has_ownership_qualifier.size() != rhs.arity) {
    return false;
  }
  if (lhs.param_ownership_insert_retain.size() != lhs.arity ||
      rhs.param_ownership_insert_retain.size() != rhs.arity ||
      lhs.param_ownership_insert_release.size() != lhs.arity ||
      rhs.param_ownership_insert_release.size() != rhs.arity ||
      lhs.param_ownership_insert_autorelease.size() != lhs.arity ||
      rhs.param_ownership_insert_autorelease.size() != rhs.arity ||
      lhs.param_ownership_is_weak_reference.size() != lhs.arity ||
      rhs.param_ownership_is_weak_reference.size() != rhs.arity ||
      lhs.param_ownership_is_unowned_reference.size() != lhs.arity ||
      rhs.param_ownership_is_unowned_reference.size() != rhs.arity ||
      lhs.param_ownership_is_unowned_safe_reference.size() != lhs.arity ||
      rhs.param_ownership_is_unowned_safe_reference.size() != rhs.arity ||
      lhs.param_ownership_arc_diagnostic_candidate.size() != lhs.arity ||
      rhs.param_ownership_arc_diagnostic_candidate.size() != rhs.arity ||
      lhs.param_ownership_arc_fixit_available.size() != lhs.arity ||
      rhs.param_ownership_arc_fixit_available.size() != rhs.arity ||
      lhs.param_ownership_arc_diagnostic_profile.size() != lhs.arity ||
      rhs.param_ownership_arc_diagnostic_profile.size() != rhs.arity ||
      lhs.param_ownership_arc_fixit_hint.size() != lhs.arity ||
      rhs.param_ownership_arc_fixit_hint.size() != rhs.arity) {
    return false;
  }
  if (!AreEquivalentProtocolCompositions(
          lhs.return_has_protocol_composition,
          lhs.return_protocol_composition_lexicographic,
          rhs.return_has_protocol_composition,
          rhs.return_protocol_composition_lexicographic)) {
    return false;
  }
  for (std::size_t i = 0; i < lhs.arity; ++i) {
    if (i >= lhs.param_types.size() || i >= lhs.param_canonical_types.size() ||
        i >= lhs.param_is_vector.size() ||
        i >= lhs.param_vector_base_spelling.size() ||
        i >= lhs.param_vector_lane_count.size() ||
        i >= lhs.param_has_protocol_composition.size() ||
        i >= lhs.param_protocol_composition_lexicographic.size() ||
        i >= rhs.param_types.size() || i >= rhs.param_canonical_types.size() ||
        i >= rhs.param_is_vector.size() ||
        i >= rhs.param_vector_base_spelling.size() ||
        i >= rhs.param_vector_lane_count.size() ||
        i >= rhs.param_has_protocol_composition.size() ||
        i >= rhs.param_protocol_composition_lexicographic.size()) {
      return false;
    }
    if (i >= lhs.param_has_ownership_qualifier.size() ||
        i >= rhs.param_has_ownership_qualifier.size()) {
      return false;
    }
    if (lhs.param_types[i] != rhs.param_types[i] ||
        lhs.param_is_vector[i] != rhs.param_is_vector[i]) {
      return false;
    }
    if (!IsCompatibleCanonicalSemanticType(lhs.param_canonical_types[i],
                                           rhs.param_canonical_types[i])) {
      return false;
    }
    if (lhs.param_has_ownership_qualifier[i] !=
        rhs.param_has_ownership_qualifier[i]) {
      return false;
    }
    if (lhs.param_ownership_insert_retain[i] !=
            rhs.param_ownership_insert_retain[i] ||
        lhs.param_ownership_insert_release[i] !=
            rhs.param_ownership_insert_release[i] ||
        lhs.param_ownership_insert_autorelease[i] !=
            rhs.param_ownership_insert_autorelease[i] ||
        lhs.param_ownership_is_weak_reference[i] !=
            rhs.param_ownership_is_weak_reference[i] ||
        lhs.param_ownership_is_unowned_reference[i] !=
            rhs.param_ownership_is_unowned_reference[i] ||
        lhs.param_ownership_is_unowned_safe_reference[i] !=
            rhs.param_ownership_is_unowned_safe_reference[i] ||
        lhs.param_ownership_arc_diagnostic_candidate[i] !=
            rhs.param_ownership_arc_diagnostic_candidate[i] ||
        lhs.param_ownership_arc_fixit_available[i] !=
            rhs.param_ownership_arc_fixit_available[i] ||
        lhs.param_ownership_arc_diagnostic_profile[i] !=
            rhs.param_ownership_arc_diagnostic_profile[i] ||
        lhs.param_ownership_arc_fixit_hint[i] !=
            rhs.param_ownership_arc_fixit_hint[i]) {
      return false;
    }
    if (lhs.param_is_vector[i] &&
        (lhs.param_vector_base_spelling[i] !=
             rhs.param_vector_base_spelling[i] ||
         lhs.param_vector_lane_count[i] != rhs.param_vector_lane_count[i])) {
      return false;
    }
    if (!AreEquivalentProtocolCompositions(
            lhs.param_has_protocol_composition[i],
            lhs.param_protocol_composition_lexicographic[i],
            rhs.param_has_protocol_composition[i],
            rhs.param_protocol_composition_lexicographic[i])) {
      return false;
    }
  }
  return true;
}
