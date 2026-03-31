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
inline constexpr const char *kObjc3SelectorResolutionDynamicFallbackPolicy =
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
// dispatch lowering ABI anchor: emitted sends already lower to the canonical
// runtime entrypoint. The compatibility symbol stays exported as a non-emitted
// alias only, while selector lookup, receiver/result ABI, and fixed-slot
// argument padding remain the compiler-to-runtime boundary.
inline constexpr const char *kObjc3RuntimeDispatchLoweringAbiContractId =
    "objc3c.runtime.dispatch.lowering.abi.freeze.v1";
inline constexpr const char *kObjc3RuntimeDispatchLoweringAbiBoundaryModel =
    "canonical-runtime-dispatch-default-target";
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
    "selector-cstring-pointer-remains-lowered-operand-until-next-runtime-phase";
inline constexpr const char *kObjc3RuntimeDispatchLoweringSelectorHandleModel =
    "runtime-lookup-produces-selector-handle-before-live-dispatch";
inline constexpr const char *kObjc3RuntimeDispatchLoweringArgumentPaddingModel =
    "zero-pad-to-fixed-runtime-arg-slot-count";
inline constexpr const char *kObjc3RuntimeDispatchLoweringDefaultTargetModel =
    "default-lowering-target-is-canonical-runtime-entrypoint";
inline constexpr const char
    *kObjc3RuntimeDispatchLoweringCompatibilityRoleModel =
        "compatibility-dispatch-symbol-remains-exported-but-not-emitted-on-live-path";
inline constexpr const char *kObjc3RuntimeDispatchLoweringDeferredCasesModel =
    "direct-dispatch-remains-fail-closed-after-live-cutover";
// runtime call ABI generation anchor: all supported send surfaces use the
// canonical runtime entrypoint; only reserved direct dispatch stays fail-closed.
inline constexpr const char *kObjc3RuntimeDispatchCallAbiGenerationContractId =
    "objc3c.runtime.call.abi.instance.class.dispatch.v1";
inline constexpr const char
    *kObjc3RuntimeDispatchCallAbiGenerationActiveLoweringModel =
        "instance-class-super-and-dynamic-sends-lower-directly-to-canonical-runtime-entrypoint";
inline constexpr const char
    *kObjc3RuntimeDispatchCallAbiGenerationDeferredLoweringModel =
        "direct-dispatch-remains-fail-closed-until-supported-surface-materializes";
// runtime call ABI generation anchor: normalized nil, instance, class, super,
// and dynamic sends route through the live runtime entrypoint. Unsupported
// direct dispatch still fails closed before IR emission.
inline constexpr const char *kObjc3RuntimeDispatchSuperNilContractId =
    "objc3c.runtime.call.abi.super.nil.direct.dispatch.v1";
inline constexpr const char
    *kObjc3RuntimeDispatchSuperNilActiveLoweringModel =
        "instance-class-super-and-nil-sends-lower-directly-to-canonical-runtime-entrypoint";
inline constexpr const char
    *kObjc3RuntimeDispatchSuperNilDeferredLoweringModel =
        "direct-dispatch-remains-fail-closed-until-supported-surface-materializes";
inline constexpr const char
    *kObjc3RuntimeDispatchSuperNilUnsupportedFallbackModel =
        "direct-dispatch-fails-closed-until-supported-surface-materializes";
// live-dispatch cutover anchor: lane-C routes normalized dynamic sends through
// the canonical runtime entrypoint too, while keeping the compatibility symbol
// exported as non-emitted test/compat evidence only and leaving direct
// dispatch fail-closed.
inline constexpr const char *kObjc3RuntimeDispatchLiveCutoverContractId =
    "objc3c.runtime.call.abi.live.dispatch.cutover.v1";
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
// lookup/dispatch runtime anchor: the live runtime owns selector interning,
// realized lookup tables, method cache, and slow-path method resolution behind
// objc3_runtime_lookup_selector plus objc3_runtime_dispatch_i32. The
// compatibility symbol is non-authoritative alias surface only.
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
inline constexpr const char *kObjc3RuntimeLookupDispatchCompatibilityModel =
    "compatibility-dispatch-symbol-remains-exported-but-not-emitted-on-live-path";
inline constexpr const char *kObjc3RuntimeLookupDispatchFailureModel =
    "only-unresolved-or-invalid-runtime-resolution-falls-back-deterministically";
// selector lookup table anchor: lane-D now materializes the selector
// table from emitted registration-table selector pools while preserving the
// frozen D001 public runtime entrypoints. Method-cache / slow-path resolution
// remains deferred to the next runtime step, and unknown selector lookups stay dynamic
// until that path exists.
inline constexpr const char *kObjc3RuntimeSelectorLookupTablesContractId =
    "objc3c.runtime.selector.lookup.tables.v1";
inline constexpr const char
    *kObjc3RuntimeSelectorLookupTablesInterningModel =
        "registered-selector-pools-materialize-process-global-stable-id-table";
inline constexpr const char
    *kObjc3RuntimeSelectorLookupTablesMergeModel =
        "per-image-selector-pools-deduplicated-and-merged-across-registration-order";
inline constexpr const char
    *kObjc3RuntimeSelectorLookupTablesFallbackModel =
        "unknown-selector-lookups-remain-dynamic-until-next-runtime-phase";
inline constexpr const char
    *kObjc3RuntimeSelectorLookupTablesReplayModel =
        "reset-replay-rebuilds-metadata-backed-selector-table-in-registration-order";
inline constexpr const char *kObjc3SelectorGlobalOrdering = "lexicographic";
// method-cache / slow-path anchor: lane-D now turns registered class
// and metaclass metadata into real runtime dispatch targets while preserving
// the frozen D001 public runtime API. Selector tables remain the authoritative
// stable-id source from D002, class-known and class-self receivers normalize
// onto one metaclass lookup family, and unresolved or ambiguous runtime
// lookups still fail closed back to the compatibility arithmetic model until
// later protocol/category-aware resolution lands in the subsequent runtime step.
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
    *kObjc3RuntimeMethodCacheSlowPathFallbackModel =
        "only-unresolved-or-ambiguous-runtime-resolution-falls-back-to-deterministic-dispatch-formula";
// protocol/category-aware resolution anchor: lane-D extends the live
// runtime slow path to consume emitted category method tables plus adopted and
// inherited protocol metadata while preserving the frozen D001 public runtime
// API and D003 cache/selector-table surfaces.
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
    *kObjc3RuntimeProtocolCategoryMethodResolutionFallbackModel =
        "only-unresolved-conflicting-category-or-protocol-resolution-falls-back-deterministically";
// live-dispatch gate anchor: lane-E now freezes one fail-closed
// evidence boundary proving supported message sends execute through the live
// runtime path rather than the compatibility shim. The upstream gate chain
// stays rooted on A002/B003/C004/D004, the compatibility shim remains
// exported only as evidence/test surface, and E002 is the next issue allowed
// to replace shim-based smoke/closeout assumptions with the integrated gate.
inline constexpr const char *kObjc3RuntimeLiveDispatchGateContractId =
    "objc3c.runtime.live.dispatch.gate.v1";
inline constexpr const char *kObjc3RuntimeLiveDispatchGateEvidenceModel =
    "source-sema-lowering-runtime-abi-summary-chain";
inline constexpr const char *kObjc3RuntimeLiveDispatchGateShimBoundaryModel =
    "live-runtime-dispatch-required-compatibility-shim-evidence-only";
inline constexpr const char *kObjc3RuntimeLiveDispatchGateFailureModel =
    "fail-closed-on-live-dispatch-evidence-drift";
inline constexpr const char *kObjc3RuntimeLiveDispatchGateNextIssue =
    "objc3c.runtime.livedispatch.closeout.v1";
// live-dispatch smoke/replay closeout anchor: lane-E now replaces
// the last shim-era smoke/replay assumptions with canonical live runtime
// dispatch evidence. Execution smoke and replay must publish
// requires_live_runtime_dispatch, the canonical runtime library path, the
// compatibility shim as non-authoritative evidence only, and the canonical
// default symbol objc3_runtime_dispatch_i32.
inline constexpr const char *kObjc3RuntimeLiveDispatchSmokeReplayCloseoutContractId =
    "objc3c.runtime.live.dispatch.smoke.replay.closeout.v1";
inline constexpr const char *kObjc3RuntimeLiveDispatchSmokeReplayCloseoutModel =
    "live-runtime-dispatch-smoke-and-replay-authoritative-shim-evidence-non-authoritative";
// emitted metadata inventory freeze anchor: lowering contracts do
// not own or infer object-file metadata inventory. The emitted inventory
// remains the frontend ABI/scaffold/object-inspection boundary for image-info
// plus class/protocol/category/property/ivar descriptor sections until later
// issues extend it explicitly.
// source-to-section matrix anchor: interface/implementation/
// metaclass/method rows stay explicit no-standalone-emission entries until
// later payload work extends them.
// layout/visibility policy anchor: lowering contracts freeze one
// emitted metadata layout policy without inferring a second model. Image-info
// emits first; descriptor families follow class/protocol/category/property/ivar
// order; descriptor ordinals ascend before the family aggregate; emitted
// metadata remains local-linkage/no-COMDAT; explicit hidden visibility is not
// spelled on IR globals because local linkage already keeps them non-exported;
// llvm.used preserves retention order; and object-format-specific variants stay
// deferred until the next runtime step.
inline constexpr const char *kObjc3RuntimeMetadataLayoutOrderingVisibilityPolicyContractId =
    "objc3c.runtime.metadata.layout.ordering.visibility.policy.freeze.v1";
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
    "object-format-neutral-until-next-runtime-phase";
// object-format policy expansion anchor: B001/B002 keep the neutral
// model frozen for historical replay, while lowering now also carries the
// explicit COFF/ELF/Mach-O mapping surface for emitted section spellings and
// retention-anchor behavior.
inline constexpr const char
    *kObjc3RuntimeMetadataObjectFormatSurfaceContractId =
        "objc3c.runtime.metadata.object.format.policy.v1";
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
// metadata section emission freeze anchor: native object files now
// carry real metadata sections, but the payload bytes remain scaffold-only
// placeholder shapes until the later lane-C implementation issues land.
inline constexpr const char *kObjc3RuntimeMetadataSectionEmissionContractId =
    "objc3c.runtime.metadata.section.emission.freeze.v1";
inline constexpr const char *kObjc3RuntimeMetadataSectionEmissionPayloadModel =
    "scaffold-placeholder-payloads-until-next-runtime-phase";
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
// class/metaclass data emission anchor: lane-C begins replacing the
// class-family placeholder byte model with real class descriptor bundles while
// keeping metaclass payloads inline with their class bundles and deferring real
// method/property/ivar lists plus selector/string pools to later issues.
// dispatch-control lowering anchor: class/metaclass payloads now
// preserve objc_final/objc_sealed container intent as explicit metadata bits.
inline constexpr const char *kObjc3RuntimeClassMetaclassEmissionContractId =
    "objc3c.runtime.class.metaclass.data.emission.v1";
inline constexpr const char *kObjc3RuntimeClassMetaclassEmissionPayloadModel =
    "class-source-record-descriptor-bundles-with-inline-metaclass-records-and-final-sealed-flags";
inline constexpr const char *kObjc3RuntimeClassMetaclassEmissionNameModel =
    "shared-class-name-cstring-per-bundle";
inline constexpr const char *kObjc3RuntimeClassMetaclassEmissionSuperLinkModel =
    "nullable-super-source-record-bundle-pointer";
inline constexpr const char
    *kObjc3RuntimeClassMetaclassEmissionMethodListReferenceModel =
        "count-plus-owner-identity-pointer-method-list-ref";
// protocol/category data emission anchor: lane-C next replaces the
// protocol/category family placeholder byte models with real descriptor bundles
// while keeping cross-protocol references and category attachments explicit and
// fail-closed without claiming that selector/string pools or standalone
// property/ivar payload sections already exist.
inline constexpr const char *kObjc3RuntimeProtocolCategoryEmissionContractId =
    "objc3c.runtime.protocol.category.data.emission.v1";
inline constexpr const char *kObjc3RuntimeProtocolEmissionPayloadModel =
    "protocol-descriptor-bundles-with-inherited-protocol-ref-lists";
inline constexpr const char *kObjc3RuntimeCategoryEmissionPayloadModel =
    "category-descriptor-bundles-with-attachment-and-protocol-ref-lists";
inline constexpr const char *kObjc3RuntimeProtocolReferenceModel =
    "count-plus-descriptor-pointer-protocol-ref-lists";
inline constexpr const char *kObjc3RuntimeCategoryAttachmentModel =
    "count-plus-owner-identity-pointer-attachment-lists";
// member-table data emission anchor: lane-C now adds real
// owner-scoped method-table payloads plus real property/ivar descriptor bytes
// without reopening the C002/C003 descriptor-family shapes. Class refs keep
// their historical prefix stable while protocol/category descriptor bundles
// remain shape-stable and gain adjacent emitted member-table payloads.
// dispatch-control lowering anchor: method-table payloads now carry
// effective direct-dispatch and objc_final intent bits alongside callable
// implementation pointers.
inline constexpr const char *kObjc3RuntimeMemberTableEmissionContractId =
    "objc3c.runtime.member.table.emission.v1";
inline constexpr const char *kObjc3RuntimeMethodListEmissionPayloadModel =
    "owner-scoped-method-table-globals-with-inline-entry-records-and-direct-final-flags";
inline constexpr const char *kObjc3RuntimeMethodListEmissionGroupingModel =
    "declaration-owner-plus-class-kind-lexicographic";
inline constexpr const char *kObjc3RuntimePropertyDescriptorEmissionPayloadModel =
    "property-descriptor-records-with-accessor-and-binding-strings";
inline constexpr const char *kObjc3RuntimeIvarDescriptorEmissionPayloadModel =
    "ivar-descriptor-records-with-property-binding-strings";
// selector/string pool expansion anchor: runtime-adjacent selector
// globals now expand into canonical selector and string pool families with
// stable ordinal aggregates, while existing descriptor bundles remain shape
// stable and keep their current inline cstring payloads.
inline constexpr const char *kObjc3RuntimeSelectorStringPoolEmissionContractId =
    "objc3c.runtime.selector.string.pool.emission.v1";
inline constexpr const char *kObjc3RuntimeSelectorPoolEmissionPayloadModel =
    "canonical-selector-cstring-pool-with-stable-ordinal-aggregate";
inline constexpr const char *kObjc3RuntimeStringPoolEmissionPayloadModel =
    "canonical-runtime-string-cstring-pool-with-stable-ordinal-aggregate";
inline constexpr const char *kObjc3RuntimeSelectorPoolLogicalSection =
    "objc3.runtime.selector_pool";
inline constexpr const char *kObjc3RuntimeStringPoolLogicalSection =
    "objc3.runtime.string_pool";
inline constexpr const char *kObjc3RuntimeKeypathDescriptorLogicalSection =
    "objc3.runtime.keypath_descriptors";
// executable object artifact lowering freeze anchor: lane-C now
// freezes the current binding surface where realized class/category metadata
// records consume owner-scoped method-list refs and implementation-backed
// method entries may point at concrete LLVM definition symbols. Later
// implementation must extend this one executable object surface rather than
// rediscovering bodies or realization edges out-of-band.
inline constexpr const char *kObjc3ExecutableObjectArtifactLoweringContractId =
    "objc3c.executable.object.artifact.lowering.v1";
inline constexpr const char
    *kObjc3ExecutableObjectArtifactLoweringMethodBodyBindingModel =
        "implementation-owner-identity-to-llvm-definition-symbol";
inline constexpr const char
    *kObjc3ExecutableObjectArtifactLoweringRealizationRecordModel =
        "class-metaclass-and-category-descriptor-bundles-point-to-owner-scoped-method-list-ref-records";
inline constexpr const char
    *kObjc3ExecutableObjectArtifactLoweringMethodEntryPayloadModel =
        "selector-owner-return-arity-implementation-symbol-has-body-direct-flag-final-flag";
inline constexpr const char *kObjc3ExecutableObjectArtifactLoweringScopeModel =
    "parser-source-identities-sema-realization-closure-ir-object-binding";
inline constexpr const char
    *kObjc3ExecutableObjectArtifactLoweringFailClosedModel =
        "no-synthetic-implementation-symbols-no-rebound-legality-no-new-section-families";
