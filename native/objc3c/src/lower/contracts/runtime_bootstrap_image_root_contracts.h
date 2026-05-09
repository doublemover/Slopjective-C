#pragma once

#include <cstdint>
#include <string>

// Registration-descriptor/image-root contracts own the emitted startup records,
// logical sections, registration-table layout invariants, and local init cell.
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
inline constexpr const char *kObjc3RuntimeBootstrapImageRootPayloadModel =
    "image-root-record-points-at-module-name-image-descriptor-registration-table-and-discovery-root";
inline constexpr const char *kObjc3RuntimeBootstrapRegistrationTableLayoutModel =
    "abi-version-field-count-image-descriptor-discovery-root-linker-anchor-family-aggregates-selector-string-pools-keypath-descriptors-image-local-init-state";
inline constexpr const char *kObjc3RuntimeBootstrapImageLocalInitializationModel =
    "guarded-once-per-image-local-state-cell";
inline constexpr std::uint64_t kObjc3RuntimeBootstrapRegistrationTableAbiVersion =
    2u;
inline constexpr std::uint64_t
    kObjc3RuntimeBootstrapRegistrationTablePointerFieldCount = 12u;

std::string Objc3RuntimeBootstrapRegistrationDescriptorImageRootLoweringSummary();
