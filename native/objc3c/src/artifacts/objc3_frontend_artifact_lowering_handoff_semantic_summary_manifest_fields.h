#pragma once

#include <iosfwd>

#include "sema/model/frontend_concurrency_symbol_graph_summaries.h"
#include "sema/model/frontend_linkage_summaries.h"

namespace objc3::artifacts::frontend {

void AppendObjc3FrontendArtifactLoweringHandoffSemanticSummaryManifestFields(
    std::ostringstream &manifest,
    const Objc3FrontendObjectPointerNullabilityGenericsSummary
        &object_pointer_nullability_generics_summary,
    const Objc3FrontendSymbolGraphScopeResolutionSummary
        &symbol_graph_scope_resolution_summary);

}  // namespace objc3::artifacts::frontend
