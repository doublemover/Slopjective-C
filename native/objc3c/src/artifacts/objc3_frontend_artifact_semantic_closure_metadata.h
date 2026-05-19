#pragma once

struct Objc3FrontendClassProtocolCategoryLinkingSummary;
struct Objc3FrontendObjectPointerNullabilityGenericsSummary;
struct Objc3FrontendPropertyAttributeSummary;
struct Objc3FrontendProtocolCategorySummary;
struct Objc3FrontendSelectorNormalizationSummary;
struct Objc3FrontendSymbolGraphScopeResolutionSummary;
struct Objc3IRFrontendMetadata;
struct Objc3InterfaceImplementationSummary;

namespace objc3::artifacts::frontend {

void ApplyObjc3FrontendSemanticClosureMetadata(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const Objc3FrontendObjectPointerNullabilityGenericsSummary
        &object_pointer_nullability_generics_summary,
    const Objc3FrontendSymbolGraphScopeResolutionSummary
        &symbol_graph_scope_resolution_summary,
    bool deterministic_interface_implementation_handoff,
    const Objc3InterfaceImplementationSummary &interface_implementation_summary,
    const Objc3FrontendProtocolCategorySummary &protocol_category_summary,
    const Objc3FrontendClassProtocolCategoryLinkingSummary
        &class_protocol_category_linking_summary,
    const Objc3FrontendSelectorNormalizationSummary
        &selector_normalization_summary,
    const Objc3FrontendPropertyAttributeSummary &property_attribute_summary);

}  // namespace objc3::artifacts::frontend
