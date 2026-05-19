#pragma once

#include <string>

// Executable property layout owns sema-approved property/ivar layout records,
// ivar offset/table emission, synthesized accessor payloads, and helper
// selection metadata that lower into executable object artifacts.
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
        "no-missing-effective-accessor-bindings-no-duplicate-synthesized-owner-identities-no-storage-global-retired-routes";

inline constexpr const char
    *kObjc3DispatchAndSynthesizedAccessorLoweringSurfaceContractId =
        "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1";
inline constexpr const char
    *kObjc3AccessorStorageLoweringMetadataModel =
        "runtime-metadata-and-executable-graph-property-records-publish-synthesized-accessor-lowering-helper-selection-through-the-live-compiler-path";
inline constexpr const char
    *kObjc3AccessorStorageLoweringHelperSelectionModel =
        "plain-accessors-use-current-property-read-write-helpers-strong-owned-setters-use-exchange-and-weak-accessors-use-weak-current-property-helpers";

std::string Objc3ExecutablePropertyAccessorLayoutLoweringSummary();
std::string Objc3ExecutableIvarLayoutEmissionSummary();
std::string Objc3ExecutableSynthesizedAccessorPropertyLoweringSummary();
