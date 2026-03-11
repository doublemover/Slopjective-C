#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>

inline constexpr std::size_t kObjc3RuntimeDispatchDefaultArgs = 4;
inline constexpr std::size_t kObjc3RuntimeDispatchMaxArgs = 16;
inline constexpr const char *kObjc3RuntimeDispatchSymbol =
    "objc3_runtime_dispatch_i32";
inline constexpr const char *kObjc3DispatchSurfaceClassificationContractId =
    "objc3c-dispatch-surface-classification/m255-a001-v1";
inline constexpr const char *kObjc3DispatchSurfaceInstanceFamily = "instance";
inline constexpr const char *kObjc3DispatchSurfaceClassFamily = "class";
inline constexpr const char *kObjc3DispatchSurfaceSuperFamily = "super";
inline constexpr const char *kObjc3DispatchSurfaceDirectFamily = "direct";
inline constexpr const char *kObjc3DispatchSurfaceDynamicFamily = "dynamic";
inline constexpr const char *kObjc3DispatchSurfaceLiveRuntimeEntrypointFamily =
    "objc3_runtime_dispatch_i32-objc3_msgsend_i32-compat";
inline constexpr const char *kObjc3DispatchSurfaceDirectDispatchBinding =
    "reserved-non-goal";
inline constexpr const char *kObjc3DispatchLegalitySelectorResolutionContractId =
    "objc3c-dispatch-legality-selector-resolution/m255-b001-v1";
inline constexpr const char *kObjc3DispatchLegalitySelectorResolutionBoundaryModel =
    "selector-normalized-arity-checked-receiver-required-no-overload";
inline constexpr const char *kObjc3DispatchLegalitySelectorResolutionAmbiguityPolicy =
    "fail-closed-on-unresolved-or-ambiguous-selector-resolution";
inline constexpr const char *kObjc3DispatchLegalitySelectorResolutionSupportedSelectorForms =
    "unary-and-keyword-selectors";
inline constexpr const char *kObjc3SelectorResolutionImplementationContractId =
    "objc3c-selector-resolution-ambiguity/m255-b002-v1";
inline constexpr const char *kObjc3SelectorResolutionConcreteReceiverPolicy =
    "self-super-known-class-receivers-resolve-concretely";
inline constexpr const char *kObjc3SelectorResolutionDynamicFallbackPolicy =
    "non-concrete-receivers-remain-runtime-dynamic";
inline constexpr const char *kObjc3SelectorResolutionOverloadPolicy =
    "no-overload-recovery-exact-signature-or-fail-closed";
inline constexpr const char *kObjc3SelectorResolutionMissingSelectorDiagnostic =
    "O3S216";
inline constexpr const char *kObjc3SelectorResolutionAmbiguousSelectorDiagnostic =
    "O3S217";
inline constexpr const char *kObjc3SuperDynamicMethodFamilyContractId =
    "objc3c-super-dynamic-method-family/m255-b003-v1";
inline constexpr const char *kObjc3SuperDispatchLegalityPolicy =
    "super-requires-enclosing-method-and-real-superclass";
inline constexpr const char *kObjc3DirectDispatchReservationPolicy =
    "direct-dispatch-remains-reserved-non-goal";
inline constexpr const char *kObjc3DynamicDispatchMethodFamilyPolicy =
    "dynamic-dispatch-preserves-runtime-resolution-and-method-family-accounting";
inline constexpr const char *kObjc3RuntimeVisibleMethodFamilyPolicy =
    "super-and-dynamic-sites-preserve-method-family-runtime-visibility";
// M255-C001 dispatch lowering ABI freeze anchor: lane-C freezes the
// compiler-to-runtime dispatch boundary without switching code generation yet.
// The canonical runtime entrypoint is objc3_runtime_dispatch_i32, selector
// handles come from objc3_runtime_lookup_selector, receiver/result ABI stays
// i32, the fixed argument-slot vector stays i32[4] with zero padding, and the
// default lowering target remains the compatibility bridge until M255-C002
// changes it explicitly.
inline constexpr const char *kObjc3RuntimeDispatchLoweringAbiContractId =
    "objc3c-runtime-dispatch-lowering-abi-freeze/m255-c001-v1";
inline constexpr const char *kObjc3RuntimeDispatchLoweringAbiBoundaryModel =
    "compatibility-bridge-default-target-before-live-runtime-dispatch-cutover";
inline constexpr const char
    *kObjc3RuntimeDispatchLoweringCanonicalEntrypointSymbol =
        "objc3_runtime_dispatch_i32";
inline constexpr const char
    *kObjc3RuntimeDispatchLoweringCompatibilityEntrypointSymbol =
        "objc3_msgsend_i32";
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
    "selector-cstring-pointer-remains-lowered-operand-until-m255-c002";
inline constexpr const char *kObjc3RuntimeDispatchLoweringSelectorHandleModel =
    "runtime-lookup-produces-selector-handle-before-live-dispatch";
inline constexpr const char *kObjc3RuntimeDispatchLoweringArgumentPaddingModel =
    "zero-pad-to-fixed-runtime-arg-slot-count";
inline constexpr const char *kObjc3RuntimeDispatchLoweringDefaultTargetModel =
    "default-lowering-target-remains-compatibility-bridge-until-m255-c002";
inline constexpr const char
    *kObjc3RuntimeDispatchLoweringCompatibilityRoleModel =
        "compatibility-bridge-remains-test-and-backcompat-surface-not-canonical-runtime-abi";
inline constexpr const char *kObjc3RuntimeDispatchLoweringDeferredCasesModel =
    "super-nil-direct-runtime-entrypoint-cutover-deferred-until-m255-c003";
// M255-C002 runtime call ABI generation anchor: lane-C now cuts instance/class
// sends over to the canonical runtime entrypoint while preserving the
// compatibility bridge for deferred super/dynamic/direct surfaces until
// M255-C003.
inline constexpr const char *kObjc3RuntimeDispatchCallAbiGenerationContractId =
    "objc3c-runtime-call-abi-instance-class-dispatch/m255-c002-v1";
inline constexpr const char
    *kObjc3RuntimeDispatchCallAbiGenerationActiveLoweringModel =
        "instance-and-class-sends-lower-directly-to-canonical-runtime-entrypoint";
inline constexpr const char
    *kObjc3RuntimeDispatchCallAbiGenerationDeferredLoweringModel =
        "super-dynamic-and-deferred-sends-stay-on-compatibility-bridge-until-m255-c003";
// M255-C003 runtime call ABI generation anchor: lane-C now routes normalized
// super sends and nil-receiver canonical surfaces through the live runtime
// entrypoint, keeps dynamic sends on the compatibility bridge until M255-C004,
// and fails closed if an unsupported reserved direct-dispatch surface reaches
// IR emission.
inline constexpr const char *kObjc3RuntimeDispatchSuperNilContractId =
    "objc3c-runtime-call-abi-super-nil-direct-dispatch/m255-c003-v1";
inline constexpr const char
    *kObjc3RuntimeDispatchSuperNilActiveLoweringModel =
        "instance-class-super-and-nil-sends-lower-directly-to-canonical-runtime-entrypoint";
inline constexpr const char
    *kObjc3RuntimeDispatchSuperNilDeferredLoweringModel =
        "dynamic-sends-stay-on-compatibility-bridge-until-m255-c004";
inline constexpr const char
    *kObjc3RuntimeDispatchSuperNilUnsupportedFallbackModel =
        "direct-dispatch-fails-closed-until-supported-surface-materializes";
// M255-C004 live-dispatch cutover anchor: lane-C removes the last live-path
// compatibility-bridge assumption by routing normalized dynamic sends through
// the canonical runtime entrypoint too, while keeping the compatibility symbol
// exported as test/compat evidence only and leaving direct dispatch fail-closed.
inline constexpr const char *kObjc3RuntimeDispatchLiveCutoverContractId =
    "objc3c-runtime-call-abi-live-dispatch-cutover/m255-c004-v1";
inline constexpr const char
    *kObjc3RuntimeDispatchLiveCutoverActiveLoweringModel =
        "all-supported-sends-lower-directly-to-canonical-runtime-entrypoint";
inline constexpr const char
    *kObjc3RuntimeDispatchLiveCutoverCompatibilityModel =
        "compatibility-dispatch-symbol-remains-exported-but-not-emitted-on-live-path";
inline constexpr const char
    *kObjc3RuntimeDispatchLiveCutoverDefaultTargetModel =
        "default-lowering-target-is-canonical-runtime-entrypoint";
inline constexpr const char
    *kObjc3RuntimeDispatchLiveCutoverDeferredCasesModel =
        "direct-dispatch-remains-fail-closed-after-live-cutover";
// M255-D001 lookup/dispatch runtime freeze anchor: lane-D now freezes the
// runtime-owned selector interning, lookup-table, cache, and slow-path
// boundary that the live lane-C call ABI targets. The canonical runtime API
// remains objc3_runtime_lookup_selector plus objc3_runtime_dispatch_i32;
// metadata-backed selector lookup tables land in M255-D002, method-cache and
// slow-path lookup land in M255-D003, protocol/category-aware method
// resolution lands in M255-D004, and unsupported runtime-resolution surfaces
// remain fail closed until those issues materialize them explicitly.
inline constexpr const char *kObjc3RuntimeLookupDispatchContractId =
    "objc3c-runtime-lookup-dispatch-freeze/m255-d001-v1";
inline constexpr const char
    *kObjc3RuntimeLookupDispatchSelectorInterningModel =
        "process-global-selector-intern-table-stable-id-per-canonical-selector-spelling";
inline constexpr const char
    *kObjc3RuntimeLookupDispatchLookupTableModel =
        "metadata-backed-selector-lookup-tables-deferred-until-m255-d002";
inline constexpr const char *kObjc3RuntimeLookupDispatchCacheModel =
    "method-cache-and-runtime-slow-path-deferred-until-m255-d003";
inline constexpr const char
    *kObjc3RuntimeLookupDispatchProtocolCategoryModel =
        "protocol-and-category-aware-method-resolution-deferred-until-m255-d004";
inline constexpr const char *kObjc3RuntimeLookupDispatchCompatibilityModel =
    "compatibility-shim-remains-test-only-non-authoritative-runtime-surface";
inline constexpr const char *kObjc3RuntimeLookupDispatchFailureModel =
    "runtime-lookup-and-dispatch-fail-closed-on-unmaterialized-resolution";
// M255-D002 selector lookup table anchor: lane-D now materializes the selector
// table from emitted registration-table selector pools while preserving the
// frozen D001 public runtime entrypoints. Method-cache / slow-path resolution
// remains deferred to M255-D003, and unknown selector lookups stay dynamic
// until that path exists.
inline constexpr const char *kObjc3RuntimeSelectorLookupTablesContractId =
    "objc3c-runtime-selector-lookup-tables/m255-d002-v1";
inline constexpr const char
    *kObjc3RuntimeSelectorLookupTablesInterningModel =
        "registered-selector-pools-materialize-process-global-stable-id-table";
inline constexpr const char
    *kObjc3RuntimeSelectorLookupTablesMergeModel =
        "per-image-selector-pools-deduplicated-and-merged-across-registration-order";
inline constexpr const char
    *kObjc3RuntimeSelectorLookupTablesFallbackModel =
        "unknown-selector-lookups-remain-dynamic-until-m255-d003";
inline constexpr const char
    *kObjc3RuntimeSelectorLookupTablesReplayModel =
        "reset-replay-rebuilds-metadata-backed-selector-table-in-registration-order";
inline constexpr const char *kObjc3SelectorGlobalOrdering = "lexicographic";
// M255-D003 method-cache / slow-path anchor: lane-D now turns registered class
// and metaclass metadata into real runtime dispatch targets while preserving
// the frozen D001 public runtime API. Selector tables remain the authoritative
// stable-id source from D002, class-known and class-self receivers normalize
// onto one metaclass lookup family, and unresolved or ambiguous runtime
// lookups still fail closed back to the compatibility arithmetic model until
// later protocol/category-aware resolution lands in M255-D004.
inline constexpr const char *kObjc3RuntimeMethodCacheSlowPathContractId =
    "objc3c-runtime-method-cache-slow-path-lookup/m255-d003-v1";
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
    *kObjc3RuntimeMethodCacheSlowPathFallbackModel =
        "unsupported-or-ambiguous-runtime-resolution-falls-back-to-compatibility-dispatch-formula";
// M255-D004 protocol/category-aware resolution anchor: lane-D extends the live
// runtime slow path to consume emitted category method tables plus adopted and
// inherited protocol metadata while preserving the frozen D001 public runtime
// API and D003 cache/selector-table surfaces.
inline constexpr const char
    *kObjc3RuntimeProtocolCategoryMethodResolutionContractId =
        "objc3c-runtime-protocol-category-method-resolution/m255-d004-v1";
inline constexpr const char
    *kObjc3RuntimeProtocolCategoryMethodResolutionCategoryModel =
        "class-bodies-win-first-category-implementation-records-supply-next-live-method-tier";
inline constexpr const char
    *kObjc3RuntimeProtocolCategoryMethodResolutionProtocolModel =
        "adopted-and-inherited-protocol-method-lists-provide-declaration-aware-negative-resolution";
inline constexpr const char
    *kObjc3RuntimeProtocolCategoryMethodResolutionFallbackModel =
        "conflicting-category-or-protocol-resolution-fails-closed-to-compatibility-dispatch";
// M255-E001 live-dispatch gate anchor: lane-E now freezes one fail-closed
// evidence boundary proving supported message sends execute through the live
// runtime path rather than the compatibility shim. The upstream gate chain
// stays rooted on A002/B003/C004/D004, the compatibility shim remains
// exported only as evidence/test surface, and E002 is the next issue allowed
// to replace shim-based smoke/closeout assumptions with the integrated gate.
inline constexpr const char *kObjc3RuntimeLiveDispatchGateContractId =
    "objc3c-runtime-live-dispatch-gate/m255-e001-v1";
inline constexpr const char *kObjc3RuntimeLiveDispatchGateEvidenceModel =
    "a002-b003-c004-d004-summary-chain";
inline constexpr const char *kObjc3RuntimeLiveDispatchGateShimBoundaryModel =
    "live-runtime-dispatch-required-compatibility-shim-evidence-only";
inline constexpr const char *kObjc3RuntimeLiveDispatchGateFailureModel =
    "fail-closed-on-live-dispatch-evidence-drift";
inline constexpr const char *kObjc3RuntimeLiveDispatchGateNextIssue =
    "M255-E002";
// M255-E002 live-dispatch smoke/replay closeout anchor: lane-E now replaces
// the last shim-era smoke/replay assumptions with canonical live runtime
// dispatch evidence. Execution smoke and replay must publish
// requires_live_runtime_dispatch, the canonical runtime library path, the
// compatibility shim as non-authoritative evidence only, and the canonical
// default symbol objc3_runtime_dispatch_i32.
inline constexpr const char *kObjc3RuntimeLiveDispatchSmokeReplayCloseoutContractId =
    "objc3c-runtime-live-dispatch-smoke-replay-closeout/m255-e002-v1";
