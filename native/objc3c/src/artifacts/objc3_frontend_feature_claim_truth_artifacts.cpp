#include "artifacts/objc3_frontend_feature_claim_truth_artifacts.h"

#include <sstream>
#include <string>
#include <vector>

#include "artifacts/objc3_frontend_feature_claim_artifacts.h"
#include "io/objc3_json.h"
#include "io/json/json_writer.h"
#include "token/objc3_token_contract.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

const char *LanguageProfileName(Objc3FrontendLanguageProfile mode) {
  switch (mode) {
    case Objc3FrontendLanguageProfile::kCanonical:
      return "canonical";
    case Objc3FrontendLanguageProfile::kStrict:
      return "strict";
    case Objc3FrontendLanguageProfile::kStrictConcurrency:
      return "strict-concurrency";
  }
  return "unknown";
}

std::string BuildStringArrayJson(const std::vector<std::string> &values) {
  return objc3::io::json::RenderJsonStringArray(values);
}

}  // namespace

std::string BuildFeatureClaimStrictnessTruthSurfaceReplayKey(
    const Objc3FrontendOptions &options,
    const Objc3FrontendPipelineResult &pipeline_result) {
  std::ostringstream out;
  out << kObjc3FeatureClaimStrictnessTruthSurfaceContractId
      << ";language_mode=" << kObjc3RunnableFeatureClaimModeName
      << ";language_version=" << static_cast<unsigned>(options.language_version)
      << ";language_profile=" << LanguageProfileName(options.language_profile)
      << ";supported_selection_surfaces="
      << BuildSupportedSelectionSurfaceIds().size()
      << ";unsupported_selection_surfaces="
      << BuildUnsupportedSelectionSurfaceIds().size()
      << ";suppressed_macro_claims=3"
      << ";parser_declared_protocols="
      << pipeline_result.program.ast.protocols.size()
      << ";parser_declared_interfaces="
      << pipeline_result.program.ast.interfaces.size()
      << ";parser_declared_implementations="
      << pipeline_result.program.ast.implementations.size();
  return out.str();
}

std::string BuildFeatureClaimStrictnessTruthSurfaceJson(
    const Objc3FrontendOptions &options,
    const Objc3FrontendPipelineResult &pipeline_result) {
  const std::vector<std::string> supported_selection_surface_ids =
      BuildSupportedSelectionSurfaceIds();
  const std::vector<std::string> unsupported_selection_surface_ids =
      BuildUnsupportedSelectionSurfaceIds();
  const std::vector<std::string> suppressed_macro_claim_ids =
      BuildSuppressedMacroClaimIds();
  const std::vector<std::string> supported_language_profiles = {
      "canonical",
      "strict",
      "strict-concurrency",
  };
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << kObjc3FeatureClaimStrictnessTruthSurfaceContractId
      << "\",\"runnable_feature_claim_inventory_contract_id\":\""
      << kObjc3RunnableFeatureClaimInventoryContractId
      << "\",\"language_mode\":\"" << kObjc3RunnableFeatureClaimModeName
      << "\",\"language_version\":"
      << static_cast<unsigned>(options.language_version)
      << ",\"effective_language_profile\":\""
      << LanguageProfileName(options.language_profile)
      << "\",\"default_language_profile\":\"canonical\""
      << ",\"canonical_literal_rejection_diagnostics_enabled\":"
      << "true"
      << ",\"driver_surface_model\":\""
      << kObjc3FeatureClaimStrictnessTruthDriverSurfaceModel
      << "\",\"language_version_selection_supported\":true"
      << ",\"language_profile_selection_supported\":true"
      << ",\"canonical_rejection_diagnostics_selection_supported\":false"
      << ",\"canonical_literal_rejection_diagnostics_hard_error\":true"
      << ",\"strictness_selection_supported\":true"
      << ",\"strict_concurrency_selection_supported\":true"
      << ",\"feature_macro_surface_supported\":false"
      << ",\"claim_truth_fail_closed\":true"
      << ",\"supported_language_profiles\":"
      << BuildStringArrayJson(supported_language_profiles)
      << ",\"supported_selection_surface_ids\":"
      << BuildStringArrayJson(supported_selection_surface_ids)
      << ",\"unsupported_selection_surface_ids\":"
      << BuildStringArrayJson(unsupported_selection_surface_ids)
      << ",\"suppressed_macro_claim_ids\":"
      << BuildStringArrayJson(suppressed_macro_claim_ids)
      << ",\"declared_protocol_count\":"
      << pipeline_result.program.ast.protocols.size()
      << ",\"declared_interface_count\":"
      << pipeline_result.program.ast.interfaces.size()
      << ",\"declared_implementation_count\":"
      << pipeline_result.program.ast.implementations.size()
      << ",\"replay_key\":\""
      << EscapeJsonString(BuildFeatureClaimStrictnessTruthSurfaceReplayKey(
             options, pipeline_result))
      << "\"}";
  return out.str();
}

}  // namespace objc3::artifacts::frontend