// accessor/layout lowering freeze anchor: lane-C now freezes the
// current property/ivar lowering surface where sema-approved property
// descriptor bundles, ivar layout symbols/slots/sizes/alignment, and
// synthesized binding identities are serialized into emitted metadata/object
// artifacts without yet synthesizing accessor bodies or runtime storage.
inline constexpr const char
    *kObjc3ExecutablePropertyAccessorLayoutLoweringContractId =
        "objc3c.executable.property.accessor.layout.lowering.v1";
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
// ivar offset/layout emission anchor: lane-C extends the frozen
// accessor/layout handoff into real object payloads by emitting per-ivar
// offset globals, per-owner layout tables, and descriptor records that carry
// the sema-approved slot/offset/size/alignment tuple without yet allocating
// runtime instances or synthesizing accessor bodies.
inline constexpr const char *kObjc3ExecutableIvarLayoutEmissionContractId =
    "objc3c.executable.ivar.layout.emission.v1";
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
// synthesized accessor/property lowering anchor: lane-C upgrades the
// frozen property/layout handoff into executable accessor support by
// materializing missing implementation-owned getter/setter method entries,
// emitting deterministic storage globals keyed by synthesized binding symbols,
// and widening property descriptor payloads with effective accessor and layout
// attachment records while still deferring true runtime instance allocation to
// later lane-D work.
inline constexpr const char
    *kObjc3ExecutableSynthesizedAccessorPropertyLoweringContractId =
        "objc3c.executable.synthesized.accessor.property.lowering.v1";
inline constexpr const char
    *kObjc3ExecutableSynthesizedAccessorPropertyLoweringSourceModel =
        "implementation-owned-properties-synthesize-missing-effective-instance-accessors-into-emitted-method-lists";
inline constexpr const char
    *kObjc3ExecutableSynthesizedAccessorPropertyLoweringStorageModel =
        "synthesized-getter-setter-bodies-lower-directly-to-runtime-current-property-helper-calls-without-storage-globals";
inline constexpr const char
    *kObjc3ExecutableSynthesizedAccessorPropertyLoweringPropertyDescriptorModel =
        "property-descriptors-carry-effective-accessor-selectors-binding-symbols-layout-symbols-and-accessor-implementation-pointers";
inline constexpr const char
    *kObjc3ExecutableSynthesizedAccessorPropertyLoweringFailClosedModel =
        "no-missing-effective-accessor-bindings-no-duplicate-synthesized-owner-identities-no-storage-global-fallbacks";
// runtime property/layout consumption freeze anchor: lane-D now
// freezes the truthful runtime boundary above C003. Runtime consumes emitted
// synthesized accessor implementation pointers plus property/layout attachment
// records through the existing lookup/dispatch ABI, but alloc/new still
// materialize one canonical realized instance identity per class and accessor
// execution still uses the lane-C storage globals until D002 introduces real
// per-instance slot allocation.
inline constexpr const char *kObjc3RuntimePropertyLayoutConsumptionContractId =
    "objc3c.runtime.property.layout.consumption.freeze.v1";
inline constexpr const char
    *kObjc3RuntimePropertyLayoutConsumptionDescriptorModel =
        "runtime-consumes-emitted-property-descriptor-accessor-pointers-binding-symbols-and-layout-identities-without-source-rediscovery";
inline constexpr const char
    *kObjc3RuntimePropertyLayoutConsumptionAllocatorModel =
        "alloc-new-consume-realized-class-layout-and-handoff-directly-to-runtime-owned-instance-slot-allocation";
inline constexpr const char
    *kObjc3RuntimePropertyLayoutConsumptionStorageModel =
        "synthesized-accessor-execution-consumes-runtime-owned-per-instance-slots-selected-by-the-dispatch-frame-property-context";
inline constexpr const char
    *kObjc3RuntimePropertyLayoutConsumptionFailClosedModel =
        "no-layout-rederivation-no-storage-global-fallbacks-no-reflective-property-registration";
// instance-allocation-layout-runtime anchor: lane-D upgrades the
// frozen D001 runtime-consumption boundary into true per-instance allocation
// backed by realized class layout, emitted ivar offsets, and runtime-owned slot
// storage without reopening source-driven layout recovery.
inline constexpr const char
    *kObjc3RuntimeInstanceAllocationLayoutSupportContractId =
        "objc3c.runtime.instance.allocation.layout.support.v1";
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
// property-metadata-reflection anchor: lane-D now freezes the
// private reflective helper surface over the realized property metadata graph
// so diagnostics and tests can query property/accessor/layout facts without
// widening the public runtime ABI or rediscovering metadata from source.
inline constexpr const char
    *kObjc3RuntimePropertyMetadataReflectionContractId =
        "objc3c.runtime.property.metadata.reflection.v1";
inline constexpr const char
    *kObjc3RuntimePropertyMetadataReflectionRegistrationModel =
        "runtime-registers-reflectable-property-accessor-and-layout-facts-from-emitted-metadata-without-source-rediscovery";
inline constexpr const char
    *kObjc3RuntimePropertyMetadataReflectionQueryModel =
        "private-testing-helpers-query-realized-property-metadata-by-class-and-property-name-including-effective-accessors-and-layout-facts";
inline constexpr const char
    *kObjc3RuntimePropertyMetadataReflectionFailClosedModel =
        "no-public-reflection-abi-no-reflective-source-recovery-no-property-query-success-without-realized-runtime-layout";
// runtime-backed object ownership attribute surface anchor: lane-A
// upgrades the frozen ownership boundary into an emitted property/member
// metadata capability by carrying sema-approved property attribute,
// lifetime/runtime-hook, and accessor ownership profiles directly in the
// runtime-facing property descriptor payload rather than leaving them manifest-
// only evidence.
inline constexpr const char
    *kObjc3RuntimeBackedObjectOwnershipAttributeSurfaceContractId =
        "objc3c.runtime.backed.object.ownership.attribute.surface.v1";
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
// retainable-object semantic-rule freeze anchor: lane-B freezes the
// truthful semantic boundary around runtime-backed object ownership by making
// it explicit that property/member ownership metadata is now live while
// retain/release legality, autoreleasepool execution, and destruction-order
// behavior remain summary-driven and fail-closed until later work.
inline constexpr const char
    *kObjc3RetainableObjectSemanticRulesFreezeContractId =
        "objc3c.retainable.object.semantic.rules.freeze.v1";
inline constexpr const char
    *kObjc3RetainableObjectSemanticRulesSemanticModel =
        "runtime-backed-object-semantic-rules-freeze-property-member-ownership-metadata-while-retain-release-remains-summary-driven-and-runtime-backed-storage-legality-is-live-sema-enforced";
inline constexpr const char
    *kObjc3RetainableObjectSemanticRulesDestructionModel =
        "destruction-order-autoreleasepool-and-live-arc-execution-stay-fail-closed-outside-runtime-backed-storage-legality";
inline constexpr const char
    *kObjc3RetainableObjectSemanticRulesFailClosedModel =
        "fail-closed-on-retainable-object-semantic-drift-or-premature-live-storage-legality-claim";
// runtime-backed storage ownership legality anchor: lane-B upgrades
// the frozen B001 boundary into live semantic enforcement for runtime-backed
// object properties by rejecting conflicting explicit ownership
// qualifier/modifier combinations while preserving the truthful owned/weak/
// unowned storage profiles the runtime already consumes.
inline constexpr const char
    *kObjc3RuntimeBackedStorageOwnershipLegalityContractId =
        "objc3c.runtime.backed.storage.ownership.legality.v1";
inline constexpr const char
    *kObjc3RuntimeBackedStorageOwnershipOwnedStorageModel =
        "explicit-strong-object-property-qualifiers-remain-legal-for-owned-runtime-backed-storage-while-conflicting-weak-or-unowned-modifiers-fail-closed";
inline constexpr const char
    *kObjc3RuntimeBackedStorageOwnershipWeakUnownedModel =
        "explicit-weak-and-unsafe-unretained-object-property-qualifiers-bind-runtime-backed-storage-legality-and-reject-conflicting-property-modifiers";
inline constexpr const char
    *kObjc3RuntimeBackedStorageOwnershipFailClosedModel =
        "fail-closed-on-runtime-backed-object-property-ownership-qualifier-modifier-drift";
// autoreleasepool/destruction-order semantic expansion anchor:
// lane-B keeps autoreleasepool scopes fail-closed but now distinguishes the
// ownership-sensitive case where owned runtime-backed object or synthesized
// property storage would require deferred destruction-order semantics.
inline constexpr const char
    *kObjc3RuntimeBackedAutoreleasepoolDestructionOrderContractId =
        "objc3c.runtime.backed.autoreleasepool.destruction.order.semantics.v1";
inline constexpr const char
    *kObjc3RuntimeBackedAutoreleasepoolModel =
        "autoreleasepool-scopes-remain-fail-closed-while-owned-runtime-backed-object-storage-publishes-destruction-order-edge-diagnostics";
inline constexpr const char
    *kObjc3RuntimeBackedDestructionOrderModel =
        "owned-runtime-backed-object-or-synthesized-property-storage-inside-autoreleasepool-requires-deferred-destruction-order-runtime-support";
inline constexpr const char
    *kObjc3RuntimeBackedAutoreleasepoolDestructionOrderFailClosedModel =
        "fail-closed-on-autoreleasepool-destruction-order-semantic-drift-for-owned-runtime-backed-storage";
// ownership-lowering baseline freeze anchor: lane-C freezes the
// current lowering boundary where runtime-backed ownership metadata and sema
// legality are live, but executable retain/release/autorelease/weak behavior
// still remains represented by legacy lowering summaries rather than emitted
// runtime hooks.
inline constexpr const char *kObjc3OwnershipLoweringBaselineContractId =
    "objc3c.ownership.lowering.baseline.freeze.v1";
inline constexpr const char *kObjc3OwnershipLoweringBaselineQualifierModel =
    "ownership-qualifier-lowering-remains-legacy-summary-driven-for-runtime-backed-object-metadata";
inline constexpr const char *kObjc3OwnershipLoweringBaselineRuntimeHookModel =
    "retain-release-autorelease-and-weak-lowering-stays-summary-only-without-live-runtime-hook-emission";
inline constexpr const char *kObjc3OwnershipLoweringBaselineAutoreleasepoolModel =
    "autoreleasepool-lowering-remains-summary-only-without-emitted-push-pop-hooks";
inline constexpr const char *kObjc3OwnershipLoweringBaselineFailClosedModel =
    "no-live-ownership-runtime-hooks-no-arc-weak-side-table-entrypoints-no-destruction-lowering-yet";
// runtime hook emission anchor: lane-C replaces the C001 summary-only
// baseline with emitted runtime helper calls for retain/release/autorelease and
// weak property paths while preserving the existing synthesized accessor
// descriptor/storage artifact surface from the earlier runtime step.
inline constexpr const char *kObjc3OwnershipRuntimeHookEmissionContractId =
    "objc3c.ownership.runtime.hook.emission.v1";
inline constexpr const char *kObjc3OwnershipRuntimeHookEmissionAccessorModel =
    "synthesized-accessors-call-runtime-owned-current-property-and-ownership-hook-entrypoints";
inline constexpr const char *kObjc3OwnershipRuntimeHookEmissionPropertyContextModel =
    "runtime-dispatch-frame-selects-current-receiver-property-accessor-and-autorelease-queue";
inline constexpr const char *kObjc3OwnershipRuntimeHookEmissionAutoreleaseModel =
    "autorelease-values-drain-at-runtime-dispatch-return";
inline constexpr const char *kObjc3OwnershipRuntimeHookEmissionFailClosedModel =
    "owned-and-weak-runtime-backed-accessors-may-not-fall-back-to-summary-only-lowering";
inline constexpr const char
    *kObjc3DispatchAndSynthesizedAccessorLoweringSurfaceContractId =
        "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1";
inline constexpr const char
    *kObjc3AccessorStorageLoweringMetadataModel =
        "runtime-metadata-and-executable-graph-property-records-publish-synthesized-accessor-lowering-helper-selection-through-the-live-compiler-path";
inline constexpr const char
    *kObjc3AccessorStorageLoweringHelperSelectionModel =
        "plain-accessors-use-current-property-read-write-helpers-strong-owned-setters-use-exchange-and-weak-accessors-use-weak-current-property-helpers";
inline constexpr const char *kObjc3RuntimeReadCurrentPropertyI32Symbol =
    "objc3_runtime_read_current_property_i32";
inline constexpr const char *kObjc3RuntimeWriteCurrentPropertyI32Symbol =
    "objc3_runtime_write_current_property_i32";
inline constexpr const char *kObjc3RuntimeExchangeCurrentPropertyI32Symbol =
    "objc3_runtime_exchange_current_property_i32";
inline constexpr const char *kObjc3RuntimeStoreThrownErrorI32Symbol =
    "objc3_runtime_store_thrown_error_i32";
inline constexpr const char *kObjc3RuntimeLoadThrownErrorI32Symbol =
    "objc3_runtime_load_thrown_error_i32";
inline constexpr const char *kObjc3RuntimeBridgeStatusErrorI32Symbol =
    "objc3_runtime_bridge_status_error_i32";
inline constexpr const char *kObjc3RuntimeBridgeNSErrorErrorI32Symbol =
    "objc3_runtime_bridge_nserror_error_i32";
inline constexpr const char *kObjc3RuntimeCatchMatchesErrorI32Symbol =
    "objc3_runtime_catch_matches_error_i32";
inline constexpr const char *kObjc3RuntimeAllocateAsyncContinuationI32Symbol =
    "objc3_runtime_allocate_async_continuation_i32";
inline constexpr const char
    *kObjc3RuntimeHandoffAsyncContinuationToExecutorI32Symbol =
        "objc3_runtime_handoff_async_continuation_to_executor_i32";
inline constexpr const char *kObjc3RuntimeResumeAsyncContinuationI32Symbol =
    "objc3_runtime_resume_async_continuation_i32";
inline constexpr const char *kObjc3RuntimeSpawnTaskI32Symbol =
    "objc3_runtime_spawn_task_i32";
inline constexpr const char *kObjc3RuntimeEnterTaskGroupScopeI32Symbol =
    "objc3_runtime_enter_task_group_scope_i32";
inline constexpr const char *kObjc3RuntimeAddTaskGroupTaskI32Symbol =
    "objc3_runtime_add_task_group_task_i32";
inline constexpr const char *kObjc3RuntimeWaitTaskGroupNextI32Symbol =
    "objc3_runtime_wait_task_group_next_i32";
inline constexpr const char *kObjc3RuntimeCancelTaskGroupI32Symbol =
    "objc3_runtime_cancel_task_group_i32";
inline constexpr const char *kObjc3RuntimeTaskIsCancelledI32Symbol =
    "objc3_runtime_task_is_cancelled_i32";
inline constexpr const char *kObjc3RuntimeTaskOnCancelI32Symbol =
    "objc3_runtime_task_on_cancel_i32";
inline constexpr const char *kObjc3RuntimeExecutorHopI32Symbol =
    "objc3_runtime_executor_hop_i32";
inline constexpr const char *kObjc3RuntimeActorEnterIsolationThunkI32Symbol =
    "objc3_runtime_actor_enter_isolation_thunk_i32";
inline constexpr const char *kObjc3RuntimeActorEnterNonisolatedI32Symbol =
    "objc3_runtime_actor_enter_nonisolated_i32";
inline constexpr const char *kObjc3RuntimeActorHopToExecutorI32Symbol =
    "objc3_runtime_actor_hop_to_executor_i32";
inline constexpr const char *kObjc3RuntimeActorRecordReplayProofI32Symbol =
    "objc3_runtime_actor_record_replay_proof_i32";
inline constexpr const char *kObjc3RuntimeActorRecordRaceGuardI32Symbol =
    "objc3_runtime_actor_record_race_guard_i32";
inline constexpr const char *kObjc3RuntimeActorBindExecutorI32Symbol =
    "objc3_runtime_actor_bind_executor_i32";
inline constexpr const char *kObjc3RuntimeActorMailboxEnqueueI32Symbol =
    "objc3_runtime_actor_mailbox_enqueue_i32";
inline constexpr const char *kObjc3RuntimeActorMailboxDrainNextI32Symbol =
    "objc3_runtime_actor_mailbox_drain_next_i32";
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
    "objc3c.runtime.block.api.object.layout.freeze.v1";
inline constexpr const char
    *kObjc3RuntimeBlockAllocationCopyDisposeInvokeSupportContractId =
        "objc3c.runtime.block.allocation.copy.dispose.invoke.support.v1";
inline constexpr const char
    *kObjc3RuntimeBlockByrefForwardingHeapPromotionInteropContractId =
        "objc3c.runtime.block.byref.forwarding.heap.promotion.interop.v1";
