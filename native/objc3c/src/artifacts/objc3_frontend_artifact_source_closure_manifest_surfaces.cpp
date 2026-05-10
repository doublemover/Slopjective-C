#include "artifacts/objc3_frontend_artifact_source_closure_manifest_surfaces.h"

#include <ostream>

#include "artifacts/objc3_frontend_source_closure_artifacts.h"
#include "sema/model/frontend_linkage_summaries.h"

namespace objc3::artifacts::frontend {

void WriteSourceClosureManifestSurfaces(
    std::ostream &manifest,
    const Objc3FrontendObjectPointerNullabilityGenericsSummary
        &object_pointer_nullability_generics_summary,
    const Objc3FrontendTypeSystemTypeSourceClosureSummary
        &type_system_type_source_closure_summary,
    const Objc3FrontendControlFlowControlFlowSourceClosureSummary
        &control_flow_control_flow_source_closure_summary,
    const Objc3FrontendErrorHandlingErrorSourceClosureSummary
        &error_handling_error_source_closure_summary,
    const Objc3FrontendConcurrencyAsyncSourceClosureSummary
        &concurrency_async_source_closure_summary,
    const Objc3FrontendOwnershipSystemExtensionSourceClosureSummary
        &ownership_system_extension_source_closure_summary,
    const Objc3FrontendOwnershipCleanupResourceCaptureSourceCompletionSummary
        &ownership_cleanup_resource_capture_source_completion_summary,
    const Objc3FrontendOwnershipRetainableCFamilySourceCompletionSummary
        &ownership_retainable_c_family_source_completion_summary,
    const Objc3FrontendDispatchDispatchIntentSourceClosureSummary
        &dispatch_dispatch_intent_source_closure_summary,
    const Objc3FrontendDispatchDispatchIntentSourceCompletionSummary
        &dispatch_dispatch_intent_source_completion_summary,
    const Objc3FrontendMetaprogrammingMetaprogrammingSourceClosureSummary
        &metaprogramming_metaprogramming_source_closure_summary,
    const Objc3FrontendMetaprogrammingMacroPackageProvenanceSourceCompletionSummary
        &metaprogramming_macro_package_provenance_source_completion_summary,
    const Objc3FrontendMetaprogrammingPropertyBehaviorSourceCompletionSummary
        &metaprogramming_property_behavior_source_completion_summary,
    const Objc3FrontendInteropForeignImportSourceClosureSummary
        &interop_foreign_import_source_closure_summary,
    const Objc3FrontendInteropCppSwiftInteropAnnotationSourceCompletionSummary
        &interop_cpp_swift_interop_annotation_source_completion_summary) {
  manifest
      << ",\"objc_object_pointer_nullability_generics_surface\":{\"object_pointer_type_spellings\":"
      << object_pointer_nullability_generics_summary.object_pointer_type_spellings
      << ",\"pointer_declarator_entries\":"
      << object_pointer_nullability_generics_summary.pointer_declarator_entries
      << ",\"pointer_declarator_depth_total\":"
      << object_pointer_nullability_generics_summary.pointer_declarator_depth_total
      << ",\"pointer_declarator_token_entries\":"
      << object_pointer_nullability_generics_summary
             .pointer_declarator_token_entries
      << ",\"nullability_suffix_entries\":"
      << object_pointer_nullability_generics_summary.nullability_suffix_entries
      << ",\"generic_suffix_entries\":"
      << object_pointer_nullability_generics_summary.generic_suffix_entries
      << ",\"terminated_generic_suffix_entries\":"
      << object_pointer_nullability_generics_summary
             .terminated_generic_suffix_entries
      << ",\"unterminated_generic_suffix_entries\":"
      << object_pointer_nullability_generics_summary
             .unterminated_generic_suffix_entries
      << ",\"deterministic_handoff\":"
      << (object_pointer_nullability_generics_summary
                  .deterministic_object_pointer_nullability_generics_handoff
              ? "true"
              : "false")
      << "}"
      << ",\"objc_type_system_type_source_closure\":"
      << BuildTypeSystemTypeSourceClosureSummaryJson(
             type_system_type_source_closure_summary)
      << ",\"objc_control_flow_control_flow_source_closure\":"
      << BuildControlFlowControlFlowSourceClosureSummaryJson(
             control_flow_control_flow_source_closure_summary)
      << ",\"objc_error_handling_error_source_closure\":"
      << BuildErrorHandlingErrorSourceClosureSummaryJson(
             error_handling_error_source_closure_summary)
      << ",\"objc_concurrency_async_source_closure\":"
      << BuildConcurrencyAsyncSourceClosureSummaryJson(
             concurrency_async_source_closure_summary)
      << ",\"objc_ownership_resource_borrowed_and_capture_list_source_closure\":"
      << BuildOwnershipSystemExtensionSourceClosureSummaryJson(
             ownership_system_extension_source_closure_summary)
      << ",\"objc_ownership_cleanup_resource_and_capture_source_completion\":"
      << BuildOwnershipCleanupResourceCaptureSourceCompletionSummaryJson(
             ownership_cleanup_resource_capture_source_completion_summary)
      << ",\"objc_ownership_retainable_c_family_source_completion\":"
      << BuildOwnershipRetainableCFamilySourceCompletionSummaryJson(
             ownership_retainable_c_family_source_completion_summary)
      << ",\"objc_dispatch_dispatch_intent_and_dynamism_source_closure\":"
      << BuildDispatchDispatchIntentSourceClosureSummaryJson(
             dispatch_dispatch_intent_source_closure_summary)
      << ",\"objc_dispatch_dispatch_intent_attribute_and_defaulting_source_completion\":"
      << BuildDispatchDispatchIntentSourceCompletionSummaryJson(
             dispatch_dispatch_intent_source_completion_summary)
      << ",\"objc_metaprogramming_derive_macro_property_behavior_source_closure\":"
      << BuildMetaprogrammingMetaprogrammingSourceClosureSummaryJson(
             metaprogramming_metaprogramming_source_closure_summary)
      << ",\"objc_metaprogramming_macro_package_and_provenance_source_completion\":"
      << BuildMetaprogrammingMacroPackageProvenanceSourceCompletionSummaryJson(
             metaprogramming_macro_package_provenance_source_completion_summary)
      << ",\"objc_metaprogramming_property_behavior_and_synthesized_declaration_source_completion\":"
      << BuildMetaprogrammingPropertyBehaviorSourceCompletionSummaryJson(
             metaprogramming_property_behavior_source_completion_summary)
      << ",\"objc_interop_foreign_declaration_and_import_source_closure\":"
      << BuildInteropForeignImportSourceClosureSummaryJson(
             interop_foreign_import_source_closure_summary)
      << ",\"objc_interop_cpp_and_swift_interop_annotation_source_completion\":"
      << BuildInteropCppSwiftInteropAnnotationSourceCompletionSummaryJson(
             interop_cpp_swift_interop_annotation_source_completion_summary);
}

}  // namespace objc3::artifacts::frontend