inline constexpr const char *kObjc3RuntimeLiveDispatchSmokeReplayCloseoutModel =
    "live-runtime-dispatch-smoke-and-replay-authoritative-shim-evidence-non-authoritative";
// M253-A001 emitted metadata inventory freeze anchor: lowering contracts do
// not own or infer object-file metadata inventory. The emitted inventory
// remains the frontend ABI/scaffold/object-inspection boundary for image-info
// plus class/protocol/category/property/ivar descriptor sections until later
// M253 issues extend it explicitly.
// M253-A002 source-to-section matrix anchor: interface/implementation/
// metaclass/method rows stay explicit no-standalone-emission entries until
// later M253 payload work extends them.
// M253-B001 layout/visibility policy anchor: lowering contracts freeze one
// emitted metadata layout policy without inferring a second model. Image-info
// emits first; descriptor families follow class/protocol/category/property/ivar
// order; descriptor ordinals ascend before the family aggregate; emitted
// metadata remains local-linkage/no-COMDAT; explicit hidden visibility is not
// spelled on IR globals because local linkage already keeps them non-exported;
// llvm.used preserves retention order; and object-format-specific variants stay
// deferred until M253-B003.
inline constexpr const char *kObjc3RuntimeMetadataLayoutOrderingVisibilityPolicyContractId =
    "objc3c-runtime-metadata-layout-ordering-visibility-policy-freeze/m253-b001-v1";
inline constexpr const char *kObjc3RuntimeMetadataLayoutFamilyOrderingModel =
    "image-info-then-class-protocol-category-property-ivar";
inline constexpr const char *kObjc3RuntimeMetadataDescriptorOrderingModel =
    "ascending-descriptor-ordinal-then-family-aggregate";
inline constexpr const char *kObjc3RuntimeMetadataAggregateRelocationPolicy =
    "zero-sentinel-or-count-plus-pointer-vector";
inline constexpr const char *kObjc3RuntimeMetadataComdatPolicy = "disabled";
inline constexpr const char *kObjc3RuntimeMetadataVisibilitySpellingPolicy =
    "local-linkage-omits-explicit-ir-visibility";
inline constexpr const char *kObjc3RuntimeMetadataRetentionOrderingModel =
    "llvm.used-emission-order";
inline constexpr const char *kObjc3RuntimeMetadataObjectFormatPolicyModel =
    "object-format-neutral-until-m253-b003";
// M253-B003 object-format policy expansion anchor: B001/B002 keep the neutral
// model frozen for historical replay, while lowering now also carries the
// explicit COFF/ELF/Mach-O mapping surface for emitted section spellings and
// retention-anchor behavior.
inline constexpr const char
    *kObjc3RuntimeMetadataObjectFormatSurfaceContractId =
        "objc3c-runtime-metadata-object-format-policy/m253-b003-v1";
inline constexpr const char *kObjc3RuntimeMetadataObjectFormatCoff = "coff";
inline constexpr const char *kObjc3RuntimeMetadataObjectFormatElf = "elf";
inline constexpr const char *kObjc3RuntimeMetadataObjectFormatMachO = "mach-o";
inline constexpr const char
    *kObjc3RuntimeMetadataSectionSpellingModelCoff =
        "coff-logical-section-spellings";
inline constexpr const char *kObjc3RuntimeMetadataSectionSpellingModelElf =
    "elf-logical-section-spellings";
inline constexpr const char
    *kObjc3RuntimeMetadataSectionSpellingModelMachO =
        "mach-o-data-segment-comma-section-spellings";
inline constexpr const char *kObjc3RuntimeMetadataRetentionAnchorModelCoff =
    "llvm.used-appending-global+coff-timestamp-normalization";
inline constexpr const char *kObjc3RuntimeMetadataRetentionAnchorModelElf =
    "llvm.used-appending-global+elf-stable-sections";
inline constexpr const char *kObjc3RuntimeMetadataRetentionAnchorModelMachO =
    "llvm.used-appending-global+mach-o-data-segment-sections";
// M253-C001 metadata section emission freeze anchor: native object files now
// carry real metadata sections, but the payload bytes remain scaffold-only
// placeholder shapes until the later lane-C implementation issues land.
inline constexpr const char *kObjc3RuntimeMetadataSectionEmissionContractId =
    "objc3c-runtime-metadata-section-emission-freeze/m253-c001-v1";
inline constexpr const char *kObjc3RuntimeMetadataSectionEmissionPayloadModel =
    "scaffold-placeholder-payloads-until-m253-c002";
inline constexpr const char
    *kObjc3RuntimeMetadataSectionEmissionInventoryModel =
        "image-info-plus-class-protocol-category-property-ivar-sections";
inline constexpr const char
    *kObjc3RuntimeMetadataSectionEmissionDescriptorPayloadModel =
        "private-[1xi8]-zeroinitializer-per-descriptor";
inline constexpr const char
    *kObjc3RuntimeMetadataSectionEmissionAggregatePayloadModel =
        "i64-count-plus-pointer-vector-aggregates";
inline constexpr const char
    *kObjc3RuntimeMetadataSectionEmissionImageInfoPayloadModel =
        "internal-{i32,i32}-zeroinitializer-image-info";
// M253-C002 class/metaclass data emission anchor: lane-C begins replacing the
// class-family placeholder byte model with real class descriptor bundles while
// keeping metaclass payloads inline with their class bundles and deferring real
// method/property/ivar lists plus selector/string pools to later issues.
inline constexpr const char *kObjc3RuntimeClassMetaclassEmissionContractId =
    "objc3c-runtime-class-metaclass-data-emission/m253-c002-v1";
inline constexpr const char *kObjc3RuntimeClassMetaclassEmissionPayloadModel =
    "class-source-record-descriptor-bundles-with-inline-metaclass-records";
inline constexpr const char *kObjc3RuntimeClassMetaclassEmissionNameModel =
    "shared-class-name-cstring-per-bundle";
inline constexpr const char *kObjc3RuntimeClassMetaclassEmissionSuperLinkModel =
    "nullable-super-source-record-bundle-pointer";
inline constexpr const char
    *kObjc3RuntimeClassMetaclassEmissionMethodListReferenceModel =
        "count-plus-owner-identity-pointer-method-list-ref";
// M253-C003 protocol/category data emission anchor: lane-C next replaces the
// protocol/category family placeholder byte models with real descriptor bundles
// while keeping cross-protocol references and category attachments explicit and
// fail-closed without claiming that selector/string pools or standalone
// property/ivar payload sections already exist.
inline constexpr const char *kObjc3RuntimeProtocolCategoryEmissionContractId =
    "objc3c-runtime-protocol-category-data-emission/m253-c003-v1";
inline constexpr const char *kObjc3RuntimeProtocolEmissionPayloadModel =
    "protocol-descriptor-bundles-with-inherited-protocol-ref-lists";
inline constexpr const char *kObjc3RuntimeCategoryEmissionPayloadModel =
    "category-descriptor-bundles-with-attachment-and-protocol-ref-lists";
inline constexpr const char *kObjc3RuntimeProtocolReferenceModel =
    "count-plus-descriptor-pointer-protocol-ref-lists";
inline constexpr const char *kObjc3RuntimeCategoryAttachmentModel =
    "count-plus-owner-identity-pointer-attachment-lists";
// M253-C004 member-table data emission anchor: lane-C now adds real
// owner-scoped method-table payloads plus real property/ivar descriptor bytes
// without reopening the C002/C003 descriptor-family shapes. Class refs keep
// their historical prefix stable while protocol/category descriptor bundles
// remain shape-stable and gain adjacent emitted member-table payloads.
inline constexpr const char *kObjc3RuntimeMemberTableEmissionContractId =
    "objc3c-runtime-member-table-emission/m253-c004-v1";
inline constexpr const char *kObjc3RuntimeMethodListEmissionPayloadModel =
    "owner-scoped-method-table-globals-with-inline-entry-records";
inline constexpr const char *kObjc3RuntimeMethodListEmissionGroupingModel =
    "declaration-owner-plus-class-kind-lexicographic";
inline constexpr const char *kObjc3RuntimePropertyDescriptorEmissionPayloadModel =
    "property-descriptor-records-with-accessor-and-binding-strings";
inline constexpr const char *kObjc3RuntimeIvarDescriptorEmissionPayloadModel =
    "ivar-descriptor-records-with-property-binding-strings";
// M253-C005 selector/string pool expansion anchor: runtime-adjacent selector
// globals now expand into canonical selector and string pool families with
// stable ordinal aggregates, while existing descriptor bundles remain shape
// stable and keep their current inline cstring payloads.
inline constexpr const char *kObjc3RuntimeSelectorStringPoolEmissionContractId =
    "objc3c-runtime-selector-string-pool-emission/m253-c005-v1";
inline constexpr const char *kObjc3RuntimeSelectorPoolEmissionPayloadModel =
    "canonical-selector-cstring-pool-with-stable-ordinal-aggregate";
inline constexpr const char *kObjc3RuntimeStringPoolEmissionPayloadModel =
    "canonical-runtime-string-cstring-pool-with-stable-ordinal-aggregate";
inline constexpr const char *kObjc3RuntimeSelectorPoolLogicalSection =
    "objc3.runtime.selector_pool";
inline constexpr const char *kObjc3RuntimeStringPoolLogicalSection =
    "objc3.runtime.string_pool";
// M256-C001 executable object artifact lowering freeze anchor: lane-C now
// freezes the current binding surface where realized class/category metadata
// records consume owner-scoped method-list refs and implementation-backed
// method entries may point at concrete LLVM definition symbols. Later
// implementation must extend this one executable object surface rather than
// rediscovering bodies or realization edges out-of-band.
inline constexpr const char *kObjc3ExecutableObjectArtifactLoweringContractId =
    "objc3c-executable-object-artifact-lowering/m256-c001-v1";
inline constexpr const char
    *kObjc3ExecutableObjectArtifactLoweringMethodBodyBindingModel =
        "implementation-owner-identity-to-llvm-definition-symbol";
inline constexpr const char
    *kObjc3ExecutableObjectArtifactLoweringRealizationRecordModel =
        "class-metaclass-and-category-descriptor-bundles-point-to-owner-scoped-method-list-ref-records";
inline constexpr const char
    *kObjc3ExecutableObjectArtifactLoweringMethodEntryPayloadModel =
        "selector-owner-return-arity-implementation-symbol-has-body";
inline constexpr const char *kObjc3ExecutableObjectArtifactLoweringScopeModel =
    "parser-source-identities-sema-realization-closure-ir-object-binding";
inline constexpr const char
    *kObjc3ExecutableObjectArtifactLoweringFailClosedModel =
        "no-synthetic-implementation-symbols-no-rebound-legality-no-new-section-families";
// M257-C001 accessor/layout lowering freeze anchor: lane-C now freezes the
// current property/ivar lowering surface where sema-approved property
// descriptor bundles, ivar layout symbols/slots/sizes/alignment, and
// synthesized binding identities are serialized into emitted metadata/object
// artifacts without yet synthesizing accessor bodies or runtime storage.
inline constexpr const char
    *kObjc3ExecutablePropertyAccessorLayoutLoweringContractId =
        "objc3c-executable-property-accessor-layout-lowering/m257-c001-v1";
inline constexpr const char
    *kObjc3ExecutablePropertyAccessorLayoutLoweringPropertyTableModel =
        "property-descriptor-bundles-carry-sema-approved-attribute-accessor-binding-and-layout-records";
inline constexpr const char
    *kObjc3ExecutablePropertyAccessorLayoutLoweringIvarLayoutModel =
        "ivar-descriptor-bundles-carry-sema-approved-layout-symbol-slot-size-alignment-records";
inline constexpr const char
    *kObjc3ExecutablePropertyAccessorLayoutLoweringAccessorBindingModel =
        "effective-accessor-selectors-and-synthesized-binding-identities-pass-through-lowering-without-body-synthesis";
inline constexpr const char
    *kObjc3ExecutablePropertyAccessorLayoutLoweringScopeModel =
        "ast-sema-property-layout-handoff-ir-object-metadata-publication";
inline constexpr const char
    *kObjc3ExecutablePropertyAccessorLayoutLoweringFailClosedModel =
        "no-synthesized-accessor-bodies-no-runtime-storage-allocation-no-layout-rederivation";
// M257-C002 ivar offset/layout emission anchor: lane-C extends the frozen
// accessor/layout handoff into real object payloads by emitting per-ivar
// offset globals, per-owner layout tables, and descriptor records that carry
// the sema-approved slot/offset/size/alignment tuple without yet allocating
// runtime instances or synthesizing accessor bodies.
inline constexpr const char *kObjc3ExecutableIvarLayoutEmissionContractId =
    "objc3c-executable-ivar-layout-emission/m257-c002-v1";
inline constexpr const char *kObjc3ExecutableIvarLayoutDescriptorModel =
    "ivar-descriptor-records-carry-layout-symbol-offset-global-slot-offset-size-alignment";
inline constexpr const char *kObjc3ExecutableIvarOffsetGlobalModel =
    "one-retained-i64-offset-global-per-emitted-ivar-binding";
inline constexpr const char *kObjc3ExecutableIvarLayoutTableModel =
    "declaration-owner-layout-tables-order-ivars-by-slot-and-publish-instance-size";
inline constexpr const char *kObjc3ExecutableIvarLayoutEmissionScopeModel =
    "sema-approved-layout-shape-lowers-into-ivar-section-payloads-without-runtime-allocation";
inline constexpr const char *kObjc3ExecutableIvarLayoutEmissionFailClosedModel =
    "no-runtime-instance-allocation-no-layout-rederivation-no-accessor-body-synthesis";
// M257-C003 synthesized accessor/property lowering anchor: lane-C upgrades the
// frozen property/layout handoff into executable accessor support by
// materializing missing implementation-owned getter/setter method entries,
// emitting deterministic storage globals keyed by synthesized binding symbols,
// and widening property descriptor payloads with effective accessor and layout
// attachment records while still deferring true runtime instance allocation to
// later lane-D work.
inline constexpr const char
    *kObjc3ExecutableSynthesizedAccessorPropertyLoweringContractId =
        "objc3c-executable-synthesized-accessor-property-lowering/m257-c003-v1";
inline constexpr const char
    *kObjc3ExecutableSynthesizedAccessorPropertyLoweringSourceModel =
        "implementation-owned-properties-synthesize-missing-effective-instance-accessors-into-emitted-method-lists";
inline constexpr const char
    *kObjc3ExecutableSynthesizedAccessorPropertyLoweringStorageModel =
        "one-private-i32-storage-global-per-synthesized-binding-symbol-pending-runtime-instance-layout";
inline constexpr const char
    *kObjc3ExecutableSynthesizedAccessorPropertyLoweringPropertyDescriptorModel =
        "property-descriptors-carry-effective-accessor-selectors-binding-symbols-layout-symbols-and-accessor-implementation-pointers";
