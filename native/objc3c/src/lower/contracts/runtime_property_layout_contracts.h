#pragma once

#include <string>

// Runtime property layout owns runtime consumption of emitted property/layout
// records, real per-instance slot allocation, and private reflection helpers.
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

std::string Objc3RuntimePropertyLayoutConsumptionSummary();
std::string Objc3RuntimeInstanceAllocationLayoutSupportSummary();
std::string Objc3RuntimePropertyMetadataReflectionSummary();
