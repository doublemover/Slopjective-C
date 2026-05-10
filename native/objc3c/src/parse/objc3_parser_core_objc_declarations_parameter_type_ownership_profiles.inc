    param.ownership_qualifier_symbol =
        BuildOwnershipQualifierSymbol(param.ownership_qualifier_spelling, false);
    const Objc3OwnershipOperationProfile param_ownership_profile =
        BuildParamOwnershipOperationProfile(param.ownership_qualifier_spelling);
    param.ownership_insert_retain = param_ownership_profile.insert_retain;
    param.ownership_insert_release = param_ownership_profile.insert_release;
    param.ownership_insert_autorelease =
        param_ownership_profile.insert_autorelease;
    param.ownership_operation_profile = param_ownership_profile.profile;
    const Objc3WeakUnownedLifetimeProfile param_lifetime_profile =
        BuildWeakUnownedLifetimeProfile(param.ownership_qualifier_spelling, false);
    param.ownership_is_weak_reference =
        param_lifetime_profile.is_weak_reference;
    param.ownership_is_unowned_reference =
        param_lifetime_profile.is_unowned_reference;
    param.ownership_is_unowned_safe_reference =
        param_lifetime_profile.is_unowned_safe_reference;
    param.ownership_lifetime_profile = param_lifetime_profile.lifetime_profile;
    param.ownership_runtime_hook_profile =
        param_lifetime_profile.runtime_hook_profile;
    const Objc3ArcDiagnosticFixitProfile param_arc_diagnostic_profile =
        BuildArcDiagnosticFixitProfile(
            param.ownership_qualifier_spelling, false, false, false);
    param.ownership_arc_diagnostic_candidate =
        param_arc_diagnostic_profile.diagnostic_candidate;
    param.ownership_arc_fixit_available =
        param_arc_diagnostic_profile.fixit_available;
    param.ownership_arc_diagnostic_profile =
        param_arc_diagnostic_profile.diagnostic_profile;
    param.ownership_arc_fixit_hint = param_arc_diagnostic_profile.fixit_hint;
