#pragma once

#include <string>

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
        "no-missing-effective-accessor-bindings-no-duplicate-synthesized-owner-identities-no-shared-storage-bypasses";

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
        "no-layout-rederivation-no-shared-storage-bypasses-no-reflective-property-registration";

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

inline constexpr const char
    *kObjc3DispatchAndSynthesizedAccessorLoweringSurfaceContractId =
        "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1";
inline constexpr const char
    *kObjc3AccessorStorageLoweringMetadataModel =
        "runtime-metadata-and-executable-graph-property-records-publish-synthesized-accessor-lowering-helper-selection-through-the-live-compiler-path";
inline constexpr const char
    *kObjc3AccessorStorageLoweringHelperSelectionModel =
        "plain-accessors-use-current-property-read-write-helpers-strong-owned-setters-use-exchange-and-weak-accessors-use-weak-current-property-helpers";

std::string Objc3ExecutableObjectArtifactLoweringSummary();
std::string Objc3ExecutablePropertyAccessorLayoutLoweringSummary();
std::string Objc3ExecutableIvarLayoutEmissionSummary();
std::string Objc3ExecutableSynthesizedAccessorPropertyLoweringSummary();
std::string Objc3RuntimePropertyLayoutConsumptionSummary();
std::string Objc3RuntimeInstanceAllocationLayoutSupportSummary();
std::string Objc3RuntimePropertyMetadataReflectionSummary();
