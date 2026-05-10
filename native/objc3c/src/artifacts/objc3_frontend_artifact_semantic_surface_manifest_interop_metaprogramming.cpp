#include "artifacts/objc3_frontend_artifact_semantic_surface_manifest_fields.h"

#include <ostream>

#include "artifacts/objc3_frontend_artifact_interop_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_preservation_plan.h"
#include "artifacts/objc3_frontend_artifact_semantic_lowering_plan.h"
#include "artifacts/objc3_frontend_interop_semantic_artifacts.h"
#include "artifacts/objc3_frontend_metaprogramming_semantic_artifacts.h"
#include "pipeline/results/pipeline_result_model.h"

namespace objc3::artifacts::frontend {

void WriteObjc3FrontendArtifactSemanticSurfaceInteropMetaprogrammingFields(
    std::ostream &manifest,
    const Objc3FrontendArtifactSemanticSurfaceManifestContext &context) {
  const auto &pipeline_result = context.pipeline_result;
  const auto &semantic_lowering_plan = context.semantic_lowering_plan;
  const auto &interop_lowering_plan = context.interop_lowering_plan;
  const auto &artifact_preservation_plan = context.artifact_preservation_plan;

  manifest
      << ",\"objc_interop_interop_semantic_model\":"
      << BuildInteropInteropSemanticModelSummaryJson(
             pipeline_result.interop_interop_semantic_model_summary)
      << ",\"objc_interop_c_and_objc_runtime_parity_semantics\":"
      << BuildInteropInteropRuntimeParitySummaryJson(
             pipeline_result.interop_interop_runtime_parity_summary)
      << ",\"objc_interop_cpp_ownership_throws_and_async_interactions\":"
      << BuildInteropCppInteropInteractionSummaryJson(
             pipeline_result.interop_cpp_interop_interaction_summary)
      << ",\"objc_interop_swift_metadata_and_isolation_mapping\":"
      << BuildInteropSwiftInteropIsolationSummaryJson(
             pipeline_result.interop_swift_interop_isolation_summary)
      << ",\"objc_interop_foreign_surface_interface_and_module_preservation\":"
      << BuildInteropForeignSurfaceInterfacePreservationSummaryJson(
             interop_lowering_plan
                 .interop_foreign_surface_interface_preservation_summary)
      << ",\"objc_interop_header_module_and_bridge_generation\":"
      << BuildInteropHeaderModuleBridgeGenerationSummaryJson(
             interop_lowering_plan
                 .interop_header_module_bridge_generation_summary)
      << ",\"objc_interop_interop_lowering_and_abi_contract\":"
      << BuildInteropInteropLoweringContractJson(
             pipeline_result.interop_interop_semantic_model_summary,
             pipeline_result.interop_interop_runtime_parity_summary,
             pipeline_result.interop_cpp_interop_interaction_summary,
             pipeline_result.interop_swift_interop_isolation_summary,
             interop_lowering_plan
                 .interop_foreign_surface_interface_preservation_summary,
             interop_lowering_plan.interop_interop_lowering_contract,
             interop_lowering_plan.interop_interop_lowering_replay_key)
      << ",\"objc_interop_foreign_call_and_lifetime_lowering\":"
      << BuildInteropForeignCallLifetimeLoweringContractJson(
             interop_lowering_plan.interop_interop_lowering_contract,
             pipeline_result.interop_cpp_interop_interaction_summary,
             interop_lowering_plan
                 .interop_foreign_surface_interface_preservation_summary,
             interop_lowering_plan
                 .interop_foreign_call_lifetime_lowering_contract,
             interop_lowering_plan
                 .interop_foreign_call_lifetime_lowering_replay_key)
      << ",\"objc_interop_ffi_metadata_and_interface_preservation\":"
      << BuildInteropFfiMetadataInterfacePreservationContractJson(
             interop_lowering_plan
                 .interop_foreign_call_lifetime_lowering_contract,
             interop_lowering_plan
                 .interop_foreign_call_lifetime_lowering_replay_key,
             interop_lowering_plan
                 .interop_foreign_surface_interface_preservation_summary,
             interop_lowering_plan
                 .interop_ffi_metadata_interface_preservation_contract,
             interop_lowering_plan
                 .interop_ffi_metadata_interface_preservation_replay_key)
      << ",\"objc_metaprogramming_expansion_and_behavior_semantic_model\":"
      << BuildMetaprogrammingExpansionBehaviorSemanticModelSummaryJson(
             pipeline_result
                 .metaprogramming_expansion_behavior_semantic_model_summary)
      << ",\"objc_metaprogramming_derive_expansion_inventory\":"
      << BuildMetaprogrammingDeriveExpansionInventorySummaryJson(
             pipeline_result.metaprogramming_derive_expansion_inventory_summary)
      << ",\"objc_metaprogramming_macro_safety_sandbox_and_determinism_semantics\":"
      << BuildMetaprogrammingMacroSafetySandboxDeterminismSummaryJson(
             pipeline_result
                 .metaprogramming_macro_safety_sandbox_determinism_summary)
      << ",\"objc_metaprogramming_property_behavior_legality_and_interaction_completion\":"
      << BuildMetaprogrammingPropertyBehaviorLegalityCompatibilitySummaryJson(
             pipeline_result
                 .metaprogramming_property_behavior_legality_compatibility_summary)
      << ",\"objc_metaprogramming_expansion_and_lowering_contract\":"
      << BuildMetaprogrammingExpansionLoweringContractJson(
             pipeline_result
                 .metaprogramming_property_behavior_source_completion_summary,
             pipeline_result.metaprogramming_derive_expansion_inventory_summary,
             pipeline_result
                 .metaprogramming_macro_safety_sandbox_determinism_summary,
             pipeline_result
                 .metaprogramming_property_behavior_legality_compatibility_summary,
             semantic_lowering_plan
                 .metaprogramming_expansion_lowering_contract,
             semantic_lowering_plan
                 .metaprogramming_expansion_lowering_replay_key)
      << ",\"objc_metaprogramming_synthesized_ast_and_ir_emission\":"
      << BuildMetaprogrammingSynthesizedArtifactEmissionContractJson(
             semantic_lowering_plan
                 .metaprogramming_expansion_lowering_contract,
             semantic_lowering_plan
                 .metaprogramming_synthesized_artifact_emission_contract,
             semantic_lowering_plan
                 .metaprogramming_synthesized_artifact_emission_replay_key)
      << ",\"objc_metaprogramming_module_interface_and_replay_preservation\":"
      << BuildMetaprogrammingModuleInterfaceReplayPreservationSummaryJson(
             artifact_preservation_plan
                 .metaprogramming_module_interface_replay_preservation_summary);
}

}  // namespace objc3::artifacts::frontend
