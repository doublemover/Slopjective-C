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
    param.module_import_graph_profile =
        BuildModuleImportGraphProfile(
            param.object_pointer_type_spelling,
            param.has_generic_suffix,
            param.generic_suffix_terminated,
            param.has_pointer_declarator,
            param.generic_suffix_text,
            param.object_pointer_type_name);
    param.module_import_graph_profile_is_normalized =
        IsModuleImportGraphProfileNormalized(
            param.object_pointer_type_spelling,
            param.has_generic_suffix,
            param.generic_suffix_terminated,
            param.generic_suffix_text);
    param.namespace_collision_shadowing_profile =
        BuildNamespaceCollisionShadowingProfile(
            param.object_pointer_type_spelling,
            param.has_generic_suffix,
            param.generic_suffix_terminated,
            param.has_pointer_declarator,
            param.generic_suffix_text,
            param.object_pointer_type_name);
    param.namespace_collision_shadowing_profile_is_normalized =
        IsNamespaceCollisionShadowingProfileNormalized(
            param.object_pointer_type_spelling,
            param.has_generic_suffix,
            param.generic_suffix_terminated,
            param.generic_suffix_text,
            param.object_pointer_type_name);
    param.public_private_api_partition_profile =
        BuildPublicPrivateApiPartitionProfile(
            param.object_pointer_type_spelling,
            param.has_generic_suffix,
            param.generic_suffix_terminated,
            param.has_pointer_declarator,
            param.generic_suffix_text,
            param.object_pointer_type_name);
    param.public_private_api_partition_profile_is_normalized =
        IsPublicPrivateApiPartitionProfileNormalized(
            param.object_pointer_type_spelling,
            param.has_generic_suffix,
            param.generic_suffix_terminated,
            param.generic_suffix_text,
            param.object_pointer_type_name);
    param.incremental_module_cache_invalidation_profile =
        BuildIncrementalModuleCacheInvalidationProfile(
            param.object_pointer_type_spelling,
            param.has_generic_suffix,
            param.generic_suffix_terminated,
            param.has_pointer_declarator,
            param.generic_suffix_text,
            param.object_pointer_type_name);
    param.incremental_module_cache_invalidation_profile_is_normalized =
        IsIncrementalModuleCacheInvalidationProfileNormalized(
            param.object_pointer_type_spelling,
            param.has_generic_suffix,
            param.generic_suffix_terminated,
            param.has_pointer_declarator,
            param.generic_suffix_text,
            param.object_pointer_type_name);
    param.cross_module_conformance_profile =
        BuildCrossModuleConformanceProfile(
            param.object_pointer_type_spelling,
            param.has_generic_suffix,
            param.generic_suffix_terminated,
            param.has_pointer_declarator,
            param.generic_suffix_text,
            param.object_pointer_type_name);
    param.cross_module_conformance_profile_is_normalized =
        IsCrossModuleConformanceProfileNormalized(
            param.object_pointer_type_spelling,
            param.has_generic_suffix,
            param.generic_suffix_terminated,
            param.has_pointer_declarator,
            param.generic_suffix_text,
            param.object_pointer_type_name);