inline constexpr const char
    *kObjc3ExecutableSynthesizedAccessorPropertyLoweringFailClosedModel =
        "no-missing-effective-accessor-bindings-no-duplicate-synthesized-owner-identities-no-runtime-layout-rederivation";
// M257-D001 runtime property/layout consumption freeze anchor: lane-D now
// freezes the truthful runtime boundary above C003. Runtime consumes emitted
// synthesized accessor implementation pointers plus property/layout attachment
// records through the existing lookup/dispatch ABI, but alloc/new still
// materialize one canonical realized instance identity per class and accessor
// execution still uses the lane-C storage globals until D002 introduces real
// per-instance slot allocation.
inline constexpr const char *kObjc3RuntimePropertyLayoutConsumptionContractId =
    "objc3c-runtime-property-layout-consumption-freeze/m257-d001-v1";
inline constexpr const char
    *kObjc3RuntimePropertyLayoutConsumptionDescriptorModel =
        "runtime-consumes-emitted-property-descriptor-accessor-pointers-binding-symbols-and-layout-identities-without-source-rediscovery";
inline constexpr const char
    *kObjc3RuntimePropertyLayoutConsumptionAllocatorModel =
        "alloc-new-return-one-canonical-realized-instance-identity-per-class-before-true-instance-slot-allocation";
inline constexpr const char
    *kObjc3RuntimePropertyLayoutConsumptionStorageModel =
        "synthesized-accessor-execution-uses-lane-c-storage-globals-pending-runtime-instance-slots";
inline constexpr const char
    *kObjc3RuntimePropertyLayoutConsumptionFailClosedModel =
        "no-layout-rederivation-no-reflective-property-registration-no-per-instance-allocation-yet";
// M257-D002 instance-allocation-layout-runtime anchor: lane-D upgrades the
// frozen D001 runtime-consumption boundary into true per-instance allocation
// backed by realized class layout, emitted ivar offsets, and runtime-owned slot
// storage without reopening source-driven layout recovery.
inline constexpr const char
    *kObjc3RuntimeInstanceAllocationLayoutSupportContractId =
        "objc3c-runtime-instance-allocation-layout-support/m257-d002-v1";
inline constexpr const char
    *kObjc3RuntimeInstanceAllocationLayoutSupportDescriptorModel =
        "runtime-consumes-emitted-property-descriptor-accessor-pointers-binding-symbols-and-layout-identities-without-source-rediscovery";
inline constexpr const char
    *kObjc3RuntimeInstanceAllocationLayoutSupportAllocatorModel =
        "alloc-new-materialize-distinct-runtime-instance-identities-backed-by-realized-class-layout";
inline constexpr const char
    *kObjc3RuntimeInstanceAllocationLayoutSupportStorageModel =
        "synthesized-accessor-execution-reads-and-writes-per-instance-slot-storage-using-emitted-ivar-offset-layout-records";
inline constexpr const char
    *kObjc3RuntimeInstanceAllocationLayoutSupportFailClosedModel =
        "no-layout-rederivation-no-shared-global-property-storage-no-reflective-property-registration-yet";
// M257-D003 property-metadata-reflection anchor: lane-D now freezes the
// private reflective helper surface over the realized property metadata graph
// so diagnostics and tests can query property/accessor/layout facts without
// widening the public runtime ABI or rediscovering metadata from source.
inline constexpr const char
    *kObjc3RuntimePropertyMetadataReflectionContractId =
        "objc3c-runtime-property-metadata-reflection/m257-d003-v1";
inline constexpr const char
    *kObjc3RuntimePropertyMetadataReflectionRegistrationModel =
        "runtime-registers-reflectable-property-accessor-and-layout-facts-from-emitted-metadata-without-source-rediscovery";
inline constexpr const char
    *kObjc3RuntimePropertyMetadataReflectionQueryModel =
        "private-testing-helpers-query-realized-property-metadata-by-class-and-property-name-including-effective-accessors-and-layout-facts";
inline constexpr const char
    *kObjc3RuntimePropertyMetadataReflectionFailClosedModel =
        "no-public-reflection-abi-no-reflective-source-recovery-no-property-query-success-without-realized-runtime-layout";
// M260-A002 runtime-backed object ownership attribute surface anchor: lane-A
// upgrades the frozen ownership boundary into an emitted property/member
// metadata capability by carrying sema-approved property attribute,
// lifetime/runtime-hook, and accessor ownership profiles directly in the
// runtime-facing property descriptor payload rather than leaving them manifest-
// only evidence.
inline constexpr const char
    *kObjc3RuntimeBackedObjectOwnershipAttributeSurfaceContractId =
        "objc3c-runtime-backed-object-ownership-attribute-surface/m260-a002-v1";
inline constexpr const char
    *kObjc3RuntimeBackedObjectOwnershipAttributeSourceModel =
        "runtime-backed-property-source-surface-publishes-attribute-lifetime-hook-and-accessor-ownership-profiles";
inline constexpr const char
    *kObjc3RuntimeBackedObjectOwnershipAttributeDescriptorModel =
        "emitted-property-descriptor-records-carry-attribute-lifetime-hook-and-accessor-ownership-strings";
inline constexpr const char
    *kObjc3RuntimeBackedObjectOwnershipAttributeRuntimeModel =
        "runtime-backed-property-metadata-consumes-emitted-ownership-strings-without-source-rediscovery";
inline constexpr const char
    *kObjc3RuntimeBackedObjectOwnershipAttributeFailClosedModel =
        "no-manifest-only-ownership-proof-no-source-recovery-no-live-arc-hook-emission-yet";
// M260-B001 retainable-object semantic-rule freeze anchor: lane-B freezes the
// truthful semantic boundary around runtime-backed object ownership by making
// it explicit that property/member ownership metadata is now live while
// retain/release legality, autoreleasepool execution, and destruction-order
// behavior remain summary-driven and fail-closed until later M260 issues.
inline constexpr const char
    *kObjc3RetainableObjectSemanticRulesFreezeContractId =
        "objc3c-retainable-object-semantic-rules-freeze/m260-b001-v1";
inline constexpr const char
    *kObjc3RetainableObjectSemanticRulesSemanticModel =
        "runtime-backed-object-semantic-rules-freeze-property-member-ownership-metadata-while-retain-release-and-storage-legality-remain-summary-driven";
inline constexpr const char
    *kObjc3RetainableObjectSemanticRulesDestructionModel =
        "destruction-order-autoreleasepool-and-live-arc-execution-stay-fail-closed-outside-runtime-backed-storage-legality";
inline constexpr const char
    *kObjc3RetainableObjectSemanticRulesFailClosedModel =
        "fail-closed-on-retainable-object-semantic-drift-or-premature-live-storage-legality-claim";
// M260-B002 runtime-backed storage ownership legality anchor: lane-B upgrades
// the frozen B001 boundary into live semantic enforcement for runtime-backed
// object properties by rejecting conflicting explicit ownership
// qualifier/modifier combinations while preserving the truthful owned/weak/
// unowned storage profiles the runtime already consumes.
inline constexpr const char
    *kObjc3RuntimeBackedStorageOwnershipLegalityContractId =
        "objc3c-runtime-backed-storage-ownership-legality/m260-b002-v1";
inline constexpr const char
    *kObjc3RuntimeBackedStorageOwnershipOwnedStorageModel =
        "explicit-strong-object-property-qualifiers-remain-legal-for-owned-runtime-backed-storage-while-conflicting-weak-or-unowned-modifiers-fail-closed";
inline constexpr const char
    *kObjc3RuntimeBackedStorageOwnershipWeakUnownedModel =
        "explicit-weak-and-unsafe-unretained-object-property-qualifiers-bind-runtime-backed-storage-legality-and-reject-conflicting-property-modifiers";
inline constexpr const char
    *kObjc3RuntimeBackedStorageOwnershipFailClosedModel =
        "fail-closed-on-runtime-backed-object-property-ownership-qualifier-modifier-drift";
// M260-B003 autoreleasepool/destruction-order semantic expansion anchor:
// lane-B keeps autoreleasepool scopes fail-closed but now distinguishes the
// ownership-sensitive case where owned runtime-backed object or synthesized
// property storage would require deferred destruction-order semantics.
inline constexpr const char
    *kObjc3RuntimeBackedAutoreleasepoolDestructionOrderContractId =
        "objc3c-runtime-backed-autoreleasepool-destruction-order-semantics/m260-b003-v1";
inline constexpr const char
    *kObjc3RuntimeBackedAutoreleasepoolModel =
        "autoreleasepool-scopes-remain-fail-closed-while-owned-runtime-backed-object-storage-publishes-destruction-order-edge-diagnostics";
inline constexpr const char
    *kObjc3RuntimeBackedDestructionOrderModel =
        "owned-runtime-backed-object-or-synthesized-property-storage-inside-autoreleasepool-requires-deferred-destruction-order-runtime-support";
inline constexpr const char
    *kObjc3RuntimeBackedAutoreleasepoolDestructionOrderFailClosedModel =
        "fail-closed-on-autoreleasepool-destruction-order-semantic-drift-for-owned-runtime-backed-storage";
// M260-C001 ownership-lowering baseline freeze anchor: lane-C freezes the
// current lowering boundary where runtime-backed ownership metadata and sema
// legality are live, but executable retain/release/autorelease/weak behavior
// still remains represented by legacy lowering summaries rather than emitted
// runtime hooks.
inline constexpr const char *kObjc3OwnershipLoweringBaselineContractId =
    "objc3c-ownership-lowering-baseline-freeze/m260-c001-v1";
inline constexpr const char *kObjc3OwnershipLoweringBaselineQualifierModel =
    "ownership-qualifier-lowering-remains-legacy-summary-driven-for-runtime-backed-object-metadata";
inline constexpr const char *kObjc3OwnershipLoweringBaselineRuntimeHookModel =
    "retain-release-autorelease-and-weak-lowering-stays-summary-only-without-live-runtime-hook-emission";
inline constexpr const char *kObjc3OwnershipLoweringBaselineAutoreleasepoolModel =
    "autoreleasepool-lowering-remains-summary-only-without-emitted-push-pop-hooks";
inline constexpr const char *kObjc3OwnershipLoweringBaselineFailClosedModel =
    "no-live-ownership-runtime-hooks-no-arc-weak-side-table-entrypoints-no-destruction-lowering-yet";
// M260-C002 runtime hook emission anchor: lane-C replaces the C001 summary-only
// baseline with emitted runtime helper calls for retain/release/autorelease and
// weak property paths while preserving the existing synthesized accessor
// descriptor/storage artifact surface from M257-C003.
inline constexpr const char *kObjc3OwnershipRuntimeHookEmissionContractId =
    "objc3c-ownership-runtime-hook-emission/m260-c002-v1";
inline constexpr const char *kObjc3OwnershipRuntimeHookEmissionAccessorModel =
    "synthesized-accessors-call-runtime-owned-current-property-and-ownership-hook-entrypoints";
inline constexpr const char *kObjc3OwnershipRuntimeHookEmissionPropertyContextModel =
    "runtime-dispatch-frame-selects-current-receiver-property-accessor-and-autorelease-queue";
inline constexpr const char *kObjc3OwnershipRuntimeHookEmissionAutoreleaseModel =
    "autorelease-values-drain-at-runtime-dispatch-return";
inline constexpr const char *kObjc3OwnershipRuntimeHookEmissionFailClosedModel =
    "owned-and-weak-runtime-backed-accessors-may-not-fall-back-to-summary-only-lowering";
inline constexpr const char *kObjc3RuntimeReadCurrentPropertyI32Symbol =
    "objc3_runtime_read_current_property_i32";
inline constexpr const char *kObjc3RuntimeWriteCurrentPropertyI32Symbol =
    "objc3_runtime_write_current_property_i32";
inline constexpr const char *kObjc3RuntimeExchangeCurrentPropertyI32Symbol =
    "objc3_runtime_exchange_current_property_i32";
inline constexpr const char *kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol =
    "objc3_runtime_load_weak_current_property_i32";
inline constexpr const char *kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol =
    "objc3_runtime_store_weak_current_property_i32";
inline constexpr const char *kObjc3RuntimeRetainI32Symbol =
    "objc3_runtime_retain_i32";
inline constexpr const char *kObjc3RuntimeReleaseI32Symbol =
    "objc3_runtime_release_i32";
inline constexpr const char *kObjc3RuntimeAutoreleaseI32Symbol =
    "objc3_runtime_autorelease_i32";
inline constexpr const char *kObjc3RuntimePromoteBlockI32Symbol =
    "objc3_runtime_promote_block_i32";
inline constexpr const char *kObjc3RuntimeInvokeBlockI32Symbol =
    "objc3_runtime_invoke_block_i32";
inline constexpr const char *kObjc3RuntimeBlockApiObjectLayoutContractId =
    "objc3c-runtime-block-api-object-layout-freeze/m261-d001-v1";
inline constexpr const char
    *kObjc3RuntimeBlockAllocationCopyDisposeInvokeSupportContractId =
        "objc3c-runtime-block-allocation-copy-dispose-invoke-support/m261-d002-v1";
inline constexpr const char
    *kObjc3RuntimeBlockByrefForwardingHeapPromotionInteropContractId =
        "objc3c-runtime-block-byref-forwarding-heap-promotion-interop/m261-d003-v1";
// M261-E001 runnable-block-runtime gate anchor: lane-E now freezes the
// truthful integrated block-runtime boundary above the retained A003/B003/C004
// and D003 evidence chain so later closeout samples cannot overclaim support.
inline constexpr const char *kObjc3RunnableBlockRuntimeGateContractId =
    "objc3c-runnable-block-runtime-gate/m261-e001-v1";
inline constexpr const char *kObjc3RunnableBlockRuntimeGateEvidenceModel =
    "a003-b003-c004-d003-summary-chain";
inline constexpr const char *kObjc3RunnableBlockRuntimeGateActiveModel =
    "runnable-block-gate-consumes-source-sema-lowering-and-runtime-proofs-rather-than-metadata-only-summaries";
inline constexpr const char *kObjc3RunnableBlockRuntimeGateNonGoalModel =
    "no-public-block-object-abi-no-public-runtime-helper-header-no-generalized-foreign-block-interop-no-caller-frame-forwarding-bridge";
inline constexpr const char *kObjc3RunnableBlockRuntimeGateFailClosedModel =
    "fail-closed-on-runnable-block-runtime-evidence-drift";
// M261-E002 runnable-block execution-matrix anchor: lane-E now closes the
// current runnable block tranche with one integrated executable matrix over
// the retained A003/B003/C004/D003/E001 chain without widening the public
// block ABI or helper surface.
inline constexpr const char *kObjc3RunnableBlockExecutionMatrixContractId =
    "objc3c-runnable-block-execution-matrix/m261-e002-v1";
