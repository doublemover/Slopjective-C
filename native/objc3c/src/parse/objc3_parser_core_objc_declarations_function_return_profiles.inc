  void FinalizeFunctionReturnTypeProfiles(FunctionDecl &fn) {
    fn.return_ownership_qualifier_symbol =
        BuildOwnershipQualifierSymbol(fn.return_ownership_qualifier_spelling, true);
    const Objc3OwnershipOperationProfile return_ownership_profile =
        BuildReturnOwnershipOperationProfile(fn.return_ownership_qualifier_spelling);
    fn.return_ownership_insert_retain = return_ownership_profile.insert_retain;
    fn.return_ownership_insert_release = return_ownership_profile.insert_release;
    fn.return_ownership_insert_autorelease = return_ownership_profile.insert_autorelease;
    fn.return_ownership_operation_profile = return_ownership_profile.profile;
    const Objc3WeakUnownedLifetimeProfile return_lifetime_profile =
        BuildWeakUnownedLifetimeProfile(fn.return_ownership_qualifier_spelling, false);
    fn.return_ownership_is_weak_reference = return_lifetime_profile.is_weak_reference;
    fn.return_ownership_is_unowned_reference = return_lifetime_profile.is_unowned_reference;
    fn.return_ownership_is_unowned_safe_reference = return_lifetime_profile.is_unowned_safe_reference;
    fn.return_ownership_lifetime_profile = return_lifetime_profile.lifetime_profile;
    fn.return_ownership_runtime_hook_profile = return_lifetime_profile.runtime_hook_profile;
    const Objc3ArcDiagnosticFixitProfile return_arc_diagnostic_profile =
        BuildArcDiagnosticFixitProfile(fn.return_ownership_qualifier_spelling, true, false, false);
    fn.return_ownership_arc_diagnostic_candidate = return_arc_diagnostic_profile.diagnostic_candidate;
    fn.return_ownership_arc_fixit_available = return_arc_diagnostic_profile.fixit_available;
    fn.return_ownership_arc_diagnostic_profile = return_arc_diagnostic_profile.diagnostic_profile;
    fn.return_ownership_arc_fixit_hint = return_arc_diagnostic_profile.fixit_hint;
    fn.return_lightweight_generic_constraint_profile =
        BuildLightweightGenericConstraintProfile(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.has_return_pointer_declarator,
            fn.return_generic_suffix_text);
    fn.return_lightweight_generic_constraint_profile_is_normalized =
        IsLightweightGenericConstraintProfileNormalized(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated);
    fn.return_nullability_flow_profile =
        BuildNullabilityFlowProfile(
            fn.return_object_pointer_type_spelling,
            fn.return_nullability_suffix_tokens.size(),
            fn.has_return_pointer_declarator,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated);
    fn.return_nullability_flow_profile_is_normalized =
        IsNullabilityFlowProfileNormalized(
            fn.return_object_pointer_type_spelling,
            fn.return_nullability_suffix_tokens.size());
    fn.return_protocol_qualified_object_type_profile =
        BuildProtocolQualifiedObjectTypeProfile(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.has_return_pointer_declarator,
            fn.return_generic_suffix_text);
    fn.return_protocol_qualified_object_type_profile_is_normalized =
        IsProtocolQualifiedObjectTypeProfileNormalized(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated);
    fn.return_variance_bridge_cast_profile =
        BuildVarianceBridgeCastProfile(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.has_return_pointer_declarator,
            fn.return_generic_suffix_text,
            fn.return_ownership_qualifier_spelling);
    fn.return_variance_bridge_cast_profile_is_normalized =
        IsVarianceBridgeCastProfileNormalized(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.return_generic_suffix_text,
            fn.return_ownership_qualifier_spelling);
    fn.return_generic_metadata_abi_profile =
        BuildGenericMetadataAbiProfile(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.has_return_pointer_declarator,
            fn.return_generic_suffix_text,
            fn.return_ownership_qualifier_spelling);
    fn.return_generic_metadata_abi_profile_is_normalized =
        IsGenericMetadataAbiProfileNormalized(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.has_return_pointer_declarator,
            fn.return_generic_suffix_text);
    fn.return_module_import_graph_profile =
        BuildModuleImportGraphProfile(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.has_return_pointer_declarator,
            fn.return_generic_suffix_text,
            fn.return_object_pointer_type_name);
    fn.return_module_import_graph_profile_is_normalized =
        IsModuleImportGraphProfileNormalized(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.return_generic_suffix_text);
    fn.return_namespace_collision_shadowing_profile =
        BuildNamespaceCollisionShadowingProfile(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.has_return_pointer_declarator,
            fn.return_generic_suffix_text,
            fn.return_object_pointer_type_name);
    fn.return_namespace_collision_shadowing_profile_is_normalized =
        IsNamespaceCollisionShadowingProfileNormalized(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.return_generic_suffix_text,
            fn.return_object_pointer_type_name);
    fn.return_public_private_api_partition_profile =
        BuildPublicPrivateApiPartitionProfile(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.has_return_pointer_declarator,
            fn.return_generic_suffix_text,
            fn.return_object_pointer_type_name);
    fn.return_public_private_api_partition_profile_is_normalized =
        IsPublicPrivateApiPartitionProfileNormalized(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.return_generic_suffix_text,
            fn.return_object_pointer_type_name);
    fn.return_incremental_module_cache_invalidation_profile =
        BuildIncrementalModuleCacheInvalidationProfile(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.has_return_pointer_declarator,
            fn.return_generic_suffix_text,
            fn.return_object_pointer_type_name);
    fn.return_incremental_module_cache_invalidation_profile_is_normalized =
        IsIncrementalModuleCacheInvalidationProfileNormalized(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.has_return_pointer_declarator,
            fn.return_generic_suffix_text,
            fn.return_object_pointer_type_name);
    fn.return_cross_module_conformance_profile =
        BuildCrossModuleConformanceProfile(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.has_return_pointer_declarator,
            fn.return_generic_suffix_text,
            fn.return_object_pointer_type_name);
    fn.return_cross_module_conformance_profile_is_normalized =
        IsCrossModuleConformanceProfileNormalized(
            fn.return_object_pointer_type_spelling,
            fn.has_return_generic_suffix,
            fn.return_generic_suffix_terminated,
            fn.has_return_pointer_declarator,
            fn.return_generic_suffix_text,
            fn.return_object_pointer_type_name);
    fn.returns_borrowed_profile = BuildReturnsBorrowedProfile(
        fn.objc_returns_borrowed_declared,
        fn.objc_returns_borrowed_owner_index,
        fn.return_borrowed_pointer_qualified);
  }