// runnable-block-runtime gate anchor: lane-E now freezes the
// truthful integrated block-runtime boundary above the retained A003/B003/C004
// and D003 evidence chain so later closeout samples cannot overclaim support.
inline constexpr const char *kObjc3RunnableBlockRuntimeGateContractId =
    "objc3c.runnable.block.runtime.gate.v1";
inline constexpr const char *kObjc3RunnableBlockRuntimeGateEvidenceModel =
    "source-sema-lowering-runtime-summary-chain";
inline constexpr const char *kObjc3RunnableBlockRuntimeGateActiveModel =
    "runnable-block-gate-consumes-source-sema-lowering-and-runtime-proofs-rather-than-metadata-only-summaries";
inline constexpr const char *kObjc3RunnableBlockRuntimeGateNonGoalModel =
    "no-public-block-object-abi-no-public-runtime-helper-header-no-generalized-foreign-block-interop-no-caller-frame-forwarding-bridge";
inline constexpr const char *kObjc3RunnableBlockRuntimeGateFailClosedModel =
    "fail-closed-on-runnable-block-runtime-evidence-drift";
// runnable-block execution-matrix anchor: lane-E now closes the
// current runnable block tranche with one integrated executable matrix over
// the retained A003/B003/C004/D003/E001 chain without widening the public
// block ABI or helper surface.
inline constexpr const char *kObjc3RunnableBlockExecutionMatrixContractId =
    "objc3c.runnable.block.execution.matrix.v1";
inline constexpr const char *kObjc3RunnableBlockExecutionMatrixEvidenceModel =
    "source-sema-lowering-runtime-integrated-native-block-smoke-matrix";
inline constexpr const char *kObjc3RunnableBlockExecutionMatrixActiveModel =
    "closeout-matrix-runs-owned-nonowning-byref-and-escaping-block-fixtures-against-the-native-runtime";
inline constexpr const char *kObjc3RunnableBlockExecutionMatrixNonGoalModel =
    "no-public-block-object-abi-no-public-runtime-helper-header-no-generalized-foreign-block-interop-no-caller-frame-forwarding-bridge";
inline constexpr const char *kObjc3RunnableBlockExecutionMatrixFailClosedModel =
    "fail-closed-on-runnable-block-execution-matrix-drift-or-doc-mismatch";
// ARC source-surface/mode-boundary freeze anchor: the compiler
// preserves ownership-qualifier, weak/unowned, autoreleasepool, and ARC
// fix-it source surfaces, and the native driver now admits the bounded
// helper-backed `-fobjc-arc` slice proven by the executable ARC probes.
inline constexpr const char *kObjc3ArcSourceModeBoundaryContractId =
    "objc3c.arc.source.mode.boundary.freeze.v1";
inline constexpr const char *kObjc3ArcSourceModeBoundarySourceModel =
    "ownership-qualifier-weak-unowned-autoreleasepool-and-arc-fixit-source-surfaces-remain-live-without-enabling-runnable-arc-mode";
inline constexpr const char *kObjc3ArcSourceModeBoundaryModeModel =
    "native-driver-admits-fobjc-arc-and-fno-objc-arc-while-runnable-arc-stays-bounded-to-the-helper-backed-supported-slice";
inline constexpr const char *kObjc3ArcSourceModeBoundaryNonGoalModel =
    "no-generalized-arc-cleanup-insertion-no-public-arc-runtime-abi-mode-split-no-full-arc-automation-beyond-the-supported-helper-backed-slice";
inline constexpr const char *kObjc3ArcSourceModeBoundaryFailClosedModel =
    "fail-closed-on-arc-source-mode-boundary-drift-before-arc-automation";
inline constexpr const char *kObjc3ArcModeHandlingContractId =
    "objc3c.arc.mode.handling.v1";
inline constexpr const char *kObjc3ArcModeHandlingSourceModel =
    "ownership-qualified-method-property-return-and-block-capture-surfaces-are-runnable-under-explicit-arc-mode";
inline constexpr const char *kObjc3ArcModeHandlingModeModel =
    "driver-admits-fobjc-arc-and-fno-objc-arc-and-threads-arc-mode-through-frontend-sema-and-ir";
inline constexpr const char *kObjc3ArcModeHandlingFailClosedModel =
    "non-arc-mode-still-rejects-executable-ownership-qualified-method-and-function-signatures";
inline constexpr const char *kObjc3ArcModeHandlingNonGoalModel =
    "no-implicit-nonarc-promotion-no-cross-module-arc-mode-inference-no-full-arc-automation-beyond-the-supported-helper-backed-slice";
inline constexpr const char *kObjc3ArcSemanticRulesContractId =
    "objc3c.arc.semantic.rules.v1";
inline constexpr const char *kObjc3ArcSemanticRulesSourceModel =
    "explicit-arc-mode-admits-only-explicit-ownership-surfaces-while-forbidden-property-forms-and-broad-inference-remain-fail-closed";
inline constexpr const char *kObjc3ArcSemanticRulesSemanticModel =
    "conflicting-property-ownership-forms-and-atomic-ownership-aware-storage-still-fail-closed-while-general-arc-inference-remains-deferred";
inline constexpr const char *kObjc3ArcSemanticRulesFailClosedModel =
    "forbidden-arc-property-forms-and-non-inferred-lifetime-semantics-terminate-deterministically";
inline constexpr const char *kObjc3ArcSemanticRulesNonGoalModel =
    "no-implicit-retain-release-inference-no-lifetime-extension-no-method-family-based-arc-semantics-yet";
std::string Objc3ArcSemanticRulesSummary();
inline constexpr const char *kObjc3ArcInferenceLifetimeContractId =
    "objc3c.arc.inference.lifetime.v1";
inline constexpr const char *kObjc3ArcInferenceLifetimeSourceModel =
    "explicit-arc-mode-now-infers-strong-owned-executable-object-signatures-for-the-supported-runnable-slice";
inline constexpr const char *kObjc3ArcInferenceLifetimeSemanticModel =
    "arc-enabled-unqualified-object-signatures-now-produce-canonical-retain-release-lifetime-accounting-while-nonarc-remains-zero-inference";
inline constexpr const char *kObjc3ArcInferenceLifetimeFailClosedModel =
    "non-arc-mode-keeps-unqualified-object-signatures-non-inferred-and-zero-retain-release-lifetime-accounting";
inline constexpr const char *kObjc3ArcInferenceLifetimeNonGoalModel =
    "no-full-arc-cleanup-synthesis-no-weak-autorelease-return-property-synthesis-or-block-interaction-arc-semantics-yet";
std::string Objc3ArcInferenceLifetimeSummary();
inline constexpr const char *kObjc3ArcInteractionSemanticsContractId =
    "objc3c.arc.interaction.semantics.v1";
inline constexpr const char *kObjc3ArcInteractionSemanticsSourceModel =
    "explicit-arc-mode-now-covers-weak-autorelease-return-property-synthesis-and-block-ownership-interactions-for-the-supported-runnable-slice";
inline constexpr const char *kObjc3ArcInteractionSemanticsSemanticModel =
    "weak-properties-and-nonowning-captures-stay-nonretaining-autorelease-returns-stay-profiled-and-synthesized-property-accessors-publish-owned-lifetime-packets-under-arc";
inline constexpr const char *kObjc3ArcInteractionSemanticsFailClosedModel =
    "unsupported-arc-cleanup-and-broader-interactions-still-remain-explicitly-deferred";
inline constexpr const char *kObjc3ArcInteractionSemanticsNonGoalModel =
    "no-general-arc-cleanup-insertion-no-cross-module-arc-interop-no-full-method-family-automation-yet";
std::string Objc3ArcInteractionSemanticsSummary();
inline constexpr const char *kObjc3ArcLoweringAbiCleanupModelContractId =
    "objc3c.arc.lowering.abi.cleanup.model.v1";
inline constexpr const char *kObjc3ArcLoweringAbiCleanupModelSourceModel =
    "arc-inference-and-interaction-semantic-packets-feed-one-lane-c-helper-call-and-cleanup-boundary";
inline constexpr const char *kObjc3ArcLoweringAbiCleanupModelAbiModel =
    "private-runtime-helper-call-boundary-over-retain-release-autorelease-weak-property-and-block-helpers";
inline constexpr const char *kObjc3ArcLoweringAbiCleanupModelCleanupModel =
    "helper-call-plus-autoreleasepool-scope-lowering-without-general-cleanup-stack-or-return-slot-optimization";
inline constexpr const char *kObjc3ArcLoweringAbiCleanupModelFailClosedModel =
    "unsupported-ownership-qualified-signatures-and-generalized-arc-cleanups-remain-fail-closed";
inline constexpr const char *kObjc3ArcLoweringAbiCleanupModelNonGoalModel =
    "no-full-arc-automation-no-exception-cleanup-widening-no-objc-runtime-abi-parity-claim";
std::string Objc3ArcLoweringAbiCleanupModelSummary();
inline constexpr const char *kObjc3ErrorHandlingThrowsAbiPropagationLoweringContractId =
    "objc3c.error_handling.throws.abi.propagation.lowering.v1";
inline constexpr const char *kObjc3ErrorHandlingThrowsAbiPropagationLoweringSourceModel =
    "error_handling-semantic-packets-feed-runnable-error-out-abi-propagation-and-catch-dispatch-lowering";
inline constexpr const char *kObjc3ErrorHandlingThrowsAbiPropagationLoweringAbiModel =
    "native-lowering-emits-hidden-error-out-abi-propagation-operators-and-do-catch-control-flow-through-real-ir-and-object-artifacts";
inline constexpr const char *kObjc3ErrorHandlingThrowsAbiPropagationLoweringFailClosedModel =
    "generalized-foreign-exception-abi-and-runtime-bridge-helper-contract-remain-deferred-to-the-error_handling-error-runtime-bridge-helper-boundary";
inline constexpr const char *kObjc3ErrorHandlingThrowsAbiPropagationLoweringNonGoalModel =
    "no-generalized-foreign-exception-abi-no-stable-cross-module-replay-claim-yet";
std::string Objc3ErrorHandlingThrowsAbiPropagationLoweringSummary();
inline constexpr const char *kObjc3ErrorHandlingResultAndBridgingArtifactReplayContractId =
    "objc3c.error_handling.result.and.bridging.artifact.replay.v1";
inline constexpr const char *kObjc3ErrorHandlingResultAndBridgingArtifactReplaySourceModel =
    "error_handling-lowering-replay-keys-survive-object-emission-manifest-emission-and-emitted-sidecar-artifacts";
inline constexpr const char *kObjc3ErrorHandlingResultAndBridgingArtifactReplayModel =
    "provider-and-consumer-sidecar-artifacts-preserve-result-and-bridge-replay-packets-for-separate-compilation-proof";
inline constexpr const char *kObjc3ErrorHandlingResultAndBridgingArtifactReplayFailClosedModel =
    "missing-or-drifted-result-bridge-replay-sidecars-disable-separate-compilation-proof";
inline constexpr const char *kObjc3ErrorHandlingResultAndBridgingArtifactReplaySurfacePath =
    "frontend.pipeline.semantic_surface.objc_error_handling_result_and_bridging_artifact_replay";
inline constexpr const char *kObjc3ErrorHandlingResultAndBridgingArtifactReplayImportArtifactMemberName =
    "objc_error_handling_result_and_bridging_artifact_replay";
inline constexpr const char *kObjc3ErrorHandlingResultAndBridgingArtifactReplayArtifactSuffix =
    ".error_handling-error-replay.json";
std::string Objc3ErrorHandlingResultAndBridgingArtifactReplaySummary();
inline constexpr const char *kObjc3ErrorHandlingErrorRuntimeBridgeHelperContractId =
    "objc3c.error_handling.error.runtime.and.bridge.helper.api.v1";
inline constexpr const char *kObjc3ErrorHandlingErrorRuntimeBridgeHelperSourceModel =
    "error_handling-lowering-routes-error-storage-bridge-normalization-and-catch-dispatch-through-private-runtime-helpers";
inline constexpr const char *kObjc3ErrorHandlingErrorRuntimeBridgeHelperAbiModel =
    "i32-backed-native-error-object-handles-and-catch-kind-matching-remain-private-bootstrap-internal-runtime-abi";
inline constexpr const char *kObjc3ErrorHandlingErrorRuntimeBridgeHelperFailClosedModel =
    "no-public-error_handling-error-runtime-header-widening-no-generalized-foreign-exception-abi-yet";
std::string Objc3ErrorHandlingErrorRuntimeBridgeHelperSummary();
inline constexpr const char *kObjc3ErrorHandlingLiveErrorRuntimeIntegrationContractId =
    "objc3c.error_handling.live.error.runtime.integration.v1";
inline constexpr const char *kObjc3ErrorHandlingLiveErrorRuntimeIntegrationSourceModel =
    "runnable-error_handling-object-code-links-and-executes-through-the-private-error-runtime-helper-cluster";
inline constexpr const char *kObjc3ErrorHandlingLiveErrorRuntimeIntegrationExecutionModel =
    "linked-native-error_handling-fixtures-drive-status-bridge-thrown-error-store-load-and-catch-dispatch-through-runtime-owned-helpers";
inline constexpr const char *kObjc3ErrorHandlingLiveErrorRuntimeIntegrationPackagingModel =
    "driver-emitted-object-and-registration-manifest-artifacts-preserve-runtime-library-link-inputs-for-runnable-error_handling-probes";
inline constexpr const char *kObjc3ErrorHandlingLiveErrorRuntimeIntegrationFailClosedModel =
    "no-public-error-runtime-abi-no-generalized-foreign-exception-support-no-cross-module-live-claim-yet";
std::string Objc3ErrorHandlingLiveErrorRuntimeIntegrationSummary();
// continuation/runtime-helper freeze anchor: lane-D now freezes a
// private Part 7 helper ABI for logical continuation handles, resume traffic,
// and executor handoff without pretending the current direct-call async slice
// already emits live suspension or scheduler traffic through it.
inline constexpr const char *kObjc3ConcurrencyContinuationRuntimeHelperContractId =
    "objc3c.concurrency.continuation.runtime.helper.api.v1";
inline constexpr const char *kObjc3ConcurrencyContinuationRuntimeHelperSourceModel =
    "concurrency-lowering-publishes-a-private-runtime-helper-abi-for-logical-continuation-allocation-resume-and-executor-handoff";
inline constexpr const char *kObjc3ConcurrencyContinuationRuntimeHelperAbiModel =
    "i32-backed-logical-continuation-handles-resume-entry-tags-and-executor-tags-remain-bootstrap-internal-runtime-abi";
inline constexpr const char *kObjc3ConcurrencyContinuationRuntimeHelperExecutionModel =
    "runtime-helpers-materialize-deterministic-logical-continuation-handles-resume-traffic-and-executor-handoff-without-public-header-widening";
inline constexpr const char *kObjc3ConcurrencyContinuationRuntimeHelperFailClosedModel =
    "no-public-async-runtime-header-no-suspension-state-machine-no-executor-runtime-scheduling-claim-yet";
std::string Objc3ConcurrencyContinuationRuntimeHelperSummary();
inline constexpr const char *kObjc3ConcurrencyLiveContinuationRuntimeIntegrationContractId =
    "objc3c.concurrency.live.continuation.runtime.integration.v1";
inline constexpr const char *kObjc3ConcurrencyLiveContinuationRuntimeIntegrationSourceModel =
    "supported-direct-call-await-sites-now-execute-through-the-private-continuation-helper-cluster";
inline constexpr const char *kObjc3ConcurrencyLiveContinuationRuntimeIntegrationExecutionModel =
    "non-suspending-async-functions-and-methods-allocate-handoff-and-resume-logical-continuations-through-runtime-owned-helpers";
inline constexpr const char *kObjc3ConcurrencyLiveContinuationRuntimeIntegrationPackagingModel =
    "driver-emitted-object-artifacts-link-against-the-existing-runtime-support-archive-for-live-concurrency-helper-execution";
inline constexpr const char *kObjc3ConcurrencyLiveContinuationRuntimeIntegrationFailClosedModel =
    "no-suspension-state-machine-no-general-executor-runtime-no-cross-module-live-claim-yet";
std::string Objc3ConcurrencyLiveContinuationRuntimeIntegrationSummary();
// scheduler/executor runtime freeze anchor: lane-D freezes the
// private Part 7 helper/runtime boundary that already exists after C002/C003.
// Task spawn, task-group scope/add/wait/cancel, cancellation polling, executor
// hops, and task-state snapshot publication remain bootstrap-internal runtime
// ABI and do not widen the public runtime header yet.
inline constexpr const char *kObjc3ConcurrencySchedulerExecutorRuntimeContractId =
    "objc3c.concurrency.scheduler.executor.runtime.contract.v1";
