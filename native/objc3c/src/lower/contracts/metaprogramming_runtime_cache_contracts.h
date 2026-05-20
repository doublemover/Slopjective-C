#pragma once

#include <string>

// Metaprogramming runtime/cache contracts own the disabled host-runtime
// boundary, private property-runtime handoff, and the macro host process cache
// integration surface that materializes deterministic expansion cache entries.
inline constexpr const char *kObjc3MetaprogrammingExpansionHostRuntimeBoundaryContractId =
    "objc3c.metaprogramming.expansion.host.runtime.boundary.v1";
inline constexpr const char
    *kObjc3MetaprogrammingExpansionHostRuntimeBoundarySourceContractId =
        "objc3c.metaprogramming.module.interface.replay.preservation.v1";
inline constexpr const char *kObjc3MetaprogrammingExpansionHostRuntimeBoundaryHostModel =
    "macro-host-execution-process-launch-and-runtime-package-loading-remain-disabled-and-fail-closed";
inline constexpr const char
    *kObjc3MetaprogrammingExpansionHostRuntimeBoundaryPropertyRuntimeModel =
        "supported-property-behavior-lowering-reuses-existing-private-runtime-property-accessor-layout-and-current-property-hooks";
inline constexpr const char
    *kObjc3MetaprogrammingExpansionHostRuntimeBoundaryPackagingModel =
        "native-driver-packaging-still-hands-off-metaprogramming-runtime-support-through-artifacts-lib-objc3_runtime-lib-and-runtime-registration-manifests";
inline constexpr const char
    *kObjc3MetaprogrammingExpansionHostRuntimeBoundaryFailClosedModel =
        "no-live-macro-expansion-host-or-runtime-package-loader-is-claimed-yet";

inline constexpr const char
    *kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationContractId =
        "objc3c.metaprogramming.macro.host.process.cache.runtime.integration.v1";
inline constexpr const char
    *kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSourceContractId =
        "objc3c.metaprogramming.expansion.host.runtime.boundary.v1";
inline constexpr const char
    *kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_metaprogramming_macro_host_process_and_cache_runtime_integration";
inline constexpr const char
    *kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationImportArtifactMemberName =
        "objc_metaprogramming_macro_host_process_and_cache_runtime_integration";
inline constexpr const char
    *kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationHostExecutableRelativePath =
        "artifacts/bin/objc3c-frontend-c-api-runner.exe";
inline constexpr const char
    *kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationCacheRootRelativePath =
        "tmp/artifacts/objc3c-native/cache/metaprogramming";
inline constexpr const char
    *kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationHostModel =
        "native-driver-launches-objc3c-frontend-c-api-runner-for-supported-metaprogramming-expansion-cache-materialization";
inline constexpr const char
    *kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationToolchainModel =
        "frontend-runner-executes-with-manifest-enabled-and-ir-object-emission-disabled-for-deterministic-cache-materialization";
inline constexpr const char
    *kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationCacheModel =
        "cache-entry-path-is-derived-from-a-stable-fnv1a64-key-over-the-metaprogramming-replay-surface-explicit-macro-cache-keys-and-policy-version-and-reused-on-subsequent-runs";
inline constexpr const char
    *kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationInvalidationModel =
        "metaprogramming-replay-key-explicit-macro-cache-key-or-sandbox-policy-drift-invalidates-the-entry-while-corrupt-or-incomplete-cache-artifacts-fail-closed";
inline constexpr const char
    *kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSandboxPolicyModel =
        "macro-host-materialization-is-deny-by-default-and-only-admits-pure-free-functions-with-objc_macro_sandbox-named-deterministic";
inline constexpr const char
    *kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationDiagnosticsModel =
        "stable-O3S331-and-O3S332-diagnostics-gate-missing-or-invalid-macro-cache-key-and-sandbox-policy-metadata";
inline constexpr const char
    *kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationFailClosedModel =
        "missing-runner-corrupt-cache-or-import-surface-drift-disables-metaprogramming-host-process-cache-claims";

std::string Objc3MetaprogrammingExpansionHostRuntimeBoundarySummary();
std::string Objc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSummary();
