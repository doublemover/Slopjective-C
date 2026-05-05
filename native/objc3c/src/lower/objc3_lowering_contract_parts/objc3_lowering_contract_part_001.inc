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
    *kObjc3RuntimeDispatchLiveCutoverLanguageProfileModel =
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
inline constexpr const char *kObjc3RuntimeLookupDispatchLanguageProfileModel =
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
// runtime path rather than the non-authoritative test surface. The upstream gate chain
// stays rooted on A002/B003/C004/D004, the non-authoritative test surface remains
// exported only as evidence/test surface, and E002 is the next issue allowed
// to replace non-authoritative smoke/closeout assumptions with the integrated gate.
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
// live-dispatch smoke/replay closeout anchor: lane-E now replaces
// the last non-authoritative smoke/replay assumptions with canonical live runtime
// dispatch evidence. Execution smoke and replay must publish
// requires_live_runtime_dispatch, the canonical runtime library path, the
// non-authoritative test surface as non-authoritative evidence only, and the canonical
// default symbol objc3_runtime_dispatch_i32.
inline constexpr const char *kObjc3RuntimeLiveDispatchSmokeReplayCloseoutContractId =
    "objc3c.runtime.live.dispatch.smoke.replay.closeout.v1";
inline constexpr const char *kObjc3RuntimeLiveDispatchSmokeReplayCloseoutModel =
    "live-runtime-dispatch-smoke-and-replay-authoritative-test-surface-non-authoritative";
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
    "property-descriptor-records-with-accessor-binding-and-sema-ivar-layout-fields";
inline constexpr const char *kObjc3RuntimeIvarDescriptorEmissionPayloadModel =
    "ivar-descriptor-records-with-property-binding-layout-replay-key-and-offset-global";
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
        "ivar-descriptor-bundles-carry-sema-approved-layout-symbol-replay-key-slot-offset-size-alignment-padding-inheritance-owner-size-records";
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
    "ivar-descriptor-records-carry-layout-symbol-replay-key-offset-global-slot-offset-size-alignment-padding-inheritance-owner-size-ordering";
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
inline constexpr const char *kObjc3RuntimeBackedSemanticsClosureContractId =
    "objc3c.runtime.backed.semantics.closure.v1";
inline constexpr const char *kObjc3RuntimeBackedSemanticsClosureSurfacePath =
    "reports/claimability/runtime-backed-semantics-closure";
inline constexpr const char *kObjc3RuntimeBackedSemanticsClosureEvidenceModel =
    "durable-source-sema-lowering-ir-runtime-fixture-report-chain";
inline constexpr const char *kObjc3RuntimeBackedSemanticsClosureBlockModel =
    "escaping-block-promotion-byref-forwarding-copy-dispose-and-captured-lifetime-lowering-route-through-private-block-runtime-helpers";
inline constexpr const char *kObjc3RuntimeBackedSemanticsClosureArcModel =
    "arc-retain-release-autorelease-weak-autoreleasepool-and-cleanup-ordering-route-through-private-runtime-helpers";
inline constexpr const char *kObjc3RuntimeBackedSemanticsClosureErrorModel =
    "throws-error-out-status-nserror-bridge-catch-dispatch-and-cleanup-paths-route-through-private-error-runtime-helpers";
inline constexpr const char *kObjc3RuntimeBackedSemanticsClosureConcurrencyModel =
    "async-continuation-task-group-cancellation-executor-hop-actor-isolation-mailbox-replay-and-race-guard-paths-route-through-private-concurrency-runtime-helpers";
inline constexpr const char *kObjc3RuntimeBackedSemanticsClosureRuntimeHelperModel =
    "private-helper-symbol-set-is-the-claimed-runtime-backed-boundary-and-public-runtime-abi-remains-unwidened";
inline constexpr const char *kObjc3RuntimeBackedSemanticsClosureFailClosedModel =
    "missing-durable-evidence-drifted-helper-symbols-unsupported-escape-throw-task-or-actor-surfaces-stay-fail-closed";
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