inline constexpr const char *kObjc3ConcurrencySchedulerExecutorRuntimeSourceModel =
    "helper-backed-task-runtime-abi-completion-freezes-one-private-scheduler-executor-task-and-cancellation-runtime-boundary";
inline constexpr const char *kObjc3ConcurrencySchedulerExecutorRuntimeAbiModel =
    "private-bootstrap-internal-task-runtime-helpers-and-snapshot-publish-executor-tags-task-state-and-cancellation-observation";
inline constexpr const char *kObjc3ConcurrencySchedulerExecutorRuntimeExecutionModel =
    "runtime-library-materializes-deterministic-task-spawn-task-group-cancellation-and-executor-hop-helper-traffic-without-public-abi-widening";
inline constexpr const char *kObjc3ConcurrencySchedulerExecutorRuntimePackagingModel =
    "native-driver-and-runtime-probes-link-against-the-existing-runtime-support-archive-for-private-task-runtime-helper-execution";
inline constexpr const char *kObjc3ConcurrencySchedulerExecutorRuntimeFailClosedModel =
    "no-public-task-runtime-header-no-general-scheduler-implementation-claim-no-cross-module-task-runtime-claim-yet";
std::string Objc3ConcurrencySchedulerExecutorRuntimeSummary();
// live task runtime anchor: lane-D now publishes the existing helper
// cluster as a live private runtime execution boundary. Supported task spawn,
// task-group, cancellation, and executor-hop traffic executes through the
// runtime library and packaged object/runtime probe path, while broader
// metadata-export and cross-module scheduler work remains deferred.
inline constexpr const char *kObjc3ConcurrencyLiveTaskRuntimeIntegrationContractId =
    "objc3c.concurrency.live.task.runtime.integration.v1";
inline constexpr const char *kObjc3ConcurrencyLiveTaskRuntimeIntegrationSourceModel =
    "supported-task-spawn-task-group-cancellation-and-executor-hop-sites-now-execute-through-the-private-task-runtime-helper-cluster";
inline constexpr const char *kObjc3ConcurrencyLiveTaskRuntimeIntegrationExecutionModel =
    "native-runtime-helpers-materialize-deterministic-task-spawn-task-group-cancellation-and-executor-hop-results-through-linked-runtime-probes";
inline constexpr const char *kObjc3ConcurrencyLiveTaskRuntimeIntegrationPackagingModel =
    "driver-emitted-object-artifacts-and-runtime-probes-link-against-the-existing-runtime-support-archive-for-live-concurrency-task-execution";
inline constexpr const char *kObjc3ConcurrencyLiveTaskRuntimeIntegrationFailClosedModel =
    "retained-runtime-metadata-export-gates-and-no-public-task-runtime-header-mean-broader-native-task-scheduler-claims-remain-deferred";
std::string Objc3ConcurrencyLiveTaskRuntimeIntegrationSummary();
// hardening anchor: lane-D now freezes one truthful edge-case
// completion surface above the live task runtime boundary. The current helper
// cluster must remain deterministic across cancellation paths, autoreleasepool
// scopes, and explicit runtime resets used by issue-local replay probes.
inline constexpr const char *kObjc3ConcurrencyTaskRuntimeHardeningContractId =
    "objc3c.concurrency.task.runtime.hardening.v1";
inline constexpr const char *kObjc3ConcurrencyTaskRuntimeHardeningSourceModel =
    "supported-task-runtime-helper-traffic-now-preserves-cancellation-cleanup-autorelease-scope-and-reset-replay-determinism";
inline constexpr const char *kObjc3ConcurrencyTaskRuntimeHardeningExecutionModel =
    "task-runtime-helper-state-memory-management-scope-state-and-arc-debug-counters-remain-stable-across-reset-and-autorelease-boundaries";
inline constexpr const char *kObjc3ConcurrencyTaskRuntimeHardeningPackagingModel =
    "linked-runtime-probes-consume-the-existing-runtime-support-archive-and-validate-two-pass-reset-stable-task-runtime-state";
inline constexpr const char *kObjc3ConcurrencyTaskRuntimeHardeningFailClosedModel =
    "no-public-task-scheduler-abi-no-cross-module-task-runtime-claim-and-no-broader-front-door-metadata-export-unblock-claim-yet";
std::string Objc3ConcurrencyTaskRuntimeHardeningSummary();
inline constexpr const char *kObjc3ArcAutomaticInsertionContractId =
    "objc3c.arc.automatic.insertion.v1";
inline constexpr const char *kObjc3ArcAutomaticInsertionSourceModel =
    "lane-c-consumes-arc-semantic-insertion-flags-for-supported-function-and-method-param-return-lowering";
inline constexpr const char *kObjc3ArcAutomaticInsertionLoweringModel =
    "owned-params-retain-on-entry-release-on-exit-and-autoreleasing-returns-lower-through-private-runtime-helpers";
inline constexpr const char *kObjc3ArcAutomaticInsertionFailureModel =
    "only-supported-runnable-arc-param-return-insertion-paths-materialize-automatic-helper-calls";
inline constexpr const char *kObjc3ArcAutomaticInsertionNonGoalModel =
    "no-general-local-lifetime-inference-no-full-cleanup-stack-no-cross-module-arc-optimization";
std::string Objc3ArcAutomaticInsertionSummary();
inline constexpr const char *kObjc3ArcCleanupWeakLifetimeHooksContractId =
    "objc3c.arc.cleanup.weak.lifetime.hooks.v1";
inline constexpr const char *kObjc3ArcCleanupWeakLifetimeHooksSourceModel =
    "lane-c-extends-the-supported-arc-lowering-slice-with-scope-exit-cleanups-weak-current-property-hooks-and-block-capture-lifetime-cleanup";
inline constexpr const char *kObjc3ArcCleanupWeakLifetimeHooksLoweringModel =
    "scope-exit-and-implicit-exit-cleanups-unwind-pending-block-dispose-and-arc-owned-storage-while-weak-current-property-access-stays-runtime-hooked";
inline constexpr const char *kObjc3ArcCleanupWeakLifetimeHooksFailureModel =
    "only-supported-scope-cleanup-weak-current-property-and-captured-lifetime-paths-materialize-runtime-lowering";
inline constexpr const char *kObjc3ArcCleanupWeakLifetimeHooksNonGoalModel =
    "no-general-weak-local-storage-lowering-no-exception-cleanup-stack-no-cross-module-arc-optimization";
std::string Objc3ArcCleanupWeakLifetimeHooksSummary();
inline constexpr const char *kObjc3ArcBlockAutoreleaseReturnLoweringContractId =
    "objc3c.arc.block.autorelease.return.lowering.v1";
inline constexpr const char *kObjc3ArcBlockAutoreleaseReturnLoweringSourceModel =
    "lane-c-extends-the-supported-arc-lowering-slice-with-escaping-block-owned-capture-cleanup-and-autorelease-return-conventions";
inline constexpr const char *kObjc3ArcBlockAutoreleaseReturnLoweringModel =
    "escaping-block-promotion-and-terminal-branch-cleanup-compose-with-autoreleasing-returns-without-dropping-live-owned-storage-cleanup";
inline constexpr const char *kObjc3ArcBlockAutoreleaseReturnLoweringFailureModel =
    "only-supported-escaping-block-owned-capture-and-autorelease-return-edge-cases-materialize-runtime-lowering";
inline constexpr const char *kObjc3ArcBlockAutoreleaseReturnLoweringNonGoalModel =
    "no-general-method-family-arc-automation-no-public-runtime-arc-abi-no-cross-module-arc-optimization";
std::string Objc3ArcBlockAutoreleaseReturnLoweringSummary();
inline constexpr const char *kObjc3RuntimeArcHelperApiSurfaceContractId =
    "objc3c.runtime.arc.helper.api.surface.freeze.v1";
inline constexpr const char *kObjc3RuntimeArcHelperApiSurfaceReferenceModel =
    "public-runtime-abi-stays-register-lookup-dispatch-while-arc-helper-entrypoints-remain-private-bootstrap-internal-runtime-abi";
inline constexpr const char *kObjc3RuntimeArcHelperApiSurfaceWeakModel =
    "weak-storage-and-current-property-access-remain-served-through-private-runtime-helper-entrypoints-and-runtime-side-tables";
inline constexpr const char *kObjc3RuntimeArcHelperApiSurfaceAutoreleasepoolModel =
    "autorelease-return-and-autoreleasepool-support-remain-private-runtime-helper-behavior-without-public-abi-widening";
inline constexpr const char *kObjc3RuntimeArcHelperApiSurfaceFailClosedModel =
    "no-public-runtime-arc-helper-api-no-user-facing-arc-runtime-header-widening-yet";
std::string Objc3RuntimeArcHelperApiSurfaceSummary();
inline constexpr const char *kObjc3RuntimeArcHelperRuntimeSupportContractId =
    "objc3c.runtime.arc.helper.runtime.support.v1";
inline constexpr const char *kObjc3RuntimeArcHelperRuntimeSupportDependencyModel =
    "runtime-baseline-plus-runnable-arc-lowering-plus-private-helper-surface";
inline constexpr const char *kObjc3RuntimeArcHelperRuntimeSupportWeakModel =
    "arc-generated-weak-current-property-access-lowers-and-links-through-private-runtime-helper-entrypoints";
inline constexpr const char *kObjc3RuntimeArcHelperRuntimeSupportAutoreleaseReturnModel =
    "arc-generated-autorelease-return-paths-link-and-execute-through-private-runtime-helper-entrypoints";
inline constexpr const char *kObjc3RuntimeArcHelperRuntimeSupportExecutionModel =
    "runtime-library-backed-helper-entrypoints-remain-private-but-executable-through-linked-native-arc-programs";
inline constexpr const char *kObjc3RuntimeArcHelperRuntimeSupportFailClosedModel =
    "unsupported-arc-runtime-surfaces-stay-private-fixture-proven-and-fail-closed-outside-the-supported-slice";
std::string Objc3RuntimeArcHelperRuntimeSupportSummary();
inline constexpr const char *kObjc3RuntimeArcDebugInstrumentationContractId =
    "objc3c.runtime.arc.debug.instrumentation.v1";
inline constexpr const char *kObjc3RuntimeArcDebugInstrumentationDependencyModel =
    "live-helper-runtime-plus-private-bootstrap-internal-debug-snapshots";
inline constexpr const char *kObjc3RuntimeArcDebugInstrumentationCoverageModel =
    "retain-release-autorelease-weak-current-property-and-autoreleasepool-helper-traffic-publishes-deterministic-debug-counters-and-last-value-context";
inline constexpr const char *kObjc3RuntimeArcDebugInstrumentationValidationModel =
    "runtime-probes-and-targeted-arc-fixtures-consume-private-debug-snapshots-without-widening-the-public-runtime-abi";
inline constexpr const char *kObjc3RuntimeArcDebugInstrumentationFailClosedModel =
    "arc-debug-hooks-remain-private-testing-surface-only-and-must-not-claim-broader-runtime-completeness";
std::string Objc3RuntimeArcDebugInstrumentationSummary();
inline constexpr const char *kObjc3RunnableArcRuntimeGateContractId =
    "objc3c.runnable.arc.runtime.gate.v1";
inline constexpr const char *kObjc3RunnableArcRuntimeGateEvidenceModel =
    "source-sema-lowering-runtime-summary-chain";
inline constexpr const char *kObjc3RunnableArcRuntimeGateActiveModel =
    "runnable-arc-gate-consumes-arc-mode-semantics-lowering-and-runtime-proofs-rather-than-parser-only-or-metadata-only-claims";
inline constexpr const char *kObjc3RunnableArcRuntimeGateNonGoalModel =
    "no-runnable-arc-closeout-matrix-no-public-runtime-abi-widening-no-cross-module-arc-claims-before-the-runnable-arc-closeout";
inline constexpr const char *kObjc3RunnableArcRuntimeGateFailClosedModel =
    "fail-closed-on-runnable-arc-runtime-evidence-drift";
std::string Objc3RunnableArcRuntimeGateSummary();
inline constexpr const char *kObjc3RunnableArcCloseoutContractId =
    "objc3c.runnable.arc.closeout.v1";
inline constexpr const char *kObjc3RunnableArcCloseoutMatrixModel =
    "closeout-matrix-consumes-lowering-runtime-and-integrated-evidence-without-widening-the-supported-runnable-arc-slice";
inline constexpr const char *kObjc3RunnableArcCloseoutSmokeModel =
    "integrated-arc-fixtures-and-private-property-runtime-probes-prove-supported-cleanup-block-and-property-behavior-through-native-toolchain-and-runtime";
inline constexpr const char *kObjc3RunnableArcCloseoutFailClosedModel =
    "fail-closed-on-runnable-arc-closeout-drift-or-runbook-mismatch";
std::string Objc3RunnableArcCloseoutSummary();
inline constexpr const char *kObjc3RuntimePushAutoreleasepoolScopeSymbol =
    "objc3_runtime_push_autoreleasepool_scope";
inline constexpr const char *kObjc3RuntimePopAutoreleasepoolScopeSymbol =
    "objc3_runtime_pop_autoreleasepool_scope";
// runtime memory-management API freeze anchor: the stable public
// runtime ABI remains register/lookup/dispatch only, while lowered
// retain/release/autorelease/current-property/weak helper entrypoints stay on
// the private bootstrap-internal surface until later runtime work decides
// whether any of that memory-management API should become public.
inline constexpr const char *kObjc3RuntimeMemoryManagementApiContractId =
    "objc3c.runtime.memory.management.api.freeze.v1";
inline constexpr const char *kObjc3RuntimeMemoryManagementApiReferenceModel =
    "public-runtime-abi-stays-register-lookup-dispatch-while-reference-counting-helpers-remain-private-runtime-entrypoints";
inline constexpr const char *kObjc3RuntimeMemoryManagementApiWeakModel =
    "weak-storage-remains-served-through-private-runtime-helper-entrypoints-and-runtime-side-tables";
inline constexpr const char *kObjc3RuntimeMemoryManagementApiAutoreleasepoolModel =
    "no-public-autoreleasepool-push-pop-api-yet-autorelease-helper-drains-only-on-dispatch-frame-return";
inline constexpr const char *kObjc3RuntimeMemoryManagementApiFailClosedModel =
    "no-public-memory-management-header-widening-no-user-facing-arc-entrypoints-yet";
// runtime memory-management implementation anchor: runtime-backed
// objects now execute refcount, weak-table, and autoreleasepool semantics
// through private runtime helpers while preserving the frozen D001 public ABI.
inline constexpr const char *kObjc3RuntimeMemoryManagementImplementationContractId =
    "objc3c.runtime.memory.management.implementation.v1";
inline constexpr const char *kObjc3RuntimeMemoryManagementImplementationRefcountModel =
    "runtime-managed-instance-retain-counts-destroy-strong-owned-storage-on-final-release";
inline constexpr const char *kObjc3RuntimeMemoryManagementImplementationWeakModel =
    "weak-side-table-tracks-runtime-storage-observers-and-zeroes-them-on-final-release";
inline constexpr const char *kObjc3RuntimeMemoryManagementImplementationAutoreleasepoolModel =
    "private-autoreleasepool-push-pop-scopes-retain-autoreleased-runtime-values-until-lifo-drain";
inline constexpr const char *kObjc3RuntimeMemoryManagementImplementationFailClosedModel =
    "memory-management-runtime-support-remains-private-lowered-and-runtime-probe-driven";
// ownership-runtime-gate freeze anchor: lane-E now freezes the
// supported ownership runtime baseline and its non-goals so later integration
// issues cannot silently claim ARC-, block-, or public-ABI-level support.
inline constexpr const char *kObjc3OwnershipRuntimeGateContractId =
    "objc3c.ownership.runtime.gate.freeze.v1";
inline constexpr const char *kObjc3OwnershipRuntimeGateSupportedModel =
    "runtime-backed-object-baseline-proves-strong-weak-and-autoreleasepool-behavior-through-private-runtime-hooks";
inline constexpr const char *kObjc3OwnershipRuntimeGateEvidenceModel =
    "gate-consumes-ownership-runtime-contract-summaries-and-runtime-probe-evidence";
inline constexpr const char *kObjc3OwnershipRuntimeGateNonGoalModel =
    "no-arc-automation-no-block-ownership-runtime-no-public-ownership-api-widening";
inline constexpr const char *kObjc3OwnershipRuntimeGateFailClosedModel =
    "integration-gate-must-not-claim-more-than-the-supported-runtime-backed-ownership-baseline";
