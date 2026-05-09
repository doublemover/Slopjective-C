#include "artifacts/objc3_frontend_runtime_capability_artifacts.h"

#include <cstddef>
#include <sstream>
#include <string>
#include <vector>

#include "io/objc3_json.h"
#include "lower/objc3_lowering_contract.h"
#include "token/objc3_token_contract.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

std::string BuildStringArrayJson(const std::vector<std::string> &values) {
  return objc3::io::json::RenderJsonStringArray(values);
}

std::string BuildRuntimeCapabilityProfilesJson() {
  return "["
         "{\"id\":\"core\",\"status\":\"claimed\"},"
         "{\"id\":\"strict\",\"status\":\"claimed\"},"
         "{\"id\":\"strict-concurrency\",\"status\":\"claimed\"},"
         "{\"id\":\"strict-system\",\"status\":\"claimed\"}"
         "]";
}

std::vector<std::string> BuildClaimedConformanceProfileIds() {
  return {"core", "strict", "strict-concurrency", "strict-system"};
}

std::vector<std::string> BuildNotClaimedConformanceProfileIds() {
  return {};
}

std::string BuildRuntimeCapabilityOptionalFeaturesJson() {
  struct OptionalFeatureEntry {
    const char *id;
    const char *status;
    const char *reason;
    const char *backing_claim_id;
  };
  constexpr OptionalFeatureEntry kEntries[] = {
      {"throws", "not-claimed",
       "runtime-backed throws/error propagation is not part of the runnable native surface yet",
       kObjc3UnsupportedFeatureClaimThrows},
      {"async-await", "not-claimed",
       "async/await lowering and runtime scheduling are not part of the runnable native surface yet",
       kObjc3UnsupportedFeatureClaimAsyncAwait},
      {"actors", "not-claimed",
       "actor isolation and actor runtime support are not part of the runnable native surface yet",
       kObjc3UnsupportedFeatureClaimActors},
      {"blocks", "not-claimed",
       "blocks are still tracked as unsupported in the public conformance claim surface",
       kObjc3UnsupportedFeatureClaimBlocks},
      {"arc", "not-claimed",
       "ARC remains unsupported in the public conformance claim surface until the full runnable ARC contract closes",
       kObjc3UnsupportedFeatureClaimArc},
  };
  constexpr std::size_t kEntryCount = sizeof(kEntries) / sizeof(kEntries[0]);
  std::ostringstream out;
  out << "[";
  for (std::size_t i = 0; i < kEntryCount; ++i) {
    const auto &entry = kEntries[i];
    if (i != 0u) {
      out << ",";
    }
    out << "{\"id\":\"" << entry.id << "\",\"status\":\"" << entry.status
        << "\",\"reason\":\"" << EscapeJsonString(entry.reason)
        << "\",\"backing_claim_id\":\"" << entry.backing_claim_id << "\"}";
  }
  out << "]";
  return out.str();
}

std::string BuildRuntimeCapabilityVersionsJson() {
  std::ostringstream out;
  out << "{\"frontend\":\"" << kObjc3RuntimeCapabilityToolchainVersion
      << "\",\"runtime\":\"" << kObjc3RuntimeCapabilityToolchainVersion
      << "\",\"stdlib\":\"" << kObjc3RuntimeCapabilityToolchainVersion
      << "\",\"module_format\":\""
      << kObjc3RuntimeCapabilityModuleFormatVersion << "\"}";
  return out.str();
}

std::string BuildPublicConformanceProfilesJson() {
  struct ProfileEntry {
    const char *id;
    const char *status;
  };
  constexpr ProfileEntry kProfiles[] = {
      {"core", "claimed"},
      {"strict", "claimed"},
      {"strict-concurrency", "claimed"},
      {"strict-system", "claimed"},
  };
  constexpr std::size_t kProfileCount = sizeof(kProfiles) / sizeof(kProfiles[0]);
  std::ostringstream out;
  out << "[";
  for (std::size_t i = 0; i < kProfileCount; ++i) {
    const auto &profile = kProfiles[i];
    if (i != 0u) {
      out << ",";
    }
    out << "{\"id\":\"" << profile.id << "\",\"status\":\"" << profile.status
        << "\"}";
  }
  out << "]";
  return out.str();
}

}  // namespace

