#pragma once

#include "pipeline/frontend_executable_metadata_semantic_surface_helpers.h"

namespace objc3c::pipeline::orchestration {

void PopulateExecutableMetadataSemanticValidationBaseState(
    Objc3ExecutableMetadataSemanticValidationSurface &surface,
    const Objc3ExecutableMetadataSourceGraph &graph,
    const Objc3ExecutableMetadataSemanticConsistencyBoundary
        &semantic_consistency_boundary,
    const Objc3SemanticTypeMetadataHandoff &sema_type_metadata_handoff,
    const Objc3FrontendClassProtocolCategoryLinkingSummary
        &class_protocol_category_linking_summary);

void PopulateExecutableMetadataInheritanceValidation(
    Objc3ExecutableMetadataSemanticValidationSurface &surface,
    const Objc3ExecutableMetadataSourceGraph &graph);

void PopulateExecutableMetadataMetaclassValidation(
    Objc3ExecutableMetadataSemanticValidationSurface &surface,
    const Objc3ExecutableMetadataSourceGraph &graph);

void PopulateExecutableMetadataOverrideValidation(
    Objc3ExecutableMetadataSemanticValidationSurface &surface,
    const Objc3ExecutableMetadataSourceGraph &graph);

void PublishExecutableMetadataSemanticValidationReadiness(
    Objc3ExecutableMetadataSemanticValidationSurface &surface,
    const Objc3MethodLookupOverrideConflictSummary
        &method_lookup_override_conflict_summary,
    const Objc3FrontendClassProtocolCategoryLinkingSummary
        &class_protocol_category_linking_summary);

}  // namespace objc3c::pipeline::orchestration