// executable method-body binding implementation anchor: lane-C now
// hardens the existing executable object surface so implementation-owned
// method entries must bind to exactly one concrete LLVM definition symbol and
// object emission fails closed when that attachment is missing or ambiguous.
inline constexpr const char *kObjc3ExecutableMethodBodyBindingContractId =
    "objc3c.executable.method.body.binding.v1";
inline constexpr const char *kObjc3ExecutableMethodBodyBindingSourceModel =
    "implementation-owned-method-entry-owner-identity-selects-one-llvm-definition-symbol";
inline constexpr const char *kObjc3ExecutableMethodBodyBindingRuntimeModel =
    "emitted-method-entry-implementation-pointer-dispatches-through-objc3_runtime_dispatch_i32";
inline constexpr const char *kObjc3ExecutableMethodBodyBindingFailClosedModel =
    "error-on-missing-or-duplicate-implementation-binding";
// executable realization-record expansion anchor: lane-C extends the
// executable object surface with realization-ready class, protocol, and
// category records that preserve the frontend-owned owner/super/adoption edges
// directly in emitted artifacts instead of forcing later runtime work to
// recover them out-of-band.
inline constexpr const char *kObjc3ExecutableRealizationRecordsContractId =
    "objc3c.executable.realization.records.v1";
inline constexpr const char *kObjc3ExecutableRealizationClassRecordModel =
    "class-and-metaclass-records-carry-bundle-object-and-super-owner-identities-plus-method-list-refs";
inline constexpr const char *kObjc3ExecutableRealizationProtocolRecordModel =
    "protocol-records-carry-owner-inherited-protocol-edges-and-split-instance-class-method-counts";
inline constexpr const char *kObjc3ExecutableRealizationCategoryRecordModel =
    "category-records-carry-explicit-class-and-category-owner-identities-plus-attachment-and-adopted-protocol-edges";
inline constexpr const char *kObjc3ExecutableRealizationFailClosedModel =
    "no-identity-edge-elision-no-out-of-band-graph-reconstruction";
// class-realization-runtime freeze anchor: lane-D now freezes the
// current runtime-owned class realization surface that consumes emitted
// realization records, walks class/metaclass chains, attaches preferred
// category implementation records, and uses protocol records as
// declaration-aware negative lookup evidence only.
inline constexpr const char *kObjc3RuntimeClassRealizationContractId =
    "objc3c.runtime.class.realization.freeze.v1";
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
// metaclass-graph-root-class anchor: lane-D now promotes the frozen
// D001 runtime boundary into a runtime-owned realized class graph keyed by
// stable receiver identities, with explicit root-class publication and
// metaclass-edge inventory available to later runtime lanes.
inline constexpr const char *kObjc3RuntimeMetaclassGraphRootClassContractId =
    "objc3c.runtime.metaclass.graph.root.class.baseline.v1";
inline constexpr const char *kObjc3RuntimeRealizedClassGraphModel =
    "runtime-owned-realized-class-nodes-bind-receiver-base-identities-to-class-and-metaclass-records";
inline constexpr const char *kObjc3RuntimeRootClassBaselineModel =
    "root-classes-realize-with-null-superclass-links-and-live-instance-plus-class-dispatch";
inline constexpr const char *kObjc3RuntimeRealizedClassGraphFailClosedModel =
    "missing-receiver-bindings-or-broken-realized-superclass-links-fall-closed-to-compatibility-dispatch";
inline constexpr const char *kObjc3RuntimeCategoryAttachmentProtocolConformanceContractId =
    "objc3c.runtime.category.attachment.protocol.conformance.v1";
inline constexpr const char *kObjc3RuntimeCategoryAttachmentRealizedGraphModel =
    "realized-class-nodes-own-preferred-category-attachments-after-registration";
inline constexpr const char *kObjc3RuntimeProtocolConformanceQueryModel =
    "runtime-protocol-conformance-queries-walk-class-category-and-inherited-protocol-closures";
inline constexpr const char *kObjc3RuntimeAttachmentConformanceFailClosedModel =
    "invalid-attachment-owner-identities-or-broken-protocol-refs-disable-runtime-attachment-queries";
inline constexpr const char *kObjc3RuntimeCanonicalRunnableObjectSampleSupportContractId =
    "objc3c.runtime.canonical.runnable.object.sample.support.v1";
inline constexpr const char *kObjc3RuntimeCanonicalRunnableObjectExecutionModel =
    "canonical-object-samples-use-runtime-owned-alloc-new-init-and-realized-class-dispatch";
inline constexpr const char *kObjc3RuntimeCanonicalRunnableObjectProbeSplitModel =
    "metadata-rich-object-samples-prove-category-and-protocol-runtime-behavior-through-library-plus-probe-splits";
inline constexpr const char *kObjc3RuntimeCanonicalRunnableObjectFailClosedModel =
    "metadata-heavy-executable-samples-stay-library-probed-until-runtime-export-gates-open";
// binary inspection harness expansion anchor: lane-C now freezes one
// emitted-metadata inspection corpus over llvm-readobj/llvm-objdump so every
// currently emitted metadata section family can be asserted structurally from
// produced objects, including fail-closed negative gating when metadata
// compilation stops before object emission.
inline constexpr const char *kObjc3RuntimeBinaryInspectionHarnessContractId =
    "objc3c.runtime.binary.inspection.harness.v1";
inline constexpr const char *kObjc3RuntimeBinaryInspectionPositiveCorpusModel =
    "positive-structural-section-and-symbol-corpus-with-case-specific-absence-checks";
inline constexpr const char *kObjc3RuntimeBinaryInspectionNegativeCorpusModel =
    "negative-compile-failure-gating-with-no-object-inspection";
inline constexpr const char *kObjc3RuntimeBinaryInspectionSectionCommand =
    "llvm-readobj --sections module.obj";
inline constexpr const char *kObjc3RuntimeBinaryInspectionSymbolCommand =
    "llvm-objdump --syms module.obj";
// object-packaging/retention freeze anchor: lane-D now freezes the
// current produced-object boundary around module.obj plus retained aggregate
// metadata symbols. Later archive/link/startup-registration work must preserve
// these anchors instead of redefining the object boundary ad hoc.
inline constexpr const char *kObjc3RuntimeObjectPackagingRetentionContractId =
    "objc3c.runtime.object.packaging.retention.boundary.v1";
inline constexpr const char *kObjc3RuntimeObjectPackagingRetentionBoundaryModel =
    "current-object-file-boundary-with-retained-metadata-section-aggregates";
inline constexpr const char *kObjc3RuntimeObjectPackagingRetentionAnchorModel =
    "llvm.used-plus-aggregate-section-symbols";
inline constexpr const char *kObjc3RuntimeObjectPackagingRetentionArtifact =
    "module.obj";
inline constexpr const char *kObjc3RuntimeObjectPackagingRetentionSymbolPrefix =
    "__objc3_sec_";
// linker-retention/dead-strip resistance anchor: lane-D now adds one
// public linker anchor plus one public discovery root over the retained
// metadata aggregates. The compiler publishes a driver-friendly linker-response
// file and a discovery JSON sidecar so single-library packaging can survive
// dead stripping without claiming the later multi-archive/TU edge cases.
inline constexpr const char *kObjc3RuntimeLinkerRetentionContractId =
    "objc3c.runtime.linker.retention.and.dead.strip.resistance.v1";
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
// archive/static-link discovery anchor: lane-D now closes the
// remaining multi-archive fan-in and cross-translation-unit discovery path by
// making linker-anchor identity translation-unit-stable and by standardizing
// one merged discovery/response artifact pair for downstream archive link
// orchestration.
inline constexpr const char *kObjc3RuntimeArchiveStaticLinkDiscoveryContractId =
    "objc3c.runtime.metadata.archive.and.static.link.discovery.v1";
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
// constructor-root/init-array lowering freeze anchor: the existing
// bootstrap-lowering surface is now the canonical live lowering contract for
// constructor roots, derived init stubs, registration tables, and
// llvm.global_ctors participation. The registration-descriptor artifact plus
// the emitted registration manifest remain the authoritative lowering inputs
// that later multi-image work must preserve rather than reconstructing symbol
// names ad hoc in IR emission or the driver.
inline constexpr const char *kObjc3RuntimeBootstrapLoweringContractId =
    "objc3c.runtime.constructor.root.init.array.lowering.v1";
inline constexpr const char *kObjc3RuntimeBootstrapLoweringBoundaryModel =
    "registration-descriptor-and-registration-manifest-drive-constructor-root-init-stub-registration-table-and-platform-init-array-lowering";
inline constexpr const char
    *kObjc3RuntimeBootstrapRegistrationDescriptorHandoffContractId =
        "objc3c.runtime.registration.descriptor.frontend.closure.v1";
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
// registration-descriptor/image-root lowering anchor: lane-C now
// materializes the registration-descriptor and image-root identities as real
// emitted globals in dedicated object sections so later multi-image runtime
// bootstrap can consume binary artifacts instead of only sidecar manifests.
inline constexpr const char
    *kObjc3RuntimeBootstrapRegistrationDescriptorImageRootLoweringContractId =
        "objc3c.runtime.registration.descriptor.and.image.root.lowering.v1";
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
// archive/static-link bootstrap replay corpus anchor: lane-C now
// binds the earlier archive/static-link retention/discovery proof to the live
// bootstrap replay runtime so archive-linked images are validated through one
// retained-binary corpus instead of section-only inspection.
inline constexpr const char
    *kObjc3RuntimeBootstrapArchiveStaticLinkReplayCorpusContractId =
        "objc3c.runtime.bootstrap.archive.static.link.replay.corpus.v1";
inline constexpr const char
    *kObjc3RuntimeBootstrapArchiveStaticLinkReplayCorpusModel =
        "merged-archive-static-link-discovery-artifacts-drive-live-bootstrap-replay-probes";
inline constexpr const char
    *kObjc3RuntimeBootstrapArchiveStaticLinkReplayCorpusBinaryProofModel =
        "plain-link-omits-bootstrap-images-retained-link-replays-them";
inline constexpr const char *kObjc3RuntimeBootstrapRegistrationTableLayoutModel =
    "abi-version-field-count-image-descriptor-discovery-root-linker-anchor-family-aggregates-selector-string-pools-keypath-descriptors-image-local-init-state";
inline constexpr const char *kObjc3RuntimeBootstrapImageLocalInitializationModel =
    "guarded-once-per-image-local-state-cell";
inline constexpr std::uint64_t kObjc3RuntimeBootstrapRegistrationTableAbiVersion =
    2u;
inline constexpr std::uint64_t
    kObjc3RuntimeBootstrapRegistrationTablePointerFieldCount = 12u;
// versioned conformance-report lowering freeze anchor: lane-C
// lowers the truthful runnable/source-only/unsupported claim packets into one
// emitted machine-readable sidecar artifact. Later runtime capability and
// driver publication issues must preserve this versioned lowering boundary
// rather than reconstructing claim truth ad hoc from docs or release evidence.
inline constexpr const char *kObjc3VersionedConformanceReportLoweringContractId =
    "objc3c.versioned.conformance.report.lowering.v1";
inline constexpr const char
    *kObjc3VersionedConformanceReportLoweringSemanticContractId =
        "objc3c.compatibility.strictness.claim.semantics.v1";
inline constexpr const char
    *kObjc3VersionedConformanceReportLoweringArtifactSuffix =
        ".objc3-conformance-report.json";
inline constexpr const char
    *kObjc3VersionedConformanceReportLoweringArtifactSchemaId =
        "objc3c-versioned-conformance-report-v1";
inline constexpr const char
    *kObjc3VersionedConformanceReportLoweringSurfacePath =
        "frontend.pipeline.semantic_surface.objc_versioned_conformance_report_lowering_contract";
inline constexpr const char
    *kObjc3VersionedConformanceReportLoweringPayloadModel =
        "frontend-truth-packets-lower-into-one-versioned-machine-readable-conformance-sidecar";
inline constexpr const char
    *kObjc3VersionedConformanceReportLoweringAuthorityModel =
        "runnable-feature-inventory-plus-truth-surface-plus-fail-closed-semantics";
inline constexpr const char
    *kObjc3VersionedConformanceReportKnownUnsupportedModel =
        "unsupported-claims-remain-published-as-known-unsupported-without-runnable-overclaim";
inline constexpr const char
    *kObjc3VersionedConformanceReportSelectionModel =
        "canonical-and-legacy-compatibility-selection-only-strictness-and-concurrency-claims-remain-fail-closed";
inline constexpr const char
    *kObjc3VersionedConformanceReportCanonicalInterfaceMode =
        "no-standalone-interface-payload-yet";
inline constexpr const char
    *kObjc3VersionedConformanceReportPublicationModel =
        "written-next-to-manifest-when-out-dir-is-present";
// runtime capability reporting anchor: lane-C turns the versioned
// conformance sidecar into a truthful machine-readable capability and public
// conformance-report payload that later driver/CLI publication lanes can emit
// directly without inventing a second truth surface.
inline constexpr const char *kObjc3RuntimeCapabilityReportingContractId =
    "objc3c.runtime.capability.reporting.v1";
inline constexpr const char *kObjc3RuntimeCapabilityReportingSchemaId =
    "objc3c-runtime-capability-report-v1";
inline constexpr const char *kObjc3RuntimeCapabilityReportingSurfacePath =
    "frontend.pipeline.semantic_surface.objc_runtime_capability_report";
inline constexpr const char *kObjc3RuntimeCapabilityReportingProfileModel =
    "core-and-strict-profiles-claimed-while-optional-feature-gaps-remain-not-claimed-until-runtime-backed";
inline constexpr const char *kObjc3RuntimeCapabilityReportingOptionalFeatureModel =
    "unsupported-runtime-feature-ids-lower-into-not-claimed-public-optional-features";
inline constexpr const char *kObjc3RuntimeCapabilityReportingVersionModel =
    "deterministic-dev-version-surface-for-frontend-runtime-stdlib-and-module-format";
inline constexpr const char *kObjc3RuntimeCapabilityPublicSchemaId =
    "objc3-conformance-report/v1";
inline constexpr const char *kObjc3RuntimeCapabilityGeneratedAtReplayValue =
    "1970-01-01T00:00:00Z";
inline constexpr const char *kObjc3RuntimeCapabilityToolchainName = "objc3c";
inline constexpr const char *kObjc3RuntimeCapabilityToolchainVendor =
    "doublemover";
inline constexpr const char *kObjc3RuntimeCapabilityToolchainVersion =
    "0.0.0-dev";
inline constexpr const char *kObjc3RuntimeCapabilityTargetTriple =
    "x86_64-pc-windows-msvc";
inline constexpr const char *kObjc3RuntimeCapabilityLanguageFamily =
    "objective-c";
inline constexpr const char *kObjc3RuntimeCapabilityLanguageVersion = "3.0";
inline constexpr const char *kObjc3RuntimeCapabilitySpecRevision = "v1";
inline constexpr const char *kObjc3RuntimeCapabilityStrictnessMode =
    "permissive";
inline constexpr const char *kObjc3RuntimeCapabilityConcurrencyMode = "off";
inline constexpr const char
    *kObjc3ToolingMachineReadableConformanceReportContractId =
        "objc3c.tooling.machine.readable.conformance.report.contract.v1";
inline constexpr const char
    *kObjc3ToolingMachineReadableConformanceReportDependencyContractId =
        "objc3c.tooling.legacy.canonical.migration.semantics.v1";
inline constexpr const char
    *kObjc3ToolingMachineReadableConformanceReportSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_tooling_machine_readable_conformance_report_contract";
inline constexpr const char
    *kObjc3ToolingMachineReadableConformanceReportPayloadModel =
        "tooling-advanced-feature-truth-reuses-the-existing-versioned-conformance-sidecar-and-runtime-capability-publication-path";
inline constexpr const char
    *kObjc3ToolingMachineReadableConformanceReportAuthorityModel =
        "tooling-machine-readable-reporting-remains-bounded-to-the-lowered-versioned-conformance-sidecar-and-live-migration-semantics";
inline constexpr const char
    *kObjc3ToolingFeatureAwareConformanceReportEmissionContractId =
        "objc3c.tooling.feature.aware.conformance.report.emission.v1";
inline constexpr const char
    *kObjc3ToolingFeatureAwareConformanceReportEmissionDependencyContractId =
        "objc3c.tooling.machine.readable.conformance.report.contract.v1";
inline constexpr const char
    *kObjc3ToolingFeatureAwareConformanceReportEmissionSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_tooling_feature_aware_conformance_report_emission";