inline constexpr const char *kObjc3RunnableBlockExecutionMatrixEvidenceModel =
    "a003-b003-c004-d003-e001-summary-plus-integrated-native-block-smoke-matrix";
inline constexpr const char *kObjc3RunnableBlockExecutionMatrixActiveModel =
    "closeout-matrix-runs-owned-nonowning-byref-and-escaping-block-fixtures-against-the-native-runtime";
inline constexpr const char *kObjc3RunnableBlockExecutionMatrixNonGoalModel =
    "no-public-block-object-abi-no-public-runtime-helper-header-no-generalized-foreign-block-interop-no-caller-frame-forwarding-bridge";
inline constexpr const char *kObjc3RunnableBlockExecutionMatrixFailClosedModel =
    "fail-closed-on-runnable-block-execution-matrix-drift-or-doc-mismatch";
// M262-A001 ARC source-surface/mode-boundary freeze anchor: the compiler
// already preserves ownership-qualifier, weak/unowned, autoreleasepool, and
// ARC fix-it source surfaces, but there is still no user-visible `-fobjc-arc`
// mode and executable ownership-qualified functions/methods stay fail closed.
inline constexpr const char *kObjc3ArcSourceModeBoundaryContractId =
    "objc3c-arc-source-mode-boundary-freeze/m262-a001-v1";
inline constexpr const char *kObjc3ArcSourceModeBoundarySourceModel =
    "ownership-qualifier-weak-unowned-autoreleasepool-and-arc-fixit-source-surfaces-remain-live-without-enabling-runnable-arc-mode";
inline constexpr const char *kObjc3ArcSourceModeBoundaryModeModel =
    "native-driver-rejects-fobjc-arc-while-executable-ownership-qualified-functions-and-methods-stay-fail-closed";
inline constexpr const char *kObjc3ArcSourceModeBoundaryNonGoalModel =
    "no-fobjc-arc-cli-mode-no-fno-objc-arc-cli-mode-no-automatic-arc-cleanup-insertion-no-user-visible-arc-runtime-mode-split";
inline constexpr const char *kObjc3ArcSourceModeBoundaryFailClosedModel =
    "fail-closed-on-arc-source-mode-boundary-drift-before-arc-automation";
inline constexpr const char *kObjc3ArcModeHandlingContractId =
    "objc3c-arc-mode-handling/m262-a002-v1";
inline constexpr const char *kObjc3ArcModeHandlingSourceModel =
    "ownership-qualified-method-property-return-and-block-capture-surfaces-are-runnable-under-explicit-arc-mode";
inline constexpr const char *kObjc3ArcModeHandlingModeModel =
    "driver-admits-fobjc-arc-and-fno-objc-arc-and-threads-arc-mode-through-frontend-sema-and-ir";
inline constexpr const char *kObjc3ArcModeHandlingFailClosedModel =
    "non-arc-mode-still-rejects-executable-ownership-qualified-method-and-function-signatures";
inline constexpr const char *kObjc3ArcModeHandlingNonGoalModel =
    "no-generalized-arc-cleanup-synthesis-no-implicit-nonarc-promotion-no-full-arc-automation-yet";
inline constexpr const char *kObjc3RuntimePushAutoreleasepoolScopeSymbol =
    "objc3_runtime_push_autoreleasepool_scope";
inline constexpr const char *kObjc3RuntimePopAutoreleasepoolScopeSymbol =
    "objc3_runtime_pop_autoreleasepool_scope";
// M260-D001 runtime memory-management API freeze anchor: the stable public
// runtime ABI remains register/lookup/dispatch only, while lowered
// retain/release/autorelease/current-property/weak helper entrypoints stay on
// the private bootstrap-internal surface until later runtime work decides
// whether any of that memory-management API should become public.
inline constexpr const char *kObjc3RuntimeMemoryManagementApiContractId =
    "objc3c-runtime-memory-management-api-freeze/m260-d001-v1";
inline constexpr const char *kObjc3RuntimeMemoryManagementApiReferenceModel =
    "public-runtime-abi-stays-register-lookup-dispatch-while-reference-counting-helpers-remain-private-runtime-entrypoints";
inline constexpr const char *kObjc3RuntimeMemoryManagementApiWeakModel =
    "weak-storage-remains-served-through-private-runtime-helper-entrypoints-and-runtime-side-tables";
inline constexpr const char *kObjc3RuntimeMemoryManagementApiAutoreleasepoolModel =
    "no-public-autoreleasepool-push-pop-api-yet-autorelease-helper-drains-only-on-dispatch-frame-return";
inline constexpr const char *kObjc3RuntimeMemoryManagementApiFailClosedModel =
    "no-public-memory-management-header-widening-no-user-facing-arc-entrypoints-yet";
// M260-D002 runtime memory-management implementation anchor: runtime-backed
// objects now execute refcount, weak-table, and autoreleasepool semantics
// through private runtime helpers while preserving the frozen D001 public ABI.
inline constexpr const char *kObjc3RuntimeMemoryManagementImplementationContractId =
    "objc3c-runtime-memory-management-implementation/m260-d002-v1";
inline constexpr const char *kObjc3RuntimeMemoryManagementImplementationRefcountModel =
    "runtime-managed-instance-retain-counts-destroy-strong-owned-storage-on-final-release";
inline constexpr const char *kObjc3RuntimeMemoryManagementImplementationWeakModel =
    "weak-side-table-tracks-runtime-storage-observers-and-zeroes-them-on-final-release";
inline constexpr const char *kObjc3RuntimeMemoryManagementImplementationAutoreleasepoolModel =
    "private-autoreleasepool-push-pop-scopes-retain-autoreleased-runtime-values-until-lifo-drain";
inline constexpr const char *kObjc3RuntimeMemoryManagementImplementationFailClosedModel =
    "memory-management-runtime-support-remains-private-lowered-and-runtime-probe-driven";
// M260-E001 ownership-runtime-gate freeze anchor: lane-E now freezes the
// supported ownership runtime baseline and its non-goals so later integration
// issues cannot silently claim ARC-, block-, or public-ABI-level support.
inline constexpr const char *kObjc3OwnershipRuntimeGateContractId =
    "objc3c-ownership-runtime-gate-freeze/m260-e001-v1";
inline constexpr const char *kObjc3OwnershipRuntimeGateSupportedModel =
    "runtime-backed-object-baseline-proves-strong-weak-and-autoreleasepool-behavior-through-private-runtime-hooks";
inline constexpr const char *kObjc3OwnershipRuntimeGateEvidenceModel =
    "gate-consumes-m260-c002-d001-d002-contract-summaries-and-runtime-probe-evidence";
inline constexpr const char *kObjc3OwnershipRuntimeGateNonGoalModel =
    "no-arc-automation-no-block-ownership-runtime-no-public-ownership-api-widening";
inline constexpr const char *kObjc3OwnershipRuntimeGateFailClosedModel =
    "integration-gate-must-not-claim-more-than-the-supported-runtime-backed-ownership-baseline";
// M256-C002 executable method-body binding implementation anchor: lane-C now
// hardens the existing executable object surface so implementation-owned
// method entries must bind to exactly one concrete LLVM definition symbol and
// object emission fails closed when that attachment is missing or ambiguous.
inline constexpr const char *kObjc3ExecutableMethodBodyBindingContractId =
    "objc3c-executable-method-body-binding/m256-c002-v1";
inline constexpr const char *kObjc3ExecutableMethodBodyBindingSourceModel =
    "implementation-owned-method-entry-owner-identity-selects-one-llvm-definition-symbol";
inline constexpr const char *kObjc3ExecutableMethodBodyBindingRuntimeModel =
    "emitted-method-entry-implementation-pointer-dispatches-through-objc3_runtime_dispatch_i32";
inline constexpr const char *kObjc3ExecutableMethodBodyBindingFailClosedModel =
    "error-on-missing-or-duplicate-implementation-binding";
// M256-C003 executable realization-record expansion anchor: lane-C extends the
// executable object surface with realization-ready class, protocol, and
// category records that preserve the frontend-owned owner/super/adoption edges
// directly in emitted artifacts instead of forcing later runtime work to
// recover them out-of-band.
inline constexpr const char *kObjc3ExecutableRealizationRecordsContractId =
    "objc3c-executable-realization-records/m256-c003-v1";
inline constexpr const char *kObjc3ExecutableRealizationClassRecordModel =
    "class-and-metaclass-records-carry-bundle-object-and-super-owner-identities-plus-method-list-refs";
inline constexpr const char *kObjc3ExecutableRealizationProtocolRecordModel =
    "protocol-records-carry-owner-inherited-protocol-edges-and-split-instance-class-method-counts";
inline constexpr const char *kObjc3ExecutableRealizationCategoryRecordModel =
    "category-records-carry-explicit-class-and-category-owner-identities-plus-attachment-and-adopted-protocol-edges";
inline constexpr const char *kObjc3ExecutableRealizationFailClosedModel =
    "no-identity-edge-elision-no-out-of-band-graph-reconstruction";
// M256-D001 class-realization-runtime freeze anchor: lane-D now freezes the
// current runtime-owned class realization surface that consumes emitted
// realization records, walks class/metaclass chains, attaches preferred
// category implementation records, and uses protocol records as
// declaration-aware negative lookup evidence only.
inline constexpr const char *kObjc3RuntimeClassRealizationContractId =
    "objc3c-runtime-class-realization-freeze/m256-d001-v1";
inline constexpr const char *kObjc3RuntimeClassRealizationModel =
    "registered-class-bundles-realize-one-deterministic-class-metaclass-chain-per-class-name";
inline constexpr const char *kObjc3RuntimeMetaclassGraphModel =
    "known-class-and-class-self-receivers-normalize-onto-the-metaclass-record-chain";
inline constexpr const char *kObjc3RuntimeClassRealizationCategoryAttachmentModel =
    "preferred-category-implementation-records-attach-after-class-bundle-resolution";
inline constexpr const char *kObjc3RuntimeProtocolCheckModel =
    "adopted-and-inherited-protocol-method-lists-provide-declaration-aware-negative-runtime-checks";
inline constexpr const char *kObjc3RuntimeClassRealizationFailClosedModel =
    "invalid-bundle-graphs-category-conflicts-and-ambiguous-runtime-resolution-fail-closed";
// M256-D002 metaclass-graph-root-class anchor: lane-D now promotes the frozen
// D001 runtime boundary into a runtime-owned realized class graph keyed by
// stable receiver identities, with explicit root-class publication and
// metaclass-edge inventory available to later runtime lanes.
inline constexpr const char *kObjc3RuntimeMetaclassGraphRootClassContractId =
    "objc3c-runtime-metaclass-graph-root-class-baseline/m256-d002-v1";
inline constexpr const char *kObjc3RuntimeRealizedClassGraphModel =
    "runtime-owned-realized-class-nodes-bind-receiver-base-identities-to-class-and-metaclass-records";
inline constexpr const char *kObjc3RuntimeRootClassBaselineModel =
    "root-classes-realize-with-null-superclass-links-and-live-instance-plus-class-dispatch";
inline constexpr const char *kObjc3RuntimeRealizedClassGraphFailClosedModel =
    "missing-receiver-bindings-or-broken-realized-superclass-links-fall-closed-to-compatibility-dispatch";
inline constexpr const char *kObjc3RuntimeCategoryAttachmentProtocolConformanceContractId =
    "objc3c-runtime-category-attachment-protocol-conformance/m256-d003-v1";
inline constexpr const char *kObjc3RuntimeCategoryAttachmentRealizedGraphModel =
    "realized-class-nodes-own-preferred-category-attachments-after-registration";
inline constexpr const char *kObjc3RuntimeProtocolConformanceQueryModel =
    "runtime-protocol-conformance-queries-walk-class-category-and-inherited-protocol-closures";
inline constexpr const char *kObjc3RuntimeAttachmentConformanceFailClosedModel =
    "invalid-attachment-owner-identities-or-broken-protocol-refs-disable-runtime-attachment-queries";
inline constexpr const char *kObjc3RuntimeCanonicalRunnableObjectSampleSupportContractId =
    "objc3c-runtime-canonical-runnable-object-sample-support/m256-d004-v1";
inline constexpr const char *kObjc3RuntimeCanonicalRunnableObjectExecutionModel =
    "canonical-object-samples-use-runtime-owned-alloc-new-init-and-realized-class-dispatch";
inline constexpr const char *kObjc3RuntimeCanonicalRunnableObjectProbeSplitModel =
    "metadata-rich-object-samples-prove-category-and-protocol-runtime-behavior-through-library-plus-probe-splits";
inline constexpr const char *kObjc3RuntimeCanonicalRunnableObjectFailClosedModel =
    "metadata-heavy-executable-samples-stay-library-probed-until-runtime-export-gates-open";
// M253-C006 binary inspection harness expansion anchor: lane-C now freezes one
// emitted-metadata inspection corpus over llvm-readobj/llvm-objdump so every
// currently emitted metadata section family can be asserted structurally from
// produced objects, including fail-closed negative gating when metadata
// compilation stops before object emission.
inline constexpr const char *kObjc3RuntimeBinaryInspectionHarnessContractId =
    "objc3c-runtime-binary-inspection-harness/m253-c006-v1";
inline constexpr const char *kObjc3RuntimeBinaryInspectionPositiveCorpusModel =
    "positive-structural-section-and-symbol-corpus-with-case-specific-absence-checks";
inline constexpr const char *kObjc3RuntimeBinaryInspectionNegativeCorpusModel =
    "negative-compile-failure-gating-with-no-object-inspection";
inline constexpr const char *kObjc3RuntimeBinaryInspectionSectionCommand =
    "llvm-readobj --sections module.obj";
inline constexpr const char *kObjc3RuntimeBinaryInspectionSymbolCommand =
    "llvm-objdump --syms module.obj";
// M253-D001 object-packaging/retention freeze anchor: lane-D now freezes the
// current produced-object boundary around module.obj plus retained aggregate
// metadata symbols. Later archive/link/startup-registration work must preserve
// these anchors instead of redefining the object boundary ad hoc.
inline constexpr const char *kObjc3RuntimeObjectPackagingRetentionContractId =
    "objc3c-runtime-object-packaging-retention-boundary/m253-d001-v1";
inline constexpr const char *kObjc3RuntimeObjectPackagingRetentionBoundaryModel =
    "current-object-file-boundary-with-retained-metadata-section-aggregates";
inline constexpr const char *kObjc3RuntimeObjectPackagingRetentionAnchorModel =
    "llvm.used-plus-aggregate-section-symbols";
inline constexpr const char *kObjc3RuntimeObjectPackagingRetentionArtifact =
    "module.obj";
inline constexpr const char *kObjc3RuntimeObjectPackagingRetentionSymbolPrefix =
    "__objc3_sec_";
// M253-D002 linker-retention/dead-strip resistance anchor: lane-D now adds one
// public linker anchor plus one public discovery root over the retained
// metadata aggregates. The compiler publishes a driver-friendly linker-response
// file and a discovery JSON sidecar so single-library packaging can survive
// dead stripping without claiming the later multi-archive/TU edge cases.
inline constexpr const char *kObjc3RuntimeLinkerRetentionContractId =
    "objc3c-runtime-linker-retention-and-dead-strip-resistance/m253-d002-v1";
