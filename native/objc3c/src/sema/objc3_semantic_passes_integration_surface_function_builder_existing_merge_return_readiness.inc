    existing.return_has_generic_suffix = existing.return_has_generic_suffix || fn.has_return_generic_suffix;
    existing.return_has_pointer_declarator = existing.return_has_pointer_declarator || fn.has_return_pointer_declarator;
    existing.return_has_nullability_suffix =
        existing.return_has_nullability_suffix || !fn.return_nullability_suffix_tokens.empty();
    existing.return_has_ownership_qualifier =
        existing.return_has_ownership_qualifier || fn.has_return_ownership_qualifier;
    existing.return_id_spelling = existing.return_id_spelling || fn.return_id_spelling;
    existing.return_class_spelling = existing.return_class_spelling || fn.return_class_spelling;
    existing.return_instancetype_spelling =
        existing.return_instancetype_spelling || fn.return_instancetype_spelling;
    existing.return_object_pointer_type_spelling =
        existing.return_object_pointer_type_spelling || fn.return_object_pointer_type_spelling;
    existing.return_has_invalid_generic_suffix =
        existing.return_has_invalid_generic_suffix || HasInvalidGenericReturnTypeSuffix(fn);
    existing.return_has_invalid_pointer_declarator =
        existing.return_has_invalid_pointer_declarator || HasInvalidPointerReturnTypeDeclarator(fn);
    existing.return_has_invalid_nullability_suffix =
        existing.return_has_invalid_nullability_suffix || HasInvalidNullabilityReturnTypeSuffix(fn);
    existing.return_has_invalid_ownership_qualifier =
        existing.return_has_invalid_ownership_qualifier || HasInvalidOwnershipQualifierReturnTypeSuffix(fn);
    existing.return_has_invalid_type_suffix = existing.return_has_invalid_type_suffix ||
                                             existing.return_has_invalid_generic_suffix ||
                                             existing.return_has_invalid_pointer_declarator ||
                                             existing.return_has_invalid_nullability_suffix ||
                                             existing.return_has_invalid_ownership_qualifier;
    existing.return_has_invalid_protocol_composition =
        existing.return_has_invalid_protocol_composition || return_protocol_composition.has_invalid_protocol_composition;
    existing.return_canonical_type =
        BuildCanonicalSemanticTypeFromFunctionReturn(fn);
