#pragma once

#include <cstddef>

// Layout ordering policy owns deterministic metadata family ordering and
// descriptor aggregation rules consumed by runtime metadata object emission.
inline constexpr const char
    *kObjc3RuntimeMetadataLayoutOrderingVisibilityPolicyContractId =
        "objc3c.runtime.metadata.layout.ordering.visibility.policy.freeze.v1";
inline constexpr const char *kObjc3RuntimeMetadataLayoutFamilyOrderingModel =
    "image-info-then-class-protocol-category-property-ivar";
inline constexpr const char *kObjc3RuntimeMetadataDescriptorOrderingModel =
    "ascending-descriptor-ordinal-then-family-aggregate";
inline constexpr const char *kObjc3RuntimeMetadataAggregateRelocationPolicy =
    "zero-sentinel-or-count-plus-pointer-vector";

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
