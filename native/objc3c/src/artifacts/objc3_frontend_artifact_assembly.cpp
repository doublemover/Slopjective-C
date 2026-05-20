#include "artifacts/objc3_frontend_artifact_assembly.h"

#include <utility>

#include "artifacts/objc3_frontend_artifact_dispatch_contract_snapshots.h"
#include "artifacts/objc3_frontend_artifact_metadata_mode.h"
#include "pipeline/frontend_artifact_semantic_accessors.h"
#include "pipeline/results/runtime_import_evidence_record.h"

namespace objc3::artifacts::frontend {
namespace {

void RecordPostPipelineFailure(
    Objc3FrontendArtifactPostPipelineFailure &post_pipeline_failure,
    const Objc3FrontendArtifactPostPipelineFailure &failure) {
  if (!failure.empty()) {
    RecordObjc3FrontendArtifactPostPipelineFailure(
        post_pipeline_failure, failure.code.c_str(), failure.message);
  }
}

template <typename FailureRange>
void RecordPostPipelineFailures(
    Objc3FrontendArtifactPostPipelineFailure &post_pipeline_failure,
    const FailureRange &failures) {
  for (const auto &failure : failures) {
    RecordObjc3FrontendArtifactPostPipelineFailure(
        post_pipeline_failure, failure.code.c_str(), failure.message);
  }
}

}  // namespace

Objc3FrontendArtifactAssemblyContext BuildObjc3FrontendArtifactAssemblyContext(
    const std::filesystem::path &input_path,
    const Objc3Program &program,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options,
    const Objc3ParseLoweringReadinessSurface &parse_lowering_readiness_surface) {
  const bool metadata_only_ir_emission_mode =
      ::Objc3FrontendArtifactMetadataOnlyIrEmissionMode(pipeline_result);
  Objc3FrontendArtifactPostPipelineFailure post_pipeline_failure;

  auto ir_emission_core_feature_impl_surface =
      ::BuildObjc3IREmissionCoreFeatureImplementationSurface(pipeline_result);
  const auto initial_post_pipeline_failure =
      BuildObjc3FrontendArtifactInitialPostPipelineFailure(
          pipeline_result, parse_lowering_readiness_surface,
          ir_emission_core_feature_impl_surface,
          metadata_only_ir_emission_mode);
  RecordPostPipelineFailure(post_pipeline_failure,
                            initial_post_pipeline_failure);

  auto function_manifest =
      ::BuildObjc3FrontendArtifactFunctionManifest(program, pipeline_result);

  auto semantic_lowering_plan =
      ::BuildObjc3FrontendArtifactSemanticLoweringPlan(program,
                                                       pipeline_result);
  RecordPostPipelineFailure(post_pipeline_failure,
                            semantic_lowering_plan.post_pipeline_failure);

  auto type_system_type_semantic_model_summary =
      BuildObjc3FrontendArtifactTypeSemanticModelSummary(pipeline_result, 4u);

  auto runtime_metadata_plan =
      ::BuildObjc3FrontendArtifactRuntimeMetadataPlan(pipeline_result);
  auto runtime_registration_plan =
      ::BuildObjc3FrontendArtifactRuntimeRegistrationPlan(
          input_path, program, pipeline_result, options, runtime_metadata_plan);

  auto conformance_report_plan = BuildObjc3FrontendArtifactConformanceReportPlan(
      options, pipeline_result,
      pipeline_result.tooling_feature_specific_fixit_synthesis_summary);
  RecordPostPipelineFailure(post_pipeline_failure,
                            conformance_report_plan.post_pipeline_failure);

  auto core_lowering_plan = ::BuildObjc3FrontendArtifactCoreLoweringPlan(
      program, pipeline_result, options, type_system_type_semantic_model_summary,
      pipeline_result.control_flow_control_flow_semantic_model_summary,
      runtime_registration_plan.runtime_bootstrap_api);
  RecordPostPipelineFailures(post_pipeline_failure,
                             core_lowering_plan.post_pipeline_failures);

  auto ownership_aware_lowering_plan =
      ::BuildObjc3FrontendArtifactOwnershipAwareLoweringPlan(
          pipeline_result, metadata_only_ir_emission_mode);
  RecordPostPipelineFailures(
      post_pipeline_failure,
      ownership_aware_lowering_plan.post_pipeline_failures);

  auto block_lowering_plan =
      ::BuildObjc3FrontendArtifactBlockLoweringPlan(pipeline_result);
  RecordPostPipelineFailures(post_pipeline_failure,
                             block_lowering_plan.post_pipeline_failures);

  auto type_system_lowering_plan =
      ::BuildObjc3FrontendArtifactTypeSystemLoweringPlan(pipeline_result);
  RecordPostPipelineFailures(post_pipeline_failure,
                             type_system_lowering_plan.post_pipeline_failures);

  auto runtime_import_plan = ::BuildObjc3FrontendArtifactRuntimeImportPlan(
      program, pipeline_result, options,
      pipeline_result.runtime_metadata_source_records,
      runtime_registration_plan.runtime_translation_unit_registration_manifest,
      !post_pipeline_failure.empty());
  RecordPostPipelineFailures(post_pipeline_failure,
                             runtime_import_plan.post_pipeline_failures);

  auto module_lowering_plan =
      ::BuildObjc3FrontendArtifactModuleLoweringPlan(pipeline_result);
  RecordPostPipelineFailures(post_pipeline_failure,
                             module_lowering_plan.post_pipeline_failures);

  auto error_lowering_plan =
      ::BuildObjc3FrontendArtifactErrorLoweringPlan(pipeline_result);
  RecordPostPipelineFailures(post_pipeline_failure,
                             error_lowering_plan.post_pipeline_failures);

  const bool runtime_import_artifact_ready =
      ::IsReadyObjc3RuntimeAwareImportModuleFrontendClosureSummary(
          runtime_import_plan.runtime_aware_import_module_frontend_closure);
  auto interop_lowering_plan = ::BuildObjc3FrontendArtifactInteropLoweringPlan(
      program, pipeline_result.interop_foreign_import_source_closure_summary,
      pipeline_result
          .interop_cpp_swift_interop_annotation_source_completion_summary,
      pipeline_result.interop_interop_semantic_model_summary,
      pipeline_result.interop_interop_runtime_parity_summary,
      pipeline_result.interop_cpp_interop_interaction_summary,
      pipeline_result.interop_swift_interop_isolation_summary,
      error_lowering_plan
          .error_handling_throws_abi_propagation_lowering_replay_key,
      error_lowering_plan.throws_propagation_lowering_replay_key,
      error_lowering_plan.result_like_lowering_replay_key,
      error_lowering_plan.ns_error_bridging_lowering_replay_key,
      error_lowering_plan.unwind_cleanup_lowering_replay_key,
      error_lowering_plan
          .deterministic_error_handling_throws_abi_propagation_lowering,
      runtime_import_artifact_ready,
      runtime_import_plan.imported_runtime_module_surfaces);
  RecordPostPipelineFailures(post_pipeline_failure,
                             interop_lowering_plan.post_pipeline_failures);

  auto artifact_preservation_plan =
      ::BuildObjc3FrontendArtifactPreservationPlan(
          pipeline_result.runtime_metadata_source_records,
          semantic_lowering_plan.dispatch_dispatch_control_lowering_replay_key,
          runtime_import_artifact_ready,
          runtime_import_plan.imported_runtime_module_surfaces,
          block_lowering_plan.block_abi_invoke_trampoline_lowering_contract,
          block_lowering_plan.block_storage_escape_lowering_contract,
          block_lowering_plan.block_copy_dispose_lowering_contract,
          ownership_aware_lowering_plan
              .retain_release_operation_lowering_contract,
          ownership_aware_lowering_plan
              .retain_release_operation_lowering_replay_key,
          ownership_aware_lowering_plan.autoreleasepool_scope_lowering_contract,
          ownership_aware_lowering_plan.autoreleasepool_scope_lowering_replay_key,
          runtime_registration_plan.runtime_support_library_link_wiring,
          semantic_lowering_plan.metaprogramming_expansion_lowering_contract,
          semantic_lowering_plan.metaprogramming_expansion_lowering_replay_key,
          semantic_lowering_plan
              .metaprogramming_synthesized_artifact_emission_contract,
          semantic_lowering_plan
              .metaprogramming_synthesized_artifact_emission_replay_key,
          semantic_lowering_plan
              .metaprogramming_property_behavior_artifact_bundles,
          options);
  const auto dispatch_dispatch_metadata_interface_preservation_snapshot =
      BuildDispatchMetadataPreservationSnapshot(
          artifact_preservation_plan
              .dispatch_dispatch_metadata_interface_preservation_summary);
  (void)dispatch_dispatch_metadata_interface_preservation_snapshot;

  auto source_shape_plan =
      ::BuildObjc3FrontendArtifactSourceShapePlan(program, pipeline_result);
  RecordPostPipelineFailures(post_pipeline_failure,
                             source_shape_plan.post_pipeline_failures);

  return Objc3FrontendArtifactAssemblyContext{
      .post_pipeline_failure = std::move(post_pipeline_failure),
      .ir_emission_core_feature_impl_surface =
          std::move(ir_emission_core_feature_impl_surface),
      .function_manifest = std::move(function_manifest),
      .semantic_lowering_plan = std::move(semantic_lowering_plan),
      .type_system_type_semantic_model_summary =
          std::move(type_system_type_semantic_model_summary),
      .runtime_metadata_plan = std::move(runtime_metadata_plan),
      .runtime_registration_plan = std::move(runtime_registration_plan),
      .conformance_report_plan = std::move(conformance_report_plan),
      .core_lowering_plan = std::move(core_lowering_plan),
      .ownership_aware_lowering_plan =
          std::move(ownership_aware_lowering_plan),
      .block_lowering_plan = std::move(block_lowering_plan),
      .type_system_lowering_plan = std::move(type_system_lowering_plan),
      .runtime_import_plan = std::move(runtime_import_plan),
      .module_lowering_plan = std::move(module_lowering_plan),
      .error_lowering_plan = std::move(error_lowering_plan),
      .interop_lowering_plan = std::move(interop_lowering_plan),
      .artifact_preservation_plan = std::move(artifact_preservation_plan),
      .source_shape_plan = std::move(source_shape_plan)};
}

}  // namespace objc3::artifacts::frontend
