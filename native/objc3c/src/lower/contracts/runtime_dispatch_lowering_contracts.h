#pragma once

#include "lower/contracts/lowering_ownership_contracts.h"

#include <cstddef>
#include <string>

inline constexpr std::size_t kObjc3RuntimeDispatchDefaultArgs = 4;
inline constexpr std::size_t kObjc3RuntimeDispatchMaxArgs = 16;
inline constexpr const char *kObjc3RuntimeDispatchSymbol =
    "objc3_runtime_dispatch_i32";
inline constexpr const char *kObjc3DispatchSurfaceClassificationContractId =
    "objc3c.dispatch.surface.classification.v1";
inline constexpr const char *kObjc3DispatchSurfaceInstanceFamily = "instance";
inline constexpr const char *kObjc3DispatchSurfaceClassFamily = "class";
inline constexpr const char *kObjc3DispatchSurfaceSuperFamily = "super";
inline constexpr const char *kObjc3DispatchSurfaceDirectFamily = "direct";
inline constexpr const char *kObjc3DispatchSurfaceDynamicFamily = "dynamic";
inline constexpr const char *kObjc3DispatchSurfaceLiveRuntimeEntrypointFamily =
    "objc3_runtime_dispatch_i32-canonical-live-runtime";
inline constexpr const char *kObjc3DispatchSurfaceDirectDispatchBinding =
    "reserved-non-goal";
inline constexpr const char *kObjc3DispatchLegalitySelectorResolutionContractId =
    "objc3c.dispatch.legality.selector.resolution.v1";
inline constexpr const char *kObjc3DispatchLegalitySelectorResolutionBoundaryModel =
    "selector-normalized-arity-checked-receiver-required-no-overload";
inline constexpr const char *kObjc3DispatchLegalitySelectorResolutionAmbiguityPolicy =
    "fail-closed-on-unresolved-or-ambiguous-selector-resolution";
inline constexpr const char *kObjc3DispatchLegalitySelectorResolutionSupportedSelectorForms =
    "unary-and-keyword-selectors";
inline constexpr const char *kObjc3SelectorResolutionImplementationContractId =
    "objc3c.selector.resolution.ambiguity.v1";
inline constexpr const char *kObjc3SelectorResolutionConcreteReceiverPolicy =
    "self-super-known-class-receivers-resolve-concretely";
inline constexpr const char *kObjc3SelectorResolutionDynamicRuntimePolicy =
    "non-concrete-receivers-remain-runtime-dynamic";
inline constexpr const char *kObjc3SelectorResolutionOverloadPolicy =
    "no-overload-recovery-exact-signature-or-fail-closed";
inline constexpr const char *kObjc3SelectorResolutionMissingSelectorDiagnostic =
    "O3S216";
inline constexpr const char *kObjc3SelectorResolutionAmbiguousSelectorDiagnostic =
    "O3S217";
inline constexpr const char *kObjc3SuperDynamicMethodFamilyContractId =
    "objc3c.super.dynamic.method.family.v1";
inline constexpr const char *kObjc3SuperDispatchLegalityPolicy =
    "super-requires-enclosing-method-and-real-superclass";
inline constexpr const char *kObjc3DirectDispatchReservationPolicy =
    "direct-dispatch-remains-reserved-non-goal";
inline constexpr const char *kObjc3DynamicDispatchMethodFamilyPolicy =
    "dynamic-dispatch-preserves-runtime-resolution-and-method-family-accounting";
inline constexpr const char *kObjc3RuntimeVisibleMethodFamilyPolicy =
    "super-and-dynamic-sites-preserve-method-family-runtime-visibility";

// Emitted sends lower to the canonical runtime entrypoint. Selector lookup,
// receiver/result ABI, fixed-slot argument padding, and strict dispatch errors
// remain the compiler-to-runtime boundary.
inline constexpr const char *kObjc3RuntimeDispatchLoweringAbiContractId =
    "objc3c.runtime.dispatch.lowering.abi.freeze.v1";
inline constexpr const char *kObjc3RuntimeDispatchLoweringAbiBoundaryModel =
    "canonical-runtime-dispatch-default-target";
inline constexpr const char
    *kObjc3RuntimeDispatchLoweringCanonicalEntrypointSymbol =
        "objc3_runtime_dispatch_i32";
inline constexpr const char *kObjc3RuntimeDispatchLoweringSelectorLookupSymbol =
    "objc3_runtime_lookup_selector";