inline constexpr const char
    *kObjc3ToolingFeatureAwareConformanceReportEmissionPayloadModel =
        "versioned-conformance-report-now-embeds-tooling-feature-aware-migration-and-fixit-state-without-introducing-a-second-report-sidecar";
inline constexpr const char
    *kObjc3ToolingFeatureAwareConformanceReportEmissionAuthorityModel =
        "tooling-feature-aware-reporting-remains-bounded-to-live-fixit-migration-and-machine-readable-report-contract-surfaces";
inline constexpr const char
    *kObjc3ToolingCorpusShardingReleaseEvidencePackagingContractId =
        "objc3c.tooling.corpus.sharding.release.evidence.packaging.v1";
inline constexpr const char
    *kObjc3ToolingCorpusShardingReleaseEvidencePackagingDependencyContractId =
        "objc3c.tooling.feature.aware.conformance.report.emission.v1";
inline constexpr const char
    *kObjc3ToolingCorpusShardingReleaseEvidencePackagingSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_tooling_corpus_sharding_release_evidence_packaging";
inline constexpr const char
    *kObjc3ToolingCorpusShardingReleaseEvidencePackagingPayloadModel =
        "versioned-conformance-report-now-embeds-tooling-corpus-shard-and-release-evidence-packaging-without-introducing-a-parallel-report-format";
inline constexpr const char
    *kObjc3ToolingCorpusShardingReleaseEvidencePackagingAuthorityModel =
        "tooling-release-evidence-packaging-remains-bounded-to-emitted-report-payloads-checklist-refs-and-stable-conformance-bucket-manifests";
inline constexpr const char *kObjc3RuntimeCapabilityModuleFormatVersion =
    "objc3c-runtime-metadata-v1";
// metadata-emission gate anchor: lane-E now freezes the upstream
// object-emission evidence contract over A002/B003/C006/D003 so later closeout
// work must fail closed if the source-to-section matrix, object-format policy,
// binary inspection corpus, or archive/static-link discovery proof drifts.
inline constexpr const char *kObjc3RuntimeMetadataEmissionGateContractId =
    "objc3c.runtime.metadata.emission.gate.v1";
inline constexpr const char *kObjc3RuntimeMetadataEmissionGateEvidenceModel =
    "source-sema-ir-runtime-summary-chain";
inline constexpr const char *kObjc3RuntimeMetadataEmissionGateFailureModel =
    "fail-closed-on-upstream-summary-drift";
// cross-lane object-emission closeout anchor: lane-E now freezes one
// integrated closeout over the E001 summary chain plus fresh native object
// probes so later startup-registration work can trust the same emitted objects
// on the class/category/message-send paths.
inline constexpr const char *kObjc3RuntimeMetadataObjectEmissionCloseoutContractId =
    "objc3c.runtime.cross.lane.object.emission.closeout.v1";
inline constexpr const char *kObjc3RuntimeMetadataObjectEmissionCloseoutEvidenceModel =
    "integrated-summary-plus-native-object-emission-probes";
inline constexpr const char *kObjc3RuntimeMetadataObjectEmissionCloseoutFailureModel =
    "fail-closed-on-summary-or-integrated-probe-drift";
// normalized layout policy anchor: semantic finalization of runtime
// metadata ordering, visibility, relocation, and retention now flows through
// one lowering-owned normalized policy packet before the IR emitter materializes
// globals. The emitter consumes the normalized plan directly instead of
// hardcoding family order or relocation semantics ad hoc.
inline constexpr const char *kObjc3RuntimeMetadataLayoutPolicyContractId =
    "objc3c.runtime.metadata.layout.policy.v1";
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
    "objc3c.method.lookup.override.conflict.v1";
inline constexpr const char *kObjc3PropertySynthesisIvarBindingLaneContract =
    "objc3c.property.synthesis.ivar.binding.v1";
inline constexpr const char *kObjc3IdClassSelObjectPointerTypecheckLaneContract =
    "objc3c.id.class.sel.object.pointer.typecheck.v1";
inline constexpr const char *kObjc3MessageSendSelectorLoweringLaneContract =
    "objc3c.message.send.selector.lowering.v1";
inline constexpr const char *kObjc3DispatchAbiMarshallingLaneContract =
    "objc3c.dispatch.abi.marshalling.v1";
inline constexpr const char *kObjc3NilReceiverSemanticsFoldabilityLaneContract =
    "objc3c.nil.receiver.semantics.foldability.v1";
inline constexpr const char *kObjc3SuperDispatchMethodFamilyLaneContract =
    "objc3c.super.dispatch.method.family.v1";
inline constexpr const char *kObjc3RuntimeShimHostLinkLaneContract =
    "objc3c.runtime.shim.host.link.v1";
inline constexpr const char *kObjc3OwnershipQualifierLoweringLaneContract =
    "objc3c.ownership.qualifier.lowering.v1";
inline constexpr const char *kObjc3RetainReleaseOperationLoweringLaneContract =
    "objc3c.retain.release.operation.lowering.v1";
inline constexpr const char *kObjc3AutoreleasePoolScopeLoweringLaneContract =
    "objc3c.autoreleasepool.scope.lowering.v1";
inline constexpr const char *kObjc3WeakUnownedSemanticsLoweringLaneContract =
    "objc3c.weak.unowned.semantics.lowering.v1";
inline constexpr const char *kObjc3ArcDiagnosticsFixitLoweringLaneContract =
    "objc3c.arc.diagnostics.fixit.lowering.v1";
inline constexpr const char *kObjc3BlockLiteralCaptureLoweringLaneContract =
    "objc3c.block.literal.capture.lowering.v1";
inline constexpr const char *kObjc3BlockAbiInvokeTrampolineLoweringLaneContract =
    "objc3c.block.abi.invoke.trampoline.lowering.v1";
inline constexpr const char *kObjc3BlockStorageEscapeLoweringLaneContract =
    "objc3c.block.storage.escape.lowering.v1";
inline constexpr const char *kObjc3BlockCopyDisposeLoweringLaneContract =
    "objc3c.block.copy.dispose.lowering.v1";
inline constexpr const char *kObjc3BlockDeterminismPerfBaselineLoweringLaneContract =
    "objc3c.block.determinism.perf.baseline.lowering.v1";
inline constexpr const char *kObjc3BlockSourceModelCompletionLaneContract =
    "objc3c.block.source.model.v1";
inline constexpr const char *kObjc3BlockSourceStorageAnnotationLaneContract =
    "objc3c.block.source.storage.annotations.v1";
// capture-legality/escape/invocation implementation anchor: lane-B
// keeps the same runtime-summary lane contract while source-only sema grows
// live capture-resolution, truthful escape classification, and local callable
// invocation typing before runnable block lowering lands.
inline constexpr const char *kObjc3BlockRuntimeSemanticRulesLaneContract =
    "objc3c.block.runtime.semantic.rules.v1";
inline constexpr const char
    *kObjc3ExecutableBlockLoweringAbiArtifactBoundaryLaneContract =
        "objc3c.block.lowering.abi.artifact.boundary.v1";
inline constexpr const char *kObjc3ExecutableBlockObjectInvokeThunkLoweringLaneContract =
    "objc3c.block.object.invoke.thunk.lowering.v1";
inline constexpr const char *kObjc3ExecutableBlockByrefHelperLoweringLaneContract =
    "objc3c.block.byref.helper.lowering.v1";
inline constexpr const char *kObjc3ExecutableBlockEscapeRuntimeHookLoweringLaneContract =
    "objc3c.block.escape.runtime.hook.lowering.v1";
inline constexpr const char *kObjc3TypeSystemOptionalKeypathLoweringLaneContract =
    "objc3c.type_system.optional.keypath.lowering.v1";
inline constexpr const char *kObjc3ControlFlowControlFlowSafetyLoweringLaneContract =
    "objc3c.control_flow.control.flow.safety.lowering.v1";
inline constexpr const char *kObjc3LightweightGenericsConstraintLoweringLaneContract =
    "objc3c.lightweight.generics.constraint.lowering.v1";
inline constexpr const char *kObjc3NullabilityFlowWarningPrecisionLoweringLaneContract =
    "objc3c.nullability.flow.warning.precision.lowering.v1";
inline constexpr const char *kObjc3ProtocolQualifiedObjectTypeLoweringLaneContract =
    "objc3c.protocol.qualified.object.type.lowering.v1";
inline constexpr const char *kObjc3VarianceBridgeCastLoweringLaneContract =
    "objc3c.variance.bridge.cast.lowering.v1";
inline constexpr const char *kObjc3GenericMetadataAbiLoweringLaneContract =
    "objc3c.generic.metadata.abi.lowering.v1";
inline constexpr const char *kObjc3ModuleImportGraphLoweringLaneContract =
    "objc3c.module.import.graph.lowering.v1";
inline constexpr const char *kObjc3NamespaceCollisionShadowingLoweringLaneContract =
    "objc3c.namespace.collision.shadowing.lowering.v1";
inline constexpr const char *kObjc3PublicPrivateApiPartitionLoweringLaneContract =
    "objc3c.public.private.api.partition.lowering.v1";
inline constexpr const char *kObjc3IncrementalModuleCacheInvalidationLoweringLaneContract =
    "objc3c.incremental.module.cache.invalidation.lowering.v1";
inline constexpr const char *kObjc3CrossModuleConformanceLoweringLaneContract =
    "objc3c.cross.module.conformance.lowering.v1";
inline constexpr const char *kObjc3ThrowsPropagationLoweringLaneContract =
    "objc3c.throws.propagation.lowering.v1";
inline constexpr const char *kObjc3ResultLikeLoweringLaneContract =
    "objc3c.result.like.lowering.v1";
inline constexpr const char *kObjc3NSErrorBridgingLoweringLaneContract =
    "objc3c.ns.error.bridging.lowering.v1";
inline constexpr const char *kObjc3UnwindCleanupLoweringLaneContract =
    "objc3c.unwind.cleanup.lowering.v1";
inline constexpr const char *kObjc3ErrorDiagnosticsRecoveryLoweringLaneContract =
    "objc3c.error.diagnostics.recovery.lowering.v1";
inline constexpr const char *kObjc3AsyncContinuationLoweringLaneContract =
    "objc3c.async.continuation.lowering.v1";
inline constexpr const char
    *kObjc3AwaitLoweringSuspensionStateLoweringLaneContract =
        "objc3c.await.lowering.suspension.state.lowering.v1";
// freeze anchor: these lane contracts become the explicit Part 7
// continuation ABI / await suspension lowering boundary once frontend artifacts
// publish the replay-stable handoff packet into emitted manifests and IR.
// implementation anchor: the currently supported runnable lowering
// slice is narrower than the lane-contract names imply. Async entry points and
// await-marked expressions lower through the existing direct-call/object path
// only for the non-suspending happy path, while continuation allocation,
// suspend/resume, and executor scheduling remain deferred.
// integration anchor: the same supported async slice composes with
// existing autoreleasepool-scope and defer-cleanup lowering rather than a
// separate suspension cleanup runtime. Later work still has to widen that
// into real suspension-frame cleanup and executor resume behavior.
inline constexpr const char *kObjc3ActorIsolationSendabilityLoweringLaneContract =
    "objc3c.actor.isolation.sendability.lowering.v1";
// lowering-freeze anchor: Part 7 actor lowering now freezes one
// dedicated emitted contract for actor metadata carriage, isolation-thunk
// planning, and hop-artifact planning. Live thunk bodies and runtime entrypoints
// remain later C002/C003 work, but this packet is the canonical lowering
// handoff for actor-focused IR metadata and manifest truth.
inline constexpr const char *kObjc3ConcurrencyActorLoweringMetadataContractId =
    "objc3c.concurrency.actor.lowering.and.metadata.contract.v1";
inline constexpr const char *kObjc3ConcurrencyActorLoweringMetadataSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_concurrency_actor_lowering_and_metadata_contract";
inline constexpr const char *kObjc3ConcurrencyActorLoweringMetadataModel =
    "actor-member-semantic-and-hazard-packets-now-lower-through-one-deterministic-actor-metadata-isolation-thunk-and-hop-artifact-contract";
inline constexpr const char *kObjc3ConcurrencyActorLoweringMetadataDeferredModel =
    "live-actor-thunk-bodies-mailbox-runtime-entrypoints-and-runnable-cross-actor-scheduling-remain-later-actor-lowering-and-replay-runtime-work";
inline constexpr const char *kObjc3ConcurrencyActorLoweringMetadataLaneContract =
    "objc3c.actor.lowering.metadata.contract.v1";
// lowering-freeze anchor: Part 8 now freezes one explicit emitted
// lowering contract that carries cleanup/resource ownership counts, borrowed
// boundary counts, and retainable-family callable counts into manifests and IR
// metadata without overclaiming live runtime cleanup or borrowed lifetime
// execution. Runtime integration must widen this exact contract family rather than inventing
// a second system-extension lowering boundary.
// lowering-implementation anchor: live Part 8 lowering now consumes
// this frozen contract directly for native helper emission and object proof.
// Stack/local cleanup-resource capture lowering is implemented; actual escaping
// move-capture promotion stays fail-closed until later runtime ownership
// transfer work widens the same boundary explicitly.
inline constexpr const char *kObjc3OwnershipSystemExtensionLoweringContractId =
    "objc3c.ownership.system.extension.lowering.contract.v1";
inline constexpr const char *kObjc3OwnershipSystemExtensionLoweringSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_ownership_system_extension_lowering_contract";
inline constexpr const char *kObjc3OwnershipSystemExtensionLoweringModel =
    "cleanup-resource-borrowed-and-retainable-family-sema-packets-now-feed-one-deterministic-ownership-lowering-contract-for-manifest-and-ir-carriage";
inline constexpr const char *kObjc3OwnershipSystemExtensionLoweringDeferredModel =
    "live-cleanup-runtime-carriers-borrowed-lifetime-enforcement-and-runnable-retainable-family-runtime-interop-remain-later-system-extension-runtime-work";
inline constexpr const char *kObjc3OwnershipSystemExtensionLoweringLaneContract =
    "objc3c.ownership.system.extension.lowering.contract.v1";
// lowering-freeze anchor: Part 9 now freezes one explicit emitted
// lowering contract for direct-call candidates, final/sealed dispatch-boundary
// metadata, and replay-stable callable/container dynamism intent carriage. Live
// selector-bypass/direct-call rewrites and runnable dispatch-boundary behavior
// remain later lane-C and lane-D work.
inline constexpr const char *kObjc3DispatchDispatchControlLoweringContractId =
    "objc3c.dispatch.dispatch.control.lowering.contract.v1";
inline constexpr const char *kObjc3DispatchDispatchControlLoweringSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_dispatch_dispatch_control_lowering_contract";
inline constexpr const char *kObjc3DispatchDispatchControlLoweringModel =
    "dispatch-direct-call-candidates-final-sealed-boundaries-and-dynamism-intent-metadata-now-feed-one-deterministic-lowering-contract-for-manifest-and-ir-carriage";
inline constexpr const char *kObjc3DispatchDispatchControlLoweringDeferredModel =
    "live-direct-call-selector-bypass-runtime-dispatch-boundary-realization-and-runnable-metadata-consumption-remain-later-dispatch-control-runtime-work";
inline constexpr const char *kObjc3DispatchDispatchControlLoweringLaneContract =
    "objc3c.dispatch.dispatch.control.lowering.contract.v1";
// expansion/lowering freeze anchor: lane-C freezes one deterministic
// Part 10 lowering packet over derived selector inventory, macro replay
// visibility, and synthesized property metadata carriage. Runnable derive body
// emission, macro execution, and property-behavior runtime hooks remain later
// work.
inline constexpr const char *kObjc3MetaprogrammingExpansionLoweringContractId =
    "objc3c.metaprogramming.expansion.lowering.contract.v1";
inline constexpr const char *kObjc3MetaprogrammingExpansionLoweringSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_metaprogramming_expansion_and_lowering_contract";
inline constexpr const char *kObjc3MetaprogrammingExpansionLoweringModel =
    "metaprogramming-derived-selector-inventory-macro-replay-visibility-and-synthesized-property-metadata-now-feed-one-deterministic-lowering-contract-for-manifest-and-ir-carriage";
inline constexpr const char *kObjc3MetaprogrammingExpansionLoweringDeferredModel =
    "runnable-derive-body-emission-macro-execution-and-property-behavior-runtime-materialization-remain-later-expansion-runtime-work";
inline constexpr const char *kObjc3MetaprogrammingExpansionLoweringLaneContract =
    "objc3c.metaprogramming.expansion.lowering.contract.v1";