inline constexpr const char *kObjc3RuntimeLinkerRetentionAnchorModel =
    "public-linker-anchor-rooted-in-discovery-table";
inline constexpr const char *kObjc3RuntimeLinkerDiscoveryModel =
    "public-discovery-root-over-retained-metadata-aggregates";
inline constexpr const char *kObjc3RuntimeLinkerResponseArtifactSuffix =
    ".runtime-metadata-linker-options.rsp";
inline constexpr const char *kObjc3RuntimeLinkerDiscoveryArtifactSuffix =
    ".runtime-metadata-discovery.json";
inline constexpr const char *kObjc3RuntimeLinkerAnchorLogicalSection =
    "objc3.runtime.linker_anchor";
inline constexpr const char *kObjc3RuntimeLinkerDiscoveryRootLogicalSection =
    "objc3.runtime.discovery_root";
inline constexpr const char *kObjc3RuntimeLinkerRetentionCoffFlagModel =
    "-Wl,/include:<symbol>";
inline constexpr const char *kObjc3RuntimeLinkerRetentionElfFlagModel =
    "-Wl,--undefined=<symbol>";
inline constexpr const char *kObjc3RuntimeLinkerRetentionMachOFlagModel =
    "-Wl,-u,_<symbol>";
// M253-D003 archive/static-link discovery anchor: lane-D now closes the
// remaining multi-archive fan-in and cross-translation-unit discovery path by
// making linker-anchor identity translation-unit-stable and by standardizing
// one merged discovery/response artifact pair for downstream archive link
// orchestration.
inline constexpr const char *kObjc3RuntimeArchiveStaticLinkDiscoveryContractId =
    "objc3c-runtime-metadata-archive-and-static-link-discovery/m253-d003-v1";
inline constexpr const char *kObjc3RuntimeArchiveStaticLinkAnchorSeedModel =
    "module-and-metadata-replay-plus-translation-unit-identity";
inline constexpr const char *kObjc3RuntimeArchiveStaticLinkTranslationUnitIdentityModel =
    "input-path-plus-parse-and-lowering-replay";
inline constexpr const char *kObjc3RuntimeArchiveStaticLinkMergeModel =
    "deduplicated-driver-flag-fan-in";
inline constexpr const char *kObjc3RuntimeMergedLinkerResponseArtifactSuffix =
    ".merged.runtime-metadata-linker-options.rsp";
inline constexpr const char *kObjc3RuntimeMergedDiscoveryArtifactSuffix =
    ".merged.runtime-metadata-discovery.json";
// M263-C001 constructor-root/init-array lowering freeze anchor: the existing
// bootstrap-lowering surface is now the canonical live lowering contract for
// constructor roots, derived init stubs, registration tables, and
// llvm.global_ctors participation. The registration-descriptor artifact plus
// the emitted registration manifest remain the authoritative lowering inputs
// that later multi-image work must preserve rather than reconstructing symbol
// names ad hoc in IR emission or the driver.
inline constexpr const char *kObjc3RuntimeBootstrapLoweringContractId =
    "objc3c-runtime-constructor-root-init-array-lowering/m263-c001-v1";
inline constexpr const char *kObjc3RuntimeBootstrapLoweringBoundaryModel =
    "registration-descriptor-and-registration-manifest-drive-constructor-root-init-stub-registration-table-and-platform-init-array-lowering";
inline constexpr const char
    *kObjc3RuntimeBootstrapRegistrationDescriptorHandoffContractId =
        "objc3c-runtime-registration-descriptor-frontend-closure/m263-a002-v1";
inline constexpr const char
    *kObjc3RuntimeBootstrapRegistrationDescriptorArtifact =
        "module.runtime-registration-descriptor.json";
inline constexpr const char
    *kObjc3RuntimeBootstrapRegistrationDescriptorHandoffModel =
        "registration-descriptor-artifact-and-registration-manifest-are-authoritative-lowering-inputs";
inline constexpr const char *kObjc3RuntimeBootstrapConstructorRootEmissionState =
    "materialized-before-user-main-via-llvm-global-ctors-single-root";
inline constexpr const char *kObjc3RuntimeBootstrapInitStubEmissionState =
    "materialized-before-user-main-via-derived-init-stub";
inline constexpr const char
    *kObjc3RuntimeBootstrapRegistrationTableEmissionState =
        "materialized-in-native-object-artifact";
inline constexpr const char *kObjc3RuntimeBootstrapGlobalCtorListModel =
    "llvm.global_ctors-single-root-priority-65535";
inline constexpr const char *kObjc3RuntimeBootstrapRegistrationTableSymbolPrefix =
    "__objc3_runtime_registration_table_";
inline constexpr const char *kObjc3RuntimeBootstrapImageLocalInitStateSymbolPrefix =
    "__objc3_runtime_image_local_init_state_";
// M263-C002 registration-descriptor/image-root lowering anchor: lane-C now
// materializes the registration-descriptor and image-root identities as real
// emitted globals in dedicated object sections so later multi-image runtime
// bootstrap can consume binary artifacts instead of only sidecar manifests.
inline constexpr const char
    *kObjc3RuntimeBootstrapRegistrationDescriptorImageRootLoweringContractId =
        "objc3c-runtime-registration-descriptor-and-image-root-lowering/m263-c002-v1";
inline constexpr const char
    *kObjc3RuntimeBootstrapRegistrationDescriptorImageRootLoweringModel =
        "frontend-identifiers-drive-emitted-registration-descriptor-and-image-root-globals";
inline constexpr const char
    *kObjc3RuntimeBootstrapRegistrationDescriptorLogicalSection =
        "objc3.runtime.registration_descriptor";
inline constexpr const char *kObjc3RuntimeBootstrapImageRootLogicalSection =
    "objc3.runtime.image_root";
inline constexpr const char
    *kObjc3RuntimeBootstrapRegistrationDescriptorSymbolPrefix =
        "__objc3_runtime_registration_descriptor_";
inline constexpr const char *kObjc3RuntimeBootstrapImageRootSymbolPrefix =
    "__objc3_runtime_image_root_";
inline constexpr const char
    *kObjc3RuntimeBootstrapRegistrationDescriptorPayloadModel =
        "registration-descriptor-record-points-at-image-root-image-descriptor-registration-table-linker-anchor-and-init-state";
inline constexpr const char
    *kObjc3RuntimeBootstrapImageRootPayloadModel =
        "image-root-record-points-at-module-name-image-descriptor-registration-table-and-discovery-root";
// M263-C003 archive/static-link bootstrap replay corpus anchor: lane-C now
// binds the earlier archive/static-link retention/discovery proof to the live
// bootstrap replay runtime so archive-linked images are validated through one
// retained-binary corpus instead of section-only inspection.
inline constexpr const char
    *kObjc3RuntimeBootstrapArchiveStaticLinkReplayCorpusContractId =
        "objc3c-runtime-bootstrap-archive-static-link-replay-corpus/m263-c003-v1";
inline constexpr const char
    *kObjc3RuntimeBootstrapArchiveStaticLinkReplayCorpusModel =
        "merged-archive-static-link-discovery-artifacts-drive-live-bootstrap-replay-probes";
inline constexpr const char
    *kObjc3RuntimeBootstrapArchiveStaticLinkReplayCorpusBinaryProofModel =
        "plain-link-omits-bootstrap-images-retained-link-replays-them";
inline constexpr const char *kObjc3RuntimeBootstrapRegistrationTableLayoutModel =
    "abi-version-field-count-image-descriptor-discovery-root-linker-anchor-family-aggregates-selector-string-pools-image-local-init-state";
inline constexpr const char *kObjc3RuntimeBootstrapImageLocalInitializationModel =
    "guarded-once-per-image-local-state-cell";
inline constexpr std::uint64_t kObjc3RuntimeBootstrapRegistrationTableAbiVersion =
    1u;
inline constexpr std::uint64_t
    kObjc3RuntimeBootstrapRegistrationTablePointerFieldCount = 11u;
// M253-E001 metadata-emission gate anchor: lane-E now freezes the upstream
// object-emission evidence contract over A002/B003/C006/D003 so later closeout
// work must fail closed if the source-to-section matrix, object-format policy,
// binary inspection corpus, or archive/static-link discovery proof drifts.
inline constexpr const char *kObjc3RuntimeMetadataEmissionGateContractId =
    "objc3c-runtime-metadata-emission-gate/m253-e001-v1";
inline constexpr const char *kObjc3RuntimeMetadataEmissionGateEvidenceModel =
    "a002-b003-c006-d003-summary-chain";
inline constexpr const char *kObjc3RuntimeMetadataEmissionGateFailureModel =
    "fail-closed-on-upstream-summary-drift";
// M253-E002 cross-lane object-emission closeout anchor: lane-E now freezes one
// integrated closeout over the E001 summary chain plus fresh native object
// probes so later startup-registration work can trust the same emitted objects
// on the class/category/message-send paths.
inline constexpr const char *kObjc3RuntimeMetadataObjectEmissionCloseoutContractId =
    "objc3c-runtime-cross-lane-object-emission-closeout/m253-e002-v1";
inline constexpr const char *kObjc3RuntimeMetadataObjectEmissionCloseoutEvidenceModel =
    "e001-summary-plus-integrated-native-object-emission-probes";
inline constexpr const char *kObjc3RuntimeMetadataObjectEmissionCloseoutFailureModel =
    "fail-closed-on-summary-or-integrated-probe-drift";
// M253-B002 normalized layout policy anchor: semantic finalization of runtime
// metadata ordering, visibility, relocation, and retention now flows through
// one lowering-owned normalized policy packet before the IR emitter materializes
// globals. The emitter consumes the normalized plan directly instead of
// hardcoding family order or relocation semantics ad hoc.
inline constexpr const char *kObjc3RuntimeMetadataLayoutPolicyContractId =
    "objc3c-runtime-metadata-layout-policy/m253-b002-v1";
inline constexpr std::size_t kObjc3RuntimeMetadataLayoutPolicyFamilyCount = 5u;
inline constexpr const char *kObjc3RuntimeMetadataLayoutPolicyClassFamily =
    "class";
inline constexpr const char *kObjc3RuntimeMetadataLayoutPolicyProtocolFamily =
    "protocol";
inline constexpr const char *kObjc3RuntimeMetadataLayoutPolicyCategoryFamily =
    "category";
inline constexpr const char *kObjc3RuntimeMetadataLayoutPolicyPropertyFamily =
    "property";
inline constexpr const char *kObjc3RuntimeMetadataLayoutPolicyIvarFamily =
    "ivar";
inline constexpr const char *kObjc3AtomicMemoryOrderRelaxed = "relaxed";
inline constexpr const char *kObjc3AtomicMemoryOrderAcquire = "acquire";
inline constexpr const char *kObjc3AtomicMemoryOrderRelease = "release";
inline constexpr const char *kObjc3AtomicMemoryOrderAcqRel = "acq_rel";
inline constexpr const char *kObjc3AtomicMemoryOrderSeqCst = "seq_cst";
inline constexpr const char *kObjc3SimdVectorLaneContract = "2,4,8,16";
inline constexpr const char *kObjc3SimdVectorBaseI32 = "i32";
inline constexpr const char *kObjc3SimdVectorBaseBool = "bool";
inline constexpr const char *kObjc3MethodLookupOverrideConflictLaneContract =
    "m153-method-lookup-override-conflict-v1";
inline constexpr const char *kObjc3PropertySynthesisIvarBindingLaneContract =
    "m154-property-synthesis-ivar-binding-v1";
inline constexpr const char *kObjc3IdClassSelObjectPointerTypecheckLaneContract =
    "m155-id-class-sel-object-pointer-typecheck-v1";
inline constexpr const char *kObjc3MessageSendSelectorLoweringLaneContract =
    "m156-message-send-selector-lowering-v1";
inline constexpr const char *kObjc3DispatchAbiMarshallingLaneContract =
    "m157-dispatch-abi-marshalling-v1";
inline constexpr const char *kObjc3NilReceiverSemanticsFoldabilityLaneContract =
    "m158-nil-receiver-semantics-foldability-v1";
inline constexpr const char *kObjc3SuperDispatchMethodFamilyLaneContract =
    "m159-super-dispatch-method-family-v1";
inline constexpr const char *kObjc3RuntimeShimHostLinkLaneContract =
    "m160-runtime-shim-host-link-v1";
inline constexpr const char *kObjc3OwnershipQualifierLoweringLaneContract =
    "m161-ownership-qualifier-lowering-v1";
inline constexpr const char *kObjc3RetainReleaseOperationLoweringLaneContract =
    "m162-retain-release-operation-lowering-v1";
inline constexpr const char *kObjc3AutoreleasePoolScopeLoweringLaneContract =
    "m163-autoreleasepool-scope-lowering-v1";
inline constexpr const char *kObjc3WeakUnownedSemanticsLoweringLaneContract =
    "m164-weak-unowned-semantics-lowering-v1";
inline constexpr const char *kObjc3ArcDiagnosticsFixitLoweringLaneContract =
    "m165-arc-diagnostics-fixit-lowering-v1";
inline constexpr const char *kObjc3BlockLiteralCaptureLoweringLaneContract =
    "m166-block-literal-capture-lowering-v1";
inline constexpr const char *kObjc3BlockAbiInvokeTrampolineLoweringLaneContract =
    "m167-block-abi-invoke-trampoline-lowering-v1";
inline constexpr const char *kObjc3BlockStorageEscapeLoweringLaneContract =
    "m168-block-storage-escape-lowering-v1";
inline constexpr const char *kObjc3BlockCopyDisposeLoweringLaneContract =
    "m169-block-copy-dispose-lowering-v1";
inline constexpr const char *kObjc3BlockDeterminismPerfBaselineLoweringLaneContract =
    "m170-block-determinism-perf-baseline-lowering-v1";
inline constexpr const char *kObjc3BlockSourceModelCompletionLaneContract =
    "m261-block-source-model-v1";
inline constexpr const char *kObjc3BlockSourceStorageAnnotationLaneContract =
    "m261-block-source-storage-annotations-v1";
// M261-B002 capture-legality/escape/invocation implementation anchor: lane-B
// keeps the same runtime-summary lane contract while source-only sema grows
// live capture-resolution, truthful escape classification, and local callable
// invocation typing before runnable block lowering lands.
inline constexpr const char *kObjc3BlockRuntimeSemanticRulesLaneContract =
    "m261-block-runtime-semantic-rules-v1";
inline constexpr const char
    *kObjc3ExecutableBlockLoweringAbiArtifactBoundaryLaneContract =
        "m261-block-lowering-abi-artifact-boundary-v1";
inline constexpr const char *kObjc3ExecutableBlockObjectInvokeThunkLoweringLaneContract =
    "m261-block-object-invoke-thunk-lowering-v1";
