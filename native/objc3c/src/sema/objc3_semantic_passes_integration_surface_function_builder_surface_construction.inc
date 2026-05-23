      FunctionInfo info;
      info.arity = fn.params.size();
      std::unordered_set<std::string> seen_function_generic_parameters;
      for (const auto &generic_param : fn.generic_params) {
        if (!seen_function_generic_parameters.insert(generic_param.name).second) {
          diagnostics.push_back(MakeDiag(
              generic_param.line, generic_param.column, "O3S200",
              "duplicate generic parameter '" + generic_param.name +
                  "' in function '" + fn.name + "'"));
          continue;
        }
        info.generic_parameter_names_source_order.push_back(generic_param.name);
        info.generic_parameter_variance_source_order.push_back(
            generic_param.variance_spelling);
        std::vector<std::string> constraints;
        if (generic_param.has_constraint) {
          if (generic_param.constraint_type_name != "id" ||
              !generic_param.has_constraint_generic_suffix) {
            diagnostics.push_back(MakeDiag(
                generic_param.constraint_line, generic_param.constraint_column,
                "O3S206",
                "type mismatch: generic parameter '" + generic_param.name +
                    "' in function '" + fn.name +
                    "' requires an id<Protocol> constraint"));
          } else {
            const ProtocolCompositionParseResult parsed =
                ParseProtocolCompositionSuffixText(
                    generic_param.constraint_generic_suffix_text);
            if (!parsed.IsValid()) {
              diagnostics.push_back(MakeDiag(
                  generic_param.constraint_line,
                  generic_param.constraint_column, "O3S206",
                  "type mismatch: generic parameter '" + generic_param.name +
                      "' in function '" + fn.name +
                      "' has an invalid protocol constraint"));
            } else {
              constraints = parsed.names_lexicographic;
              for (const auto &protocol_name : constraints) {
                if (protocol_definitions.find(protocol_name) ==
                    protocol_definitions.end()) {
                  diagnostics.push_back(MakeDiag(
                      generic_param.constraint_line,
                      generic_param.constraint_column, "O3S206",
                      "type mismatch: generic parameter '" +
                          generic_param.name + "' in function '" + fn.name +
                          "' references unknown constraint protocol '" +
                          protocol_name + "'"));
                }
              }
            }
          }
        }
        info.generic_parameter_constraints_lexicographic.push_back(
            std::move(constraints));
      }
      info.generic_callable_reification_policy =
          Objc3GenericCallableReificationPolicyForMarker(
              fn.generic_callable_reification_marker_declared);
      info.generic_callable_mangling_policy_id =
          Objc3GenericCallableManglingPolicyForReificationPolicy(
              info.generic_callable_reification_policy);
      info.generic_callable_signature_replay_key =
          BuildObjc3GenericCallableSignatureReplayKey(
              "function", fn.name, info.generic_parameter_names_source_order,
              info.generic_parameter_variance_source_order,
              info.generic_parameter_constraints_lexicographic,
              info.generic_callable_reification_policy,
              info.generic_callable_mangling_policy_id);
      info.generic_callable_contract_deterministic =
          !info.generic_callable_signature_replay_key.empty() &&
          IsObjc3GenericCallableReificationPolicyClaimable(
              info.generic_callable_reification_policy) &&
          IsObjc3GenericCallableManglingPolicyClaimable(
              info.generic_callable_mangling_policy_id);
      info.param_types.reserve(fn.params.size());
      info.param_canonical_types.reserve(fn.params.size());
      info.param_is_vector.reserve(fn.params.size());
      info.param_vector_base_spelling.reserve(fn.params.size());
      info.param_vector_lane_count.reserve(fn.params.size());
      info.param_has_generic_suffix.reserve(fn.params.size());
      info.param_has_pointer_declarator.reserve(fn.params.size());
      info.param_has_nullability_suffix.reserve(fn.params.size());
      info.param_has_ownership_qualifier.reserve(fn.params.size());
      info.param_id_spelling.reserve(fn.params.size());
      info.param_class_spelling.reserve(fn.params.size());
      info.param_instancetype_spelling.reserve(fn.params.size());
      info.param_object_pointer_type_spelling.reserve(fn.params.size());
      info.param_has_invalid_generic_suffix.reserve(fn.params.size());
      info.param_has_invalid_pointer_declarator.reserve(fn.params.size());
      info.param_has_invalid_nullability_suffix.reserve(fn.params.size());
      info.param_has_invalid_ownership_qualifier.reserve(fn.params.size());
      info.param_has_invalid_type_suffix.reserve(fn.params.size());
      info.param_ownership_insert_retain.reserve(fn.params.size());
      info.param_ownership_insert_release.reserve(fn.params.size());
      info.param_ownership_insert_autorelease.reserve(fn.params.size());
      info.param_ownership_is_weak_reference.reserve(fn.params.size());
      info.param_ownership_is_unowned_reference.reserve(fn.params.size());
      info.param_ownership_is_unowned_safe_reference.reserve(fn.params.size());
      info.param_ownership_arc_diagnostic_candidate.reserve(fn.params.size());
      info.param_ownership_arc_fixit_available.reserve(fn.params.size());
      info.param_ownership_arc_diagnostic_profile.reserve(fn.params.size());
      info.param_ownership_arc_fixit_hint.reserve(fn.params.size());
      info.param_has_protocol_composition.reserve(fn.params.size());
      info.param_protocol_composition_lexicographic.reserve(fn.params.size());
      info.param_has_invalid_protocol_composition.reserve(fn.params.size());
      for (const auto &param : fn.params) {
        const ProtocolCompositionInfo protocol_composition =
            BuildProtocolCompositionInfoFromParam(param,
                                                 &generic_interface_names);
        info.param_types.push_back(param.type);
        info.param_canonical_types.push_back(
            BuildCanonicalSemanticTypeFromParam(param, &generic_interface_names));
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
        const std::size_t param_index =
            info.param_has_ownership_qualifier.size() - 1u;
        const bool arc_inferred_object_param =
            arc_mode_enabled &&
            !info.param_has_ownership_qualifier[param_index] &&
            (info.param_id_spelling[param_index] ||
             info.param_instancetype_spelling[param_index] ||
             info.param_object_pointer_type_spelling[param_index]);
        if (arc_inferred_object_param) {
          // ARC inference/lifetime implementation anchor: function
          // parameters in the supported ARC slice now receive the same
          // canonical strong-owned retain/release profile as method
          // parameters, keeping one semantic contract for executable
          // unqualified object signatures.
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
          BuildProtocolCompositionInfoFromFunctionReturn(fn,
                                                        &generic_interface_names);
      info.return_has_generic_suffix = fn.has_return_generic_suffix;
      info.return_has_pointer_declarator = fn.has_return_pointer_declarator;
      info.return_has_nullability_suffix = !fn.return_nullability_suffix_tokens.empty();
      info.return_has_ownership_qualifier = fn.has_return_ownership_qualifier;
      info.return_id_spelling = fn.return_id_spelling;
      info.return_class_spelling = fn.return_class_spelling;
      info.return_instancetype_spelling = fn.return_instancetype_spelling;
      info.return_object_pointer_type_spelling = fn.return_object_pointer_type_spelling;
      info.return_has_invalid_generic_suffix = HasInvalidGenericReturnTypeSuffix(fn);
      info.return_has_invalid_pointer_declarator = HasInvalidPointerReturnTypeDeclarator(fn);
      info.return_has_invalid_nullability_suffix = HasInvalidNullabilityReturnTypeSuffix(fn);
      info.return_has_invalid_ownership_qualifier = HasInvalidOwnershipQualifierReturnTypeSuffix(fn);
      info.return_has_invalid_type_suffix = info.return_has_invalid_generic_suffix ||
                                            info.return_has_invalid_pointer_declarator ||
                                            info.return_has_invalid_nullability_suffix ||
                                            info.return_has_invalid_ownership_qualifier;
      info.return_ownership_insert_retain = fn.return_ownership_insert_retain;
      info.return_ownership_insert_release = fn.return_ownership_insert_release;
      info.return_ownership_insert_autorelease = fn.return_ownership_insert_autorelease;
      info.return_ownership_is_weak_reference = fn.return_ownership_is_weak_reference;
      info.return_ownership_is_unowned_reference = fn.return_ownership_is_unowned_reference;
      info.return_ownership_is_unowned_safe_reference = fn.return_ownership_is_unowned_safe_reference;
      info.return_ownership_arc_diagnostic_candidate = fn.return_ownership_arc_diagnostic_candidate;
      info.return_ownership_arc_fixit_available = fn.return_ownership_arc_fixit_available;
      info.return_ownership_arc_diagnostic_profile = fn.return_ownership_arc_diagnostic_profile;
      info.return_ownership_arc_fixit_hint = fn.return_ownership_arc_fixit_hint;
      info.return_type = fn.return_type;
      info.return_canonical_type =
          BuildCanonicalSemanticTypeFromFunctionReturn(fn, &generic_interface_names);
      info.return_is_vector = fn.return_vector_spelling;
      info.return_vector_base_spelling = fn.return_vector_base_spelling;
      info.return_vector_lane_count = fn.return_vector_lane_count;
      info.return_has_protocol_composition = return_protocol_composition.has_protocol_composition;
      info.return_protocol_composition_lexicographic = return_protocol_composition.names_lexicographic;
      info.return_has_invalid_protocol_composition = return_protocol_composition.has_invalid_protocol_composition;
      const bool arc_inferred_object_return =
          arc_mode_enabled && !info.return_has_ownership_qualifier &&
          (info.return_id_spelling || info.return_instancetype_spelling ||
           info.return_object_pointer_type_spelling);
      if (arc_inferred_object_return) {
        // ARC inference/lifetime implementation anchor: function
        // returns in the supported ARC slice now receive the same canonical
        // strong-owned retain/release profile as method returns.
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
