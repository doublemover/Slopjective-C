#include "io/objc3_process_internal.h"

std::vector<std::string> BuildObjc3ClaimedConformanceProfileIds() {
  return BuildFixedStringVector({"core", "strict", "strict-concurrency"});
}

std::vector<std::string> BuildObjc3RejectedConformanceProfileIds() {
  return BuildFixedStringVector({"strict-system"});
}

std::vector<std::string> BuildObjc3ReleaseTargetedProfileIds() {
  return BuildFixedStringVector({"strict-system"});
}

bool IsObjc3ClaimedConformanceProfile(const std::string &profile_id) {
  return profile_id == "core" || profile_id == "strict" ||
         profile_id == "strict-concurrency";
}

bool IsObjc3JsonConformanceFormat(const std::string &format) {
  return format == "json";
}

std::string BuildUnsupportedObjc3ConformanceProfileSelectionDiagnostic(
    const std::string &profile_id) {
  std::ostringstream out;
  out << "O3C038 unsupported --objc3-conformance-profile selection: "
      << profile_id
      << " (claimed profiles: core, strict, strict-concurrency; rejected built-in profiles: strict-system; targeted release-evidence profiles: strict-system; strict profile is backed by native parser/sema/lowering handoff policy; strict-concurrency profile enables strict actor-isolation/sendability/task-lifecycle/scheduler enforcement; no compatibility modes or aliases; policy="
      << kObjc3ConformanceProfileClaimPolicyModel << ")";
  return out.str();
}

std::string BuildUnsupportedObjc3ConformanceFormatSelectionDiagnostic(
    const std::string &format) {
  std::ostringstream out;
  out << "unsupported --emit-objc3-conformance-format selection: " << format
      << " (claimed publication format: json; targeted release-evidence profiles: strict-system; policy="
      << kObjc3ConformanceFormatClaimPolicyModel << ")";
  return out.str();
}
