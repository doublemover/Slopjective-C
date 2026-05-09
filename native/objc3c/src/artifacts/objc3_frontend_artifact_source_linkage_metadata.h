#pragma once

#include "ir/objc3_ir_frontend_metadata.h"
#include "pipeline/results/canonical_literal_rejection_counts.h"
#include "pipeline/results/compile_options.h"
#include "pipeline/results/report_dto.h"
#include "sema/model/frontend_linkage_summaries.h"
#include "sema/objc3_sema_contract_type_handoff.h"

namespace objc3::artifacts::frontend {

void ApplyObjc3FrontendSourceLinkageMetadata(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const Objc3FrontendOptions &options,
    const Objc3VersionedConformanceReportLoweringSummary
        &versioned_conformance_report_lowering,
    const Objc3FrontendCanonicalLiteralRejectionCounts
        &canonical_literal_rejection_counts,
    const Objc3InterfaceImplementationSummary &interface_implementation_summary,
    const Objc3FrontendProtocolCategorySummary &protocol_category_summary,
    const Objc3FrontendClassProtocolCategoryLinkingSummary
        &class_protocol_category_linking_summary,
    const Objc3FrontendSelectorNormalizationSummary
        &selector_normalization_summary,
    const Objc3FrontendPropertyAttributeSummary &property_attribute_summary);

}  // namespace objc3::artifacts::frontend