std::string BuildRuntimeCapabilityReportJson(
    const Objc3VersionedConformanceReportLoweringSummary &summary) {
  const std::vector<std::string> claimed_profile_ids =
      BuildClaimedConformanceProfileIds();
  const std::vector<std::string> not_claimed_profile_ids =
      BuildNotClaimedConformanceProfileIds();
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(kObjc3RuntimeCapabilityReportingContractId)
      << "\",\"schema_id\":\""
      << EscapeJsonString(kObjc3RuntimeCapabilityReportingSchemaId)
      << "\",\"source_contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"frontend_surface_path\":\""
      << EscapeJsonString(kObjc3RuntimeCapabilityReportingSurfacePath)
      << "\",\"source_frontend_surface_path\":\""
      << EscapeJsonString(summary.frontend_surface_path)
      << "\",\"profile_model\":\""
      << EscapeJsonString(kObjc3RuntimeCapabilityReportingProfileModel)
      << "\",\"optional_feature_model\":\""
      << EscapeJsonString(kObjc3RuntimeCapabilityReportingOptionalFeatureModel)
      << "\",\"version_model\":\""
      << EscapeJsonString(kObjc3RuntimeCapabilityReportingVersionModel)
      << "\",\"strictness_mode\":\""
      << EscapeJsonString(kObjc3RuntimeCapabilityStrictnessMode)
      << "\",\"concurrency_mode\":\""
      << EscapeJsonString(kObjc3RuntimeCapabilityConcurrencyMode)
      << "\",\"claimed_profile_ids\":"
      << BuildStringArrayJson(claimed_profile_ids)
      << ",\"not_claimed_profile_ids\":"
      << BuildStringArrayJson(not_claimed_profile_ids)
      << ",\"profiles\":" << BuildRuntimeCapabilityProfilesJson()
      << ",\"runtime_capability_ids\":"
      << BuildStringArrayJson(summary.runnable_feature_claim_ids)
      << ",\"source_only_feature_claim_ids\":"
      << BuildStringArrayJson(summary.source_only_feature_claim_ids)
      << ",\"unsupported_feature_claim_ids\":"
      << BuildStringArrayJson(summary.unsupported_feature_claim_ids)
      << ",\"optional_features\":"
      << BuildRuntimeCapabilityOptionalFeaturesJson()
      << ",\"versions\":" << BuildRuntimeCapabilityVersionsJson()
      << ",\"public_schema_id\":\""
      << EscapeJsonString(kObjc3RuntimeCapabilityPublicSchemaId)
      << "\",\"replay_generated_at\":\""
      << EscapeJsonString(kObjc3RuntimeCapabilityGeneratedAtReplayValue)
      << "\",\"ready\":"
      << (IsReadyObjc3VersionedConformanceReportLoweringSummary(summary)
              ? "true"
              : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildPublicConformanceReportJson(
    const Objc3VersionedConformanceReportLoweringSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"schema_id\":\""
      << EscapeJsonString(kObjc3RuntimeCapabilityPublicSchemaId)
      << "\",\"generated_at\":\""
      << EscapeJsonString(kObjc3RuntimeCapabilityGeneratedAtReplayValue)
      << "\",\"toolchain\":{\"name\":\""
      << EscapeJsonString(kObjc3RuntimeCapabilityToolchainName)
      << "\",\"vendor\":\""
      << EscapeJsonString(kObjc3RuntimeCapabilityToolchainVendor)
      << "\",\"version\":\""
      << EscapeJsonString(kObjc3RuntimeCapabilityToolchainVersion)
      << "\",\"target_triple\":\""
      << EscapeJsonString(kObjc3RuntimeCapabilityTargetTriple)
      << "\"},\"language\":{\"language_family\":\""
      << EscapeJsonString(kObjc3RuntimeCapabilityLanguageFamily)
      << "\",\"language_version\":\""
      << EscapeJsonString(kObjc3RuntimeCapabilityLanguageVersion)
      << "\",\"spec_revision\":\""
      << EscapeJsonString(kObjc3RuntimeCapabilitySpecRevision)
      << "\"},\"mode\":{\"strictness\":\""
      << EscapeJsonString(kObjc3RuntimeCapabilityStrictnessMode)
      << "\",\"concurrency\":\""
      << EscapeJsonString(kObjc3RuntimeCapabilityConcurrencyMode)
      << "\",\"compatibility\":\""
      << EscapeJsonString(summary.effective_language_profile)
      << "\",\"canonical_literal_rejection_diagnostics\":"
      << (summary.canonical_literal_rejection_diagnostics_enabled ? "true"
                                                                  : "false")
      << "},\"profiles\":" << BuildPublicConformanceProfilesJson()
      << ",\"optional_features\":"
      << BuildRuntimeCapabilityOptionalFeaturesJson()
      << ",\"versions\":" << BuildRuntimeCapabilityVersionsJson()
      << ",\"known_deviations\":[]"
      << ",\"source_replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

}  // namespace objc3::artifacts::frontend
