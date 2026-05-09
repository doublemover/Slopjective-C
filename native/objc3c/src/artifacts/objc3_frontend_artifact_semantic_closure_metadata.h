#pragma once

#include "ir/objc3_ir_frontend_metadata.h"
#include "sema/model/frontend_concurrency_symbol_graph_summaries.h"
#include "sema/model/frontend_linkage_summaries.h"
#include "sema/objc3_sema_contract_type_handoff.h"

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
