#pragma once

inline constexpr const char
    *kObjc3RuntimeRegistrationDescriptorImageRootSourceSurfaceContractId =
        "objc3c.bootstrap.registration.descriptor.image.root.source.surface.v1";
inline constexpr const char
    *kObjc3RuntimeRegistrationDescriptorImageRootSourceSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_runtime_registration_descriptor_image_root_source_surface";
inline constexpr const char
    *kObjc3RuntimeBootstrapModuleIdentitySourceModel =
        "module-declaration-or-default";
inline constexpr const char
    *kObjc3RuntimeBootstrapDerivedIdentitySourcePragma =
        "source-pragma";
inline constexpr const char
    *kObjc3RuntimeBootstrapDerivedIdentitySourceModuleDefault =
        "module-derived-default";
inline constexpr const char
    *kObjc3RuntimeBootstrapVisibleMetadataOwnershipModel =
        "image-root-owns-registration-descriptor-runtime-owns-bootstrap-state";
inline constexpr const char
    *kObjc3RuntimeBootstrapRegistrationDescriptorDefaultSuffix =
        "_registration_descriptor";
inline constexpr const char
    *kObjc3RuntimeBootstrapImageRootDefaultSuffix = "_image_root";
inline constexpr const char
    *kObjc3RuntimeRegistrationDescriptorFrontendClosureContractId =
        "objc3c.runtime.registration.descriptor.frontend.closure.v1";
inline constexpr const char
    *kObjc3RuntimeRegistrationDescriptorFrontendClosureSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_runtime_registration_descriptor_frontend_closure";
inline constexpr const char
    *kObjc3RuntimeRegistrationDescriptorFrontendClosurePayloadModel =
        "runtime-registration-descriptor-json-v1";
inline constexpr const char
    *kObjc3RuntimeRegistrationDescriptorFrontendClosureArtifactSuffix =
        ".runtime-registration-descriptor.json";
inline constexpr const char
    *kObjc3RuntimeRegistrationDescriptorFrontendClosureArtifactRelativePath =
        "module.runtime-registration-descriptor.json";
inline constexpr const char
    *kObjc3RuntimeRegistrationDescriptorFrontendAuthorityModel =
        "registration-descriptor-artifact-derived-from-source-surface-and-registration-manifest";
inline constexpr const char
    *kObjc3RuntimeRegistrationDescriptorPayloadOwnershipModel =
        "compiler-emits-registration-descriptor-artifact-runtime-consumes-bootstrap-identity";
