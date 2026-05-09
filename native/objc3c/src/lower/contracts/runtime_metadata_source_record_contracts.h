#pragma once

#include <string>

// Runtime metadata source records own class/metaclass, protocol/category,
// member table, selector pool, and runtime string-pool emission contracts.
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

std::string Objc3RuntimeMetadataClassMetaclassEmissionSummary();
std::string Objc3RuntimeMetadataProtocolCategoryEmissionSummary();
std::string Objc3RuntimeMetadataMemberTableEmissionSummary();
std::string Objc3RuntimeMetadataSelectorStringPoolEmissionSummary();
