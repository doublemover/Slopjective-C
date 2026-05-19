#pragma once

namespace objc3::artifacts::frontend {

inline constexpr const char *kStorageAccessorRuntimePropertyMetadataReflectionContractId =
    "objc3c.runtime.property.metadata.reflection.v1";
inline constexpr const char
    *kStorageAccessorRuntimeBackedObjectOwnershipAttributeSurfaceContractId =
        "objc3c.runtime.backed.object.ownership.attribute.surface.v1";

inline constexpr const char *kStorageAccessorReadCurrentPropertyI32Symbol =
    "objc3_runtime_read_current_property_i32";
inline constexpr const char *kStorageAccessorWriteCurrentPropertyI32Symbol =
    "objc3_runtime_write_current_property_i32";
inline constexpr const char *kStorageAccessorExchangeCurrentPropertyI32Symbol =
    "objc3_runtime_exchange_current_property_i32";
inline constexpr const char *kStorageAccessorLoadWeakCurrentPropertyI32Symbol =
    "objc3_runtime_load_weak_current_property_i32";
inline constexpr const char *kStorageAccessorStoreWeakCurrentPropertyI32Symbol =
    "objc3_runtime_store_weak_current_property_i32";

inline constexpr const char
    *kStorageAccessorDispatchAndSynthesizedAccessorLoweringSurfaceContractId =
        "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1";
inline constexpr const char
    *kStorageAccessorExecutablePropertyAccessorLayoutLoweringContractId =
        "objc3c.executable.property.accessor.layout.lowering.v1";
inline constexpr const char *kStorageAccessorExecutableIvarLayoutEmissionContractId =
    "objc3c.executable.ivar.layout.emission.v1";
inline constexpr const char
    *kStorageAccessorExecutableSynthesizedAccessorPropertyLoweringContractId =
        "objc3c.executable.synthesized.accessor.property.lowering.v1";
inline constexpr const char *kStorageAccessorLoweringMetadataModel =
    "runtime-metadata-and-executable-graph-property-records-publish-synthesized-accessor-lowering-helper-selection-through-the-live-compiler-path";
inline constexpr const char *kStorageAccessorLoweringHelperSelectionModel =
    "plain-accessors-use-current-property-read-write-helpers-strong-owned-setters-use-exchange-and-weak-accessors-use-weak-current-property-helpers";

struct Objc3StorageAccessorRuntimeAbiFields {
  bool deterministic = true;
};

}  // namespace objc3::artifacts::frontend