inline constexpr const char *kObjc3ExecutableBlockByrefHelperLoweringLaneContract =
    "m261-block-byref-helper-lowering-v1";
inline constexpr const char *kObjc3ExecutableBlockEscapeRuntimeHookLoweringLaneContract =
    "m261-block-escape-runtime-hook-lowering-v1";
inline constexpr const char *kObjc3LightweightGenericsConstraintLoweringLaneContract =
    "m171-lightweight-generics-constraint-lowering-v1";
inline constexpr const char *kObjc3NullabilityFlowWarningPrecisionLoweringLaneContract =
    "m172-nullability-flow-warning-precision-lowering-v1";
inline constexpr const char *kObjc3ProtocolQualifiedObjectTypeLoweringLaneContract =
    "m173-protocol-qualified-object-type-lowering-v1";
inline constexpr const char *kObjc3VarianceBridgeCastLoweringLaneContract =
    "m174-variance-bridge-cast-lowering-v1";
inline constexpr const char *kObjc3GenericMetadataAbiLoweringLaneContract =
    "m175-generic-metadata-abi-lowering-v1";
inline constexpr const char *kObjc3ModuleImportGraphLoweringLaneContract =
    "m176-module-import-graph-lowering-v1";
inline constexpr const char *kObjc3NamespaceCollisionShadowingLoweringLaneContract =
    "m177-namespace-collision-shadowing-lowering-v1";
inline constexpr const char *kObjc3PublicPrivateApiPartitionLoweringLaneContract =
    "m178-public-private-api-partition-lowering-v1";
inline constexpr const char *kObjc3IncrementalModuleCacheInvalidationLoweringLaneContract =
    "m179-incremental-module-cache-invalidation-lowering-v1";
inline constexpr const char *kObjc3CrossModuleConformanceLoweringLaneContract =
    "m180-cross-module-conformance-lowering-v1";
inline constexpr const char *kObjc3ThrowsPropagationLoweringLaneContract =
    "m181-throws-propagation-lowering-v1";
inline constexpr const char *kObjc3ResultLikeLoweringLaneContract =
    "m182-result-like-lowering-v1";
inline constexpr const char *kObjc3NSErrorBridgingLoweringLaneContract =
    "m183-ns-error-bridging-lowering-v1";
inline constexpr const char *kObjc3UnwindCleanupLoweringLaneContract =
    "m184-unwind-cleanup-lowering-v1";
inline constexpr const char *kObjc3ErrorDiagnosticsRecoveryLoweringLaneContract =
    "m185-error-diagnostics-recovery-lowering-v1";
inline constexpr const char *kObjc3AsyncContinuationLoweringLaneContract =
    "m186-async-continuation-lowering-v1";
inline constexpr const char
    *kObjc3AwaitLoweringSuspensionStateLoweringLaneContract =
        "m187-await-lowering-suspension-state-lowering-v1";
inline constexpr const char *kObjc3ActorIsolationSendabilityLoweringLaneContract =
    "m188-actor-isolation-sendability-lowering-v1";
inline constexpr const char *kObjc3TaskRuntimeInteropCancellationLoweringLaneContract =
    "m189-task-runtime-interop-cancellation-lowering-v1";
inline constexpr const char *kObjc3ConcurrencyReplayRaceGuardLoweringLaneContract =
    "m190-concurrency-replay-race-guard-lowering-v1";
inline constexpr const char *kObjc3UnsafePointerExtensionLoweringLaneContract =
    "m191-unsafe-pointer-extension-gating-lowering-v1";
inline constexpr const char *kObjc3InlineAsmIntrinsicGovernanceLoweringLaneContract =
    "m192-inline-asm-intrinsic-governance-lowering-v1";

enum class Objc3AtomicMemoryOrder : std::uint8_t {
  Relaxed = 0,
  Acquire = 1,
  Release = 2,
  AcqRel = 3,
  SeqCst = 4,
};

struct Objc3LoweringContract {
  std::size_t max_message_send_args = kObjc3RuntimeDispatchDefaultArgs;
  std::string runtime_dispatch_symbol = kObjc3RuntimeDispatchSymbol;
};

struct Objc3LoweringIRBoundary {
  std::size_t runtime_dispatch_arg_slots = kObjc3RuntimeDispatchDefaultArgs;
  std::string runtime_dispatch_symbol = kObjc3RuntimeDispatchSymbol;
  std::string selector_global_ordering = kObjc3SelectorGlobalOrdering;
};

struct Objc3RuntimeMetadataLayoutPolicyFamilyInput {
  std::string kind;
  std::string section_name;
  std::string aggregate_symbol_name;
  std::size_t descriptor_count = 0;
};

struct Objc3RuntimeMetadataLayoutPolicyInput {
  std::string abi_contract_id;
  std::string scaffold_contract_id;
  bool section_boundary_ready = false;
  bool runtime_export_ready = false;
  bool scaffold_emitted = false;
  bool scaffold_fail_closed = false;
  bool uses_llvm_used = false;
  bool image_info_emitted = false;
  std::string image_info_symbol;
  std::string image_info_section;
  std::string descriptor_symbol_prefix;
  std::string descriptor_linkage;
  std::string aggregate_linkage;
  std::string metadata_visibility;
  std::string retention_root;
  std::size_t total_retained_global_count = 0;
  std::array<Objc3RuntimeMetadataLayoutPolicyFamilyInput,
             kObjc3RuntimeMetadataLayoutPolicyFamilyCount>
      families;
};

struct Objc3RuntimeMetadataLayoutPolicyFamily {
  std::string kind;
  std::string logical_section_name;
  std::string emitted_section_name;
  std::string aggregate_symbol_name;
  std::size_t descriptor_count = 0;
};

struct Objc3RuntimeMetadataLayoutPolicy {
  std::string contract_id = kObjc3RuntimeMetadataLayoutPolicyContractId;
  std::string abi_contract_id;
  std::string scaffold_contract_id;
  std::string family_ordering_model =
      kObjc3RuntimeMetadataLayoutFamilyOrderingModel;
  std::string descriptor_ordering_model =
      kObjc3RuntimeMetadataDescriptorOrderingModel;
  std::string aggregate_relocation_policy =
      kObjc3RuntimeMetadataAggregateRelocationPolicy;
  std::string comdat_policy = kObjc3RuntimeMetadataComdatPolicy;
  std::string visibility_spelling_policy =
      kObjc3RuntimeMetadataVisibilitySpellingPolicy;
  std::string retention_ordering_model =
      kObjc3RuntimeMetadataRetentionOrderingModel;
  std::string object_format_policy_model =
      kObjc3RuntimeMetadataObjectFormatPolicyModel;
  std::string object_format_surface_contract_id =
      kObjc3RuntimeMetadataObjectFormatSurfaceContractId;
  std::string object_format;
  std::string section_spelling_model;
  std::string retention_anchor_model;
  std::string image_info_symbol;
  std::string logical_image_info_section;
  std::string emitted_image_info_section;
  std::string descriptor_symbol_prefix;
  std::string descriptor_linkage;
  std::string aggregate_linkage;
  std::string metadata_visibility;
  std::string retention_root;
  std::size_t total_retained_global_count = 0;
  bool ready = false;
  bool fail_closed = false;
  std::array<Objc3RuntimeMetadataLayoutPolicyFamily,
             kObjc3RuntimeMetadataLayoutPolicyFamilyCount>
      families;
  std::string failure_reason;
};

struct Objc3MethodLookupOverrideConflictContract {
  std::size_t method_lookup_sites = 0;
  std::size_t method_lookup_hits = 0;
  std::size_t method_lookup_misses = 0;
  std::size_t override_lookup_sites = 0;
  std::size_t override_lookup_hits = 0;
  std::size_t override_lookup_misses = 0;
  std::size_t override_conflicts = 0;
  std::size_t unresolved_base_interfaces = 0;
  bool deterministic = true;
};

struct Objc3PropertySynthesisIvarBindingContract {
  std::size_t property_synthesis_sites = 0;
  std::size_t property_synthesis_explicit_ivar_bindings = 0;
  std::size_t property_synthesis_default_ivar_bindings = 0;
  std::size_t ivar_binding_sites = 0;
  std::size_t ivar_binding_resolved = 0;
  std::size_t ivar_binding_missing = 0;
  std::size_t ivar_binding_conflicts = 0;
  bool deterministic = true;
};

struct Objc3IdClassSelObjectPointerTypecheckContract {
  std::size_t id_typecheck_sites = 0;
  std::size_t class_typecheck_sites = 0;
  std::size_t sel_typecheck_sites = 0;
  std::size_t object_pointer_typecheck_sites = 0;
  std::size_t total_typecheck_sites = 0;
  bool deterministic = true;
};

struct Objc3DispatchSurfaceClassificationContract {
  std::size_t instance_dispatch_sites = 0;
  std::size_t class_dispatch_sites = 0;
  std::size_t super_dispatch_sites = 0;
  std::size_t direct_dispatch_sites = 0;
  std::size_t dynamic_dispatch_sites = 0;
  std::string instance_entrypoint_family = kObjc3DispatchSurfaceLiveRuntimeEntrypointFamily;
  std::string class_entrypoint_family = kObjc3DispatchSurfaceLiveRuntimeEntrypointFamily;
  std::string super_entrypoint_family = kObjc3DispatchSurfaceLiveRuntimeEntrypointFamily;
  std::string direct_entrypoint_family = kObjc3DispatchSurfaceDirectDispatchBinding;
  std::string dynamic_entrypoint_family = kObjc3DispatchSurfaceLiveRuntimeEntrypointFamily;
  bool deterministic = true;
};

struct Objc3MessageSendSelectorLoweringContract {
  std::size_t message_send_sites = 0;
  std::size_t unary_selector_sites = 0;
  std::size_t keyword_selector_sites = 0;
  std::size_t selector_piece_sites = 0;
  std::size_t argument_expression_sites = 0;
  std::size_t receiver_expression_sites = 0;
  std::size_t selector_literal_entries = 0;
  std::size_t selector_literal_characters = 0;
  bool deterministic = true;
};

struct Objc3DispatchAbiMarshallingContract {
  std::size_t message_send_sites = 0;
  std::size_t receiver_slots_marshaled = 0;
  std::size_t selector_slots_marshaled = 0;
  std::size_t argument_value_slots_marshaled = 0;
  std::size_t argument_padding_slots_marshaled = 0;
  std::size_t argument_total_slots_marshaled = 0;
  std::size_t total_marshaled_slots = 0;
  std::size_t runtime_dispatch_arg_slots = kObjc3RuntimeDispatchDefaultArgs;
  bool deterministic = true;
};

