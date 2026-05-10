#pragma once

#include <iosfwd>

struct Objc3FrontendConcurrencyAsyncSourceClosureSummary;
struct Objc3FrontendControlFlowControlFlowSourceClosureSummary;
struct Objc3FrontendDispatchDispatchIntentSourceClosureSummary;
struct Objc3FrontendDispatchDispatchIntentSourceCompletionSummary;
struct Objc3FrontendErrorHandlingErrorSourceClosureSummary;
struct Objc3FrontendInteropCppSwiftInteropAnnotationSourceCompletionSummary;
struct Objc3FrontendInteropForeignImportSourceClosureSummary;
struct Objc3FrontendMetaprogrammingMacroPackageProvenanceSourceCompletionSummary;
struct Objc3FrontendMetaprogrammingMetaprogrammingSourceClosureSummary;
struct Objc3FrontendMetaprogrammingPropertyBehaviorSourceCompletionSummary;
struct Objc3FrontendObjectPointerNullabilityGenericsSummary;
struct Objc3FrontendOwnershipCleanupResourceCaptureSourceCompletionSummary;
struct Objc3FrontendOwnershipRetainableCFamilySourceCompletionSummary;
struct Objc3FrontendOwnershipSystemExtensionSourceClosureSummary;
struct Objc3FrontendTypeSystemTypeSourceClosureSummary;

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
        &interop_cpp_swift_interop_annotation_source_completion_summary);

}  // namespace objc3::artifacts::frontend
