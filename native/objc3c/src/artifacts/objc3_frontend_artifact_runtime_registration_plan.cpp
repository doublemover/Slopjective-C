#include "artifacts/objc3_frontend_artifact_runtime_registration_plan.h"

#include "artifacts/identity/artifact_identity.h"
#include "artifacts/objc3_frontend_runtime_bootstrap_artifacts.h"
#include "artifacts/objc3_frontend_runtime_descriptor_artifacts.h"
#include "artifacts/objc3_frontend_runtime_registration_artifacts.h"

namespace {

using objc3::artifacts::identity::BuildObjc3TranslationUnitIdentityKey;
using objc3::artifacts::identity::Objc3TranslationUnitIdentityEvidence;
using objc3::artifacts::frontend::BuildRuntimeBootstrapApiSummary;
using objc3::artifacts::frontend::
    BuildRuntimeBootstrapFailureRestartSemanticsSummary;
using objc3::artifacts::frontend::
    BuildRuntimeBootstrapLegalityFailureContractSummary;
using objc3::artifacts::frontend::
    BuildRuntimeBootstrapLegalitySemanticsSummary;
using objc3::artifacts::frontend::BuildRuntimeBootstrapLoweringSummary;
using objc3::artifacts::frontend::BuildRuntimeBootstrapSemanticsSummary;
using objc3::artifacts::frontend::
    BuildRuntimeRegistrationDescriptorFrontendClosureSummary;
using objc3::artifacts::frontend::
    BuildRuntimeRegistrationDescriptorImageRootSourceSurfaceSummary;
using objc3::artifacts::frontend::BuildRuntimeStartupBootstrapInvariantSummary;
using objc3::artifacts::frontend::BuildRuntimeSupportLibraryContractSummary;
using objc3::artifacts::frontend::BuildRuntimeSupportLibraryCoreFeatureSummary;
using objc3::artifacts::frontend::BuildRuntimeSupportLibraryLinkWiringSummary;
using objc3::artifacts::frontend::
    BuildRuntimeTranslationUnitRegistrationContractSummary;
using objc3::artifacts::frontend::
    BuildRuntimeTranslationUnitRegistrationManifestSummary;

}  // namespace

Objc3FrontendArtifactRuntimeRegistrationPlan
BuildObjc3FrontendArtifactRuntimeRegistrationPlan(
    const std::filesystem::path &input_path,
    const Objc3Program &program,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options,
    const Objc3FrontendArtifactRuntimeMetadataPlan &runtime_metadata_plan) {
  Objc3FrontendArtifactRuntimeRegistrationPlan plan;
  plan.runtime_support_library = BuildRuntimeSupportLibraryContractSummary();
  plan.runtime_support_library_core_feature =
      BuildRuntimeSupportLibraryCoreFeatureSummary(
          plan.runtime_support_library);
  plan.runtime_support_library_link_wiring =
      BuildRuntimeSupportLibraryLinkWiringSummary(
          plan.runtime_support_library_core_feature);
  plan.runtime_translation_unit_registration_contract =
      BuildRuntimeTranslationUnitRegistrationContractSummary(
          runtime_metadata_plan.executable_metadata_runtime_ingest_binary_boundary,
          plan.runtime_support_library_link_wiring);
  plan.runtime_translation_unit_registration_manifest =
      BuildRuntimeTranslationUnitRegistrationManifestSummary(
          plan.runtime_translation_unit_registration_contract,
          plan.runtime_support_library_link_wiring,
          runtime_metadata_plan.runtime_metadata_section_publication,
          options.bootstrap_registration_order_ordinal);
  plan.runtime_registration_descriptor_image_root_source_surface =
      BuildRuntimeRegistrationDescriptorImageRootSourceSurfaceSummary(
          program,
          pipeline_result.bootstrap_registration_source_pragma_contract,
          plan.runtime_translation_unit_registration_manifest);
  plan.runtime_registration_descriptor_frontend_closure =
      BuildRuntimeRegistrationDescriptorFrontendClosureSummary(
          plan.runtime_registration_descriptor_image_root_source_surface,
          plan.runtime_translation_unit_registration_manifest);
  plan.translation_unit_identity_key =
      BuildObjc3TranslationUnitIdentityKey(Objc3TranslationUnitIdentityEvidence{
          input_path,
          pipeline_result.parse_lowering_readiness_surface
              .parse_artifact_replay_key,
          pipeline_result.parse_lowering_readiness_surface
              .lowering_boundary_replay_key,
      });
  plan.runtime_startup_bootstrap_invariants =
      BuildRuntimeStartupBootstrapInvariantSummary(
          plan.runtime_translation_unit_registration_manifest);
  plan.runtime_bootstrap_api = BuildRuntimeBootstrapApiSummary(
      plan.runtime_support_library_core_feature,
      plan.runtime_support_library_link_wiring);
  plan.runtime_bootstrap_semantics = BuildRuntimeBootstrapSemanticsSummary(
      plan.runtime_startup_bootstrap_invariants,
      plan.runtime_translation_unit_registration_manifest);
  plan.runtime_bootstrap_legality_failure_contract =
      BuildRuntimeBootstrapLegalityFailureContractSummary(
          pipeline_result.sema_parity_surface
              .bootstrap_legality_failure_contract_summary,
          plan.runtime_registration_descriptor_frontend_closure,
          plan.runtime_bootstrap_semantics);
  plan.runtime_bootstrap_legality_semantics =
      BuildRuntimeBootstrapLegalitySemanticsSummary(
          pipeline_result.sema_parity_surface
              .bootstrap_legality_semantics_summary,
          plan.runtime_bootstrap_legality_failure_contract,
          plan.runtime_registration_descriptor_frontend_closure,
          plan.runtime_bootstrap_semantics,
          plan.translation_unit_identity_key);
  plan.runtime_bootstrap_lowering = BuildRuntimeBootstrapLoweringSummary(
      plan.runtime_translation_unit_registration_manifest,
      plan.runtime_bootstrap_semantics,
      plan.runtime_registration_descriptor_frontend_closure);
  plan.runtime_bootstrap_failure_restart_semantics =
      BuildRuntimeBootstrapFailureRestartSemanticsSummary(
          pipeline_result.sema_parity_surface
              .bootstrap_failure_restart_semantics_summary,
          plan.runtime_bootstrap_legality_semantics,
          plan.runtime_bootstrap_semantics,
          plan.runtime_bootstrap_api,
          plan.runtime_bootstrap_lowering,
          plan.translation_unit_identity_key);
  return plan;
}
