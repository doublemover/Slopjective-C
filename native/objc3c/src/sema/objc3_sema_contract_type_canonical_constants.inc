inline constexpr std::uint32_t kObjc3SemaBoundaryContractVersionMajor = 1;
inline constexpr std::uint32_t kObjc3SemaBoundaryContractVersionMinor = 0;
inline constexpr std::uint32_t kObjc3SemaBoundaryContractVersionPatch = 0;
inline constexpr const char *kObjc3RuntimeLinkHostLinkDefaultDispatchSymbol =
    "objc3_runtime_dispatch_i32";
inline constexpr std::array<ValueType, 6> kObjc3CanonicalReferenceTypeForms = {
    ValueType::ObjCId,
    ValueType::ObjCClass,
    ValueType::ObjCSel,
    ValueType::ObjCProtocol,
    ValueType::ObjCInstancetype,
    ValueType::ObjCObjectPtr,
};
inline constexpr std::array<ValueType, 2> kObjc3CanonicalScalarMessageSendTypeForms = {
    ValueType::I32,
    ValueType::Bool,
};
inline constexpr std::array<ValueType, 5> kObjc3CanonicalBridgeTopReferenceTypeForms = {
    ValueType::ObjCId,
    ValueType::ObjCClass,
    ValueType::ObjCProtocol,
    ValueType::ObjCInstancetype,
    ValueType::ObjCObjectPtr,
};
inline constexpr const char *kObjc3CompatibilityStrictnessClaimSemanticsContractId =
    "objc3c.canonical.selection.claim.semantics.v1";
inline constexpr const char *kObjc3CompatibilityStrictnessClaimSemanticsSurfacePath =
    "frontend.pipeline.semantic_surface.objc_canonical_selection_claim_semantics";
// runnable-core compatibility guard anchor: sema owns the current
// truthful split between live selections, source-only downgraded claims, and
// fail-closed unsupported advanced surfaces around the runnable core.
inline constexpr const char *kObjc3CompatibilityStrictnessClaimSemanticModel =
    "canonical-selection-only-strictness-and-concurrency-claims-remain-fail-closed";
inline constexpr const char *kObjc3CompatibilityStrictnessClaimDowngradeModel =
    "source-only-claims-remain-recognized-but-never-promote-to-runnable";
inline constexpr const char *kObjc3CompatibilityStrictnessClaimRejectionModel =
    "strictness-strict-concurrency-and-feature-macro-claims-remain-fail-closed";
inline constexpr const char *kObjc3CompatibilityStrictnessClaimCanonicalInterfaceTruthModel =
    "no-standalone-interface-payload-yet-and-any-future-canonical-interface-must-stay-bounded-to-runnable-and-source-downgraded-claims";
inline constexpr const char *kObjc3CompatibilityStrictnessClaimSeparateCompilationMacroTruthModel =
    "suppressed-feature-macro-claims-remain-unpublished-across-manifest-interface-and-conformance-surfaces-until-executable";
inline constexpr const char *kObjc3CompatibilityStrictnessClaimCanonicalInterfacePayloadMode =
    "no-standalone-interface-payload-yet";
inline constexpr std::size_t kObjc3CompatibilityStrictnessClaimValidLanguageProfileCount = 1u;
inline constexpr std::size_t kObjc3CompatibilityStrictnessClaimLiveSelectionSurfaceCount = 2u;
inline constexpr std::size_t kObjc3CompatibilityStrictnessClaimValidSelectionCombinationCount = 1u;
inline constexpr std::size_t kObjc3CompatibilityStrictnessClaimRunnableFeatureCount = 7u;
inline constexpr std::size_t kObjc3CompatibilityStrictnessClaimSourceOnlyFeatureCount = 6u;
inline constexpr std::size_t kObjc3CompatibilityStrictnessClaimRejectedFeatureCount = 11u;
inline constexpr std::size_t kObjc3CompatibilityStrictnessClaimRejectedSelectionSurfaceCount = 3u;
inline constexpr std::size_t kObjc3CompatibilityStrictnessClaimSuppressedMacroClaimCount = 3u;