inline constexpr const char *kObjc3RuntimeDispatchLoweringSelectorHandleType =
    "objc3_runtime_selector_handle";
inline constexpr const char *kObjc3RuntimeDispatchLoweringReceiverAbiType =
    "i32";
inline constexpr const char *kObjc3RuntimeDispatchLoweringSelectorAbiType =
    "ptr";
inline constexpr const char *kObjc3RuntimeDispatchLoweringArgumentAbiType =
    "i32";
inline constexpr const char *kObjc3RuntimeDispatchLoweringResultAbiType =
    "i32";
inline constexpr const char *kObjc3RuntimeDispatchLoweringSelectorOperandModel =
    "selector-cstring-pointer-remains-lowered-operand-until-next-runtime-phase";
inline constexpr const char *kObjc3RuntimeDispatchLoweringSelectorHandleModel =
    "runtime-lookup-produces-selector-handle-before-live-dispatch";
inline constexpr const char *kObjc3RuntimeDispatchLoweringArgumentPaddingModel =
    "zero-pad-to-fixed-runtime-arg-slot-count";
inline constexpr const char *kObjc3RuntimeDispatchLoweringDefaultTargetModel =
    "default-lowering-target-is-canonical-runtime-entrypoint";
inline constexpr const char
    *kObjc3RuntimeDispatchLoweringStrictDispatchErrorModel =
        "resolved-runtime-call-or-hard-diagnostic-error-only";
inline constexpr const char *kObjc3RuntimeDispatchLoweringDeferredCasesModel =
    "direct-dispatch-remains-fail-closed-after-live-cutover";
inline constexpr const char *kObjc3RuntimeDispatchLoweringOwnerModel =
    kObjc3LoweringNoFallbackOwnerModel;

inline bool Objc3RuntimeDispatchLoweringOwnerIsReady() {
  return Objc3LoweringStrictOwnerModelIsReady(
      kObjc3RuntimeDispatchLoweringOwner,
      kObjc3RuntimeDispatchLoweringOwnerModel,
      true,
      true);
}

inline std::string Objc3RuntimeDispatchLoweringOwnerReplayKey() {
  return Objc3LoweringOwnerReplayKey(
      kObjc3RuntimeDispatchLoweringOwner,
      kObjc3RuntimeDispatchLoweringOwnerModel,
      true,
      true);
}

inline constexpr const char *kObjc3RuntimeDispatchCallAbiGenerationContractId =
    "objc3c.runtime.call.abi.instance.class.dispatch.v1";
inline constexpr const char
    *kObjc3RuntimeDispatchCallAbiGenerationActiveLoweringModel =
        "instance-class-super-and-dynamic-sends-lower-directly-to-canonical-runtime-entrypoint";
inline constexpr const char
    *kObjc3RuntimeDispatchCallAbiGenerationDeferredLoweringModel =
        "direct-dispatch-remains-fail-closed-until-supported-surface-materializes";
inline constexpr const char *kObjc3RuntimeDispatchSuperNilContractId =
    "objc3c.runtime.call.abi.super.nil.direct.dispatch.v1";
inline constexpr const char
    *kObjc3RuntimeDispatchSuperNilActiveLoweringModel =
        "instance-class-super-and-nil-sends-lower-directly-to-canonical-runtime-entrypoint";
inline constexpr const char
    *kObjc3RuntimeDispatchSuperNilDeferredLoweringModel =
        "direct-dispatch-remains-fail-closed-until-supported-surface-materializes";
inline constexpr const char
    *kObjc3RuntimeDispatchSuperNilUnsupportedModel =
        "direct-dispatch-fails-closed-until-supported-surface-materializes";
inline constexpr const char *kObjc3RuntimeDispatchLiveCutoverContractId =
    "objc3c.runtime.call.abi.live.dispatch.cutover.v1";
inline constexpr const char
    *kObjc3RuntimeDispatchLiveCutoverActiveLoweringModel =
        "all-supported-sends-lower-directly-to-canonical-runtime-entrypoint";
inline constexpr const char
    *kObjc3RuntimeDispatchLiveCutoverStrictDispatchModel =
        "resolved-runtime-call-or-hard-diagnostic-error-only";
inline constexpr const char
    *kObjc3RuntimeDispatchLiveCutoverDefaultTargetModel =
        "default-lowering-target-is-canonical-runtime-entrypoint";
inline constexpr const char
    *kObjc3RuntimeDispatchLiveCutoverDeferredCasesModel =
        "direct-dispatch-remains-fail-closed-after-live-cutover";

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
