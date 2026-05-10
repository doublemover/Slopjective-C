#include "artifacts/objc3_frontend_artifact_semantic_surface_manifest_fields.h"

#include <ostream>

#include "artifacts/objc3_frontend_source_closure_artifacts.h"
#include "artifacts/objc3_frontend_tooling_source_artifacts.h"
#include "pipeline/results/pipeline_result_model.h"

namespace objc3::artifacts::frontend {

void WriteObjc3FrontendArtifactSemanticSurfaceSourceToolingFields(
    std::ostream &manifest,
    const Objc3FrontendArtifactSemanticSurfaceManifestContext &context) {
  const auto &pipeline_result = context.pipeline_result;

  manifest
      << ",\"objc_type_system_type_source_closure\":"
      << BuildTypeSystemTypeSourceClosureSummaryJson(
             pipeline_result.type_system_type_source_closure_summary)
      << ",\"objc_control_flow_control_flow_source_closure\":"
      << BuildControlFlowControlFlowSourceClosureSummaryJson(
             pipeline_result.control_flow_control_flow_source_closure_summary)
      << ",\"objc_error_handling_error_source_closure\":"
      << BuildErrorHandlingErrorSourceClosureSummaryJson(
             pipeline_result.error_handling_error_source_closure_summary)
      << ",\"objc_concurrency_async_source_closure\":"
      << BuildConcurrencyAsyncSourceClosureSummaryJson(
             pipeline_result.concurrency_async_source_closure_summary)
      << ",\"objc_ownership_resource_borrowed_and_capture_list_source_closure\":"
      << BuildOwnershipSystemExtensionSourceClosureSummaryJson(
             pipeline_result.ownership_system_extension_source_closure_summary)
      << ",\"objc_ownership_cleanup_resource_and_capture_source_completion\":"
      << BuildOwnershipCleanupResourceCaptureSourceCompletionSummaryJson(
             pipeline_result
                 .ownership_cleanup_resource_capture_source_completion_summary)
      << ",\"objc_ownership_retainable_c_family_source_completion\":"
      << BuildOwnershipRetainableCFamilySourceCompletionSummaryJson(
             pipeline_result
                 .ownership_retainable_c_family_source_completion_summary)
      << ",\"objc_dispatch_dispatch_intent_and_dynamism_source_closure\":"
      << BuildDispatchDispatchIntentSourceClosureSummaryJson(
             pipeline_result.dispatch_dispatch_intent_source_closure_summary)
      << ",\"objc_dispatch_dispatch_intent_attribute_and_defaulting_source_completion\":"
      << BuildDispatchDispatchIntentSourceCompletionSummaryJson(
             pipeline_result.dispatch_dispatch_intent_source_completion_summary)
      << ",\"objc_metaprogramming_derive_macro_property_behavior_source_closure\":"
      << BuildMetaprogrammingMetaprogrammingSourceClosureSummaryJson(
             pipeline_result
                 .metaprogramming_metaprogramming_source_closure_summary)
      << ",\"objc_metaprogramming_macro_package_and_provenance_source_completion\":"
      << BuildMetaprogrammingMacroPackageProvenanceSourceCompletionSummaryJson(
             pipeline_result
                 .metaprogramming_macro_package_provenance_source_completion_summary)
      << ",\"objc_metaprogramming_property_behavior_and_synthesized_declaration_source_completion\":"
      << BuildMetaprogrammingPropertyBehaviorSourceCompletionSummaryJson(
             pipeline_result
                 .metaprogramming_property_behavior_source_completion_summary)
      << ",\"objc_interop_foreign_declaration_and_import_source_closure\":"
      << BuildInteropForeignImportSourceClosureSummaryJson(
             pipeline_result.interop_foreign_import_source_closure_summary)
      << ",\"objc_interop_cpp_and_swift_interop_annotation_source_completion\":"
      << BuildInteropCppSwiftInteropAnnotationSourceCompletionSummaryJson(
             pipeline_result
                 .interop_cpp_swift_interop_annotation_source_completion_summary)
      << ",\"objc_tooling_diagnostics_fixit_and_migrator_source_inventory\":"
      << BuildToolingDiagnosticsMigratorSourceInventorySummaryJson(
             pipeline_result
                 .tooling_diagnostics_migrator_source_inventory_summary)
      << ",\"objc_tooling_migration_and_canonicalization_source_completion\":"
      << BuildToolingMigrationCanonicalizationSourceCompletionSummaryJson(
             pipeline_result
                 .tooling_migration_canonicalization_source_completion_summary)
      << ",\"objc_tooling_diagnostic_taxonomy_and_portability_contract\":"
      << BuildToolingDiagnosticTaxonomyPortabilityContractSummaryJson(
             pipeline_result
                 .tooling_diagnostic_taxonomy_portability_contract_summary)
      << ",\"objc_tooling_feature_specific_fixit_synthesis\":"
      << BuildToolingFeatureSpecificFixitSynthesisSummaryJson(
             pipeline_result.tooling_feature_specific_fixit_synthesis_summary);
}

}  // namespace objc3::artifacts::frontend
