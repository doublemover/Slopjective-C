#pragma once

// Runtime dispatch lookup contracts own selector interning, selector lookup
// tables, method-cache slow path, protocol/category method resolution, and
// live-dispatch gate/closeout constants.
inline constexpr const char *kObjc3RuntimeLookupDispatchContractId =
    "objc3c.runtime.lookup.dispatch.freeze.v1";
inline constexpr const char
    *kObjc3RuntimeLookupDispatchSelectorInterningModel =
        "process-global-selector-intern-table-stable-id-per-canonical-selector-spelling";
inline constexpr const char
    *kObjc3RuntimeLookupDispatchLookupTableModel =
        "registered-selector-pools-materialize-process-global-stable-id-table";
inline constexpr const char *kObjc3RuntimeLookupDispatchCacheModel =
    "method-cache-and-runtime-slow-path-resolve-realized-methods";
inline constexpr const char
    *kObjc3RuntimeLookupDispatchProtocolCategoryModel =
        "protocol-and-category-aware-method-resolution-participate-in-realized-slow-path";
inline constexpr const char *kObjc3RuntimeLookupDispatchLanguageProfileModel =
    "resolved-runtime-call-or-strict-dispatch-error";
inline constexpr const char *kObjc3RuntimeLookupDispatchFailureModel =
    "only-unresolved-or-invalid-runtime-resolution-publishes-strict-dispatch-error";

inline constexpr const char *kObjc3RuntimeSelectorLookupTablesContractId =
    "objc3c.runtime.selector.lookup.tables.v1";
inline constexpr const char
    *kObjc3RuntimeSelectorLookupTablesInterningModel =
        "registered-selector-pools-materialize-process-global-stable-id-table";
inline constexpr const char
    *kObjc3RuntimeSelectorLookupTablesMergeModel =
        "per-image-selector-pools-deduplicated-and-merged-across-registration-order";
inline constexpr const char
    *kObjc3RuntimeSelectorLookupTablesDynamicMissModel =
        "unknown-selector-lookups-remain-dynamic-until-next-runtime-phase";
inline constexpr const char
    *kObjc3RuntimeSelectorLookupTablesReplayModel =
        "reset-replay-rebuilds-metadata-backed-selector-table-in-registration-order";
inline constexpr const char *kObjc3SelectorGlobalOrdering = "lexicographic";

inline constexpr const char *kObjc3RuntimeMethodCacheSlowPathContractId =
    "objc3c.runtime.method.cache.slow.path.lookup.v1";
inline constexpr const char
    *kObjc3RuntimeMethodCacheSlowPathReceiverNormalizationModel =
        "known-class-and-class-self-receivers-normalize-to-one-metaclass-cache-key";
inline constexpr const char
    *kObjc3RuntimeMethodCacheSlowPathResolutionModel =
        "registered-class-and-metaclass-records-drive-deterministic-slow-path-method-resolution";
inline constexpr const char
    *kObjc3RuntimeMethodCacheSlowPathCacheModel =
        "normalized-receiver-plus-selector-stable-id-positive-and-negative-cache";
inline constexpr const char
    *kObjc3RuntimeMethodCacheSlowPathStrictErrorModel =
        "only-unresolved-or-ambiguous-runtime-resolution-publishes-typed-strict-dispatch-error";
inline constexpr const char
    *kObjc3RuntimeProtocolCategoryMethodResolutionContractId =
        "objc3c.runtime.protocol.category.method.resolution.v1";
inline constexpr const char
    *kObjc3RuntimeProtocolCategoryMethodResolutionCategoryModel =
        "class-bodies-win-first-category-implementation-records-supply-next-live-method-tier";
inline constexpr const char
    *kObjc3RuntimeProtocolCategoryMethodResolutionProtocolModel =
        "adopted-and-inherited-protocol-method-lists-provide-declaration-aware-negative-resolution";
inline constexpr const char
    *kObjc3RuntimeProtocolCategoryMethodResolutionStrictErrorModel =
        "only-unresolved-conflicting-category-or-protocol-resolution-publishes-typed-strict-dispatch-error";

inline constexpr const char *kObjc3RuntimeLiveDispatchGateContractId =
    "objc3c.runtime.live.dispatch.gate.v1";
inline constexpr const char *kObjc3RuntimeLiveDispatchGateEvidenceModel =
    "source-sema-lowering-runtime-abi-summary-chain";
inline constexpr const char *kObjc3RuntimeLiveDispatchGateTestSurfaceBoundaryModel =
    "live-runtime-dispatch-required-test-surface-evidence-only";
inline constexpr const char *kObjc3RuntimeLiveDispatchGateFailureModel =
    "fail-closed-on-live-dispatch-evidence-drift";
inline constexpr const char *kObjc3RuntimeLiveDispatchGateNextIssue =
    "objc3c.runtime.livedispatch.closeout.v1";
inline constexpr const char *kObjc3RuntimeLiveDispatchSmokeReplayCloseoutContractId =
    "objc3c.runtime.live.dispatch.smoke.replay.closeout.v1";
inline constexpr const char *kObjc3RuntimeLiveDispatchSmokeReplayCloseoutModel =
    "live-runtime-dispatch-smoke-and-replay-authoritative-strict-error-on-unresolved";
