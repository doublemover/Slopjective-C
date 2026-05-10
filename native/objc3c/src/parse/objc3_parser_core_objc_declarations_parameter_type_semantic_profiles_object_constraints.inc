    param.lightweight_generic_constraint_profile =
        BuildLightweightGenericConstraintProfile(
            param.object_pointer_type_spelling,
            param.has_generic_suffix,
            param.generic_suffix_terminated,
            param.has_pointer_declarator,
            param.generic_suffix_text);
    param.lightweight_generic_constraint_profile_is_normalized =
        IsLightweightGenericConstraintProfileNormalized(
            param.object_pointer_type_spelling,
            param.has_generic_suffix,
            param.generic_suffix_terminated);
    param.nullability_flow_profile =
        BuildNullabilityFlowProfile(
            param.object_pointer_type_spelling,
            param.nullability_suffix_tokens.size(),
            param.has_pointer_declarator,
            param.has_generic_suffix,
            param.generic_suffix_terminated);
    param.nullability_flow_profile_is_normalized =
        IsNullabilityFlowProfileNormalized(
            param.object_pointer_type_spelling,
            param.nullability_suffix_tokens.size());
    param.protocol_qualified_object_type_profile =
        BuildProtocolQualifiedObjectTypeProfile(
            param.object_pointer_type_spelling,
            param.has_generic_suffix,
            param.generic_suffix_terminated,
            param.has_pointer_declarator,
            param.generic_suffix_text);
    param.protocol_qualified_object_type_profile_is_normalized =
        IsProtocolQualifiedObjectTypeProfileNormalized(
            param.object_pointer_type_spelling,
            param.has_generic_suffix,
            param.generic_suffix_terminated);
