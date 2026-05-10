#include "pipeline/frontend_pipeline_orchestration_owners.h"

#include "parse/objc3_ast_builder_contract.h"
#include "pipeline/dispatch_surface_classification.h"
#include "pipeline/frontend_concurrency_source_closure_helpers.h"
#include "pipeline/frontend_control_flow_source_closure_helpers.h"
#include "pipeline/frontend_dispatch_source_completion_helpers.h"
#include "pipeline/frontend_error_handling_source_closure_helpers.h"
#include "pipeline/frontend_interop_source_closure_helpers.h"
#include "pipeline/frontend_interop_source_completion_helpers.h"
#include "pipeline/frontend_metaprogramming_source_completion_helpers.h"
#include "pipeline/frontend_ownership_retainable_c_family_completion_helpers.h"
#include "pipeline/frontend_ownership_source_closure_helpers.h"
#include "pipeline/frontend_ownership_source_completion_helpers.h"
#include "pipeline/frontend_semantic_metadata_summary_helpers.h"
#include "pipeline/frontend_tooling_source_completion_helpers.h"
#include "pipeline/frontend_type_system_source_closure_helpers.h"

namespace objc3_frontend_pipeline_orchestration {

void PopulateSourceReadinessSummaries(
    Objc3FrontendPipelineResult &result,
    const Objc3FrontendOptions &options,
    const std::vector<Objc3LexToken> &tokens) {
  result.selector_normalization_summary =
      objc3c::pipeline::orchestration::BuildSelectorNormalizationSummary(
          Objc3ParsedProgramAst(result.program));
  result.property_attribute_summary =
      objc3c::pipeline::orchestration::BuildPropertyAttributeSummary(
          Objc3ParsedProgramAst(result.program));
  result.object_pointer_nullability_generics_summary =
      objc3c::pipeline::orchestration::BuildObjectPointerNullabilityGenericsSummary(
          Objc3ParsedProgramAst(result.program));
  result.type_system_type_source_closure_summary =
      objc3c::pipeline::orchestration::BuildTypeSystemTypeSourceClosureSummary(
          Objc3ParsedProgramAst(result.program),
          result.object_pointer_nullability_generics_summary);
  result.control_flow_control_flow_source_closure_summary =
      objc3c::pipeline::orchestration::BuildControlFlowControlFlowSourceClosureSummary(
          Objc3ParsedProgramAst(result.program), tokens);
  result.error_handling_error_source_closure_summary =
      objc3c::pipeline::orchestration::BuildErrorHandlingErrorSourceClosureSummary(
          Objc3ParsedProgramAst(result.program), tokens);
  result.concurrency_async_source_closure_summary =
      objc3c::pipeline::orchestration::BuildConcurrencyAsyncSourceClosureSummary(
          Objc3ParsedProgramAst(result.program), tokens);
  result.ownership_system_extension_source_closure_summary =
      objc3c::pipeline::orchestration::BuildOwnershipSystemExtensionSourceClosureSummary(
          Objc3ParsedProgramAst(result.program));
  result.ownership_cleanup_resource_capture_source_completion_summary =
      objc3c::pipeline::orchestration::BuildOwnershipCleanupResourceCaptureSourceCompletionSummary(
          Objc3ParsedProgramAst(result.program));
  result.ownership_retainable_c_family_source_completion_summary =
      objc3c::pipeline::orchestration::BuildOwnershipRetainableCFamilySourceCompletionSummary(
          Objc3ParsedProgramAst(result.program));
  result.dispatch_dispatch_intent_source_closure_summary =
      objc3c::pipeline::orchestration::BuildDispatchDispatchIntentSourceClosureSummary(
          Objc3ParsedProgramAst(result.program));
  result.dispatch_dispatch_intent_source_completion_summary =
      objc3c::pipeline::orchestration::BuildDispatchDispatchIntentSourceCompletionSummary(
          Objc3ParsedProgramAst(result.program));
  result.metaprogramming_metaprogramming_source_closure_summary =
      objc3c::pipeline::orchestration::BuildMetaprogrammingMetaprogrammingSourceClosureSummary(
          Objc3ParsedProgramAst(result.program));
  result.metaprogramming_macro_package_provenance_source_completion_summary =
      objc3c::pipeline::orchestration::BuildMetaprogrammingMacroPackageProvenanceSourceCompletionSummary(
          Objc3ParsedProgramAst(result.program));
  result.metaprogramming_property_behavior_source_completion_summary =
      objc3c::pipeline::orchestration::BuildMetaprogrammingPropertyBehaviorSourceCompletionSummary(
          Objc3ParsedProgramAst(result.program));
  result.interop_foreign_import_source_closure_summary =
      objc3c::pipeline::orchestration::BuildInteropForeignImportSourceClosureSummary(
          Objc3ParsedProgramAst(result.program));
  result.interop_cpp_swift_interop_annotation_source_completion_summary =
      objc3c::pipeline::orchestration::BuildInteropCppSwiftInteropAnnotationSourceCompletionSummary(
          Objc3ParsedProgramAst(result.program));
  result.concurrency_actor_member_isolation_source_closure_summary =
      objc3c::pipeline::orchestration::BuildConcurrencyActorMemberIsolationSourceClosureSummary(
          Objc3ParsedProgramAst(result.program));
  result.concurrency_task_group_cancellation_source_closure_summary =
      objc3c::pipeline::orchestration::BuildConcurrencyTaskGroupCancellationSourceClosureSummary(
          Objc3ParsedProgramAst(result.program));
  result.tooling_diagnostics_migrator_source_inventory_summary =
      objc3c::pipeline::orchestration::BuildToolingDiagnosticsMigratorSourceInventorySummary(
          result.canonical_literal_rejection_counts,
          result.error_handling_error_source_closure_summary,
          result.concurrency_async_source_closure_summary,
          result.concurrency_actor_member_isolation_source_closure_summary,
          result.concurrency_task_group_cancellation_source_closure_summary,
          result.ownership_system_extension_source_closure_summary,
          result.ownership_cleanup_resource_capture_source_completion_summary,
          result.ownership_retainable_c_family_source_completion_summary,
          result.dispatch_dispatch_intent_source_closure_summary,
          result.dispatch_dispatch_intent_source_completion_summary,
          result.metaprogramming_metaprogramming_source_closure_summary,
          result.metaprogramming_macro_package_provenance_source_completion_summary,
          result.metaprogramming_property_behavior_source_completion_summary,
          result.interop_foreign_import_source_closure_summary,
          result.interop_cpp_swift_interop_annotation_source_completion_summary);
  result.tooling_migration_canonicalization_source_completion_summary =
      objc3c::pipeline::orchestration::BuildToolingMigrationCanonicalizationSourceCompletionSummary(
          options, result.canonical_literal_rejection_counts,
          result.tooling_diagnostics_migrator_source_inventory_summary);
}

}  // namespace objc3_frontend_pipeline_orchestration