inline constexpr const char *kObjc3MetaprogrammingSynthesizedArtifactEmissionContractId =
    "objc3c.metaprogramming.synthesized.ast.ir.emission.v1";
inline constexpr const char *kObjc3MetaprogrammingSynthesizedArtifactEmissionSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_metaprogramming_synthesized_ast_and_ir_emission";
inline constexpr const char *kObjc3MetaprogrammingSynthesizedArtifactEmissionModel =
    "supported-derive-macro-and-property-behavior-sites-now-materialize-deterministic-synthesized-ir-artifacts-and-runtime-visible-method-bodies";
inline constexpr const char
    *kObjc3MetaprogrammingSynthesizedArtifactEmissionDeferredModel =
        "cross-module-preservation-expansion-host-execution-and-cached-macro-toolchain-integration-remain-deferred-to-later-runtime-work";
inline constexpr const char *kObjc3MetaprogrammingSynthesizedArtifactEmissionLaneContract =
    "objc3c.metaprogramming.synthesized.ast.ir.emission.v1";
// preservation anchor: lane-C extends the local C002 synthesis win by
// preserving derive/macro/property-behavior replay facts, interface-facing
// artifact splits, and deterministic import-surface summaries across module
// boundaries before lane-D runtime execution work begins.
inline constexpr const char
    *kObjc3MetaprogrammingModuleInterfaceReplayPreservationContractId =
        "objc3c.metaprogramming.module.interface.replay.preservation.v1";
inline constexpr const char
    *kObjc3MetaprogrammingModuleInterfaceReplayPreservationSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_metaprogramming_module_interface_and_replay_preservation";
inline constexpr const char
    *kObjc3MetaprogrammingModuleInterfaceReplayPreservationImportArtifactMemberName =
        "objc_metaprogramming_module_interface_and_replay_preservation";
inline constexpr const char
    *kObjc3MetaprogrammingModuleInterfaceReplayPreservationSourceModel =
        "runtime-import-surface-artifacts-preserve-metaprogramming-derived-method-macro-and-property-behavior-replay-facts-for-separate-compilation-and-interface-inspection";
inline constexpr const char
    *kObjc3MetaprogrammingModuleInterfaceReplayPreservationModel =
        "provider-and-consumer-import-surfaces-preserve-metaprogramming-synthesized-emission-counts-replay-keys-and-interface-vs-implementation-property-behavior-splits-beyond-local-ir-object-emission";
inline constexpr const char
    *kObjc3MetaprogrammingModuleInterfaceReplayPreservationFailClosedModel =
        "missing-or-drifted-metaprogramming-preservation-packets-disable-cross-module-metaprogramming-preservation-claims";
// host/runtime-boundary freeze anchor: lane-D freezes the truthful
// Part 10 runtime boundary around the existing packaged runtime archive and
// private property-accessor helpers while keeping macro host execution and
// runtime package loading explicitly disabled and fail-closed.
inline constexpr const char *kObjc3MetaprogrammingExpansionHostRuntimeBoundaryContractId =
    "objc3c.metaprogramming.expansion.host.runtime.boundary.v1";
inline constexpr const char
    *kObjc3MetaprogrammingExpansionHostRuntimeBoundarySourceContractId =
        "objc3c.metaprogramming.module.interface.replay.preservation.v1";
inline constexpr const char *kObjc3MetaprogrammingExpansionHostRuntimeBoundaryHostModel =
    "macro-host-execution-process-launch-and-runtime-package-loading-remain-disabled-and-fail-closed";
inline constexpr const char
    *kObjc3MetaprogrammingExpansionHostRuntimeBoundaryPropertyRuntimeModel =
        "supported-property-behavior-lowering-reuses-existing-private-runtime-property-accessor-layout-and-current-property-hooks";
inline constexpr const char
    *kObjc3MetaprogrammingExpansionHostRuntimeBoundaryPackagingModel =
        "native-driver-packaging-still-hands-off-metaprogramming-runtime-support-through-artifacts-lib-objc3_runtime-lib-and-runtime-registration-manifests";
inline constexpr const char
    *kObjc3MetaprogrammingExpansionHostRuntimeBoundaryFailClosedModel =
        "no-live-macro-expansion-host-or-runtime-package-loader-is-claimed-yet";
inline constexpr const char
    *kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationContractId =
        "objc3c.metaprogramming.macro.host.process.cache.runtime.integration.v1";
inline constexpr const char
    *kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSourceContractId =
        "objc3c.metaprogramming.expansion.host.runtime.boundary.v1";
inline constexpr const char
    *kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_metaprogramming_macro_host_process_and_cache_runtime_integration";
inline constexpr const char
    *kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationImportArtifactMemberName =
        "objc_metaprogramming_macro_host_process_and_cache_runtime_integration";
inline constexpr const char
    *kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationHostExecutableRelativePath =
        "artifacts/bin/objc3c-frontend-c-api-runner.exe";
inline constexpr const char
    *kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationCacheRootRelativePath =
        "tmp/artifacts/objc3c-native/cache/metaprogramming";
inline constexpr const char
    *kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationHostModel =
        "native-driver-launches-objc3c-frontend-c-api-runner-for-supported-metaprogramming-expansion-cache-materialization";
inline constexpr const char
    *kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationToolchainModel =
        "frontend-runner-executes-with-manifest-enabled-and-ir-object-emission-disabled-for-deterministic-cache-materialization";
inline constexpr const char
    *kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationCacheModel =
        "cache-entry-path-is-derived-from-a-stable-fnv1a64-key-over-the-metaprogramming-replay-surface-and-reused-on-subsequent-runs";
inline constexpr const char
    *kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationFailClosedModel =
        "missing-runner-corrupt-cache-or-import-surface-drift-disables-metaprogramming-host-process-cache-claims";
// lowering-freeze anchor: Part 11 now freezes one explicit emitted
// lowering contract for foreign callable ABI carriage, ownership/error bridge
// interaction counts, Swift-facing concurrency metadata, and interface/import
// preservation facts across FFI boundaries. Live bridge helper emission and
// runnable foreign-call execution remain later lane-C and lane-D work.
inline constexpr const char *kObjc3InteropInteropLoweringContractId =
    "objc3c.interop.interop.lowering.and.abi.contract.v1";
inline constexpr const char *kObjc3InteropInteropLoweringSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_interop_interop_lowering_and_abi_contract";
inline constexpr const char *kObjc3InteropInteropLoweringModel =
    "interop-foreign-callable-sema-runtime-parity-cpp-interaction-swift-isolation-and-interface-preservation-packets-now-feed-one-deterministic-lowering-contract-for-manifest-and-ir-carriage";
inline constexpr const char *kObjc3InteropInteropLoweringDeferredModel =
    "live-ffi-call-lowering-ownership-bridge-helper-emission-error-runtime-integration-and-cross-module-runtime-consumption-remain-later-interop-runtime-work";
inline constexpr const char *kObjc3InteropInteropLoweringLaneContract =
    "objc3c.interop.interop.lowering.abi.contract.v1";
// lowering-implementation anchor: lane-C now materializes a second
// Part 11 packet over the actual callable-lowering slice, proving that foreign,
// C++-annotated, and Swift-facing call boundaries emit deterministic ownership
// and lifetime bridge operations in IR rather than stopping at the frozen C001
// contract surface.
inline constexpr const char *kObjc3InteropForeignCallLifetimeLoweringContractId =
    "objc3c.interop.foreign.call.and.lifetime.lowering.v1";
inline constexpr const char *kObjc3InteropForeignCallLifetimeLoweringSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_interop_foreign_call_and_lifetime_lowering";
inline constexpr const char *kObjc3InteropForeignCallLifetimeLoweringModel =
    "foreign-calls-and-cpp-swift-facing-free-functions-now-lower-through-one-deterministic-interop-call-boundary-that-preserves-ownership-lifetime-and-annotation-facts-in-manifest-and-ir";
inline constexpr const char
    *kObjc3InteropForeignCallLifetimeLoweringDeferredModel =
        "cross-module-runtime-consumption-live-foreign-linking-and-runnable-host-language-integration-remain-later-interop-closeout-work";
inline constexpr const char
    *kObjc3InteropForeignCallLifetimeLoweringDependencyContractId =
        kObjc3InteropInteropLoweringContractId;
// preservation-expansion anchor: lane-C now publishes one explicit
// replay/import packet proving Part 11 callable and annotation metadata survive
// separate compilation through the runtime-import-surface artifact and emitted
// IR payloads instead of remaining local-only facts from A003/C002.
inline constexpr const char
    *kObjc3InteropFfiMetadataInterfacePreservationContractId =
        "objc3c.interop.ffi.metadata.interface.preservation.v1";
inline constexpr const char
    *kObjc3InteropFfiMetadataInterfacePreservationSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_interop_ffi_metadata_and_interface_preservation";
inline constexpr const char
    *kObjc3InteropFfiMetadataInterfacePreservationImportArtifactMemberName =
        "objc_interop_ffi_metadata_and_interface_preservation";
inline constexpr const char
    *kObjc3InteropFfiMetadataInterfacePreservationSourceContractId =
        kObjc3InteropForeignCallLifetimeLoweringContractId;
inline constexpr const char
    *kObjc3InteropFfiMetadataInterfacePreservationPreservationModel =
        "provider-runtime-import-surfaces-now-preserve-interop-callable-and-annotation-metadata-beyond-local-manifest-ir-and-object-emission";
inline constexpr const char
    *kObjc3InteropFfiMetadataInterfacePreservationSourceModel =
        "local-lane-c-interop-lowering-and-lane-a-preservation-packets-feed-one-runtime-import-surface-summary-for-separate-compilation-replay";
inline constexpr const char
    *kObjc3InteropFfiMetadataInterfacePreservationFailClosedModel =
        "missing-import-surface-packet-drifted-replay-keys-or-non-deterministic-provider-preservation-disables-cross-module-interop-preservation-claims";
// packaging/toolchain freeze anchor: lane-D now freezes the truthful
// Part 11 bridge-packaging/toolchain boundary around the existing packaged
// runtime archive, registration-manifest and cross-module link-plan topology,
// and operator-visible interop evidence artifacts while header/module/bridge
// generation remain explicitly deferred to the next runtime step.
inline constexpr const char
    *kObjc3InteropBridgePackagingToolchainContractId =
        "objc3c.interop.bridge.packaging.and.toolchain.contract.v1";
inline constexpr const char
    *kObjc3InteropBridgePackagingToolchainSourceContractId =
        kObjc3InteropFfiMetadataInterfacePreservationContractId;
inline constexpr const char
    *kObjc3InteropBridgePackagingToolchainPreservationContractId =
        "objc3c.interop.foreign.surface.interface.preservation.v1";
inline constexpr const char
    *kObjc3InteropBridgePackagingToolchainPackagingModel =
        "runtime-registration-manifests-runtime-import-surfaces-cross-module-link-plans-and-linker-response-sidecars-are-the-current-toolchain-visible-interop-packaging-topology";
inline constexpr const char
    *kObjc3InteropBridgePackagingToolchainEvidenceModel =
        "operator-visible-interop-evidence-is-published-through-the-packaged-runtime-archive-registration-manifest-cross-module-link-plan-and-ir-summary";
inline constexpr const char
    *kObjc3InteropBridgePackagingToolchainFailClosedModel =
        "header-module-and-bridge-generation-remain-unclaimed-until-next-runtime-phase";
inline constexpr const char
    *kObjc3InteropHeaderModuleBridgeGenerationLoweringContractId =
        "objc3c.interop.header.module.and.bridge.generation.v1";
inline constexpr const char
    *kObjc3InteropHeaderModuleBridgeGenerationLoweringSourceContractId =
        "objc3c.interop.bridge.packaging.and.toolchain.contract.v1";
inline constexpr const char
    *kObjc3InteropHeaderModuleBridgeGenerationLoweringPreservationContractId =
        "objc3c.interop.ffi.metadata.interface.preservation.v1";
inline constexpr const char
    *kObjc3InteropHeaderModuleBridgeGenerationLoweringPackagingModel =
        "compiler-emits-deterministic-interop-bridge-header-modulemap-and-bridge-json-artifacts-and-preserves-them-through-runtime-import-surfaces-and-cross-module-link-plans";
inline constexpr const char
    *kObjc3InteropHeaderModuleBridgeGenerationLoweringFailClosedModel =
        "missing-generated-bridge-artifacts-or-drifted-import-surface-paths-disable-live-interop-bridge-generation-claims";
// preservation anchor: lane-C now preserves the direct/final/sealed
// intent introduced by C002 through runtime metadata source records, emitted
// runtime-import-surface artifacts, and replay-stable frontend metadata instead
// of restricting those facts to local IR/object payloads only.
inline constexpr const char
    *kObjc3DispatchDispatchMetadataInterfacePreservationContractId =
        "objc3c.dispatch.dispatch.metadata.interface.preservation.v1";
inline constexpr const char
    *kObjc3DispatchDispatchMetadataInterfacePreservationSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_dispatch_dispatch_metadata_and_interface_preservation";
inline constexpr const char
    *kObjc3DispatchDispatchMetadataInterfacePreservationImportArtifactMemberName =
        "objc_dispatch_dispatch_metadata_and_interface_preservation";
inline constexpr const char
    *kObjc3DispatchDispatchMetadataInterfacePreservationSourceModel =
        "runtime-metadata-source-records-and-runtime-import-surface-artifacts-preserve-direct-final-sealed-intent-for-separate-compilation-and-interface-replay";
inline constexpr const char
    *kObjc3DispatchDispatchMetadataInterfacePreservationModel =
        "provider-and-consumer-runtime-import-surface-artifacts-preserve-direct-final-sealed-dispatch-intent-beyond-local-ir-object-emission";
inline constexpr const char
    *kObjc3DispatchDispatchMetadataInterfacePreservationFailClosedModel =
        "missing-or-drifted-dispatch-intent-preservation-packets-disable-cross-module-dispatch-preservation-claims";
// ABI/artifact completion anchor: keep the frozen Part 8 lowering
// contract from C001 as the single lowering boundary, but publish one
// dedicated ABI/replay packet above it for borrowed-return contracts and
// retainable-family callable inventories so lane-D runtime work can consume a
// stable artifact surface instead of re-deriving those call-boundary facts.
inline constexpr const char *kObjc3OwnershipBorrowedRetainableAbiCompletionContractId =
    "objc3c.ownership.borrowed.retainable.family.abi.completion.v1";
inline constexpr const char *kObjc3OwnershipBorrowedRetainableAbiCompletionSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_ownership_borrowed_pointer_and_retainable_family_abi_completion";
inline constexpr const char *kObjc3OwnershipBorrowedRetainableAbiCompletionLaneContract =
    "objc3c.ownership.borrowed.retainable.family.abi.completion.v1";
// runtime/helper-freeze anchor: lane-D does not invent a second
// Part 8 runtime subsystem. Cleanup execution, resource invalidation, and
// retainable-family helper integration freeze the already-live private ARC /
// autorelease / snapshot helper slice plus the existing packaged runtime
// archive path. Borrowed lifetime runtime enforcement and escaping
// cleanup/resource ownership transfer remain later work.
inline constexpr const char *kObjc3OwnershipSystemHelperRuntimeContractId =
    "objc3c.ownership.system.helper.runtime.contract.v1";
inline constexpr const char *kObjc3OwnershipSystemHelperRuntimeSourceModel =
    "cleanup-resource-invalidation-and-retainable-family-runtime-proof-reuses-existing-private-arc-autorelease-and-snapshot-helpers";
inline constexpr const char *kObjc3OwnershipSystemHelperRuntimeAbiModel =
    "private-retain-release-autorelease-autoreleasepool-and-testing-snapshot-helper-cluster";
inline constexpr const char *kObjc3OwnershipSystemHelperRuntimePackagingModel =
    "same-packaged-runtime-archive-no-public-runtime-header-widening-and-no-new-ownership-import-surface";
inline constexpr const char *kObjc3OwnershipSystemHelperRuntimeFailClosedModel =
    "borrowed-lifetime-runtime-enforcement-and-escaping-resource-ownership-transfer-remain-deferred";
// live runtime-integration anchor: the supported Part 8 cleanup /
// retainable-family slice now proves actual linked execution through the same
// private helper cluster frozen in D001 plus emitted scope-exit cleanup calls.
inline constexpr const char *kObjc3OwnershipLiveCleanupRetainableIntegrationContractId =
    "objc3c.ownership.live.cleanup.retainable.runtime.integration.v1";
