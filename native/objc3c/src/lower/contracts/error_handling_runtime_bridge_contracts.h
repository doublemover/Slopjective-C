#pragma once

#include <string>

// Error runtime bridge contracts own the private helper ABI for thrown-error
// storage, status/NSError normalization, catch matching, and the live runtime
// integration boundary that proves those helpers without public ABI widening.
inline constexpr const char *kObjc3RuntimeStoreThrownErrorI32Symbol =
    "objc3_runtime_store_thrown_error_i32";
inline constexpr const char *kObjc3RuntimeLoadThrownErrorI32Symbol =
    "objc3_runtime_load_thrown_error_i32";
inline constexpr const char *kObjc3RuntimeBridgeStatusErrorI32Symbol =
    "objc3_runtime_bridge_status_error_i32";
inline constexpr const char *kObjc3RuntimeBridgeNSErrorErrorI32Symbol =
    "objc3_runtime_bridge_nserror_error_i32";
inline constexpr const char *kObjc3RuntimeBridgeForeignExceptionErrorI32Symbol =
    "objc3_runtime_bridge_foreign_exception_error_i32";
inline constexpr const char *kObjc3RuntimeCatchMatchesErrorI32Symbol =
    "objc3_runtime_catch_matches_error_i32";

inline constexpr const char
    *kObjc3ErrorHandlingErrorRuntimeBridgeHelperContractId =
        "objc3c.error_handling.error.runtime.and.bridge.helper.api.v1";
inline constexpr const char
    *kObjc3ErrorHandlingErrorRuntimeBridgeHelperSourceModel =
        "error_handling-lowering-routes-error-storage-bridge-normalization-and-catch-dispatch-through-private-runtime-helpers";
inline constexpr const char *kObjc3ErrorHandlingErrorRuntimeBridgeHelperAbiModel =
    "i32-backed-native-error-object-handles-foreign-exception-normalization-and-catch-kind-matching-remain-private-bootstrap-internal-runtime-abi";
inline constexpr const char
    *kObjc3ErrorHandlingErrorRuntimeBridgeHelperFailClosedModel =
        "foreign-exception-bridging-stays-private-and-fail-closed-with-stable-diagnostic-handles-no-public-error-runtime-header-widening";

inline constexpr const char
    *kObjc3ErrorHandlingLiveErrorRuntimeIntegrationContractId =
        "objc3c.error_handling.live.error.runtime.integration.v1";
inline constexpr const char
    *kObjc3ErrorHandlingLiveErrorRuntimeIntegrationSourceModel =
        "runnable-error_handling-object-code-links-and-executes-through-the-private-error-runtime-helper-cluster";
inline constexpr const char
    *kObjc3ErrorHandlingLiveErrorRuntimeIntegrationExecutionModel =
        "linked-native-error_handling-fixtures-drive-status-bridge-thrown-error-store-load-and-catch-dispatch-through-runtime-owned-helpers";
inline constexpr const char
    *kObjc3ErrorHandlingLiveErrorRuntimeIntegrationPackagingModel =
        "driver-emitted-object-and-registration-manifest-artifacts-preserve-runtime-library-link-inputs-for-runnable-error_handling-probes";
inline constexpr const char
    *kObjc3ErrorHandlingLiveErrorRuntimeIntegrationFailClosedModel =
        "no-public-error-runtime-abi-foreign-exception-bridging-remains-private-helper-backed-no-cross-module-live-claim-yet";

std::string Objc3ErrorHandlingErrorRuntimeBridgeHelperSummary();
std::string Objc3ErrorHandlingLiveErrorRuntimeIntegrationSummary();
