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
// M254-C001 bootstrap lowering freeze anchor: lane-C now freezes the lowering
// boundary that will eventually materialize one constructor root, one derived
// init stub, and one registration table per translation unit from the emitted
// registration manifest plus the live bootstrap semantics contract. The
// current boundary is explicit that those IR globals are not materialized yet.
inline constexpr const char *kObjc3RuntimeBootstrapLoweringContractId =
    "objc3c-runtime-bootstrap-lowering-freeze/m254-c001-v1";
inline constexpr const char *kObjc3RuntimeBootstrapLoweringBoundaryModel =
    "registration-manifest-driven-constructor-root-init-stub-and-registration-table-lowering";
inline constexpr const char *kObjc3RuntimeBootstrapConstructorRootEmissionState =
    "deferred-until-m254-c002";
inline constexpr const char *kObjc3RuntimeBootstrapInitStubEmissionState =
    "deferred-until-m254-c002";
inline constexpr const char
    *kObjc3RuntimeBootstrapRegistrationTableEmissionState =
        "deferred-until-m254-c002";
inline constexpr const char *kObjc3RuntimeBootstrapGlobalCtorListModel =
    "llvm.global_ctors-single-root-priority-65535";
inline constexpr const char *kObjc3RuntimeBootstrapRegistrationTableSymbolPrefix =
    "__objc3_runtime_registration_table_";
inline constexpr const char *kObjc3RuntimeBootstrapImageLocalInitStateSymbolPrefix =
    "__objc3_runtime_image_local_init_state_";
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
std::string Objc3RuntimeMetadataBinaryInspectionHarnessSummary();
std::string Objc3RuntimeMetadataObjectPackagingRetentionSummary();
std::string Objc3RuntimeMetadataLinkerRetentionSummary();
std::string Objc3RuntimeMetadataArchiveStaticLinkDiscoverySummary();
std::string Objc3RuntimeBootstrapLoweringBoundarySummary();
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
