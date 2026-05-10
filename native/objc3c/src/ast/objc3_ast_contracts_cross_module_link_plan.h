#pragma once

inline constexpr const char *kObjc3CrossModuleRuntimeLinkPlanContractId =
    "objc3c.cross.module.runtime.packaging.link.plan.v1";
inline constexpr const char
    *kObjc3RuntimeCrossModuleRealizedMetadataReplayPreservationSurfaceContractId =
        "objc3c.runtime.cross.module.realized.metadata.replay.preservation.surface.v1";
inline constexpr const char
    *kObjc3RuntimeObjectModelAbiQuerySurfaceContractId =
        "objc3c.runtime.object.model.abi.query.surface.v1";
inline constexpr const char
    *kObjc3RuntimeRealizationLookupReflectionImplementationSurfaceContractId =
        "objc3c.runtime.realization.lookup.reflection.implementation.surface.v1";
inline constexpr const char *kObjc3CrossModuleRuntimeLinkPlanPayloadModel =
    "cross-module-runtime-link-plan-json-v1";
inline constexpr const char *kObjc3CrossModuleRuntimeLinkPlanArtifactSuffix =
    ".cross-module-runtime-link-plan.json";
inline constexpr const char
    *kObjc3CrossModuleRuntimeLinkPlanArtifactRelativePath =
        "module.cross-module-runtime-link-plan.json";
inline constexpr const char
    *kObjc3CrossModuleRuntimeLinkerResponseArtifactSuffix =
        ".cross-module-runtime-linker-options.rsp";
inline constexpr const char
    *kObjc3CrossModuleRuntimeLinkerResponseArtifactRelativePath =
        "module.cross-module-runtime-linker-options.rsp";
inline constexpr const char
    *kObjc3MetaprogrammingMacroHostProcessCacheArtifactSuffix =
        ".metaprogramming-macro-host-cache.json";
inline constexpr const char
    *kObjc3MetaprogrammingMacroHostProcessCacheArtifactRelativePath =
        "module.metaprogramming-macro-host-cache.json";
inline constexpr const char
    *kObjc3InteropBridgeHeaderArtifactSuffix = ".interop-bridge.h";
inline constexpr const char
    *kObjc3InteropBridgeHeaderArtifactRelativePath =
        "module.interop-bridge.h";
inline constexpr const char
    *kObjc3InteropBridgeModuleArtifactSuffix = ".interop-bridge.modulemap";
inline constexpr const char
    *kObjc3InteropBridgeModuleArtifactRelativePath =
        "module.interop-bridge.modulemap";
inline constexpr const char
    *kObjc3InteropBridgeArtifactSuffix = ".interop-bridge.json";
inline constexpr const char
    *kObjc3InteropBridgeArtifactRelativePath = "module.interop-bridge.json";
inline constexpr const char
    *kObjc3CrossModuleRuntimeLinkPlanAuthorityModel =
        "runtime-import-surface-plus-imported-registration-manifest-peer-artifacts-drive-cross-module-link-plan";
inline constexpr const char
    *kObjc3CrossModuleRuntimeLinkPlanPackagingModel =
        "compiler-emits-cross-module-link-plan-and-merged-linker-response";
inline constexpr const char
    *kObjc3CrossModuleRuntimeLinkPlanRegistrationScopeModel =
        "registration-ordinal-sorted-link-plan-drives-multi-image-startup-registration";
inline constexpr const char
    *kObjc3CrossModuleRuntimeLinkObjectOrderModel =
        "ascending-registration-ordinal-then-translation-unit-identity-key";
inline constexpr const char
    *kObjc3CrossModuleRealizedMetadataReplayPreservationModel =
        "cross-module-link-plan-preserves-local-and-imported-realized-metadata-descriptor-counts-identities-and-reset-replay-readiness-from-runtime-registration-manifests";
inline constexpr const char
    *kObjc3RuntimeTranslationUnitRegistrationInitStubSymbolPrefix =
        "__objc3_runtime_register_image_init_stub_";
inline constexpr const char
    *kObjc3RuntimeTranslationUnitRegistrationInitStubOwnershipModel =
        "lowering-emits-init-stub-from-registration-manifest";
inline constexpr const char
    *kObjc3RuntimeTranslationUnitRegistrationManifestAuthorityModel =
        "registration-manifest-authoritative-for-constructor-root-shape";
inline constexpr const char
    *kObjc3RuntimeTranslationUnitRegistrationManifestPriorityPolicy =
        "deferred-until-next-runtime-phase";
inline constexpr const char
    *kObjc3RuntimeStartupBootstrapInvariantContractId =
        "objc3c.runtime.startup.bootstrap.invariants.v1";
inline constexpr const char
    *kObjc3RuntimeStartupBootstrapInvariantSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_runtime_startup_bootstrap_invariants";
inline constexpr const char
    *kObjc3RuntimeStartupBootstrapDuplicateRegistrationPolicy =
        "fail-closed-by-translation-unit-identity-key";
inline constexpr const char
    *kObjc3RuntimeStartupBootstrapRealizationOrderPolicy =
        "constructor-root-then-registration-manifest-order";
inline constexpr const char
    *kObjc3RuntimeStartupBootstrapFailureMode =
        "abort-before-user-main-no-partial-registration-commit";
inline constexpr const char
    *kObjc3RuntimeStartupBootstrapImageLocalInitializationScope =
        "runtime-owned-image-local-registration-state";
inline constexpr const char
    *kObjc3RuntimeStartupBootstrapConstructorRootUniquenessPolicy =
        "one-startup-root-per-translation-unit-identity";
inline constexpr const char
    *kObjc3RuntimeStartupBootstrapConsumptionModel =
        "startup-root-consumes-registration-manifest";
inline constexpr const char
    *kObjc3RuntimeStartupBootstrapExecutionMode =
        "deferred-until-next-runtime-phase";
