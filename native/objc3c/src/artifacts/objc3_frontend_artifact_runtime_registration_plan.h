#pragma once

#include <filesystem>
#include <string>

#include "artifacts/objc3_frontend_artifact_runtime_metadata_plan.h"
#include "pipeline/objc3_frontend_types.h"

struct Objc3FrontendArtifactRuntimeRegistrationPlan {
  Objc3RuntimeSupportLibraryContractSummary runtime_support_library;
  Objc3RuntimeSupportLibraryCoreFeatureSummary
      runtime_support_library_core_feature;
  Objc3RuntimeSupportLibraryLinkWiringSummary runtime_support_library_link_wiring;
  Objc3RuntimeTranslationUnitRegistrationContractSummary
      runtime_translation_unit_registration_contract;
  Objc3RuntimeTranslationUnitRegistrationManifestSummary
      runtime_translation_unit_registration_manifest;
  Objc3RuntimeRegistrationDescriptorImageRootSourceSurfaceSummary
      runtime_registration_descriptor_image_root_source_surface;
  Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
      runtime_registration_descriptor_frontend_closure;
  std::string translation_unit_identity_key;
  Objc3RuntimeStartupBootstrapInvariantSummary
      runtime_startup_bootstrap_invariants;
  Objc3RuntimeBootstrapApiSummary runtime_bootstrap_api;
  Objc3RuntimeBootstrapSemanticsSummary runtime_bootstrap_semantics;
  Objc3RuntimeBootstrapLegalityFailureContractSummary
      runtime_bootstrap_legality_failure_contract;
  Objc3RuntimeBootstrapLegalitySemanticsSummary
      runtime_bootstrap_legality_semantics;
  Objc3RuntimeBootstrapLoweringSummary runtime_bootstrap_lowering;
  Objc3RuntimeBootstrapFailureRestartSemanticsSummary
      runtime_bootstrap_failure_restart_semantics;
};

Objc3FrontendArtifactRuntimeRegistrationPlan
BuildObjc3FrontendArtifactRuntimeRegistrationPlan(
    const std::filesystem::path &input_path,
    const Objc3Program &program,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options,
    const Objc3FrontendArtifactRuntimeMetadataPlan &runtime_metadata_plan);
