#pragma once

namespace objc3::artifacts::frontend {

inline constexpr const char
    *kExecutableAccessorLayoutPropertyLoweringContractId =
        "objc3c.executable.property.accessor.layout.lowering.v1";
inline constexpr const char *kExecutableAccessorLayoutIvarEmissionContractId =
    "objc3c.executable.ivar.layout.emission.v1";
inline constexpr const char
    *kExecutableAccessorLayoutSynthesizedAccessorPropertyLoweringContractId =
        "objc3c.executable.synthesized.accessor.property.lowering.v1";
inline constexpr const char
    *kExecutableAccessorLayoutDispatchAndSynthesizedAccessorLoweringSurfaceContractId =
        "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1";

inline constexpr const char *kExecutableAccessorLayoutPropertyTableModel =
    "property-descriptor-bundles-carry-sema-approved-attribute-accessor-binding-and-layout-records";
inline constexpr const char *kExecutableAccessorLayoutIvarLayoutModel =
    "ivar-descriptor-bundles-carry-sema-approved-layout-symbol-replay-key-slot-offset-size-alignment-padding-inheritance-owner-size-records";
inline constexpr const char *kExecutableAccessorLayoutAccessorBindingModel =
    "effective-accessor-selectors-and-synthesized-binding-identities-pass-through-lowering-without-body-synthesis";
inline constexpr const char *kExecutableAccessorLayoutScopeModel =
    "ast-sema-property-layout-handoff-ir-object-metadata-publication";
inline constexpr const char *kExecutableAccessorLayoutFailClosedModel =
    "no-synthesized-accessor-bodies-no-runtime-storage-allocation-no-layout-rederivation";

inline constexpr const char *kExecutableAccessorLayoutIvarDescriptorModel =
    "ivar-descriptor-records-carry-layout-symbol-replay-key-offset-global-slot-offset-size-alignment-padding-inheritance-owner-size-ordering";
inline constexpr const char *kExecutableAccessorLayoutIvarOffsetGlobalModel =
    "one-retained-i64-offset-global-per-emitted-ivar-binding";
inline constexpr const char *kExecutableAccessorLayoutIvarLayoutTableModel =
    "declaration-owner-layout-tables-order-ivars-by-slot-and-publish-instance-size";
inline constexpr const char *kExecutableAccessorLayoutIvarEmissionScopeModel =
    "sema-approved-layout-shape-lowers-into-ivar-section-payloads-without-runtime-allocation";
inline constexpr const char *kExecutableAccessorLayoutIvarEmissionFailClosedModel =
    "no-runtime-instance-allocation-no-layout-rederivation-no-accessor-body-synthesis";

inline constexpr const char *kExecutableAccessorLayoutSynthesizedAccessorSourceModel =
    "implementation-owned-properties-synthesize-missing-effective-instance-accessors-into-emitted-method-lists";
inline constexpr const char *kExecutableAccessorLayoutSynthesizedAccessorStorageModel =
    "synthesized-getter-setter-bodies-lower-directly-to-runtime-current-property-helper-calls-without-storage-globals";
inline constexpr const char
    *kExecutableAccessorLayoutSynthesizedAccessorPropertyDescriptorModel =
        "property-descriptors-carry-effective-accessor-selectors-binding-symbols-layout-symbols-and-accessor-implementation-pointers";
inline constexpr const char
    *kExecutableAccessorLayoutSynthesizedAccessorFailClosedModel =
        "no-missing-effective-accessor-bindings-no-duplicate-synthesized-owner-identities-no-storage-global-retired-routes";

}  // namespace objc3::artifacts::frontend
