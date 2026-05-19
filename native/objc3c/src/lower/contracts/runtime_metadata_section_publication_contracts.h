#pragma once

#include <string>

// Runtime metadata section publication owns the lower-owned section/object
// payload boundary that publishes image-info, descriptor, and aggregate records
// for native IR/object emission.
inline constexpr const char *kObjc3RuntimeMetadataSectionEmissionContractId =
    "objc3c.runtime.metadata.section.emission.freeze.v1";
inline constexpr const char
    *kObjc3RuntimeMetadataSectionEmissionOwnerContractId =
        "objc3c.runtime.metadata.section.emission.owner.record.v1";
inline constexpr const char *kObjc3RuntimeMetadataSectionEmissionPayloadModel =
    "lowering-owned-zero-payload-section-records";
inline constexpr const char *kObjc3RuntimeMetadataSectionEmissionOwnerModel =
    "native.lower.runtime-metadata-publishes-section-records-native.ir-consumes-object-payloads";
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

std::string Objc3RuntimeMetadataSectionEmissionBoundarySummary();
