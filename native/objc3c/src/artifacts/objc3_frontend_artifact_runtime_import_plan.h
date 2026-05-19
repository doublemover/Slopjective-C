#pragma once

#include <string>
#include <vector>

#include "artifacts/objc3_frontend_artifact_diagnostics.h"
#include "ast/objc3_ast_contracts_metadata_packaging.h"
#include "ast/objc3_ast_declarations.h"
#include "lower/contracts/cross_module_lowering_contracts.h"
#include "pipeline/results/compile_options.h"
#include "pipeline/results/pipeline_result_model.h"
#include "pipeline/results/runtime_import_evidence_record.h"
#include "runtime/metadata/runtime_metadata_model.h"
#include "pipeline/objc3_runtime_import_surface.h"
#include "runtime/metadata/selector_metadata_registration_manifest.h"
#include "sema/model/semantic_type_cross_module_build_orchestration.h"
#include "sema/model/semantic_type_cross_module_runtime_preservation.h"
#include "sema/model/semantic_type_imported_runtime_metadata_rules.h"
#include "sema/model/semantic_type_serialized_runtime_metadata.h"

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
