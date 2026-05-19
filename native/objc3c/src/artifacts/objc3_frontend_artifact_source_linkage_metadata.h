#pragma once

struct Objc3FrontendCanonicalLiteralRejectionCounts;
struct Objc3FrontendClassProtocolCategoryLinkingSummary;
struct Objc3FrontendOptions;
struct Objc3FrontendPropertyAttributeSummary;
struct Objc3FrontendProtocolCategorySummary;
struct Objc3FrontendSelectorNormalizationSummary;
struct Objc3IRFrontendMetadata;
struct Objc3InterfaceImplementationSummary;
struct Objc3VersionedConformanceReportLoweringSummary;

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
