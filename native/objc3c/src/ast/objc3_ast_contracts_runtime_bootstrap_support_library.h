#pragma once

#include "artifacts/identity/artifact_identity.h"

inline constexpr const char *kObjc3RuntimeSupportLibraryTargetName =
    "objc3_runtime";
inline constexpr const char *kObjc3RuntimeSupportLibrarySourceRoot =
    "native/objc3c/src/runtime";
inline constexpr const char *kObjc3RuntimeSupportLibraryPublicHeaderPath =
    "native/objc3c/src/runtime/public/objc3_runtime_api.h";
inline constexpr const char *kObjc3RuntimeSupportLibraryKind =
    objc3::artifacts::identity::kObjc3NativeRuntimeLibraryKind;
inline constexpr const char *kObjc3RuntimeSupportLibraryArchiveBasename =
    objc3::artifacts::identity::kObjc3NativeRuntimeLibraryBasename;
inline constexpr const char *kObjc3RuntimeSupportLibraryArchiveRelativePath =
    objc3::artifacts::identity::kObjc3NativeRuntimeLibraryRelativePath;
inline constexpr const char *kObjc3RuntimeSupportLibraryImplementationSourcePath =
    "native/objc3c/src/runtime/objc3_runtime.cpp";
inline constexpr const char *kObjc3RuntimeSupportLibraryProbeSourcePath =
    "tests/tooling/runtime/runtime_library_probe.cpp";
inline constexpr const char *kObjc3RuntimeSupportLibraryRegisterImageSymbol =
    "objc3_runtime_register_image";
inline constexpr const char *kObjc3RuntimeSupportLibraryLookupSelectorSymbol =
    "objc3_runtime_lookup_selector";
inline constexpr const char *kObjc3RuntimeSupportLibraryDispatchI32Symbol =
    "objc3_runtime_dispatch_i32";
inline constexpr const char *kObjc3RuntimeSupportLibraryResetForTestingSymbol =
    "objc3_runtime_reset_for_testing";
inline constexpr const char *kObjc3RuntimeSupportLibraryDriverLinkMode =
    "not-linked-until-next-runtime-phase";
inline constexpr const char *kObjc3RuntimeSupportLibraryLinkWiringContractId =
    "objc3c.runtime.support.library.link.wiring.v1";
inline constexpr const char
    *kObjc3RuntimeSupportLibraryExecutionSmokeScriptPath =
        "scripts/check_objc3c_native_execution_smoke.ps1";
inline constexpr const char *kObjc3RuntimeSupportLibraryLinkWiringMode =
    "emitted-object-links-against-runtime-support-library";
inline constexpr const char
    *kObjc3RuntimeSupportLibraryCompilerOwnershipBoundary =
        "compiler-emits-metadata-runtime-does-not-own-source-records";
inline constexpr const char
    *kObjc3RuntimeSupportLibraryRuntimeOwnershipBoundary =
        "runtime-owns-registration-lookup-and-dispatch-state";