inline constexpr const char *kObjc3OwnershipLiveCleanupRetainableIntegrationSourceModel =
    "supported-ownership-cleanup-resource-and-retainable-family-sites-now-link-and-execute-through-emitted-cleanup-calls-and-the-private-helper-cluster";
inline constexpr const char *kObjc3OwnershipLiveCleanupRetainableIntegrationExecutionModel =
    "linked-native-probes-execute-lifo-cleanup-resource-invalidation-and-retainable-family-helper-traffic-on-the-supported-slice";
inline constexpr const char *kObjc3OwnershipLiveCleanupRetainableIntegrationPackagingModel =
    "linked-module-object-plus-existing-runtime-support-archive-no-new-runtime-package-surface";
inline constexpr const char *kObjc3OwnershipLiveCleanupRetainableIntegrationFailClosedModel =
    "borrowed-lifetime-runtime-enforcement-and-escaping-resource-ownership-transfer-remain-deferred";
inline constexpr const char *kObjc3TaskRuntimeInteropCancellationLoweringLaneContract =
    "objc3c.task.runtime.interop.cancellation.lowering.v1";
inline constexpr const char *kObjc3ConcurrencyReplayRaceGuardLoweringLaneContract =
    "objc3c.concurrency.replay.race.guard.lowering.v1";
// lowering-freeze anchor: these existing Part 7 concurrency lane
// contracts now become the explicit task-runtime lowering handoff for task
// creation, executor hops, cancellation polling, and task-group artifacts.
// Later lane-C work must widen the same contract family with native
// spawn/hop/cancel entrypoints and task-group ABI completion rather than
// inventing a second lowering boundary.
inline constexpr const char *kObjc3ConcurrencyTaskRuntimeLoweringContractId =
    "objc3c.concurrency.task.runtime.lowering.contract.v1";
inline constexpr const char *kObjc3ConcurrencyTaskRuntimeLoweringSurfacePath =
    "frontend.pipeline.semantic_surface.objc_concurrency_task_runtime_lowering_contract";
// ABI/artifact completion anchor: the supported Part 7 helper-backed
// lowering slice now also publishes a dedicated runtime-ABI packet and IR
// boundary so later lane-D runtime freezes consume a stable artifact surface
// instead of rediscovering helper names ad hoc.
inline constexpr const char *kObjc3ConcurrencyTaskRuntimeAbiCompletionContractId =
    "objc3c.concurrency.task.runtime.abi.completion.v1";
inline constexpr const char *kObjc3ConcurrencyTaskRuntimeAbiCompletionSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_concurrency_task_group_and_runtime_abi_completion";
inline constexpr const char *kObjc3UnsafePointerExtensionLoweringLaneContract =
    "objc3c.unsafe.pointer.extension.gating.lowering.v1";
inline constexpr const char *kObjc3InlineAsmIntrinsicGovernanceLoweringLaneContract =
    "objc3c.inline.asm.intrinsic.governance.lowering.v1";

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
  std::size_t interface_owned_property_synthesis_sites = 0;
  std::size_t implementation_property_redeclaration_sites = 0;
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

struct Objc3TypeSystemOptionalKeypathLoweringContract {
  std::size_t optional_binding_sites = 0;
  std::size_t optional_binding_clause_sites = 0;
  std::size_t optional_send_sites = 0;
  std::size_t nil_coalescing_sites = 0;
  std::size_t typed_keypath_literal_sites = 0;
  std::size_t typed_keypath_self_root_sites = 0;
  std::size_t typed_keypath_class_root_sites = 0;
  std::size_t live_optional_lowering_sites = 0;
  std::size_t single_evaluation_nil_short_circuit_sites = 0;
  std::size_t live_typed_keypath_artifact_sites = 0;
  std::size_t deferred_typed_keypath_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ControlFlowControlFlowSafetyLoweringContract {
  std::size_t guard_statement_sites = 0;
  std::size_t guard_clause_sites = 0;
  std::size_t match_statement_sites = 0;
  std::size_t defer_statement_sites = 0;
  std::size_t live_guard_short_circuit_sites = 0;
  std::size_t live_match_dispatch_sites = 0;
  std::size_t live_defer_cleanup_sites = 0;
  std::size_t fail_closed_guard_short_circuit_sites = 0;
  std::size_t fail_closed_match_dispatch_sites = 0;
  std::size_t fail_closed_defer_cleanup_sites = 0;
  std::size_t deterministic_fail_closed_sites = 0;
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

struct Objc3ActorLoweringMetadataContract {
  std::size_t actor_interface_sites = 0;
  std::size_t actor_method_sites = 0;
  std::size_t actor_metadata_record_sites = 0;
  std::size_t nonisolated_entry_sites = 0;
  std::size_t executor_affinity_sites = 0;
  std::size_t actor_hop_artifact_sites = 0;
  std::size_t actor_isolation_thunk_sites = 0;
  std::size_t replay_proof_dependency_sites = 0;
  std::size_t race_guard_dependency_sites = 0;
  std::size_t task_handoff_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3OwnershipSystemExtensionLoweringContract {
  std::size_t cleanup_hook_sites = 0;
  std::size_t resource_local_sites = 0;
  std::size_t cleanup_owned_local_sites = 0;
  std::size_t resource_move_capture_sites = 0;
  std::size_t borrowed_parameter_sites = 0;
  std::size_t borrowed_return_callable_sites = 0;
  std::size_t borrowed_escape_candidate_sites = 0;
  std::size_t explicit_capture_item_sites = 0;
  std::size_t retainable_family_callable_sites = 0;
  std::size_t retainable_family_operation_callable_sites = 0;
  std::size_t retainable_family_alias_callable_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3DispatchDispatchControlLoweringContract {
  std::size_t direct_call_candidate_sites = 0;
  std::size_t direct_members_defaulted_sites = 0;
  std::size_t dynamic_opt_out_sites = 0;
  std::size_t final_container_sites = 0;
  std::size_t sealed_container_sites = 0;
  std::size_t override_legality_sites = 0;
  std::size_t metadata_preserved_callable_sites = 0;
  std::size_t metadata_preserved_container_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3MetaprogrammingExpansionLoweringContract {
  std::size_t derive_inventory_sites = 0;
  std::size_t derived_selector_artifact_sites = 0;
  std::size_t macro_replay_visible_sites = 0;
  std::size_t property_behavior_sites = 0;
  std::size_t synthesized_binding_sites = 0;
  std::size_t synthesized_getter_sites = 0;
  std::size_t synthesized_setter_sites = 0;
  std::size_t replay_visible_metadata_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3InteropInteropLoweringContract {
  std::size_t foreign_callable_sites = 0;
  std::size_t c_foreign_callable_sites = 0;
  std::size_t objc_runtime_parity_callable_sites = 0;
  std::size_t ownership_bridge_callable_sites = 0;
  std::size_t error_surface_sites = 0;
  std::size_t async_boundary_sites = 0;
  std::size_t swift_concurrency_metadata_sites = 0;
  std::size_t interface_preserved_foreign_callable_sites = 0;
  std::size_t interface_preserved_metadata_annotation_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3InteropForeignCallLifetimeLoweringContract {
  std::size_t foreign_callable_sites = 0;
  std::size_t c_foreign_callable_sites = 0;
  std::size_t objc_runtime_parity_callable_sites = 0;
  std::size_t ownership_bridge_sites = 0;
  std::size_t lifetime_bridge_sites = 0;
  std::size_t metadata_preservation_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3InteropFfiMetadataInterfacePreservationContract {
  std::size_t local_foreign_callable_count = 0;
  std::size_t local_metadata_preservation_sites = 0;
  std::size_t local_interface_annotation_sites = 0;
  std::size_t imported_module_count = 0;
  std::size_t imported_foreign_callable_count = 0;
  std::size_t imported_metadata_preservation_sites = 0;
  std::size_t imported_interface_annotation_sites = 0;
  bool runtime_import_artifact_ready = false;
  bool separate_compilation_preservation_ready = false;
  bool deterministic = false;
};

struct Objc3InteropBridgePackagingToolchainContract {
  bool packaging_topology_ready = false;
  bool operator_visible_evidence_ready = false;
  bool header_generation_ready = false;
  bool module_generation_ready = false;
  bool bridge_generation_ready = false;
  bool deterministic = false;
};

struct Objc3MetaprogrammingSynthesizedArtifactEmissionContract {
  std::size_t derive_inventory_sites = 0;
  std::size_t emitted_derive_method_sites = 0;
  std::size_t emitted_macro_artifact_sites = 0;
  std::size_t emitted_property_behavior_artifact_sites = 0;
  std::size_t emitted_global_artifact_sites = 0;
  std::size_t emitted_runtime_method_list_sites = 0;
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
// Part 3 lowering freeze anchor: native lowering now
// truthfully freezes the live optional binding/send/optional-member-access/
// coalescing path with single-evaluation nil short-circuit semantics and, as
// of the later lowering step, lowers validated typed key-path literals into retained native
// descriptor artifacts with stable runtime handles while broader key-path
// execution remains a later runtime milestone.
inline constexpr const char *kObjc3TypeSystemOptionalKeypathLoweringContractId =
    "objc3c.type_system.optional.keypath.lowering.v1";
inline constexpr const char *kObjc3ControlFlowControlFlowSafetyLoweringContractId =
    "objc3c.control_flow.control.flow.safety.lowering.v1";
inline constexpr const char *kObjc3ControlFlowControlFlowSafetyLoweringSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_control_flow_control_flow_safety_lowering_contract";
inline constexpr const char *kObjc3TypeSystemOptionalKeypathLoweringOptionalModel =
    "optional-bindings-sends-optional-member-access-and-coalescing-lower-natively-with-single-evaluation-and-nil-short-circuit";
inline constexpr const char *kObjc3TypeSystemOptionalKeypathLoweringTypedKeypathModel =
    "validated-single-component-typed-keypath-literals-lower-to-canonical-runtime-descriptor-handles-with-generic-metadata-preservation";
inline constexpr const char *kObjc3TypeSystemOptionalKeypathLoweringAuthorityModel =
    "type_system-semantic-summary-plus-message-send-selector-dispatch-and-nil-receiver-lowering-contracts";
inline constexpr const char *kObjc3TypeSystemOptionalKeypathLoweringFailClosedModel =
    "native-lowering-fails-closed-on-lowering-contract-drift-and-on-semantically-unsupported-typed-keypath-shapes";
inline constexpr const char *kObjc3ControlFlowControlFlowSafetyLoweringGuardModel =
    "native-lowering-executes-guard-clauses-via-short-circuit-control-flow-and-else-edge-cleanup";
inline constexpr const char *kObjc3ControlFlowControlFlowSafetyLoweringMatchModel =
    "native-lowering-executes-literal-default-wildcard-and-binding-match-arms-while-result-case-patterns-remain-explicitly-fail-closed";
inline constexpr const char *kObjc3ControlFlowControlFlowSafetyLoweringDeferModel =
    "native-lowering-registers-defer-cleanups-per-scope-and-emits-lifo-cleanup-insertion-on-scope-exit";
inline constexpr const char *kObjc3ControlFlowControlFlowSafetyLoweringAuthorityModel =
    "control_flow-source-closure-plus-control_flow-semantic-model-own-the-current-lowering-boundary";
inline constexpr const char *kObjc3ControlFlowControlFlowSafetyLoweringFailClosedModel =
    "native-ir-emission-fails-closed-with-o3l300-on-result-case-match-patterns-until-a-runtime-result-payload-abi-lands";
inline constexpr const char *kObjc3TypeSystemOptionalKeypathRuntimeHelperContractId =
    "objc3c.type_system.optional.keypath.runtime.helper.contract.v1";
inline constexpr const char
    *kObjc3TypeSystemOptionalKeypathRuntimeHelperSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_type_system_optional_keypath_runtime_helper_contract";
inline constexpr const char
    *kObjc3TypeSystemOptionalKeypathRuntimeHelperOptionalModel =
        "optional-send-and-optional-member-access-sites-use-lowering-owned-nil-short-circuit-plus-public-runtime-selector-lookup-dispatch";
inline constexpr const char
    *kObjc3TypeSystemOptionalKeypathRuntimeHelperTypedKeypathModel =
        "validated-single-component-typed-keypath-sites-publish-stable-descriptor-handles-and-retained-descriptor-sections-while-runtime-evaluation-helpers-remain-a-follow-on-private-runtime-step";
inline constexpr const char
    *kObjc3TypeSystemOptionalKeypathRuntimeHelperDiagnosticModel =
        "unsupported-typed-keypath-shapes-and-non-objc-optional-member-access-fail-closed-before-runtime";
std::string Objc3TypeSystemOptionalKeypathLoweringSummary();
std::string Objc3ControlFlowControlFlowSafetyLoweringSummary();
std::string Objc3TypeSystemOptionalKeypathRuntimeHelperContractSummary();
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
std::string Objc3ToolingMachineReadableConformanceReportContractLoweringSummary();
std::string Objc3ToolingFeatureAwareConformanceReportEmissionLoweringSummary();
std::string Objc3ToolingCorpusShardingReleaseEvidencePackagingLoweringSummary();
std::string Objc3VersionedConformanceReportLoweringContractSummary();
std::string Objc3RuntimeCapabilityReportingContractSummary();
std::string Objc3OwnershipSystemHelperRuntimeContractSummary();
std::string Objc3OwnershipLiveCleanupRetainableIntegrationSummary();
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
bool IsValidObjc3TypeSystemOptionalKeypathLoweringContract(
    const Objc3TypeSystemOptionalKeypathLoweringContract &contract);
std::string Objc3TypeSystemOptionalKeypathLoweringReplayKey(
    const Objc3TypeSystemOptionalKeypathLoweringContract &contract);
bool IsValidObjc3ControlFlowControlFlowSafetyLoweringContract(
    const Objc3ControlFlowControlFlowSafetyLoweringContract &contract);
std::string Objc3ControlFlowControlFlowSafetyLoweringReplayKey(
    const Objc3ControlFlowControlFlowSafetyLoweringContract &contract);
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
bool IsValidObjc3ActorLoweringMetadataContract(
    const Objc3ActorLoweringMetadataContract &contract);
std::string Objc3ActorLoweringMetadataReplayKey(
    const Objc3ActorLoweringMetadataContract &contract);
bool IsValidObjc3OwnershipSystemExtensionLoweringContract(
    const Objc3OwnershipSystemExtensionLoweringContract &contract);
std::string Objc3OwnershipSystemExtensionLoweringReplayKey(
    const Objc3OwnershipSystemExtensionLoweringContract &contract);
bool IsValidObjc3DispatchDispatchControlLoweringContract(
    const Objc3DispatchDispatchControlLoweringContract &contract);
std::string Objc3DispatchDispatchControlLoweringReplayKey(
    const Objc3DispatchDispatchControlLoweringContract &contract);
bool IsValidObjc3MetaprogrammingExpansionLoweringContract(
    const Objc3MetaprogrammingExpansionLoweringContract &contract);
std::string Objc3MetaprogrammingExpansionLoweringReplayKey(
    const Objc3MetaprogrammingExpansionLoweringContract &contract);
bool IsValidObjc3InteropInteropLoweringContract(
    const Objc3InteropInteropLoweringContract &contract);
std::string Objc3InteropInteropLoweringReplayKey(
    const Objc3InteropInteropLoweringContract &contract);
bool IsValidObjc3InteropForeignCallLifetimeLoweringContract(
    const Objc3InteropForeignCallLifetimeLoweringContract &contract);
std::string Objc3InteropForeignCallLifetimeLoweringReplayKey(
    const Objc3InteropForeignCallLifetimeLoweringContract &contract);
bool IsValidObjc3InteropFfiMetadataInterfacePreservationContract(
    const Objc3InteropFfiMetadataInterfacePreservationContract &contract);
std::string Objc3InteropFfiMetadataInterfacePreservationReplayKey(
    const Objc3InteropFfiMetadataInterfacePreservationContract &contract);
std::string Objc3InteropBridgePackagingToolchainSummary();
std::string Objc3InteropHeaderModuleBridgeGenerationBoundarySummary();
bool IsValidObjc3MetaprogrammingSynthesizedArtifactEmissionContract(
    const Objc3MetaprogrammingSynthesizedArtifactEmissionContract &contract);
std::string Objc3MetaprogrammingSynthesizedArtifactEmissionReplayKey(
    const Objc3MetaprogrammingSynthesizedArtifactEmissionContract &contract);
std::string Objc3MetaprogrammingModuleInterfaceReplayPreservationSummary();
std::string Objc3MetaprogrammingExpansionHostRuntimeBoundarySummary();
std::string Objc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSummary();
std::string Objc3DispatchDispatchMetadataInterfacePreservationSummary();
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
