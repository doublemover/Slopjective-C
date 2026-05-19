#include "artifacts/objc3_frontend_artifact_manifest_header.h"

#include <ostream>

#include "io/objc3_json.h"
#include "token/objc3_token_contract.h"

namespace {

using objc3::io::EscapeJsonString;

const char *LanguageProfileName(Objc3FrontendLanguageProfile mode) {
  (void)mode;
  return "canonical";
}

const char *ArcModeName(Objc3FrontendArcMode mode) {
  return mode == Objc3FrontendArcMode::kEnabled ? "enabled" : "disabled";
}

}  // namespace

namespace objc3::artifacts::frontend {

void AppendObjc3FrontendArtifactManifestHeader(
    std::ostream &manifest,
    const std::filesystem::path &input_path,
    const Objc3Program &program,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options) {
  manifest << "{\n";
  manifest << "  \"source\": \"" << input_path.generic_string() << "\",\n";
  manifest << "  \"module\": \"" << program.module_name << "\",\n";
  manifest << "  \"frontend\": {\n";
  manifest << "    \"language_version\":"
           << static_cast<unsigned>(options.language_version) << ",\n";
  manifest << "    \"language_profile\":\""
           << LanguageProfileName(options.language_profile) << "\",\n";
  manifest << "    \"arc_mode\":\"" << ArcModeName(options.arc_mode)
           << "\",\n";
  manifest << "    \"default_language_profile\":\"canonical\",\n";
  manifest << "    \"canonical_literal_rejection_diagnostics\":true,\n";
  manifest << "    \"language_version_selection_supported\":true,\n";
  manifest << "    \"language_profile_selection_supported\":true,\n";
  manifest
      << "    \"canonical_rejection_diagnostics_selection_supported\":false,\n";
  manifest << "    \"canonical_literal_rejection_diagnostics_hard_error\":true,\n";
  manifest << "    \"strictness_selection_supported\":false,\n";
  manifest << "    \"strict_concurrency_selection_supported\":false,\n";
  manifest << "    \"feature_macro_surface_supported\":false,\n";
  manifest << "    \"feature_claim_truth_surface_contract_id\":\""
           << kObjc3FeatureClaimStrictnessTruthSurfaceContractId << "\",\n";
  manifest << "    \"canonical_literal_rejection_counts\":{\"yes_literal_sites\":"
           << pipeline_result.canonical_literal_rejection_counts.yes_literal_sites
           << ",\"no_literal_sites\":"
           << pipeline_result.canonical_literal_rejection_counts.no_literal_sites
           << ",\"null_literal_sites\":"
           << pipeline_result.canonical_literal_rejection_counts
                  .null_literal_sites
           << ",\"total_literal_sites\":"
           << pipeline_result.canonical_literal_rejection_counts
                  .total_literal_sites()
           << "},\n";
  manifest << "    \"language_version_pragma_contract\":{\"seen\":"
           << (pipeline_result.language_version_pragma_contract.seen ? "true"
                                                                      : "false")
           << ",\"directive_count\":"
           << pipeline_result.language_version_pragma_contract.directive_count
           << ",\"duplicate\":"
           << (pipeline_result.language_version_pragma_contract.duplicate
                   ? "true"
                   : "false")
           << ",\"non_leading\":"
           << (pipeline_result.language_version_pragma_contract.non_leading
                   ? "true"
                   : "false")
           << ",\"first_line\":"
           << pipeline_result.language_version_pragma_contract.first_line
           << ",\"first_column\":"
           << pipeline_result.language_version_pragma_contract.first_column
           << ",\"last_line\":"
           << pipeline_result.language_version_pragma_contract.last_line
           << ",\"last_column\":"
           << pipeline_result.language_version_pragma_contract.last_column
           << "},\n";
  manifest << "    \"bootstrap_registration_source_pragma_contract\":{"
           << "\"registration_descriptor\":{\"seen\":"
           << (pipeline_result.bootstrap_registration_source_pragma_contract
                       .registration_descriptor.seen
                   ? "true"
                   : "false")
           << ",\"directive_count\":"
           << pipeline_result.bootstrap_registration_source_pragma_contract
                  .registration_descriptor.directive_count
           << ",\"duplicate\":"
           << (pipeline_result.bootstrap_registration_source_pragma_contract
                       .registration_descriptor.duplicate
                   ? "true"
                   : "false")
           << ",\"non_leading\":"
           << (pipeline_result.bootstrap_registration_source_pragma_contract
                       .registration_descriptor.non_leading
                   ? "true"
                   : "false")
           << ",\"first_line\":"
           << pipeline_result.bootstrap_registration_source_pragma_contract
                  .registration_descriptor.first_line
           << ",\"first_column\":"
           << pipeline_result.bootstrap_registration_source_pragma_contract
                  .registration_descriptor.first_column
           << ",\"last_line\":"
           << pipeline_result.bootstrap_registration_source_pragma_contract
                  .registration_descriptor.last_line
           << ",\"last_column\":"
           << pipeline_result.bootstrap_registration_source_pragma_contract
                  .registration_descriptor.last_column
           << ",\"identifier\":\""
           << EscapeJsonString(
                  pipeline_result.bootstrap_registration_source_pragma_contract
                      .registration_descriptor.identifier)
           << "\"},\"image_root\":{\"seen\":"
           << (pipeline_result.bootstrap_registration_source_pragma_contract
                       .image_root.seen
                   ? "true"
                   : "false")
           << ",\"directive_count\":"
           << pipeline_result.bootstrap_registration_source_pragma_contract
                  .image_root.directive_count
           << ",\"duplicate\":"
           << (pipeline_result.bootstrap_registration_source_pragma_contract
                       .image_root.duplicate
                   ? "true"
                   : "false")
           << ",\"non_leading\":"
           << (pipeline_result.bootstrap_registration_source_pragma_contract
                       .image_root.non_leading
                   ? "true"
                   : "false")
           << ",\"first_line\":"
           << pipeline_result.bootstrap_registration_source_pragma_contract
                  .image_root.first_line
           << ",\"first_column\":"
           << pipeline_result.bootstrap_registration_source_pragma_contract
                  .image_root.first_column
           << ",\"last_line\":"
           << pipeline_result.bootstrap_registration_source_pragma_contract
                  .image_root.last_line
           << ",\"last_column\":"
           << pipeline_result.bootstrap_registration_source_pragma_contract
                  .image_root.last_column
           << ",\"identifier\":\""
           << EscapeJsonString(
                  pipeline_result.bootstrap_registration_source_pragma_contract
                      .image_root.identifier)
           << "\"},\"registration_descriptor_pragma_name\":\""
           << EscapeJsonString(kObjc3BootstrapRegistrationDescriptorPragmaName)
           << "\",\"image_root_pragma_name\":\""
           << EscapeJsonString(kObjc3BootstrapImageRootPragmaName)
           << "\"},\n";
}

}  // namespace objc3::artifacts::frontend
