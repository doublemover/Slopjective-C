#include "artifacts/objc3_frontend_artifacts.h"

#include <string>

#include "artifacts/objc3_frontend_artifact_assembly.h"
#include "artifacts/objc3_frontend_artifact_bundle_publication.h"
#include "contracts/objc3_frontend_diagnostic_stage_flattening.h"
#include "parse/objc3_parser_contract_types.h"
#include "pipeline/objc3_parse_lowering_readiness_surface.h"

Objc3FrontendArtifactBundle BuildObjc3FrontendArtifacts(
    const std::filesystem::path &input_path,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options) {
  Objc3FrontendArtifactBundle bundle;
  const Objc3Program &program = Objc3ParsedProgramAst(pipeline_result.program);
  bundle.stage_diagnostics = pipeline_result.stage_diagnostics;
  bundle.parse_lowering_readiness_surface =
      BuildObjc3ParseLoweringReadinessSurface(pipeline_result, options);
  bundle.diagnostics = FlattenStageDiagnostics(bundle.stage_diagnostics);
  if (!bundle.diagnostics.empty()) {
    return bundle;
  }

  const auto context =
      objc3::artifacts::frontend::BuildObjc3FrontendArtifactAssemblyContext(
          input_path, program, pipeline_result, options,
          bundle.parse_lowering_readiness_surface);
  const std::string manifest_json =
      objc3::artifacts::frontend::BuildObjc3FrontendArtifactManifest(
          input_path, program, pipeline_result, options, bundle, context);

  objc3::artifacts::frontend::PublishObjc3FrontendArtifactBundleOutputs({
      .bundle = bundle,
      .input_path = input_path,
      .pipeline_result = pipeline_result,
      .options = options,
      .program = program,
      .manifest_json = manifest_json,
      .post_pipeline_failure = context.post_pipeline_failure,
      .ir_emission_core_feature_impl_surface =
          context.ir_emission_core_feature_impl_surface,
      .type_system_type_semantic_model_summary =
          context.type_system_type_semantic_model_summary,
      .conformance_report_plan = context.conformance_report_plan,
      .semantic_lowering_plan = context.semantic_lowering_plan,
      .core_lowering_plan = context.core_lowering_plan,
      .ownership_aware_lowering_plan = context.ownership_aware_lowering_plan,
      .block_lowering_plan = context.block_lowering_plan,
      .type_system_lowering_plan = context.type_system_lowering_plan,
      .runtime_import_plan = context.runtime_import_plan,
      .module_lowering_plan = context.module_lowering_plan,
      .error_lowering_plan = context.error_lowering_plan,
      .interop_lowering_plan = context.interop_lowering_plan,
      .artifact_preservation_plan = context.artifact_preservation_plan,
      .runtime_metadata_plan = context.runtime_metadata_plan,
      .runtime_registration_plan = context.runtime_registration_plan});

  return bundle;
}
