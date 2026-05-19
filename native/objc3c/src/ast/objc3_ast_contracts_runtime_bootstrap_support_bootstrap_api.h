#pragma once

#include <cstdint>

inline constexpr const char *kObjc3RuntimeBootstrapSemanticsContractId =
    "objc3c.runtime.startup.bootstrap.semantics.v1";
inline constexpr const char *kObjc3RuntimeBootstrapSemanticsSurfacePath =
    "frontend.pipeline.semantic_surface.objc_runtime_startup_bootstrap_semantics";
inline constexpr const char *kObjc3RuntimeBootstrapResultModel =
    "zero-success-negative-fail-closed";
inline constexpr const char
    *kObjc3RuntimeBootstrapRegistrationOrderOrdinalModel =
        "strictly-monotonic-positive-registration-order-ordinal";
inline constexpr const char *kObjc3RuntimeBootstrapStateSnapshotSymbol =
    "objc3_runtime_copy_registration_state_for_testing";
inline constexpr const char
    *kObjc3RuntimeBootstrapDuplicateInstallDiagnosticModel =
        "duplicate-install-rejections-publish-the-rejected-module-identity-and-registration-ordinal-without-advancing-runtime-installation-state";
inline constexpr const char
    *kObjc3RuntimeBootstrapOutOfOrderDiagnosticModel =
        "out-of-order-install-rejections-publish-the-rejected-module-identity-and-registration-ordinal-without-advancing-runtime-installation-state";
inline constexpr const char
    *kObjc3RuntimeBootstrapRejectedModuleNameField =
        "last_rejected_module_name";
inline constexpr const char
    *kObjc3RuntimeBootstrapRejectedTranslationUnitIdentityKeyField =
        "last_rejected_translation_unit_identity_key";
inline constexpr const char
    *kObjc3RuntimeBootstrapNextExpectedRegistrationOrderField =
        "next_expected_registration_order_ordinal";
inline constexpr const char
    *kObjc3RuntimeBootstrapLastSuccessfulRegistrationOrderField =
        "last_successful_registration_order_ordinal";
inline constexpr const char
    *kObjc3RuntimeBootstrapLastRejectedRegistrationOrderField =
        "last_rejected_registration_order_ordinal";
inline constexpr int kObjc3RuntimeBootstrapSuccessStatusCode = 0;
inline constexpr int kObjc3RuntimeBootstrapInvalidDescriptorStatusCode = -1;
inline constexpr int
    kObjc3RuntimeBootstrapDuplicateRegistrationStatusCode = -2;
inline constexpr int kObjc3RuntimeBootstrapOutOfOrderStatusCode = -3;
inline constexpr int
    kObjc3RuntimeBootstrapInvalidRegistrationRootsStatusCode = -4;
inline constexpr std::uint64_t
    kObjc3RuntimeBootstrapTranslationUnitRegistrationOrderOrdinal = 1u;
inline constexpr const char *kObjc3RuntimeBootstrapApiContractId =
    "objc3c.runtime.bootstrap.api.freeze.v1";
inline constexpr const char *kObjc3RuntimeBootstrapApiSurfacePath =
    "frontend.pipeline.semantic_surface.objc_runtime_bootstrap_api_contract";
inline constexpr const char *kObjc3RuntimeBootstrapApiStatusEnumType =
    "objc3_runtime_registration_status_code";
inline constexpr const char *kObjc3RuntimeBootstrapApiImageDescriptorType =
    "objc3_runtime_image_descriptor";
inline constexpr const char *kObjc3RuntimeBootstrapApiSelectorHandleType =
    "objc3_runtime_selector_handle";
inline constexpr const char *kObjc3RuntimeBootstrapApiRegistrationSnapshotType =
    "objc3_runtime_registration_state_snapshot";
inline constexpr const char *kObjc3RuntimeBootstrapApiStateLockingModel =
    "process-global-mutex-serialized-runtime-state";
inline constexpr const char *kObjc3RuntimeBootstrapApiStartupInvocationModel =
    "generated-init-stub-calls-runtime-register-image";
inline constexpr const char *kObjc3RuntimeBootstrapApiImageWalkLifecycleModel =
    "deferred-until-next-runtime-phase";
inline constexpr const char
    *kObjc3RuntimeBootstrapApiDeterministicResetLifecycleModel =
        "deferred-until-next-runtime-phase";
