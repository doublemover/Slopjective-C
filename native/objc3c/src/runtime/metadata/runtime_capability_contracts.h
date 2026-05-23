#pragma once

#include <cstddef>
#include <cstdlib>
#include <string>

#if defined(_WIN32)
#include <windows.h>
#endif

namespace objc3c::runtime {

inline constexpr const char *kObjc3DefaultMetaprogrammingHostCacheRoot =
    "tmp/artifacts/objc3c-native/cache/metaprogramming";
inline constexpr const char *kObjc3ConformancePublicationContractId =
    "objc3c.driver.conformance.report.publication.v1";
inline constexpr const char *kObjc3ConformanceClaimOperationsContractId =
    "objc3c.toolchain.conformance.claim.operations.v1";
inline constexpr const char *kObjc3ConformanceClaimSelectionModel =
    "core-strict-and-strict-concurrency-language-profiles-drive-current-publication-surface";
inline constexpr const char *kObjc3ConformanceClaimFailureModel =
    "strict-system-remains-fail-closed-and-unclaimed";
inline constexpr const char *kObjc3AdvancedFeatureOpsContractId =
    "objc3c.advanced.feature.ci.runbook.dashboard.contract.v1";
inline constexpr const char *kObjc3AdvancedFeatureReportingContractId =
    "objc3c.tooling.feature.aware.conformance.report.emission.v1";
inline constexpr const char *kObjc3AdvancedFeatureReleaseEvidenceContractId =
    "objc3c.tooling.corpus.sharding.release.evidence.packaging.v1";
inline constexpr const char *kObjc3AdvancedFeatureEvidenceGateScriptPath =
    "scripts/check_release_evidence.py";
inline constexpr const char *kObjc3AdvancedFeatureEvidenceRunbookPath =
    "spec/conformance/release_evidence_gate_maintenance.md";
inline constexpr const char *kObjc3AdvancedFeatureDashboardSchemaPath =
    "schemas/objc3-conformance-dashboard-status-v1.schema.json";
inline constexpr const char
    *kObjc3AdvancedFeatureReleaseEvidenceOperationContractId =
        "objc3c.tooling.release.evidence.toolchain.operations.v1";
inline constexpr const char
    *kObjc3AdvancedFeatureDashboardStatusPublicationContractId =
        "objc3c.tooling.dashboard.status.publication.v1";

inline const char *MetaprogrammingHostCacheRootForTesting() {
#if defined(_WIN32)
  char buffer[32768] = {};
  const DWORD length = GetEnvironmentVariableA(
      "OBJC3C_METAPROGRAMMING_CACHE_ROOT", buffer,
      static_cast<DWORD>(sizeof(buffer)));
  if (length == 0 || length >= sizeof(buffer)) {
    return kObjc3DefaultMetaprogrammingHostCacheRoot;
  }
  static std::string cached_override;
  cached_override.assign(buffer, static_cast<std::size_t>(length));
  return cached_override.c_str();
#else
  const char *override_value = std::getenv("OBJC3C_METAPROGRAMMING_CACHE_ROOT");
  if (override_value == nullptr || override_value[0] == '\0') {
    return kObjc3DefaultMetaprogrammingHostCacheRoot;
  }
  static std::string cached_override;
  cached_override = override_value;
  return cached_override.c_str();
#endif
}

}  // namespace objc3c::runtime
