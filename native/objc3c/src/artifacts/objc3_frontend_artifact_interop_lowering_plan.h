#pragma once

#include <string>
#include <vector>

#include "artifacts/evidence/error_handling_replay_evidence.h"
#include "artifacts/objc3_frontend_artifact_diagnostics.h"
#include "artifacts/objc3_frontend_interop_semantic_artifacts.h"
#include "pipeline/objc3_frontend_types.h"
#include "pipeline/objc3_runtime_import_surface.h"

struct Objc3FrontendArtifactInteropLoweringPlan {
  objc3::artifacts::evidence::ErrorHandlingResultAndBridgingArtifactReplayEvidence
      error_handling_result_and_bridging_artifact_replay_summary;
  Objc3InteropForeignSurfaceInterfacePreservationSummary
      interop_foreign_surface_interface_preservation_summary;
  Objc3InteropInteropLoweringContract interop_interop_lowering_contract;
  std::string interop_interop_lowering_replay_key;
  Objc3InteropForeignCallLifetimeLoweringContract
      interop_foreign_call_lifetime_lowering_contract;
  std::string interop_foreign_call_lifetime_lowering_replay_key;
  std::string interop_ffi_metadata_interface_preservation_replay_key;
  Objc3InteropFfiMetadataInterfacePreservationContract
      interop_ffi_metadata_interface_preservation_contract;
  Objc3InteropHeaderModuleBridgeGenerationSummary
      interop_header_module_bridge_generation_summary;
  std::vector<
      objc3::artifacts::frontend::Objc3FrontendArtifactPostPipelineFailure>
      post_pipeline_failures;
};

Objc3FrontendArtifactInteropLoweringPlan
BuildObjc3FrontendArtifactInteropLoweringPlan(
    const Objc3Program &program,
    const Objc3FrontendInteropForeignImportSourceClosureSummary
        &interop_foreign_import_source_closure_summary,
    const Objc3FrontendInteropCppSwiftInteropAnnotationSourceCompletionSummary
        &interop_cpp_swift_interop_annotation_source_completion_summary,
    const Objc3InteropInteropSemanticModelSummary
        &interop_interop_semantic_model_summary,
    const Objc3InteropInteropRuntimeParitySummary
        &interop_interop_runtime_parity_summary,
    const Objc3InteropCppInteropInteractionSummary
        &interop_cpp_interop_interaction_summary,
    const Objc3InteropSwiftInteropIsolationSummary
        &interop_swift_interop_isolation_summary,
    const std::string
        &error_handling_throws_abi_propagation_lowering_replay_key,
    const std::string &throws_propagation_lowering_replay_key,
    const std::string &result_like_lowering_replay_key,
    const std::string &ns_error_bridging_lowering_replay_key,
    const std::string &unwind_cleanup_lowering_replay_key,
    bool deterministic_error_handling_throws_abi_propagation_lowering,
    bool runtime_import_artifact_ready,
    const std::vector<Objc3ImportedRuntimeModuleSurface>
        &imported_runtime_module_surfaces);
