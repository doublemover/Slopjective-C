#include "artifacts/objc3_frontend_artifact_ir_emission_completion.h"

#include <string>

#include "artifacts/objc3_frontend_artifact_debug_projection_metadata.h"
#include "artifacts/objc3_frontend_artifact_object_inspection_metadata.h"
#include "artifacts/objc3_frontend_artifact_pipeline_readiness_metadata.h"
#include "artifacts/objc3_frontend_artifact_runtime_bootstrap_metadata.h"
#include "artifacts/objc3_frontend_artifact_runtime_metadata_typed_bundles.h"
#include "artifacts/objc3_frontend_artifact_runtime_support_library_metadata.h"
#include "artifacts/objc3_frontend_artifact_sanity.h"
#include "diag/objc3_diag_format.h"
#include "pipeline/frontend_ir_text_emission.h"

namespace objc3::artifacts::frontend {

void ApplyObjc3FrontendFinalRuntimeAndReadinessMetadata(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const Objc3FrontendArtifactBundle &bundle,
    const Objc3ExecutableMetadataTypedLoweringHandoff
        &executable_metadata_typed_lowering_handoff,
    const Objc3RuntimeMetadataSectionPublicationSummary
        &runtime_metadata_section_publication,
    const Objc3RuntimeMetadataObjectInspectionHarnessSummary
        &runtime_metadata_object_inspection,
    const Objc3ExecutableMetadataDebugProjectionSummary
        &executable_metadata_debug_projection,
    const Objc3RuntimeSupportLibraryContractSummary &runtime_support_library,
    const Objc3RuntimeSupportLibraryCoreFeatureSummary
        &runtime_support_library_core_feature,
    const Objc3RuntimeSupportLibraryLinkWiringSummary
        &runtime_support_library_link_wiring,
    const Objc3OwnershipAwareLoweringBehaviorScaffold
        &ownership_aware_lowering_behavior_scaffold,
    const Objc3IREmissionCompletenessScaffold
        &ir_emission_completeness_scaffold,
    const Objc3LoweringPipelinePassGraphCoreFeatureSurface
        &lowering_pipeline_pass_graph_core_feature_surface,
    const Objc3IREmissionCoreFeatureImplementationSurface
        &ir_emission_core_feature_impl_surface) {
  // The native IR emitter consumes this lowering packet directly when it
  // materializes the ctor root, derived init stub, registration table, image
  // descriptor, and image-local init-state model.
  ApplyObjc3FrontendRuntimeBootstrapMetadata(
      ir_frontend_metadata, bundle.runtime_bootstrap_lowering_summary,
      bundle.runtime_registration_descriptor_frontend_closure_summary,
      bundle.runtime_translation_unit_registration_manifest_summary);
  ApplyObjc3FrontendRuntimeMetadataTypedLoweringBundles(
      ir_frontend_metadata, ir_frontend_metadata,
      ir_frontend_metadata.metaprogramming_derived_method_bundles_lexicographic,
      executable_metadata_typed_lowering_handoff,
      runtime_metadata_section_publication);
  ApplyObjc3FrontendObjectInspectionMetadata(
      ir_frontend_metadata, runtime_metadata_object_inspection);
  ApplyObjc3FrontendDebugProjectionMetadata(ir_frontend_metadata,
                                            executable_metadata_debug_projection);
  ApplyObjc3FrontendRuntimeSupportLibraryMetadata(
      ir_frontend_metadata, runtime_support_library,
      runtime_support_library_core_feature, runtime_support_library_link_wiring);
  ApplyObjc3FrontendPipelineReadinessMetadata(
      ir_frontend_metadata, ownership_aware_lowering_behavior_scaffold,
      ir_emission_completeness_scaffold,
      lowering_pipeline_pass_graph_core_feature_surface,
      ir_emission_core_feature_impl_surface);
}

bool CompleteObjc3FrontendArtifactIREmission(
    Objc3FrontendArtifactBundle &bundle,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options,
    const Objc3Program &program,
    const Objc3IRFrontendMetadata &ir_frontend_metadata,
    const std::string &runtime_dispatch_lowering_abi_boundary_summary,
    std::size_t message_send_sites) {
  std::string ir_error;
  // Historical extraction contract marker:
  // EmitObjc3IRText(pipeline_result.program, options.lowering,
  // ir_frontend_metadata, bundle.ir_text, ir_error)
  if (!EmitObjc3FrontendIRTextForArtifact(
          pipeline_result.program.ast, options.lowering, ir_frontend_metadata,
          bundle.ir_text, ir_error)) {
    bundle.post_pipeline_diagnostics = {
        MakeDiag(1, 1, "O3L300", "LLVM IR emission failed: " + ir_error)};
    bundle.diagnostics = bundle.post_pipeline_diagnostics;
    bundle.manifest_json.clear();
    bundle.runtime_metadata_binary.clear();
    bundle.ir_text.clear();
    return false;
  }
  bundle.ir_text =
      std::string("; runtime_dispatch_lowering_abi_boundary = ") +
      runtime_dispatch_lowering_abi_boundary_summary +
      "\n" + bundle.ir_text;

  if (objc3c::artifacts::IsSuspiciousObjc3NativeIRTruthGap(
          bundle.ir_text, program, message_send_sites)) {
    bundle.post_pipeline_diagnostics = {
        MakeDiag(1, 1, "O3L330",
                 "LLVM IR emission failed: emitted native IR is suspiciously "
                 "trivial for runtime-bearing executable surface")};
    bundle.diagnostics = bundle.post_pipeline_diagnostics;
    bundle.manifest_json.clear();
    bundle.runtime_metadata_binary.clear();
    bundle.ir_text.clear();
    return false;
  }

  return true;
}

}  // namespace objc3::artifacts::frontend
