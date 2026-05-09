#pragma once

#include <string>
#include <vector>

#include "artifacts/objc3_frontend_artifact_diagnostics.h"
#include "artifacts/objc3_frontend_runtime_import_artifacts.h"
#include "artifacts/objc3_frontend_type_system_contract_artifacts.h"
#include "pipeline/objc3_frontend_types.h"

struct Objc3FrontendArtifactRuntimeImportPlan {
  Objc3ModuleImportGraphLoweringContract module_import_graph_lowering_contract;
  std::string module_import_graph_lowering_replay_key;
  Objc3RuntimeAwareImportModuleFrontendClosureSummary
      runtime_aware_import_module_frontend_closure;
  Objc3CrossModuleRuntimeMetadataSemanticPreservationSummary
      cross_module_runtime_metadata_semantic_preservation;
  std::vector<Objc3ImportedRuntimeModuleSurface>
      imported_runtime_module_surfaces;
  Objc3ImportedRuntimeMetadataSemanticRulesSummary
      imported_runtime_metadata_semantic_rules;
  bool has_imported_runtime_surface_inputs = false;
  Objc3SerializedRuntimeMetadataImportLoweringSummary
      serialized_runtime_metadata_import_lowering;
  std::vector<std::string> serialized_runtime_metadata_reused_module_names;
  Objc3RuntimeMetadataSourceRecordSet serialized_runtime_metadata_reuse_records;
  Objc3SerializedRuntimeMetadataArtifactReuseSummary
      serialized_runtime_metadata_artifact_reuse;
  Objc3CrossModuleBuildRuntimeOrchestrationSummary
      cross_module_build_runtime_orchestration;
  std::vector<
      objc3::artifacts::frontend::Objc3FrontendArtifactPostPipelineFailure>
      post_pipeline_failures;
};

Objc3FrontendArtifactRuntimeImportPlan
BuildObjc3FrontendArtifactRuntimeImportPlan(
    const Objc3Program &program,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    bool prior_post_pipeline_failure_present);
