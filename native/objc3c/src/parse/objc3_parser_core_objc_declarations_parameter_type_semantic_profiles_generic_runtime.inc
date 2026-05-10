    param.variance_bridge_cast_profile =
        BuildVarianceBridgeCastProfile(
            param.object_pointer_type_spelling,
            param.has_generic_suffix,
            param.generic_suffix_terminated,
            param.has_pointer_declarator,
            param.generic_suffix_text,
            param.ownership_qualifier_spelling);
    param.variance_bridge_cast_profile_is_normalized =
        IsVarianceBridgeCastProfileNormalized(
            param.object_pointer_type_spelling,
            param.has_generic_suffix,
            param.generic_suffix_terminated,
            param.generic_suffix_text,
            param.ownership_qualifier_spelling);
    param.generic_metadata_abi_profile =
        BuildGenericMetadataAbiProfile(
            param.object_pointer_type_spelling,
            param.has_generic_suffix,
            param.generic_suffix_terminated,
            param.has_pointer_declarator,
            param.generic_suffix_text,
            param.ownership_qualifier_spelling);
    param.generic_metadata_abi_profile_is_normalized =
        IsGenericMetadataAbiProfileNormalized(
            param.object_pointer_type_spelling,
            param.has_generic_suffix,
            param.generic_suffix_terminated,
            param.has_pointer_declarator,
            param.generic_suffix_text);
