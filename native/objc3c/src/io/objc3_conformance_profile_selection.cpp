#include "io/objc3_process_internal.h"

std::vector<std::string> BuildObjc3ClaimedConformanceProfileIds() {
  return BuildFixedStringVector({"core"});
}

std::vector<std::string> BuildObjc3RejectedConformanceProfileIds() {
  return BuildFixedStringVector({"strict", "strict-concurrency", "strict-system"});
}

std::vector<std::string> BuildObjc3ReleaseTargetedProfileIds() {
  return BuildFixedStringVector({"strict", "strict-concurrency", "strict-system"});
}

bool IsObjc3ClaimedConformanceProfile(const std::string &profile_id) {
  return profile_id == "core";
}

bool IsObjc3JsonConformanceFormat(const std::string &format) {
  return format == "json";
}

std::string BuildUnsupportedObjc3ConformanceProfileSelectionDiagnostic(
    const std::string &profile_id) {
  std::ostringstream out;
  out << "unsupported --objc3-conformance-profile selection: " << profile_id
      << " (claimed profiles: core; rejected built-in profiles: strict, strict-concurrency, strict-system; targeted release-evidence profiles: strict, strict-concurrency, strict-system; policy="
      << kObjc3ConformanceProfileClaimPolicyModel << ")";
  return out.str();
}

std::string BuildUnsupportedObjc3ConformanceFormatSelectionDiagnostic(
    const std::string &format) {
  std::ostringstream out;
  out << "unsupported --emit-objc3-conformance-format selection: " << format
      << " (claimed publication format: json; targeted release-evidence profiles: strict, strict-concurrency, strict-system; policy="
      << kObjc3ConformanceFormatClaimPolicyModel << ")";
  return out.str();
}