struct Objc3NilReceiverSemanticsFoldabilityContract {
  std::size_t message_send_sites = 0;
  std::size_t receiver_nil_literal_sites = 0;
  std::size_t nil_receiver_semantics_enabled_sites = 0;
  std::size_t nil_receiver_foldable_sites = 0;
  std::size_t nil_receiver_runtime_dispatch_required_sites = 0;
  std::size_t non_nil_receiver_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3SuperDispatchMethodFamilyContract {
  std::size_t message_send_sites = 0;
  std::size_t receiver_super_identifier_sites = 0;
  std::size_t super_dispatch_enabled_sites = 0;
  std::size_t super_dispatch_requires_class_context_sites = 0;
  std::size_t method_family_init_sites = 0;
  std::size_t method_family_copy_sites = 0;
  std::size_t method_family_mutable_copy_sites = 0;
  std::size_t method_family_new_sites = 0;
  std::size_t method_family_none_sites = 0;
  std::size_t method_family_returns_retained_result_sites = 0;
  std::size_t method_family_returns_related_result_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3RuntimeShimHostLinkContract {
  std::size_t message_send_sites = 0;
  std::size_t runtime_shim_required_sites = 0;
  std::size_t runtime_shim_elided_sites = 0;
  std::size_t runtime_dispatch_arg_slots = kObjc3RuntimeDispatchDefaultArgs;
  std::size_t runtime_dispatch_declaration_parameter_count = 0;
  std::size_t contract_violation_sites = 0;
  std::string runtime_dispatch_symbol = kObjc3RuntimeDispatchSymbol;
  bool default_runtime_dispatch_symbol_binding = true;
  bool deterministic = true;
};

struct Objc3RuntimeDispatchLoweringAbiContract {
  std::size_t message_send_sites = 0;
  std::size_t fixed_argument_slot_count = kObjc3RuntimeDispatchDefaultArgs;
  std::size_t runtime_dispatch_parameter_count = 0;
  std::string lowering_boundary_model =
      kObjc3RuntimeDispatchLoweringAbiBoundaryModel;
  std::string canonical_runtime_dispatch_symbol =
      kObjc3RuntimeDispatchLoweringCanonicalEntrypointSymbol;
  std::string compatibility_runtime_dispatch_symbol =
      kObjc3RuntimeDispatchLoweringCompatibilityEntrypointSymbol;
  std::string default_lowering_target_symbol = kObjc3RuntimeDispatchSymbol;
  std::string selector_lookup_symbol =
      kObjc3RuntimeDispatchLoweringSelectorLookupSymbol;
  std::string selector_handle_type =
      kObjc3RuntimeDispatchLoweringSelectorHandleType;
  std::string receiver_abi_type = kObjc3RuntimeDispatchLoweringReceiverAbiType;
  std::string selector_abi_type = kObjc3RuntimeDispatchLoweringSelectorAbiType;
  std::string argument_abi_type = kObjc3RuntimeDispatchLoweringArgumentAbiType;
  std::string result_abi_type = kObjc3RuntimeDispatchLoweringResultAbiType;
  std::string selector_operand_model =
      kObjc3RuntimeDispatchLoweringSelectorOperandModel;
  std::string selector_handle_model =
      kObjc3RuntimeDispatchLoweringSelectorHandleModel;
  std::string argument_padding_model =
      kObjc3RuntimeDispatchLoweringArgumentPaddingModel;
  std::string default_lowering_target_model =
      kObjc3RuntimeDispatchLoweringDefaultTargetModel;
  std::string compatibility_bridge_role_model =
      kObjc3RuntimeDispatchLoweringCompatibilityRoleModel;
  std::string deferred_cases_model =
      kObjc3RuntimeDispatchLoweringDeferredCasesModel;
  bool fail_closed = true;
  bool deterministic = true;
};

struct Objc3OwnershipQualifierLoweringContract {
  std::size_t ownership_qualifier_sites = 0;
  std::size_t invalid_ownership_qualifier_sites = 0;
  std::size_t object_pointer_type_annotation_sites = 0;
  bool deterministic = true;
};

struct Objc3RetainReleaseOperationLoweringContract {
  std::size_t ownership_qualified_sites = 0;
  std::size_t retain_insertion_sites = 0;
  std::size_t release_insertion_sites = 0;
  std::size_t autorelease_insertion_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3AutoreleasePoolScopeLoweringContract {
  std::size_t scope_sites = 0;
  std::size_t scope_symbolized_sites = 0;
  unsigned max_scope_depth = 0;
  std::size_t scope_entry_transition_sites = 0;
  std::size_t scope_exit_transition_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3WeakUnownedSemanticsLoweringContract {
  std::size_t ownership_candidate_sites = 0;
  std::size_t weak_reference_sites = 0;
  std::size_t unowned_reference_sites = 0;
  std::size_t unowned_safe_reference_sites = 0;
  std::size_t weak_unowned_conflict_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ArcDiagnosticsFixitLoweringContract {
  std::size_t ownership_arc_diagnostic_candidate_sites = 0;
  std::size_t ownership_arc_fixit_available_sites = 0;
  std::size_t ownership_arc_profiled_sites = 0;
  std::size_t ownership_arc_weak_unowned_conflict_diagnostic_sites = 0;
  std::size_t ownership_arc_empty_fixit_hint_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3BlockLiteralCaptureLoweringContract {
  std::size_t block_literal_sites = 0;
  std::size_t block_parameter_entries = 0;
  std::size_t block_capture_entries = 0;
  std::size_t block_body_statement_entries = 0;
  std::size_t block_empty_capture_sites = 0;
  std::size_t block_nondeterministic_capture_sites = 0;
  std::size_t block_non_normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3BlockSourceModelCompletionContract {
  std::size_t block_literal_sites = 0;
  std::size_t signature_entries_total = 0;
  std::size_t explicit_typed_parameter_entries_total = 0;
  std::size_t implicit_parameter_entries_total = 0;
  std::size_t capture_inventory_entries_total = 0;
  std::size_t byvalue_readonly_capture_entries_total = 0;
  std::size_t invoke_surface_entries_total = 0;
  std::size_t non_normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3BlockSourceStorageAnnotationContract {
  std::size_t block_literal_sites = 0;
  std::size_t capture_entries_total = 0;
  std::size_t mutated_capture_entries_total = 0;
  std::size_t byref_capture_entries_total = 0;
  std::size_t copy_helper_intent_sites = 0;
  std::size_t dispose_helper_intent_sites = 0;
  std::size_t heap_candidate_sites = 0;
  std::size_t expression_sites = 0;
  std::size_t global_initializer_sites = 0;
  std::size_t binding_initializer_sites = 0;
  std::size_t assignment_value_sites = 0;
  std::size_t return_value_sites = 0;
  std::size_t call_argument_sites = 0;
  std::size_t message_argument_sites = 0;
  std::size_t non_normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3BlockAbiInvokeTrampolineLoweringContract {
  std::size_t block_literal_sites = 0;
  std::size_t invoke_argument_slots_total = 0;
  std::size_t capture_word_count_total = 0;
  std::size_t parameter_entries_total = 0;
  std::size_t capture_entries_total = 0;
  std::size_t body_statement_entries_total = 0;
  std::size_t descriptor_symbolized_sites = 0;
  std::size_t invoke_trampoline_symbolized_sites = 0;
  std::size_t missing_invoke_trampoline_sites = 0;
  std::size_t non_normalized_layout_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3BlockStorageEscapeLoweringContract {
  std::size_t block_literal_sites = 0;
  std::size_t mutable_capture_count_total = 0;
  std::size_t byref_slot_count_total = 0;
  std::size_t parameter_entries_total = 0;
  std::size_t capture_entries_total = 0;
  std::size_t body_statement_entries_total = 0;
  std::size_t requires_byref_cells_sites = 0;
  std::size_t escape_analysis_enabled_sites = 0;
  std::size_t escape_to_heap_sites = 0;
  std::size_t escape_profile_normalized_sites = 0;
  std::size_t byref_layout_symbolized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3BlockCopyDisposeLoweringContract {
  std::size_t block_literal_sites = 0;
  std::size_t mutable_capture_count_total = 0;
  std::size_t byref_slot_count_total = 0;
  std::size_t parameter_entries_total = 0;
  std::size_t capture_entries_total = 0;
  std::size_t body_statement_entries_total = 0;
  std::size_t copy_helper_required_sites = 0;
  std::size_t dispose_helper_required_sites = 0;
  std::size_t profile_normalized_sites = 0;
  std::size_t copy_helper_symbolized_sites = 0;
  std::size_t dispose_helper_symbolized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3BlockDeterminismPerfBaselineLoweringContract {
  std::size_t block_literal_sites = 0;
  std::size_t baseline_weight_total = 0;
  std::size_t parameter_entries_total = 0;
  std::size_t capture_entries_total = 0;
  std::size_t body_statement_entries_total = 0;
  std::size_t deterministic_capture_sites = 0;
  std::size_t heavy_tier_sites = 0;
  std::size_t normalized_profile_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3LightweightGenericsConstraintLoweringContract {
  std::size_t generic_constraint_sites = 0;
  std::size_t generic_suffix_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t terminated_generic_suffix_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_constraint_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3NullabilityFlowWarningPrecisionLoweringContract {
  std::size_t nullability_flow_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t nullability_suffix_sites = 0;
  std::size_t nullable_suffix_sites = 0;
  std::size_t nonnull_suffix_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ProtocolQualifiedObjectTypeLoweringContract {
  std::size_t protocol_qualified_object_type_sites = 0;
  std::size_t protocol_composition_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t terminated_protocol_composition_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_protocol_composition_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3VarianceBridgeCastLoweringContract {
  std::size_t variance_bridge_cast_sites = 0;
  std::size_t protocol_composition_sites = 0;
  std::size_t ownership_qualifier_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3GenericMetadataAbiLoweringContract {
  std::size_t generic_metadata_abi_sites = 0;
  std::size_t generic_suffix_sites = 0;
  std::size_t protocol_composition_sites = 0;
  std::size_t ownership_qualifier_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ModuleImportGraphLoweringContract {
  std::size_t module_import_graph_sites = 0;
  std::size_t import_edge_candidate_sites = 0;
  std::size_t namespace_segment_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3NamespaceCollisionShadowingLoweringContract {
  std::size_t namespace_collision_shadowing_sites = 0;
  std::size_t namespace_segment_sites = 0;
  std::size_t import_edge_candidate_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3PublicPrivateApiPartitionLoweringContract {
  std::size_t public_private_api_partition_sites = 0;
  std::size_t namespace_segment_sites = 0;
  std::size_t import_edge_candidate_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3IncrementalModuleCacheInvalidationLoweringContract {
  std::size_t incremental_module_cache_invalidation_sites = 0;
  std::size_t namespace_segment_sites = 0;
  std::size_t import_edge_candidate_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t cache_invalidation_candidate_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3CrossModuleConformanceLoweringContract {
  std::size_t cross_module_conformance_sites = 0;
  std::size_t namespace_segment_sites = 0;
  std::size_t import_edge_candidate_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t cache_invalidation_candidate_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ThrowsPropagationLoweringContract {
  std::size_t throws_propagation_sites = 0;
  std::size_t namespace_segment_sites = 0;
  std::size_t import_edge_candidate_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t cache_invalidation_candidate_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ResultLikeLoweringContract {
  std::size_t result_like_sites = 0;
  std::size_t result_success_sites = 0;
  std::size_t result_failure_sites = 0;
  std::size_t result_branch_sites = 0;
  std::size_t result_payload_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t branch_merge_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3NSErrorBridgingLoweringContract {
  std::size_t ns_error_bridging_sites = 0;
  std::size_t ns_error_parameter_sites = 0;
  std::size_t ns_error_out_parameter_sites = 0;
  std::size_t ns_error_bridge_path_sites = 0;
  std::size_t failable_call_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t bridge_boundary_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3UnwindCleanupLoweringContract {
  std::size_t unwind_cleanup_sites = 0;
  std::size_t unwind_edge_sites = 0;
  std::size_t cleanup_scope_sites = 0;
  std::size_t cleanup_emit_sites = 0;
  std::size_t landing_pad_sites = 0;
  std::size_t cleanup_resume_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ErrorDiagnosticsRecoveryLoweringContract {
  std::size_t error_diagnostic_sites = 0;
  std::size_t parser_diagnostic_sites = 0;
  std::size_t semantic_diagnostic_sites = 0;
  std::size_t fixit_hint_sites = 0;
  std::size_t recovery_candidate_sites = 0;
  std::size_t recovery_applied_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3AsyncContinuationLoweringContract {
  std::size_t async_continuation_sites = 0;
  std::size_t async_keyword_sites = 0;
  std::size_t async_function_sites = 0;
  std::size_t continuation_allocation_sites = 0;
  std::size_t continuation_resume_sites = 0;
  std::size_t continuation_suspend_sites = 0;
  std::size_t async_state_machine_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t gate_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3AwaitLoweringSuspensionStateLoweringContract {
  std::size_t await_suspension_sites = 0;
  std::size_t await_keyword_sites = 0;
  std::size_t await_suspension_point_sites = 0;
  std::size_t await_resume_sites = 0;
  std::size_t await_state_machine_sites = 0;
  std::size_t await_continuation_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t gate_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ActorIsolationSendabilityLoweringContract {
  std::size_t actor_isolation_sites = 0;
  std::size_t sendability_check_sites = 0;
  std::size_t cross_actor_hop_sites = 0;
  std::size_t non_sendable_capture_sites = 0;
  std::size_t sendable_transfer_sites = 0;
  std::size_t isolation_boundary_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3TaskRuntimeInteropCancellationLoweringContract {
  std::size_t task_runtime_sites = 0;
  std::size_t task_runtime_interop_sites = 0;
  std::size_t cancellation_probe_sites = 0;
  std::size_t cancellation_handler_sites = 0;
  std::size_t runtime_resume_sites = 0;
  std::size_t runtime_cancel_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ConcurrencyReplayRaceGuardLoweringContract {
  std::size_t concurrency_replay_sites = 0;
  std::size_t replay_proof_sites = 0;
  std::size_t race_guard_sites = 0;
  std::size_t task_handoff_sites = 0;
  std::size_t actor_isolation_sites = 0;
  std::size_t deterministic_schedule_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3UnsafePointerExtensionLoweringContract {
  std::size_t unsafe_pointer_extension_sites = 0;
  std::size_t unsafe_keyword_sites = 0;
  std::size_t pointer_arithmetic_sites = 0;
  std::size_t raw_pointer_type_sites = 0;
  std::size_t unsafe_operation_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t gate_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3InlineAsmIntrinsicGovernanceLoweringContract {
  std::size_t inline_asm_intrinsic_sites = 0;
  std::size_t inline_asm_sites = 0;
  std::size_t intrinsic_sites = 0;
  std::size_t governed_intrinsic_sites = 0;
  std::size_t privileged_intrinsic_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t gate_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

bool IsValidRuntimeDispatchSymbol(const std::string &symbol);
bool TryNormalizeObjc3LoweringContract(const Objc3LoweringContract &input,
                                       Objc3LoweringContract &normalized,
                                       std::string &error);
bool TryBuildObjc3LoweringIRBoundary(const Objc3LoweringContract &input,
                                     Objc3LoweringIRBoundary &boundary,
                                     std::string &error);
std::string Objc3LoweringIRBoundaryReplayKey(const Objc3LoweringIRBoundary &boundary);
bool UsesCanonicalObjc3RuntimeDispatchEntrypoint(
    const std::string &dispatch_surface_family);
bool RequiresFailClosedObjc3RuntimeDispatchFallback(
    const std::string &dispatch_surface_family);
const char *Objc3DispatchSurfaceRuntimeEntrypointSymbol(
    const std::string &dispatch_surface_family);
std::string Objc3RuntimeDispatchDeclarationReplayKey(const Objc3LoweringIRBoundary &boundary);
bool TryBuildObjc3RuntimeMetadataLayoutPolicy(
    const Objc3RuntimeMetadataLayoutPolicyInput &input,
    Objc3RuntimeMetadataLayoutPolicy &policy, std::string &error);
bool IsReadyObjc3RuntimeMetadataLayoutPolicy(
    const Objc3RuntimeMetadataLayoutPolicy &policy);
std::string Objc3RuntimeMetadataLayoutPolicyReplayKey(
    const Objc3RuntimeMetadataLayoutPolicy &policy);
std::string Objc3RuntimeMetadataSectionEmissionBoundarySummary();
std::string Objc3RuntimeMetadataClassMetaclassEmissionSummary();
std::string Objc3RuntimeMetadataProtocolCategoryEmissionSummary();
std::string Objc3RuntimeMetadataMemberTableEmissionSummary();
std::string Objc3RuntimeMetadataSelectorStringPoolEmissionSummary();
std::string Objc3ExecutableObjectArtifactLoweringSummary();
std::string Objc3ExecutablePropertyAccessorLayoutLoweringSummary();
std::string Objc3ExecutableIvarLayoutEmissionSummary();
std::string Objc3ExecutableSynthesizedAccessorPropertyLoweringSummary();
std::string Objc3RuntimePropertyLayoutConsumptionSummary();
std::string Objc3RuntimeInstanceAllocationLayoutSupportSummary();
std::string Objc3RuntimePropertyMetadataReflectionSummary();
std::string Objc3RuntimeBackedObjectOwnershipAttributeSurfaceSummary();
std::string Objc3RetainableObjectSemanticRulesFreezeSummary();
std::string Objc3RuntimeBackedStorageOwnershipLegalitySummary();
std::string Objc3RuntimeBackedAutoreleasepoolDestructionOrderSummary();
std::string Objc3OwnershipLoweringBaselineSummary();
std::string Objc3OwnershipRuntimeHookEmissionSummary();
std::string Objc3RuntimeMemoryManagementApiSummary();
std::string Objc3RuntimeMemoryManagementImplementationSummary();
std::string Objc3OwnershipRuntimeGateSummary();
std::string Objc3ExecutableBlockSourceClosureSummary();
std::string Objc3ExecutableBlockSourceModelCompletionSummary();
std::string Objc3ExecutableBlockSourceStorageAnnotationSummary();
std::string Objc3ExecutableBlockRuntimeSemanticRulesSummary();
std::string Objc3ExecutableBlockLoweringAbiArtifactBoundarySummary();
std::string Objc3ExecutableBlockObjectInvokeThunkLoweringSummary();
std::string Objc3ExecutableBlockByrefHelperLoweringSummary();
std::string Objc3ExecutableBlockEscapeRuntimeHookLoweringSummary();
std::string Objc3RuntimeBlockApiObjectLayoutSummary();
std::string Objc3RuntimeBlockAllocationCopyDisposeInvokeSupportSummary();
std::string Objc3RuntimeBlockByrefForwardingHeapPromotionInteropSummary();
std::string Objc3RunnableBlockRuntimeGateSummary();
std::string Objc3RunnableBlockExecutionMatrixSummary();
std::string Objc3ArcSourceModeBoundarySummary();
std::string Objc3ArcModeHandlingSummary(bool arc_mode_enabled);
std::string Objc3ExecutableMethodBodyBindingSummary();
std::string Objc3ExecutableRealizationRecordsSummary();
std::string Objc3RuntimeClassRealizationSummary();
std::string Objc3RuntimeMetaclassGraphRootClassSummary();
std::string Objc3RuntimeCategoryAttachmentProtocolConformanceSummary();
std::string Objc3RuntimeCanonicalRunnableObjectSampleSupportSummary();
std::string Objc3RuntimeMetadataBinaryInspectionHarnessSummary();
std::string Objc3RuntimeMetadataObjectPackagingRetentionSummary();
std::string Objc3RuntimeMetadataLinkerRetentionSummary();
std::string Objc3RuntimeMetadataArchiveStaticLinkDiscoverySummary();
std::string Objc3RuntimeBootstrapLoweringBoundarySummary();
std::string Objc3RuntimeBootstrapRegistrationDescriptorImageRootLoweringSummary();
std::string Objc3RuntimeBootstrapArchiveStaticLinkReplayCorpusSummary();
std::string Objc3RuntimeMetadataEmissionGateSummary();
std::string Objc3RuntimeMetadataObjectEmissionCloseoutSummary();
std::string Objc3RuntimeMetadataSectionForObjectFormat(
    const std::string &object_format, const std::string &logical_section);
std::string Objc3RuntimeMetadataDriverLinkerRetentionFlagForObjectFormat(
    const std::string &object_format, const std::string &symbol_name);
std::string Objc3RuntimeMetadataHostSectionForLogicalName(
    const std::string &logical_section);
bool TryGetCompoundAssignmentBinaryOpcode(const std::string &op, std::string &opcode);
bool TryParseObjc3AtomicMemoryOrder(const std::string &token, Objc3AtomicMemoryOrder &order);
const char *Objc3AtomicMemoryOrderToLLVMOrdering(Objc3AtomicMemoryOrder order);
std::string Objc3AtomicMemoryOrderMappingReplayKey();
bool IsSupportedObjc3SimdVectorLaneCount(unsigned lane_count);
bool TryBuildObjc3SimdVectorLLVMType(const std::string &base_spelling, unsigned lane_count, std::string &llvm_type);
std::string Objc3SimdVectorTypeLoweringReplayKey();
bool IsValidObjc3MethodLookupOverrideConflictContract(const Objc3MethodLookupOverrideConflictContract &contract);
std::string Objc3MethodLookupOverrideConflictReplayKey(const Objc3MethodLookupOverrideConflictContract &contract);
Objc3PropertySynthesisIvarBindingContract Objc3DefaultPropertySynthesisIvarBindingContract(
    std::size_t property_synthesis_sites,
    bool deterministic = true);
bool IsValidObjc3PropertySynthesisIvarBindingContract(
    const Objc3PropertySynthesisIvarBindingContract &contract);
std::string Objc3PropertySynthesisIvarBindingReplayKey(
    const Objc3PropertySynthesisIvarBindingContract &contract);
bool IsValidObjc3IdClassSelObjectPointerTypecheckContract(
    const Objc3IdClassSelObjectPointerTypecheckContract &contract);
std::string Objc3IdClassSelObjectPointerTypecheckReplayKey(
    const Objc3IdClassSelObjectPointerTypecheckContract &contract);
bool IsValidObjc3DispatchSurfaceClassificationContract(
    const Objc3DispatchSurfaceClassificationContract &contract);
std::string Objc3DispatchSurfaceClassificationReplayKey(
    const Objc3DispatchSurfaceClassificationContract &contract);
bool IsValidObjc3MessageSendSelectorLoweringContract(
    const Objc3MessageSendSelectorLoweringContract &contract);
std::string Objc3MessageSendSelectorLoweringReplayKey(
    const Objc3MessageSendSelectorLoweringContract &contract);
bool IsValidObjc3DispatchAbiMarshallingContract(
    const Objc3DispatchAbiMarshallingContract &contract);
std::string Objc3DispatchAbiMarshallingReplayKey(
    const Objc3DispatchAbiMarshallingContract &contract);
bool IsValidObjc3NilReceiverSemanticsFoldabilityContract(
    const Objc3NilReceiverSemanticsFoldabilityContract &contract);
std::string Objc3NilReceiverSemanticsFoldabilityReplayKey(
    const Objc3NilReceiverSemanticsFoldabilityContract &contract);
bool IsValidObjc3SuperDispatchMethodFamilyContract(
    const Objc3SuperDispatchMethodFamilyContract &contract);
std::string Objc3SuperDispatchMethodFamilyReplayKey(
    const Objc3SuperDispatchMethodFamilyContract &contract);
bool IsValidObjc3RuntimeShimHostLinkContract(
    const Objc3RuntimeShimHostLinkContract &contract);
std::string Objc3RuntimeShimHostLinkReplayKey(
    const Objc3RuntimeShimHostLinkContract &contract);
bool IsValidObjc3RuntimeDispatchLoweringAbiContract(
    const Objc3RuntimeDispatchLoweringAbiContract &contract);
std::string Objc3RuntimeDispatchLoweringAbiReplayKey(
    const Objc3RuntimeDispatchLoweringAbiContract &contract);
std::string Objc3RuntimeDispatchLoweringAbiBoundarySummary(
    const Objc3RuntimeDispatchLoweringAbiContract &contract);
bool IsValidObjc3OwnershipQualifierLoweringContract(
    const Objc3OwnershipQualifierLoweringContract &contract);
std::string Objc3OwnershipQualifierLoweringReplayKey(
    const Objc3OwnershipQualifierLoweringContract &contract);
bool IsValidObjc3RetainReleaseOperationLoweringContract(
    const Objc3RetainReleaseOperationLoweringContract &contract);
std::string Objc3RetainReleaseOperationLoweringReplayKey(
    const Objc3RetainReleaseOperationLoweringContract &contract);
bool IsValidObjc3AutoreleasePoolScopeLoweringContract(
    const Objc3AutoreleasePoolScopeLoweringContract &contract);
std::string Objc3AutoreleasePoolScopeLoweringReplayKey(
    const Objc3AutoreleasePoolScopeLoweringContract &contract);
bool IsValidObjc3WeakUnownedSemanticsLoweringContract(
    const Objc3WeakUnownedSemanticsLoweringContract &contract);
std::string Objc3WeakUnownedSemanticsLoweringReplayKey(
    const Objc3WeakUnownedSemanticsLoweringContract &contract);
bool IsValidObjc3ArcDiagnosticsFixitLoweringContract(
    const Objc3ArcDiagnosticsFixitLoweringContract &contract);
std::string Objc3ArcDiagnosticsFixitLoweringReplayKey(
    const Objc3ArcDiagnosticsFixitLoweringContract &contract);
bool IsValidObjc3BlockSourceModelCompletionContract(
    const Objc3BlockSourceModelCompletionContract &contract);
std::string Objc3BlockSourceModelCompletionReplayKey(
    const Objc3BlockSourceModelCompletionContract &contract);
bool IsValidObjc3BlockSourceStorageAnnotationContract(
    const Objc3BlockSourceStorageAnnotationContract &contract);
std::string Objc3BlockSourceStorageAnnotationReplayKey(
    const Objc3BlockSourceStorageAnnotationContract &contract);
bool IsValidObjc3BlockLiteralCaptureLoweringContract(
    const Objc3BlockLiteralCaptureLoweringContract &contract);
std::string Objc3BlockLiteralCaptureLoweringReplayKey(
    const Objc3BlockLiteralCaptureLoweringContract &contract);
bool IsValidObjc3BlockAbiInvokeTrampolineLoweringContract(
    const Objc3BlockAbiInvokeTrampolineLoweringContract &contract);
std::string Objc3BlockAbiInvokeTrampolineLoweringReplayKey(
    const Objc3BlockAbiInvokeTrampolineLoweringContract &contract);
bool IsValidObjc3BlockStorageEscapeLoweringContract(
    const Objc3BlockStorageEscapeLoweringContract &contract);
std::string Objc3BlockStorageEscapeLoweringReplayKey(
    const Objc3BlockStorageEscapeLoweringContract &contract);
bool IsValidObjc3BlockCopyDisposeLoweringContract(
    const Objc3BlockCopyDisposeLoweringContract &contract);
std::string Objc3BlockCopyDisposeLoweringReplayKey(
    const Objc3BlockCopyDisposeLoweringContract &contract);
bool IsValidObjc3BlockDeterminismPerfBaselineLoweringContract(
    const Objc3BlockDeterminismPerfBaselineLoweringContract &contract);
std::string Objc3BlockDeterminismPerfBaselineLoweringReplayKey(
    const Objc3BlockDeterminismPerfBaselineLoweringContract &contract);
bool IsValidObjc3LightweightGenericsConstraintLoweringContract(
    const Objc3LightweightGenericsConstraintLoweringContract &contract);
std::string Objc3LightweightGenericsConstraintLoweringReplayKey(
    const Objc3LightweightGenericsConstraintLoweringContract &contract);
bool IsValidObjc3NullabilityFlowWarningPrecisionLoweringContract(
    const Objc3NullabilityFlowWarningPrecisionLoweringContract &contract);
std::string Objc3NullabilityFlowWarningPrecisionLoweringReplayKey(
    const Objc3NullabilityFlowWarningPrecisionLoweringContract &contract);
bool IsValidObjc3ProtocolQualifiedObjectTypeLoweringContract(
    const Objc3ProtocolQualifiedObjectTypeLoweringContract &contract);
std::string Objc3ProtocolQualifiedObjectTypeLoweringReplayKey(
    const Objc3ProtocolQualifiedObjectTypeLoweringContract &contract);
bool IsValidObjc3VarianceBridgeCastLoweringContract(
    const Objc3VarianceBridgeCastLoweringContract &contract);
std::string Objc3VarianceBridgeCastLoweringReplayKey(
    const Objc3VarianceBridgeCastLoweringContract &contract);
bool IsValidObjc3GenericMetadataAbiLoweringContract(
    const Objc3GenericMetadataAbiLoweringContract &contract);
std::string Objc3GenericMetadataAbiLoweringReplayKey(
    const Objc3GenericMetadataAbiLoweringContract &contract);
bool IsValidObjc3ModuleImportGraphLoweringContract(
    const Objc3ModuleImportGraphLoweringContract &contract);
std::string Objc3ModuleImportGraphLoweringReplayKey(
    const Objc3ModuleImportGraphLoweringContract &contract);
bool IsValidObjc3NamespaceCollisionShadowingLoweringContract(
    const Objc3NamespaceCollisionShadowingLoweringContract &contract);
std::string Objc3NamespaceCollisionShadowingLoweringReplayKey(
    const Objc3NamespaceCollisionShadowingLoweringContract &contract);
bool IsValidObjc3PublicPrivateApiPartitionLoweringContract(
    const Objc3PublicPrivateApiPartitionLoweringContract &contract);
std::string Objc3PublicPrivateApiPartitionLoweringReplayKey(
    const Objc3PublicPrivateApiPartitionLoweringContract &contract);
bool IsValidObjc3IncrementalModuleCacheInvalidationLoweringContract(
    const Objc3IncrementalModuleCacheInvalidationLoweringContract &contract);
std::string Objc3IncrementalModuleCacheInvalidationLoweringReplayKey(
    const Objc3IncrementalModuleCacheInvalidationLoweringContract &contract);
bool IsValidObjc3CrossModuleConformanceLoweringContract(
    const Objc3CrossModuleConformanceLoweringContract &contract);
std::string Objc3CrossModuleConformanceLoweringReplayKey(
    const Objc3CrossModuleConformanceLoweringContract &contract);
bool IsValidObjc3ThrowsPropagationLoweringContract(
    const Objc3ThrowsPropagationLoweringContract &contract);
std::string Objc3ThrowsPropagationLoweringReplayKey(
    const Objc3ThrowsPropagationLoweringContract &contract);
bool IsValidObjc3ResultLikeLoweringContract(
    const Objc3ResultLikeLoweringContract &contract);
std::string Objc3ResultLikeLoweringReplayKey(
    const Objc3ResultLikeLoweringContract &contract);
bool IsValidObjc3NSErrorBridgingLoweringContract(
    const Objc3NSErrorBridgingLoweringContract &contract);
std::string Objc3NSErrorBridgingLoweringReplayKey(
    const Objc3NSErrorBridgingLoweringContract &contract);
bool IsValidObjc3UnwindCleanupLoweringContract(
    const Objc3UnwindCleanupLoweringContract &contract);
std::string Objc3UnwindCleanupLoweringReplayKey(
    const Objc3UnwindCleanupLoweringContract &contract);
bool IsValidObjc3ErrorDiagnosticsRecoveryLoweringContract(
    const Objc3ErrorDiagnosticsRecoveryLoweringContract &contract);
std::string Objc3ErrorDiagnosticsRecoveryLoweringReplayKey(
    const Objc3ErrorDiagnosticsRecoveryLoweringContract &contract);
bool IsValidObjc3AsyncContinuationLoweringContract(
    const Objc3AsyncContinuationLoweringContract &contract);
std::string Objc3AsyncContinuationLoweringReplayKey(
    const Objc3AsyncContinuationLoweringContract &contract);
bool IsValidObjc3AwaitLoweringSuspensionStateLoweringContract(
    const Objc3AwaitLoweringSuspensionStateLoweringContract &contract);
std::string Objc3AwaitLoweringSuspensionStateLoweringReplayKey(
    const Objc3AwaitLoweringSuspensionStateLoweringContract &contract);
bool IsValidObjc3ActorIsolationSendabilityLoweringContract(
    const Objc3ActorIsolationSendabilityLoweringContract &contract);
std::string Objc3ActorIsolationSendabilityLoweringReplayKey(
    const Objc3ActorIsolationSendabilityLoweringContract &contract);
bool IsValidObjc3TaskRuntimeInteropCancellationLoweringContract(
    const Objc3TaskRuntimeInteropCancellationLoweringContract &contract);
std::string Objc3TaskRuntimeInteropCancellationLoweringReplayKey(
    const Objc3TaskRuntimeInteropCancellationLoweringContract &contract);
bool IsValidObjc3ConcurrencyReplayRaceGuardLoweringContract(
    const Objc3ConcurrencyReplayRaceGuardLoweringContract &contract);
std::string Objc3ConcurrencyReplayRaceGuardLoweringReplayKey(
    const Objc3ConcurrencyReplayRaceGuardLoweringContract &contract);
bool IsValidObjc3UnsafePointerExtensionLoweringContract(
    const Objc3UnsafePointerExtensionLoweringContract &contract);
std::string Objc3UnsafePointerExtensionLoweringReplayKey(
    const Objc3UnsafePointerExtensionLoweringContract &contract);
bool IsValidObjc3InlineAsmIntrinsicGovernanceLoweringContract(
    const Objc3InlineAsmIntrinsicGovernanceLoweringContract &contract);
std::string Objc3InlineAsmIntrinsicGovernanceLoweringReplayKey(
    const Objc3InlineAsmIntrinsicGovernanceLoweringContract &contract);
