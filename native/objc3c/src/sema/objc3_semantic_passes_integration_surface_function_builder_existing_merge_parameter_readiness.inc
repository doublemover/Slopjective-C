    for (std::size_t i = 0; i < fn.params.size() && i < existing.param_has_generic_suffix.size(); ++i) {
      existing.param_has_generic_suffix[i] =
          existing.param_has_generic_suffix[i] || fn.params[i].has_generic_suffix;
    }
    for (std::size_t i = 0; i < fn.params.size() && i < existing.param_has_pointer_declarator.size(); ++i) {
      existing.param_has_pointer_declarator[i] =
          existing.param_has_pointer_declarator[i] || fn.params[i].has_pointer_declarator;
    }
    for (std::size_t i = 0; i < fn.params.size() && i < existing.param_has_nullability_suffix.size(); ++i) {
      existing.param_has_nullability_suffix[i] =
          existing.param_has_nullability_suffix[i] || !fn.params[i].nullability_suffix_tokens.empty();
    }
    for (std::size_t i = 0; i < fn.params.size() && i < existing.param_has_ownership_qualifier.size(); ++i) {
      existing.param_has_ownership_qualifier[i] =
          existing.param_has_ownership_qualifier[i] || fn.params[i].has_ownership_qualifier;
    }
    for (std::size_t i = 0; i < fn.params.size() && i < existing.param_id_spelling.size(); ++i) {
      existing.param_id_spelling[i] = existing.param_id_spelling[i] || fn.params[i].id_spelling;
    }
    for (std::size_t i = 0; i < fn.params.size() && i < existing.param_class_spelling.size(); ++i) {
      existing.param_class_spelling[i] = existing.param_class_spelling[i] || fn.params[i].class_spelling;
    }
    for (std::size_t i = 0; i < fn.params.size() && i < existing.param_instancetype_spelling.size(); ++i) {
      existing.param_instancetype_spelling[i] =
          existing.param_instancetype_spelling[i] || fn.params[i].instancetype_spelling;
    }
    for (std::size_t i = 0; i < fn.params.size() && i < existing.param_object_pointer_type_spelling.size(); ++i) {
      existing.param_object_pointer_type_spelling[i] =
          existing.param_object_pointer_type_spelling[i] || fn.params[i].object_pointer_type_spelling;
    }
    for (std::size_t i = 0; i < fn.params.size() && i < existing.param_has_invalid_generic_suffix.size(); ++i) {
      existing.param_has_invalid_generic_suffix[i] =
          existing.param_has_invalid_generic_suffix[i] || HasInvalidGenericParamTypeSuffix(fn.params[i]);
    }
    for (std::size_t i = 0; i < fn.params.size() && i < existing.param_has_invalid_pointer_declarator.size(); ++i) {
      existing.param_has_invalid_pointer_declarator[i] =
          existing.param_has_invalid_pointer_declarator[i] || HasInvalidPointerParamTypeDeclarator(fn.params[i]);
    }
    for (std::size_t i = 0; i < fn.params.size() && i < existing.param_has_invalid_nullability_suffix.size(); ++i) {
      existing.param_has_invalid_nullability_suffix[i] =
          existing.param_has_invalid_nullability_suffix[i] || HasInvalidNullabilityParamTypeSuffix(fn.params[i]);
    }
    for (std::size_t i = 0; i < fn.params.size() && i < existing.param_has_invalid_ownership_qualifier.size(); ++i) {
      existing.param_has_invalid_ownership_qualifier[i] =
          existing.param_has_invalid_ownership_qualifier[i] ||
          HasInvalidOwnershipQualifierParamTypeSuffix(fn.params[i]);
    }
    for (std::size_t i = 0; i < fn.params.size() && i < existing.param_has_invalid_type_suffix.size(); ++i) {
      existing.param_has_invalid_type_suffix[i] =
          existing.param_has_invalid_type_suffix[i] || HasInvalidParamTypeSuffix(fn.params[i]);
    }
    for (std::size_t i = 0; i < fn.params.size() && i < existing.param_has_invalid_protocol_composition.size(); ++i) {
      const ProtocolCompositionInfo param_protocol_composition =
          BuildProtocolCompositionInfoFromParam(fn.params[i],
                                               &generic_interface_names);
      existing.param_has_invalid_protocol_composition[i] =
          existing.param_has_invalid_protocol_composition[i] || param_protocol_composition.has_invalid_protocol_composition;
    }
    for (std::size_t i = 0; i < fn.params.size() && i < existing.param_canonical_types.size(); ++i) {
      existing.param_canonical_types[i] = BuildCanonicalSemanticTypeFromParam(fn.params[i]);
    }
